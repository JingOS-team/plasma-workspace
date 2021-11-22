/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */

#include "statuspanel.h"
#include <stdint.h>
#include <QDebug>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QDBusReply>

#include <solid/devicenotifier.h>
#include <solid/device.h>
#include <solid/deviceinterface.h>
#include <solid/battery.h>

#include <KSharedConfig>
#include <KConfigGroup>

#include <QStorageInfo>


StatusPanelEngine::StatusPanelEngine(QObject *parent, const QVariantList &args)
    : Plasma::DataEngine(parent, args),
      m_alarmVisible(false),
      m_udiskInsert(false),
      m_soundInsert(false),
      m_flightMode(false)
{
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceAdded,
                this,                              &StatusPanelEngine::slotDeviceAdded);
        connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceRemoved,
                this,                              &StatusPanelEngine::slotDeviceRemoved);


    //添加闹钟dbus响应
    QDBusConnection::sessionBus().connect(QString(), QString("/jingos/alarm/statusbaricon"),
        QString("jingos.alarm.statusbaricon"), QString("getVisible"), this,
        SLOT(alarmVisibleChanged(bool)));

    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.kclockd"),
                                                      QStringLiteral("/Alarms"),
                                                      QStringLiteral("org.kde.kclock.AlarmModel"),
                                                      QStringLiteral("getNextAlarm"));
    QDBusReply<uint64_t> reply = QDBusConnection::sessionBus().call(msg);
    uint64_t nAlarmID = reply.isValid() ? reply.value() : 0;
    m_alarmVisible = (nAlarmID != 0);


    QDBusConnection::sessionBus().connect(QStringLiteral(""),
                                                       QStringLiteral("/PlasmaShell"),
                                                       "org.kde.PlasmaShell",
                                                       QStringLiteral("flightModeChanged"),
                                                       this,
                                                       SLOT(onFlightModeChanged(bool)));


    initEngine();

    m_initEngineTimer = new QTimer(this);
    m_initEngineTimer->setInterval(30000);
    connect(m_initEngineTimer, &QTimer::timeout, this, &StatusPanelEngine::initUdiskState);
    m_initEngineTimer->start();
}

StatusPanelEngine::~StatusPanelEngine()
{
    delete m_initEngineTimer;
}

void StatusPanelEngine::initWifiData()
{

    QDBusConnection::sessionBus().connect(QStringLiteral(""),
                                                       QStringLiteral("/PlasmaShell"),
                                                       "org.kde.PlasmaShell",
                                                       QStringLiteral("wifiEnableChanged"),
                                                       this,
                                                       SLOT(onWifiEnableChanged(bool)));
    QDBusConnection::sessionBus().connect(QStringLiteral(""),
                                                       QStringLiteral("/PlasmaShell"),
                                                       "org.kde.PlasmaShell",
                                                       QStringLiteral("wifiNameChanged"),
                                                       this,
                                                       SLOT(onWifiNameChanged(QString)));
    QDBusConnection::sessionBus().connect(QStringLiteral(""),
                                                       QStringLiteral("/PlasmaShell"),
                                                       "org.kde.PlasmaShell",
                                                       QStringLiteral("connectionIconChanged"),
                                                       this,
                                                       SLOT(onConnectionIconChanged(QString)));
    QDBusConnection::sessionBus().connect(QStringLiteral(""),
                                                       QStringLiteral("/PlasmaShell"),
                                                       "org.kde.PlasmaShell",
                                                       QStringLiteral("networkStatusChanged"),
                                                       this,
                                                       SLOT(onNetworkStatusChanged(QString)));


    QDBusMessage msg = QDBusMessage::createMethodCall(QStringLiteral("org.kde.plasmanetworkservice"),
                                                      QStringLiteral("/org/kde/plasmanetworkservice"),
                                                      QStringLiteral("org.kde.plasmanetworkservice"),
                                                      QStringLiteral("isWifiEnabled"));
    QDBusReply<bool> reply = QDBusConnection::sessionBus().call(msg);
    bool isWifiEnabled = reply.isValid() ? reply.value() : false;
    setData(QStringLiteral("StatusPanel"), QStringLiteral("wifiEnabled"), isWifiEnabled);

    QDBusMessage msg1 = QDBusMessage::createMethodCall(QStringLiteral("org.kde.plasmanetworkservice"),
                                                      QStringLiteral("/org/kde/plasmanetworkservice"),
                                                      QStringLiteral("org.kde.plasmanetworkservice"),
                                                      QStringLiteral("currentWifiName"));
    QDBusReply<QString> reply1 = QDBusConnection::sessionBus().call(msg1);
    QString currentWifiName = reply1.isValid() ? reply1.value() : "";
    setData(QStringLiteral("StatusPanel"), QStringLiteral("currentWifiName"), currentWifiName);

    QDBusMessage msg2 = QDBusMessage::createMethodCall(QStringLiteral("org.kde.plasmanetworkservice"),
                                                      QStringLiteral("/org/kde/plasmanetworkservice"),
                                                      QStringLiteral("org.kde.plasmanetworkservice"),
                                                      QStringLiteral("currentConnectionIcon"));
    QDBusReply<QString> reply2 = QDBusConnection::sessionBus().call(msg2);
    QString currentConnectionIcon = reply2.isValid() ? reply2.value() : "";
    setData(QStringLiteral("StatusPanel"), QStringLiteral("currentConnectionIcon"), currentConnectionIcon);

    QDBusMessage msg3 = QDBusMessage::createMethodCall(QStringLiteral("org.kde.plasmanetworkservice"),
                                                      QStringLiteral("/org/kde/plasmanetworkservice"),
                                                      QStringLiteral("org.kde.plasmanetworkservice"),
                                                      QStringLiteral("connectionStatus"));
    QDBusReply<QString> reply3 = QDBusConnection::sessionBus().call(msg3);
    QString connectionStatus = reply3.isValid() ? reply3.value() : "";
    setData(QStringLiteral("StatusPanel"), QStringLiteral("networkStatus"), connectionStatus);

}

