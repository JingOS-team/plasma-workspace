/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */
#include "regionmanager.h"
#include <KLocalizedString>
#include <QFile>
#include <QDebug>
#include <QStandardPaths>
#include <KSharedConfig>
#include <KPluginFactory>
#include <QTextCodec>
#include <QProcess>

RegionManager::RegionManager()
{
    load();
}

RegionManager::~RegionManager()
{
    qDebug() << "RegionManager::~RegionManager()";
}

QHash<int, QByteArray> RegionManager::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();

    QMetaEnum e = metaObject()->enumerator(metaObject()->indexOfEnumerator("RegionDataRole"));

    for (int i = 0; i < e.keyCount(); ++i) {
        roles.insert(e.value(i), e.key(i));
    }
    qDebug()<< Q_FUNC_INFO << " roles::" << roles.values();

    return roles;
}

QVariant RegionManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_allLocales.count()) {
        return QVariant();
    }

    if (role == RegionName) {
        QLocale gloc = m_allLocales.at(index.row());
        QString lnString;
        QString clabel = !gloc.nativeCountryName().isEmpty() ? gloc.nativeCountryName() : QLocale::countryToString(gloc.country());
        if(clabel == "台灣" || clabel == "台湾"){
            clabel = "中國台灣";
        }
        const QString nativeLangName = gloc.nativeLanguageName();
           if (!nativeLangName.isEmpty()) {
               lnString = i18n("%1 - %2", clabel, nativeLangName);
           } else {
               lnString = i18n("%1", clabel);
           }
        return lnString;
    } else if (role == RegionHead) {
        QLocale gloc = m_allLocales.at(index.row());
        QString lnString;
        QString clabel = !gloc.nativeCountryName().isEmpty() ? gloc.nativeCountryName() : QLocale::countryToString(gloc.country());
        if(clabel == "台灣" || clabel == "台湾"){
            clabel = "中國台灣";
        }
        return QString(clabel[0]);
    }

    return QVariant();
}

int RegionManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_allLocales.count();
}

void RegionManager::load()
{
    QProcess getProcess;
    getProcess.start("locale -a");
    getProcess.waitForFinished(); // sets current thread to sleep and waits for pingProcess end
    QString output(getProcess.readAllStandardOutput());
    QStringList locationLang = output.split("\n");

    qDebug() << Q_FUNC_INFO << " output data:" << output << " locationLang:" << locationLang;
    m_cacheLocales.clear();
    foreach(QString itemLang , locationLang){
        //.toLower() no load default item
        if(itemLang == "" || !itemLang.contains("utf")){
            continue;
        }
        QLocale itemLocale(itemLang);
        m_cacheLocales.append(itemLocale);
    }
//    m_cacheLocales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);
   std::sort(m_cacheLocales.begin(), m_cacheLocales.end(), [](const QLocale & c1, const QLocale & c2){
       // Ensure that the "Default (C)" locale always appears at the top
       if (c1.name()== QLatin1Char('C') && c2.name()!=QLatin1String("C")) return true;
       if (c2.name()== QLatin1Char('C')) return false;

       const QString ncn1 = !c1.nativeCountryName().isEmpty() ? c1.nativeCountryName() : QLocale::countryToString(c1.country());
       const QString ncn2 = !c2.nativeCountryName().isEmpty() ? c2.nativeCountryName() : QLocale::countryToString(c2.country());
       return QString::localeAwareCompare(ncn1, ncn2) < 0;
   });
    m_allLocales = m_cacheLocales;

//    foreach(QLocale gloc, m_cacheLocales){
//        const QString clabel = !gloc.nativeCountryName().isEmpty() ? gloc.nativeCountryName() : QLocale::countryToString(gloc.country());
//        m_maplocales.insert(clabel,gloc);
//    }

    readConfig();

    updateExample(m_language);
    qDebug()<<Q_FUNC_INFO << " dataSize:" << m_allLocales.size();
    emit modelChanged();
}

