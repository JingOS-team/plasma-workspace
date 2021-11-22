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

#ifndef TIMEZONEMODEL_H
#define TIMEZONEMODEL_H

#include <KSharedConfig>
#include <KConfigWatcher>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>

#include "timezonedata.h"

class TimezonesI18n;

class TimeZoneFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterString WRITE setFilterString MEMBER m_filterString NOTIFY filterStringChanged)
    Q_PROPERTY(bool onlyShowChecked WRITE setOnlyShowChecked MEMBER m_onlyShowChecked NOTIFY onlyShowCheckedChanged)
	Q_PROPERTY(bool isSystem24HourFormat READ isSystem24HourFormat NOTIFY isSystem24HourFormatChanged);
    Q_PROPERTY(QString getDateByTimezone READ getDateByTimezone NOTIFY timezoneChanged);

public:
    explicit TimeZoneFilterProxy(QObject *parent = nullptr);
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

    void setFilterString(const QString &filterString);
    void setOnlyShowChecked(const bool show);
public Q_SLOTS:
    bool isSystem24HourFormat();
    void kcmClockUpdated();
    QString getDateByTimezone();
    QString getRegionTimeFormat();

Q_SIGNALS:
    void filterStringChanged();
	void isSystem24HourFormatChanged();
	void timezoneChanged();
    void onlyShowCheckedChanged();

private:
    QString m_filterString;
    bool m_onlyShowChecked;
    QStringMatcher m_stringMatcher;

    KConfigWatcher::Ptr m_localeConfigWatcher;
    KSharedConfig::Ptr m_localeConfig;
    KConfigWatcher::Ptr m_timezoneConfigWatcher;
    KSharedConfig::Ptr m_timezoneConfig;
};

//=============================================================================

class TimeZoneModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QStringList selectedTimeZones WRITE setSelectedTimeZones MEMBER m_selectedTimeZones NOTIFY selectedTimeZonesChanged)

public:
    explicit TimeZoneModel(QObject *parent = nullptr);
    ~TimeZoneModel() override;

    enum Roles {
        TimeZoneIdRole = Qt::UserRole + 1,
        RegionRole,
        CityRole,
        CommentRole,
        CheckedRole,
        IsLocalTimeZoneRole,
    };

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void update();
    void setSelectedTimeZones(const QStringList &selectedTimeZones);

    Q_INVOKABLE void selectLocalTimeZone();

Q_SIGNALS:
    void selectedTimeZonesChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    void sortTimeZones();

    QList<TimeZoneData> m_data;
    QHash<QString, int> m_offsetData; // used for sorting
    QStringList m_selectedTimeZones;
    TimezonesI18n *m_timezonesI18n;
};

#endif // TIMEZONEMODEL_H