void StatusPanelEngine::initEngine()
{
    initWifiData();

    KSharedConfigPtr profilesConfig = KSharedConfig::openConfig("jingnetworkprofilesrc", KConfig::SimpleConfig);
    // Let's start: AC profile before anything else
    KConfigGroup flightProfile(profilesConfig, "NetStatus");

    m_flightMode = flightProfile.readEntry< bool >("flightMode", false);
    setData(QStringLiteral("StatusPanel"), QStringLiteral("flight mode"), m_flightMode);
    setData(QStringLiteral("StatusPanel"), QStringLiteral("alarm active"), m_alarmVisible);
    setData(QStringLiteral("StatusPanel"), QStringLiteral("sound insert"), m_soundInsert);
    setData(QStringLiteral("StatusPanel"), QStringLiteral("udisk insert"), m_udiskInsert);
}

void StatusPanelEngine::initUdiskState()
{
    QList<QStorageInfo> list = QStorageInfo::mountedVolumes();
    for(int i = 0; i < list.count(); ++i)
    {
        QStorageInfo diskInfo = list.at(i);
        qint64 freeSize = diskInfo.bytesFree();
        qint64 totalSize = diskInfo.bytesTotal();
        QString tempInfo = "";

        if(diskInfo.displayName().startsWith("/media") ||
                (!diskInfo.displayName().startsWith("/")&& diskInfo.displayName() != "vendor" && diskInfo.displayName() != "productinfo") ){
            m_udiskInsert=true;
            return;
        }
    }
    m_udiskInsert=false;
}

void StatusPanelEngine::onFlightModeChanged(bool enable)
{
    m_flightMode = enable;
    setData(QStringLiteral("StatusPanel"), QStringLiteral("flight mode"), m_flightMode);
}

bool StatusPanelEngine::sourceRequestEvent(const QString &source)
{
    setData(QStringLiteral("StatusPanel"), QStringLiteral("alarm active"), m_alarmVisible);
    setData(QStringLiteral("StatusPanel"), QStringLiteral("sound insert"), m_soundInsert);
    setData(QStringLiteral("StatusPanel"), QStringLiteral("udisk insert"), m_udiskInsert);
    setData(QStringLiteral("StatusPanel"), QStringLiteral("flight mode"), m_flightMode);
    return true;
}

void StatusPanelEngine::alarmVisibleChanged(bool visble)
{
    m_alarmVisible = visble;
    setData(QStringLiteral("StatusPanel"), QStringLiteral("alarm active"), visble);
}

void StatusPanelEngine::slotDeviceAdded(QString message)
{
    if(message.contains("input") || message.contains("mouse")|| message.contains("net") || message.contains("keyboard"))
        return;

    if(message.contains("sound")){
        m_soundInsert = true;
        setData(QStringLiteral("StatusPanel"), QStringLiteral("sound insert"), m_soundInsert);
        return;
    }
    if(message.contains("UDisks")){
        m_udiskInsert = true;
        setData(QStringLiteral("StatusPanel"), QStringLiteral("udisk insert"), m_udiskInsert);
    }
}

void StatusPanelEngine::slotDeviceRemoved(QString message)
{
    if(message.contains("input") || message.contains("mouse") || message.contains("net") || message.contains("keyboard"))
        return;
    if(message.contains("sound")){
        m_soundInsert = false;
        setData(QStringLiteral("StatusPanel"), QStringLiteral("sound insert"), m_soundInsert);
        return;
    }
    if(message.contains("UDisks")){
        m_udiskInsert = false;
        setData(QStringLiteral("StatusPanel"), QStringLiteral("udisk insert"), m_udiskInsert);
    }
}

void StatusPanelEngine::onWifiNameChanged(QString name)
{

    setData(QStringLiteral("StatusPanel"), QStringLiteral("currentWifiName"), name);
}

void StatusPanelEngine::onConnectionIconChanged(QString name)
{

    setData(QStringLiteral("StatusPanel"), QStringLiteral("currentConnectionIcon"), name);
}

void StatusPanelEngine::onNetworkStatusChanged(QString name)
{

    setData(QStringLiteral("StatusPanel"), QStringLiteral("networkStatus"), name);
}

void StatusPanelEngine::onWifiEnableChanged(bool enable)
{

    setData(QStringLiteral("StatusPanel"), QStringLiteral("wifiEnabled"), enable);
}

K_EXPORT_PLASMA_DATAENGINE_WITH_JSON(statuspanel, StatusPanelEngine, "plasma-dataengine-statuspanel.json")

#include "statuspanel.moc"