void RegionManager::cacheSectionIndex()
{
    if(m_cacheLocales.size() <= 0){
        return;
    }
    for(int i = 0 ; i < m_cacheLocales.size() ; i++){
        QLocale gloc = m_cacheLocales[i];
        QString clabel = !gloc.nativeCountryName().isEmpty() ? gloc.nativeCountryName() : QLocale::countryToString(gloc.country());
        if(clabel == "台灣" || clabel == "台湾"){
            clabel = "中國台灣";
        }
        bool isContains = m_cacheSectionIndex.contains(clabel.left(1));
        if(!isContains){
            m_cacheSectionIndex.insert(clabel.left(1),i);
        }
    }
}
int RegionManager::readSectionIndex(QString sectionString)
{
  int index =  m_cacheSectionIndex.value(sectionString,-1);
  qDebug()<< Q_FUNC_INFO << " sectionString:" << sectionString << " index:" << index;
  return index;
}

bool countryLessThan(const QLocale & c1, const QLocale & c2)
{
    // Ensure that the "Default (C)" locale always appears at the top
    if (c1.name()== QLatin1Char('C') && c2.name()!=QLatin1String("C")) return true;
    if (c2.name()== QLatin1Char('C')) return false;

    const QString ncn1 = !c1.nativeCountryName().isEmpty() ? c1.nativeCountryName() : QLocale::countryToString(c1.country());
    const QString ncn2 = !c2.nativeCountryName().isEmpty() ? c2.nativeCountryName() : QLocale::countryToString(c2.country());
    return QString::localeAwareCompare(ncn1, ncn2) < 0;
}

 void RegionManager::updateRegionModel(QString localName)
 {
     beginResetModel();
     m_allLocales.clear();
     if(localName != ""){
         QList<QLocale> cq;
         foreach(QLocale gloc, m_cacheLocales){
            QString clabel = !gloc.nativeCountryName().isEmpty() ? gloc.nativeCountryName() : QLocale::countryToString(gloc.country());
             if(clabel == "台灣" || clabel == "台湾"){
                clabel = "中國台灣";
             }
             if(clabel.toLower().contains(localName.toLower())){
                 cq.append(gloc);
             }
         }
         m_allLocales = cq;
     } else {
         m_allLocales = m_cacheLocales;
     }
     endResetModel();
 }

 void RegionManager::showUpdateExampleByIndex(int globIndex)
 {
     QLocale locale = m_allLocales.at(globIndex);
     QString cvalue = locale.name();
     if (!cvalue.contains(QLatin1Char('.')) && cvalue != QLatin1Char('C')) {
         // explicitly add the encoding,
         // otherwise Qt doesn't accept dead keys and garbles the output as well
         cvalue.append(QLatin1Char('.') + QTextCodec::codecForLocale()->name());
     }
     updateExample(cvalue);
 }

void RegionManager::updateExample(QString globName)
{
    QLocale gloc = QLocale(globName);

    const QString numberExample = gloc.toString(1000.01);
    const QString timeExample = i18n("%1 (long format)", gloc.toString(QDateTime::currentDateTime())) + QLatin1Char('\n') +
            i18n("%1 (short format)", gloc.toString(QDateTime::currentDateTime(), QLocale::ShortFormat));
    const QString currencyExample = gloc.toCurrencyString(24.00);

    QString measurementSetting;
    if (gloc.measurementSystem() == QLocale::ImperialUKSystem) {
        measurementSetting = i18nc("Measurement combobox", "Imperial UK");
    } else if (gloc.measurementSystem() == QLocale::ImperialUSSystem || gloc.measurementSystem() == QLocale::ImperialSystem) {
        measurementSetting = i18nc("Measurement combobox", "Imperial US");
    } else {
        measurementSetting = i18nc("Measurement combobox", "Metric");
    }
    QString clabel = !gloc.nativeCountryName().isEmpty() ? gloc.nativeCountryName() : QLocale::countryToString(gloc.country());

    if(clabel == "台灣" || clabel == "台湾"){
        clabel = "中國台灣";
    }
    m_lcNumericContent = numberExample;
    m_lcTimeContent = timeExample;
    m_lcMeasurementContent = measurementSetting;
    m_lcMonetaryContent = currencyExample;
    m_lcLocation = clabel;
    const QString nativeLangName = gloc.nativeLanguageName();
       if (!nativeLangName.isEmpty()) {
           m_lcLocation = i18n("%1 - %2", clabel, nativeLangName);
       } else {
           m_lcLocation = i18n("%1", clabel);
       }
    for (int i = 0 ; i < m_cacheLocales.size() ; i++) {
        QLocale cLocale = m_cacheLocales.at(i);
        if(cLocale.name() == gloc.name()){
            m_regionSystemIndex = i;
            break;
        }
    }
    emit dataChanged();

    qDebug()<< " numberS:" << numberExample << " timeExample:" << timeExample << " currencyExample:" << currencyExample
            << " measurementSetting:" << measurementSetting<< " m_lcLocation:" << m_lcLocation
            << " gloc.name():" << gloc.name();
}

