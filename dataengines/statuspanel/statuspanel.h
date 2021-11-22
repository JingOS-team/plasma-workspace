/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */

#ifndef STATUSPANEL_DATAENGINE_H
#define STATUSPANEL_DATAENGINE_H

#include <QTimer>
#include <KProcess>
#include <Plasma/DataContainer>
#include <Plasma/DataEngine>


class StatusPanelEngine : public Plasma::DataEngine
{
    Q_OBJECT
public:
    StatusPanelEngine(QObject *parent, const QVariantList &args);
    virtual ~StatusPanelEngine();

private:
    void initFlightMode();
    void initUdiskState();
    void initWifiData();

public Q_SLOTS:
    void alarmVisibleChanged(bool);
    void slotDeviceAdded(QString);
    void slotDeviceRemoved(QString);
    void onFlightModeChanged(bool enable);
    void onWifiEnableChanged(bool);
    void onWifiNameChanged(QString name);
    void onNetworkStatusChanged(QString name);
    void onConnectionIconChanged(QString name);
    void initEngine();
protected:
    bool sourceRequestEvent(const QString &source) override;

private:
    bool m_alarmVisible;
    bool m_udiskInsert;
    bool m_soundInsert;
    bool m_flightMode;

    QTimer* m_initEngineTimer;
};

#endif
