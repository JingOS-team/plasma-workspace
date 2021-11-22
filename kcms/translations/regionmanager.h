/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */
#ifndef REGIONMANAGER_H
#define REGIONMANAGER_H
#include <KConfigGroup>
#include <QLocale>
#include <QDateTime>

#include <QHash>
#include <QMultiMap>
#include <QAbstractListModel>
#include <QMetaEnum>

class RegionManager: public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString lcNumericContent READ lcNumericContent NOTIFY dataChanged)
    Q_PROPERTY(QString lcTimeContent READ lcTimeContent NOTIFY dataChanged)

    Q_PROPERTY(QString lcMonetaryContent READ lcMonetaryContent NOTIFY dataChanged)
    Q_PROPERTY(QString lcMeasurementContent READ lcMeasurementContent NOTIFY dataChanged)
    Q_PROPERTY(QString lcLocation READ lcLocation NOTIFY dataChanged)
    Q_PROPERTY(int regionSystemIndex READ regionSystemIndex NOTIFY dataChanged)


public:
    enum RegionDataRole {
            RegionName = 1,
            RegionHead
        };
    Q_ENUM(RegionDataRole)
    RegionManager();
    ~RegionManager();
    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    void load();
    Q_INVOKABLE void updateExample(QString globName);
    Q_INVOKABLE void showUpdateExampleByIndex(int globIndex);
    Q_INVOKABLE void updateRegionModel(QString localName);
    void readConfig();
    Q_INVOKABLE void writeConfig(int globalIndex);
    Q_INVOKABLE void writeLanguage(QString langText);
    Q_INVOKABLE void cacheSectionIndex();
    Q_INVOKABLE int readSectionIndex(QString sectionString);
    QString lcNumericContent(){
        return m_lcNumericContent;
    }
    QString lcTimeContent(){
        return m_lcTimeContent;
    }
    QString lcMonetaryContent(){
        return m_lcMonetaryContent;
    }
    QString lcMeasurementContent(){
        return m_lcMeasurementContent;
    }
    QString lcLocation(){
        return m_lcLocation;
    }
    int regionSystemIndex(){
        return m_regionSystemIndex;
    }
private:
    QList<QLocale> m_allLocales;
    QList<QLocale> m_cacheLocales;
    QMultiMap <QString,QLocale> m_maplocales;
    QMultiMap<QString,int> m_cacheSectionIndex;
    KConfigGroup m_config;
    QString m_lcNumericContent;
    QString m_lcTimeContent;
    QString m_lcMonetaryContent;
    QString m_lcMeasurementContent;
    QString m_language;
    QString m_lcLocation;
    int m_regionSystemIndex = 0;
signals:
    void modelChanged();
    void dataChanged();
};

#endif // REGIONMANAGER_H