const static QString configFile = QStringLiteral("plasma-localerc");
const static QString exportFile = QStringLiteral("plasma-locale-settings.sh");

const static QString lcLang = QStringLiteral("LANG");
const static QString lcFLang = QStringLiteral("FLANG");
const static QString lcNumeric = QStringLiteral("LC_NUMERIC");
const static QString lcTime = QStringLiteral("LC_TIME");
const static QString lcMonetary = QStringLiteral("LC_MONETARY");
const static QString lcMeasurement = QStringLiteral("LC_MEASUREMENT");
const static QString lcCollate = QStringLiteral("LC_COLLATE");
const static QString lcCtype = QStringLiteral("LC_CTYPE");

const static QString lcLanguage = QStringLiteral("LANGUAGE");

void RegionManager::readConfig()
{
    m_config = KConfigGroup(KSharedConfig::openConfig(configFile, KConfig::FullConfig), "Formats");

    bool useDetailed = m_config.readEntry("useDetailed", false);
    m_language = m_config.readEntry(lcFLang, qgetenv(lcTime.toLatin1()));
    m_lcNumericContent = m_config.readEntry(lcNumeric, qgetenv(lcNumeric.toLatin1()));
    m_lcTimeContent = m_config.readEntry(lcTime, qgetenv(lcTime.toLatin1()));
    QString lcCollateStr = m_config.readEntry(lcCollate, qgetenv(lcCollate.toLatin1()));
    m_lcMonetaryContent = m_config.readEntry(lcMonetary, qgetenv(lcMonetary.toLatin1()));
    m_lcMeasurementContent = m_config.readEntry(lcMeasurement, qgetenv(lcMeasurement.toLatin1()));
    qDebug()  << " lcStr:" << m_language
              << " lcNumStr:" << m_lcNumericContent
              << " lcTimeStr:" << m_lcTimeContent
              << " lcCollateStr:" << lcCollateStr
              << " lcMonetaryStr:" << m_lcMonetaryContent
              << " lcMeasurementStr:" << m_lcMeasurementContent;
}

void RegionManager::writeLanguage(QString langText)
{
    langText.append(QLatin1Char('.') + QTextCodec::codecForLocale()->name());
    qDebug()<< " lang string:" << langText;
    m_config = KConfigGroup(KSharedConfig::openConfig(configFile, KConfig::FullConfig), "Formats");
    m_config.writeEntry(lcLang, langText);
    m_config.sync();
}

void RegionManager::writeConfig(int global)
{
    m_config = KConfigGroup(KSharedConfig::openConfig(configFile, KConfig::FullConfig), "Formats");

    // global ends up empty here when OK button is clicked from kcmshell5,
    // apparently the data in the combo is gone by the time save() is called.
    // This might be a problem in KCModule, but does not directly affect us
    // since within systemsettings, it works fine.
    // See https://bugs.kde.org/show_bug.cgi?id=334624


    if (global == -1) {
        m_config.deleteEntry(lcFLang);
        m_config.deleteEntry(lcNumeric);
        m_config.deleteEntry(lcTime);
        m_config.deleteEntry(lcMonetary);
        m_config.deleteEntry(lcMeasurement);
        m_config.deleteEntry(lcCollate);
        m_config.deleteEntry(lcCtype);
    } else {
        QLocale locale = m_allLocales.at(global);
        QString cvalue = locale.name();
        if (!cvalue.contains(QLatin1Char('.')) && cvalue != QLatin1Char('C')) {
            // explicitly add the encoding,
            // otherwise Qt doesn't accept dead keys and garbles the output as well
            cvalue.append(QLatin1Char('.') + QTextCodec::codecForLocale()->name());
        }
        m_config.writeEntry("useDetailed", true);
        m_config.writeEntry(lcNumeric, cvalue);
        m_config.writeEntry(lcTime, cvalue);
        m_config.writeEntry(lcFLang, cvalue);
        m_config.writeEntry(lcMonetary, cvalue);
        m_config.writeEntry(lcMeasurement, cvalue);
        m_config.writeEntry(lcCollate, cvalue);
    }

    m_config.sync();
}
