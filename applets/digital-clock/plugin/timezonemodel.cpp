/***************************************************************************
 *   Copyright (C) 2014 Kai Uwe Broulik <kde@privat.broulik.de>            *
 *   Copyright (C) 2014  Martin Klapetek <mklapetek@kde.org>               *
 *   Copyright (C) 2021  Liu Bangguo <liubangguo@jingos.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/

#include "timezonemodel.h"
#include "timezonesi18n.h"

#include <QDateTime>
#include <QLocale>
#include <KLocalizedString>
#include <QStringMatcher>
#include <QTimeZone>
#include <QString>
#include <QDBusPendingReply>
#include <QDBusConnection>

#define FORMAT24H "HH:mm:ss"

TimeZoneFilterProxy::TimeZoneFilterProxy(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_stringMatcher.setCaseSensitivity(Qt::CaseInsensitive);

//liubangguo for clock update
    m_localeConfig = KSharedConfig::openConfig(QStringLiteral("kdeglobals"), KConfig::SimpleConfig);
    m_localeConfigWatcher = KConfigWatcher::create(m_localeConfig);
    QDBusConnection::sessionBus().connect(QString(), QString("/org/kde/kcmshell_clock"),
                                          QString("org.kde.kcmshell_clock"), QString("clockUpdated"), this,
                                          SLOT(kcmClockUpdated()));
    // watch for changes to locale config, to update 12/24 hour time
    connect(m_localeConfigWatcher.data(), &KConfigWatcher::configChanged,
            this, [this](const KConfigGroup &group, const QByteArrayList &names) -> void {
                if (group.name() == "Locale") {
                    // we have to reparse for new changes (from system settings)
                    m_localeConfig->reparseConfiguration();
                    Q_EMIT isSystem24HourFormatChanged();
                }
            });

    //liubangguo for timezone api
    m_timezoneConfig = KSharedConfig::openConfig(QStringLiteral("ktimezonedrc"), KConfig::SimpleConfig);
    m_timezoneConfigWatcher = KConfigWatcher::create(m_timezoneConfig);

    // watch for changes to locale config, to update 12/24 hour time
    connect(m_timezoneConfigWatcher.data(), &KConfigWatcher::configChanged,
            this, [this](const KConfigGroup &group, const QByteArrayList &names) -> void {
                if (group.name() == "TimeZones") {
                    // we have to reparse for new changes (from system settings)
                    m_timezoneConfig->reparseConfiguration();
                    Q_EMIT timezoneChanged();
                }
            });
}

bool TimeZoneFilterProxy::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!sourceModel() || (m_filterString.isEmpty() && !m_onlyShowChecked)) {
        return true;
    }

    const bool checked = sourceModel()->index(source_row, 0, source_parent).data(TimeZoneModel::CheckedRole).toBool();
    if (m_onlyShowChecked && !checked) {
        return false;
    }

    const QString city = sourceModel()->index(source_row, 0, source_parent).data(TimeZoneModel::CityRole).toString();
    const QString region = sourceModel()->index(source_row, 0, source_parent).data(TimeZoneModel::RegionRole).toString();
    const QString comment = sourceModel()->index(source_row, 0, source_parent).data(TimeZoneModel::CommentRole).toString();

    if (m_stringMatcher.indexIn(city) != -1 || m_stringMatcher.indexIn(region) != -1 || m_stringMatcher.indexIn(comment) != -1) {
        return true;
    }

    return false;
}

void TimeZoneFilterProxy::setFilterString(const QString &filterString)
{
    m_filterString = filterString;
    m_stringMatcher.setPattern(filterString);
    emit filterStringChanged();
    invalidateFilter();
}

void TimeZoneFilterProxy::setOnlyShowChecked(const bool show)
{
    if (m_onlyShowChecked == show) {
        return;
    }
    m_onlyShowChecked = show;
    emit onlyShowCheckedChanged();
}

void TimeZoneFilterProxy::kcmClockUpdated()
{
    m_localeConfig->reparseConfiguration();
    Q_EMIT isSystem24HourFormatChanged();
}

bool TimeZoneFilterProxy::isSystem24HourFormat()
{
    KConfigGroup localeSettings = KConfigGroup(m_localeConfig, "Locale");

    QString timeFormat = localeSettings.readEntry("TimeFormat", QStringLiteral(FORMAT24H));
    return timeFormat == QStringLiteral(FORMAT24H);
}


QString TimeZoneFilterProxy::getDateByTimezone()
{
    KSharedConfig::Ptr timezoneConfig = KSharedConfig::openConfig(QStringLiteral("plasma-localerc"), KConfig::SimpleConfig);
    KConfigGroup timezoneSettings = KConfigGroup(timezoneConfig, "Formats");
    QString timezone = timezoneSettings.readEntry("LC_TIME", QStringLiteral("en_US.UTF-8"));
    QDateTime dateTime = QDateTime::currentDateTime();
    QLocale locale = QLocale::English;

    if(timezone.contains("zh_"))
        locale = QLocale::Chinese;

    QString strFormat = "ddd, MMM d ";
    QString strDateTime = locale.toString(dateTime, strFormat);
    return strDateTime;
}
QString TimeZoneFilterProxy::getRegionTimeFormat()
{
    KSharedConfig::Ptr timezoneConfig = KSharedConfig::openConfig(QStringLiteral("plasma-localerc"), KConfig::SimpleConfig);
    KConfigGroup timezoneSettings = KConfigGroup(timezoneConfig, "Formats");
    QString timezone = timezoneSettings.readEntry("LC_TIME", QStringLiteral("en_US.UTF-8"));

    if(timezone.contains("zh_")){
        return QString("zh_");
    }else{
        return QString();
    }
}

//=============================================================================

TimeZoneModel::TimeZoneModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_timezonesI18n(new TimezonesI18n(this))
{
    update();
}

TimeZoneModel::~TimeZoneModel()
{
}

int TimeZoneModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant TimeZoneModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid()) {
        TimeZoneData currentData = m_data.at(index.row());

        switch (role) {
        case TimeZoneIdRole:
            return currentData.id;
        case RegionRole:
            return currentData.region;
        case CityRole:
            return currentData.city;
        case CommentRole:
            return currentData.comment;
        case CheckedRole:
            return currentData.checked;
        case IsLocalTimeZoneRole:
            return currentData.isLocalTimeZone;
        }
    }

    return QVariant();
}

bool TimeZoneModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || value.isNull()) {
        return false;
    }

    if (role == CheckedRole) {
        m_data[index.row()].checked = value.toBool();
        emit dataChanged(index, index);

        if (m_data[index.row()].checked) {
            m_selectedTimeZones.append(m_data[index.row()].id);
            m_offsetData.insert(m_data[index.row()].id, m_data[index.row()].offsetFromUtc);
        } else {
            m_selectedTimeZones.removeAll(m_data[index.row()].id);
            m_offsetData.remove(m_data[index.row()].id);
        }

        sortTimeZones();

        emit selectedTimeZonesChanged();
        return true;
    }

    return false;
}

void TimeZoneModel::update()
{
    beginResetModel();
    m_data.clear();

    QTimeZone localZone = QTimeZone(QTimeZone::systemTimeZoneId());
    const QStringList data = QString::fromUtf8(localZone.id()).split(QLatin1Char('/'));

    TimeZoneData local;
    local.isLocalTimeZone = true;
    local.id = QStringLiteral("Local");
    local.region = i18nc("This means \"Local Timezone\"", "Local");
    local.city = m_timezonesI18n->i18nCity(data.last());
    local.comment = i18n("System's local time zone");
    local.checked = false;

    m_data.append(local);

    QStringList cities;
    QHash<QString, QTimeZone> zonesByCity;

    const QList<QByteArray> systemTimeZones = QTimeZone::availableTimeZoneIds();

    for (auto it = systemTimeZones.constBegin(); it != systemTimeZones.constEnd(); ++it) {
        const QTimeZone zone(*it);
        const QStringList splitted = QString::fromUtf8(zone.id()).split(QStringLiteral("/"));

        // CITY | COUNTRY | CONTINENT
        const QString key = QStringLiteral("%1|%2|%3").arg(splitted.last(), QLocale::countryToString(zone.country()), splitted.first());

        cities.append(key);
        zonesByCity.insert(key, zone);
    }
    cities.sort(Qt::CaseInsensitive);

    for (const QString &key : qAsConst(cities)) {
        const QTimeZone timeZone = zonesByCity.value(key);
        QString comment = timeZone.comment();

        if (!comment.isEmpty()) {
            comment = i18n(comment.toUtf8());
        }

        const QStringList cityCountryContinent = key.split(QLatin1Char('|'));

        TimeZoneData newData;
        newData.isLocalTimeZone = false;
        newData.id = timeZone.id();
        newData.region = timeZone.country() == QLocale::AnyCountry
            ? QString()
            : m_timezonesI18n->i18nContinents(cityCountryContinent.at(2)) + QLatin1Char('/') + m_timezonesI18n->i18nCountry(timeZone.country());
        newData.city = m_timezonesI18n->i18nCity(cityCountryContinent.at(0));
        newData.comment = comment;
        newData.checked = false;
        newData.offsetFromUtc = timeZone.offsetFromUtc(QDateTime::currentDateTimeUtc());
        m_data.append(newData);
    }

    endResetModel();
}

void TimeZoneModel::setSelectedTimeZones(const QStringList &selectedTimeZones)
{
    m_selectedTimeZones = selectedTimeZones;
    for (int i = 0; i < m_data.size(); i++) {
        if (m_selectedTimeZones.contains(m_data.at(i).id)) {
            m_data[i].checked = true;
            m_offsetData.insert(m_data[i].id, m_data[i].offsetFromUtc);

            QModelIndex index = createIndex(i, 0);
            emit dataChanged(index, index);
        }
    }

    sortTimeZones();
}

void TimeZoneModel::selectLocalTimeZone()
{
    m_data[0].checked = true;

    QModelIndex index = createIndex(0, 0);
    emit dataChanged(index, index);

    m_selectedTimeZones << m_data[0].id;
    emit selectedTimeZonesChanged();
}

QHash<int, QByteArray> TimeZoneModel::roleNames() const
{
    return QHash<int, QByteArray>({
        {TimeZoneIdRole, "timeZoneId"},
        {RegionRole, "region"},
        {CityRole, "city"},
        {CommentRole, "comment"},
        {CheckedRole, "checked"},
        {IsLocalTimeZoneRole, "isLocalTimeZone"},
    });
}

void TimeZoneModel::sortTimeZones()
{
    std::sort(m_selectedTimeZones.begin(), m_selectedTimeZones.end(), [this](const QString &a, const QString &b) {
        return m_offsetData.value(a) < m_offsetData.value(b);
    });
}
