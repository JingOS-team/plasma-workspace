/*
 *   Copyright 2007 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2007-2008 Sebastian Kuegler <sebas@kde.org>
 *   CopyRight 2007 Maor Vanmak <mvanmak1@gmail.com>
 *   Copyright 2008 Dario Freddi <drf54321@gmail.com>
 *   Copyright 2021 Liu Bangguo <liubangguo@jingos.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "powermanagementengine.h"

// kde-workspace/libs
#include "sessionmanagementbackend.h"
#include <sessionmanagement.h>
#include <QFile>

// solid specific includes
#include <solid/device.h>
#include <solid/deviceinterface.h>
#include <solid/devicenotifier.h>

#include <KAuthorized>
#include <KIdleTime>
#include <klocalizedstring.h>

#include <QDebug>

#include <QDBusConnectionInterface>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingCallWatcher>
#include <QDBusReply>
#include <QDBusArgument>

#include "powermanagementservice.h"
#include <Plasma/DataContainer>
#include <fcntl.h>
#include <unistd.h>

static const char SOLID_POWERMANAGEMENT_SERVICE[] = "org.kde.Solid.PowerManagement";
static const char JINGOS_REPOWER_SCREEN_SERVICE[] = "com.jingos.repowerd.Screen";
static int s_brightness = -1;

Q_DECLARE_METATYPE(QList<InhibitionInfo>)
Q_DECLARE_METATYPE(InhibitionInfo)
Q_DECLARE_METATYPE(BrightnessParam);

QDBusArgument & operator<<(QDBusArgument &argument, const BrightnessParam& param)
{
    argument.beginStructure();
    argument << param.dim;
    argument << param.minBrightness;
    argument << param.maxBrightness;
    argument << param.defaultBrightness;
    argument << param.autoBrightness;
    argument.endStructure();

    return argument;
}

const QDBusArgument & operator>>(const QDBusArgument &argument, BrightnessParam &param)
{
    argument.beginStructure();
    argument >> param.dim;
    argument >> param.minBrightness;
    argument >> param.maxBrightness;
    argument >> param.defaultBrightness;
    argument >> param.autoBrightness;
    argument.endStructure();

    return argument;
}

PowermanagementEngine::PowermanagementEngine(QObject *parent, const QVariantList &args)
    : Plasma::DataEngine(parent, args)
    , m_sources(basicSourceNames())
    , m_session(new SessionManagement(this))
{
    Q_UNUSED(args)
    qDBusRegisterMetaType<QList<InhibitionInfo>>();
    qDBusRegisterMetaType<InhibitionInfo>();
    qDBusRegisterMetaType<BrightnessParam>();

    m_quickChargingTimer.setInterval(5000);
    m_pluginTimer.setInterval(5000);
    m_pluginTimer.setSingleShot(true);

#if defined (__arm64__) || defined (__aarch64__)
    initArm();
#else
    init();
#endif
}

PowermanagementEngine::~PowermanagementEngine()
{
}

void PowermanagementEngine::initArm()
{
    connect(&m_powerButtonTimer,&QTimer::timeout,&m_powerButtonTimer,&QTimer::stop);
    connect(&m_pluginTimer,&QTimer::timeout,this,&PowermanagementEngine::onPluginTimerout);
    connect(&m_quickChargingTimer,&QTimer::timeout,this,&PowermanagementEngine::onQuickChargingTimerout);
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceAdded,
            this,                              &PowermanagementEngine::deviceAdded);
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceRemoved,
            this,                              &PowermanagementEngine::deviceRemoved);

    /*
    if (QDBusConnection::systemBus().interface()->isServiceRegistered(JINGOS_REPOWER_SCREEN_SERVICE)) {
        if (!QDBusConnection::systemBus().connect(JINGOS_REPOWER_SCREEN_SERVICE,
                                                   QStringLiteral("/com/jingos/repowerd/Screen"),
                                                   QStringLiteral("com.jingos.repowerd.Screen"),
                                                   QStringLiteral("DisplayPowerStateChange"), this,
                                                   SLOT(DisplayPowerStateChange(int,int)))) {
            qDebug() << "error connecting to Brightness changes via dbus";
        }
    }
    */

    QDBusConnection::systemBus().connect(QStringLiteral("com.jingos.repowerd.Screen"),
                                                QStringLiteral("/com/jingos/repowerd/Screen"),
                                                QStringLiteral("com.jingos.repowerd.Screen"),
                                                QStringLiteral("AutobrightnessChange"),
                                                this,
                                                SLOT(autoBrightnessChanged(bool)));

    //[liubangguo]添加repower按键dbus支持
//        QDBusConnection::systemBus().connect("",
//                                                  QStringLiteral("/com/jingos/repowerd/PowerButton"),
//                                                  QStringLiteral("com.jingos.repowerd.PowerButton"),
//                                                  QStringLiteral("Press"), this, SLOT(onRepowerButtonPressed()));
//        QDBusConnection::systemBus().connect("",
//                                                  QStringLiteral("/com/jingos/repowerd/PowerButton"),
//                                                  QStringLiteral("com.jingos.repowerd.PowerButton"),
//                                                  QStringLiteral("Release"), this, SLOT(onRepowerButtonReleased()));

        QDBusConnection::systemBus().connect("",
                                                  QStringLiteral("/com/jingos/repowerd/PowerButton"),
                                                  QStringLiteral("com.jingos.repowerd.PowerButton"),
                                                  QStringLiteral("LongPress"), this, SLOT(onRepowerButtonLongpressed()));

        QDBusConnection::systemBus().connect("",
                                                  QStringLiteral("/com/jingos/powerd"),
                                                  QStringLiteral("org.freedesktop.DBus.Properties"),
                                                  QStringLiteral("PropertiesChanged"), this, SLOT(onPropertiesChanged(QString,QVariantMap,QStringList)));
        QDBusConnection::systemBus().connect("",
                                                  QStringLiteral("/org/freedesktop/UPower/devices/battery_battery"),
                                                  QStringLiteral("org.freedesktop.DBus.Properties"),
                                                  QStringLiteral("PropertiesChanged"), this, SLOT(onBatteryPropertiesChanged(QString,QVariantMap,QStringList)));





}

void PowermanagementEngine::init()
{
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceAdded, this, &PowermanagementEngine::deviceAdded);
    connect(Solid::DeviceNotifier::instance(), &Solid::DeviceNotifier::deviceRemoved, this, &PowermanagementEngine::deviceRemoved);

    if (QDBusConnection::sessionBus().interface()->isServiceRegistered(SOLID_POWERMANAGEMENT_SERVICE)) {
        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/BrightnessControl"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.BrightnessControl"),
                                                   QStringLiteral("brightnessChanged"),
                                                   this,
                                                   SLOT(screenBrightnessChanged(int)))) {
            qDebug() << "error connecting to Brightness changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/BrightnessControl"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.BrightnessControl"),
                                                   QStringLiteral("brightnessMaxChanged"),
                                                   this,
                                                   SLOT(maximumScreenBrightnessChanged(int)))) {
            qDebug() << "error connecting to max brightness changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/KeyboardBrightnessControl"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.KeyboardBrightnessControl"),
                                                   QStringLiteral("keyboardBrightnessChanged"),
                                                   this,
                                                   SLOT(keyboardBrightnessChanged(int)))) {
            qDebug() << "error connecting to Keyboard Brightness changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/KeyboardBrightnessControl"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.KeyboardBrightnessControl"),
                                                   QStringLiteral("keyboardBrightnessMaxChanged"),
                                                   this,
                                                   SLOT(maximumKeyboardBrightnessChanged(int)))) {
            qDebug() << "error connecting to max keyboard Brightness changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/HandleButtonEvents"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.HandleButtonEvents"),
                                                   QStringLiteral("triggersLidActionChanged"),
                                                   this,
                                                   SLOT(triggersLidActionChanged(bool)))) {
            qDebug() << "error connecting to lid action trigger changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement/PolicyAgent"),
                                                   QStringLiteral("org.kde.Solid.PowerManagement.PolicyAgent"),
                                                   QStringLiteral("InhibitionsChanged"),
                                                   this,
                                                   SLOT(inhibitionsChanged(QList<InhibitionInfo>, QStringList)))) {
            qDebug() << "error connecting to inhibition changes via dbus";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                   SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("batteryRemainingTimeChanged"),
                                                   this,
                                                   SLOT(batteryRemainingTimeChanged(qulonglong)))) {
            qDebug() << "error connecting to remaining time changes";
        }

        if (!QDBusConnection::sessionBus().connect(SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                   SOLID_POWERMANAGEMENT_SERVICE,
                                                   QStringLiteral("chargeStopThresholdChanged"),
                                                   this,
                                                   SLOT(chargeStopThresholdChanged(int)))) {
            qDebug() << "error connecting to charge stop threshold changes via dbus";
        }
    }
}

QStringList PowermanagementEngine::basicSourceNames() const
{
    QStringList sources;
    sources << QStringLiteral("Battery") << QStringLiteral("AC Adapter") << QStringLiteral("Sleep States") << QStringLiteral("PowerDevil")
            << QStringLiteral("Inhibitions");
    return sources;
}

QStringList PowermanagementEngine::sources() const
{
    return m_sources;
}

bool PowermanagementEngine::sourceRequestEvent(const QString &name)
{
    if (name == QLatin1String("Battery")) {
        const QList<Solid::Device> listBattery = Solid::Device::listFromType(Solid::DeviceInterface::Battery);
        m_batterySources.clear();

        double percent = getCurrentBatteryPrecent();
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), percent);

        if (listBattery.isEmpty()) {
            setData(QStringLiteral("Battery"), QStringLiteral("Has Battery"), false);
            setData(QStringLiteral("Battery"), QStringLiteral("Has Cumulative"), false);
            return true;
        }

        uint index = 0;
        QStringList batterySources;

        foreach (const Solid::Device &deviceBattery, listBattery) {
            const Solid::Battery *battery = deviceBattery.as<Solid::Battery>();

#if defined (__arm64__) || defined (__aarch64__)
            if(deviceBattery.udi().contains("battery_battery") == false)
                continue;
#endif

            const QString source = QStringLiteral("Battery%1").arg(index++);

            batterySources << source;
            m_batterySources[deviceBattery.udi()] = source;

            connect(battery, &Solid::Battery::chargeStateChanged, this, &PowermanagementEngine::updateBatteryChargeState);
            connect(battery, &Solid::Battery::chargePercentChanged, this, &PowermanagementEngine::updateBatteryChargePercent);
            connect(battery, &Solid::Battery::energyChanged, this, &PowermanagementEngine::updateBatteryEnergy);
            connect(battery, &Solid::Battery::presentStateChanged, this, &PowermanagementEngine::updateBatteryPresentState);

            // Set initial values
            updateBatteryChargeState(battery->chargeState(), deviceBattery.udi());
            updateBatteryChargePercent(battery->chargePercent(), deviceBattery.udi());
            updateBatteryEnergy(battery->energy(), deviceBattery.udi());
            updateBatteryPresentState(battery->isPresent(), deviceBattery.udi());
            updateBatteryPowerSupplyState(battery->isPowerSupply(), deviceBattery.udi());

            setData(source, QStringLiteral("Vendor"), deviceBattery.vendor());
            setData(source, QStringLiteral("Product"), deviceBattery.product());
            setData(source, QStringLiteral("Capacity"), battery->capacity());
            setData(source, QStringLiteral("Type"), batteryType(battery));
        }

        updateBatteryNames();
        updateOverallBattery();

        setData(QStringLiteral("Battery"), QStringLiteral("Has Battery"), !batterySources.isEmpty());
        if (!batterySources.isEmpty()) {
            setData(QStringLiteral("Battery"), QStringLiteral("Sources"), batterySources);
#if defined (__arm64__) || defined (__aarch64__)
            if(m_pluginTimer.isActive() == true)
            {
                setData(QStringLiteral("Battery"), QStringLiteral("Full msec"), 0);
                setData(QStringLiteral("Battery"), QStringLiteral("Remaining msec"), 0);

            }
            else{
                updateBatteryFullRemainTime();
            }
#else
            QDBusMessage msg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                              QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                              SOLID_POWERMANAGEMENT_SERVICE,
                                                              QStringLiteral("batteryRemainingTime"));
            QDBusPendingReply<qulonglong> reply = QDBusConnection::sessionBus().asyncCall(msg);
            QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
            QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
                QDBusPendingReply<qulonglong> reply = *watcher;
                if (!reply.isError()) {
                    batteryRemainingTimeChanged(reply.value());
                }
                watcher->deleteLater();
            });
#endif
        }

        QDBusMessage msg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                          QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                          SOLID_POWERMANAGEMENT_SERVICE,
                                                          QStringLiteral("chargeStopThreshold"));
        QDBusPendingReply<int> reply = QDBusConnection::sessionBus().asyncCall(msg);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        QObject::connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<int> reply = *watcher;
            if (!reply.isError()) {
                chargeStopThresholdChanged(reply.value());
            }
            watcher->deleteLater();
        });

        m_sources = basicSourceNames() + batterySources;
    } else if (name == QLatin1String("AC Adapter")) {

    } else if (name == QLatin1String("Sleep States")) {
        setData(QStringLiteral("Sleep States"), QStringLiteral("Standby"), m_session->canSuspend());
        setData(QStringLiteral("Sleep States"), QStringLiteral("Suspend"), m_session->canSuspend());
        setData(QStringLiteral("Sleep States"), QStringLiteral("Hibernate"), m_session->canHibernate());
        setData(QStringLiteral("Sleep States"), QStringLiteral("HybridSuspend"), m_session->canHybridSuspend());
        setData(QStringLiteral("Sleep States"), QStringLiteral("LockScreen"), m_session->canLock());
        setData(QStringLiteral("Sleep States"), QStringLiteral("Logout"), m_session->canLogout());
    } else if (name == QLatin1String("PowerDevil")) {
#if defined (__arm64__) || defined (__aarch64__)

        QDBusMessage maxScreenMsg = QDBusMessage::createMethodCall(QStringLiteral("com.jingos.powerd"),
                                                                  QStringLiteral("/com/jingos/powerd"),
                                                                  QStringLiteral("com.jingos.powerd"),
                                                                  QStringLiteral("getBrightnessParams"));
        QDBusPendingReply<BrightnessParam> maxScreenReply = QDBusConnection::systemBus().asyncCall(maxScreenMsg);
        QDBusPendingCallWatcher *maxScreenWatcher = new QDBusPendingCallWatcher(maxScreenReply, this);
        QObject::connect(maxScreenWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {

            QDBusPendingReply<BrightnessParam> reply = *watcher;

            if (!reply.isError()) {
                maximumScreenBrightnessChanged(reply.value().maxBrightness);
                if(s_brightness <=0)
                    screenBrightnessChanged(reply.value().defaultBrightness);
                else
                    screenBrightnessChanged(s_brightness);
                autoBrightnessChanged(reply.value().autoBrightness);
            }
            watcher->deleteLater();
        });
#else
        QDBusMessage screenMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                QStringLiteral("/org/kde/Solid/PowerManagement/Actions/BrightnessControl"),
                                                                QStringLiteral("org.kde.Solid.PowerManagement.Actions.BrightnessControl"),
                                                                QStringLiteral("brightness"));
        QDBusPendingReply<int> screenReply = QDBusConnection::sessionBus().asyncCall(screenMsg);
        QDBusPendingCallWatcher *screenWatcher = new QDBusPendingCallWatcher(screenReply, this);
        QObject::connect(screenWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<int> reply = *watcher;
            if (!reply.isError()) {
                screenBrightnessChanged(reply.value());
            }
            watcher->deleteLater();
        });

        QDBusMessage maxScreenMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                   QStringLiteral("/org/kde/Solid/PowerManagement/Actions/BrightnessControl"),
                                                                   QStringLiteral("org.kde.Solid.PowerManagement.Actions.BrightnessControl"),
                                                                   QStringLiteral("brightnessMax"));
        QDBusPendingReply<int> maxScreenReply = QDBusConnection::sessionBus().asyncCall(maxScreenMsg);
        QDBusPendingCallWatcher *maxScreenWatcher = new QDBusPendingCallWatcher(maxScreenReply, this);
        QObject::connect(maxScreenWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<int> reply = *watcher;
            if (!reply.isError()) {
                maximumScreenBrightnessChanged(reply.value());
            }
            watcher->deleteLater();
        });
#endif

        QDBusMessage keyboardMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                  QStringLiteral("/org/kde/Solid/PowerManagement/Actions/KeyboardBrightnessControl"),
                                                                  QStringLiteral("org.kde.Solid.PowerManagement.Actions.KeyboardBrightnessControl"),
                                                                  QStringLiteral("keyboardBrightness"));
        QDBusPendingReply<int> keyboardReply = QDBusConnection::sessionBus().asyncCall(keyboardMsg);
        QDBusPendingCallWatcher *keyboardWatcher = new QDBusPendingCallWatcher(keyboardReply, this);
        QObject::connect(keyboardWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<int> reply = *watcher;
            if (!reply.isError()) {
                keyboardBrightnessChanged(reply.value());
            }
            watcher->deleteLater();
        });

        QDBusMessage maxKeyboardMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                     QStringLiteral("/org/kde/Solid/PowerManagement/Actions/KeyboardBrightnessControl"),
                                                                     QStringLiteral("org.kde.Solid.PowerManagement.Actions.KeyboardBrightnessControl"),
                                                                     QStringLiteral("keyboardBrightnessMax"));
        QDBusPendingReply<int> maxKeyboardReply = QDBusConnection::sessionBus().asyncCall(maxKeyboardMsg);
        QDBusPendingCallWatcher *maxKeyboardWatcher = new QDBusPendingCallWatcher(maxKeyboardReply, this);
        QObject::connect(maxKeyboardWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<int> reply = *watcher;
            if (!reply.isError()) {
                maximumKeyboardBrightnessChanged(reply.value());
            }
            watcher->deleteLater();
        });

        QDBusMessage lidIsPresentMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                      QStringLiteral("/org/kde/Solid/PowerManagement"),
                                                                      SOLID_POWERMANAGEMENT_SERVICE,
                                                                      QStringLiteral("isLidPresent"));
        QDBusPendingReply<bool> lidIsPresentReply = QDBusConnection::sessionBus().asyncCall(lidIsPresentMsg);
        QDBusPendingCallWatcher *lidIsPresentWatcher = new QDBusPendingCallWatcher(lidIsPresentReply, this);
        QObject::connect(lidIsPresentWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<bool> reply = *watcher;
            if (!reply.isError()) {
                setData(QStringLiteral("PowerDevil"), QStringLiteral("Is Lid Present"), reply.value());
            }
            watcher->deleteLater();
        });

        QDBusMessage triggersLidActionMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                           QStringLiteral("/org/kde/Solid/PowerManagement/Actions/HandleButtonEvents"),
                                                                           QStringLiteral("org.kde.Solid.PowerManagement.Actions.HandleButtonEvents"),
                                                                           QStringLiteral("triggersLidAction"));
        QDBusPendingReply<bool> triggersLidActionReply = QDBusConnection::sessionBus().asyncCall(triggersLidActionMsg);
        QDBusPendingCallWatcher *triggersLidActionWatcher = new QDBusPendingCallWatcher(triggersLidActionReply, this);
        QObject::connect(triggersLidActionWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<bool> reply = *watcher;
            if (!reply.isError()) {
                triggersLidActionChanged(reply.value());
            }
            watcher->deleteLater();
        });

    } else if (name == QLatin1String("Inhibitions")) {
        QDBusMessage inhibitionsMsg = QDBusMessage::createMethodCall(SOLID_POWERMANAGEMENT_SERVICE,
                                                                     QStringLiteral("/org/kde/Solid/PowerManagement/PolicyAgent"),
                                                                     QStringLiteral("org.kde.Solid.PowerManagement.PolicyAgent"),
                                                                     QStringLiteral("ListInhibitions"));
        QDBusPendingReply<QList<InhibitionInfo>> inhibitionsReply = QDBusConnection::sessionBus().asyncCall(inhibitionsMsg);
        QDBusPendingCallWatcher *inhibitionsWatcher = new QDBusPendingCallWatcher(inhibitionsReply, this);
        QObject::connect(inhibitionsWatcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *watcher) {
            QDBusPendingReply<QList<InhibitionInfo>> reply = *watcher;
            watcher->deleteLater();

            if (!reply.isError()) {
                removeAllData(QStringLiteral("Inhibitions"));

                inhibitionsChanged(reply.value(), QStringList());
            }
        });

        // any info concerning lock screen/screensaver goes here
    } else if (name == QLatin1String("UserActivity")) {
        setData(QStringLiteral("UserActivity"), QStringLiteral("IdleTime"), KIdleTime::instance()->idleTime());
    } else {
        qDebug() << "Data for '" << name << "' not found";
        return false;
    }
    return true;
}

QString PowermanagementEngine::batteryType(const Solid::Battery *battery) const
{
    switch (battery->type()) {
    case Solid::Battery::PrimaryBattery:
        return QStringLiteral("Battery");
        break;
    case Solid::Battery::UpsBattery:
        return QStringLiteral("Ups");
        break;
    case Solid::Battery::MonitorBattery:
        return QStringLiteral("Monitor");
        break;
    case Solid::Battery::MouseBattery:
        return QStringLiteral("Mouse");
        break;
    case Solid::Battery::KeyboardBattery:
        return QStringLiteral("Keyboard");
        break;
    case Solid::Battery::PdaBattery:
        return QStringLiteral("Pda");
        break;
    case Solid::Battery::PhoneBattery:
        return QStringLiteral("Phone");
        break;
    case Solid::Battery::GamingInputBattery:
        return QStringLiteral("GamingInput");
        break;
    case Solid::Battery::BluetoothBattery:
        return QStringLiteral("Bluetooth");
        break;
    default:
        return QStringLiteral("Unknown");
    }

    return QStringLiteral("Unknown");
}

bool PowermanagementEngine::updateSourceEvent(const QString &source)
{
    if (source == QLatin1String("UserActivity")) {
        setData(QStringLiteral("UserActivity"), QStringLiteral("IdleTime"), KIdleTime::instance()->idleTime());
        return true;
    }
    return Plasma::DataEngine::updateSourceEvent(source);
}

Plasma::Service *PowermanagementEngine::serviceForSource(const QString &source)
{
    if (source == QLatin1String("PowerDevil")) {
        return new PowerManagementService(this);
    }

    return nullptr;
}

QString PowermanagementEngine::batteryStateToString(int newState) const
{
    QString state(QStringLiteral("Unknown"));
    if (newState == Solid::Battery::NoCharge) {
        state = QLatin1String("NoCharge");
    } else if (newState == Solid::Battery::Charging) {
        state = QLatin1String("Charging");
    } else if (newState == Solid::Battery::Discharging) {
        state = QLatin1String("Discharging");
    } else if (newState == Solid::Battery::FullyCharged) {
        state = QLatin1String("FullyCharged");
    }

    return state;
}

void PowermanagementEngine::updateBatteryChargeState(int newState, const QString &udi)
{
    const QString source = m_batterySources[udi];
    setData(source, QStringLiteral("State"), batteryStateToString(newState));
#if defined (__arm64__) || defined (__aarch64__)
    //[liubangguo]support upower for arm pad
    if(newState == 2)//discharging
    {
        m_quickChargingTimer.stop();
        m_pluginTimer.start();
        setData(QStringLiteral("AC Adapter"), QStringLiteral("Plugged in"), false);
        setData(QStringLiteral("Battery"), QStringLiteral("Remaining msec"), 0);
        setData(QStringLiteral("Battery"), QStringLiteral("Full msec"), 0);
    }
    else if(newState == 1)//charging
    {
        setData(QStringLiteral("AC Adapter"), QStringLiteral("Plugged in"), true);
        setData(QStringLiteral("AC Adapter"), QStringLiteral("Quick charging"), false);
        setData(QStringLiteral("Battery"), QStringLiteral("Remaining msec"), 0);
        setData(QStringLiteral("Battery"), QStringLiteral("Full msec"), 0);
        m_quickChargingTimer.start();
        m_pluginTimer.start();

        //[liubangguo]hide low battery dialog when charging
        QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasmashell",
                                                                           "/org/kde/messageboxService",
                                                                           "org.kde.messageboxService",
                                                                           "hideDialog");
        QDBusConnection::sessionBus().asyncCall(message);
    }
#endif
    updateOverallBattery();
}

void PowermanagementEngine::updateBatteryPresentState(bool newState, const QString &udi)
{
    const QString source = m_batterySources[udi];
    setData(source, QStringLiteral("Plugged in"), newState); // FIXME This needs to be renamed and Battery Monitor adjusted
}

void PowermanagementEngine::updateBatteryChargePercent(int newValue, const QString &udi)
{
    const QString source = m_batterySources[udi];
    setData(source, QStringLiteral("Percent"), newValue);
    updateOverallBattery();
}

void PowermanagementEngine::updateBatteryEnergy(double newValue, const QString &udi)
{
    const QString source = m_batterySources[udi];
    setData(source, QStringLiteral("Energy"), newValue);
}

void PowermanagementEngine::updateBatteryPowerSupplyState(bool newState, const QString &udi)
{
    const QString source = m_batterySources[udi];
    setData(source, QStringLiteral("Is Power Supply"), newState);
}

void PowermanagementEngine::updateBatteryFullRemainTime()
{
    QDBusInterface interface("org.freedesktop.UPower", "/org/freedesktop/UPower/devices/battery_battery",
                             "org.freedesktop.UPower.Device",
                             QDBusConnection::systemBus());

    qint64 timeToFull = 0;
    if (interface.isValid()) {
        QVariant reply = interface.property("TimeToFull");
        if (reply.isValid()) {
            timeToFull = reply.toLongLong();
        }
    }
    setData(QStringLiteral("Battery"), QStringLiteral("Full msec"), timeToFull);

    QDBusInterface interface1("org.freedesktop.UPower", "/org/freedesktop/UPower/devices/battery_battery",
                             "org.freedesktop.UPower.Device",
                             QDBusConnection::systemBus());

    qint64 timeRemain = 0;
    if (interface1.isValid()) {
        QVariant reply = interface1.property("TimeToEmpty");
        if (reply.isValid()) {
            timeRemain = reply.toLongLong();
        }
    }
    setData(QStringLiteral("Battery"), QStringLiteral("Remaining msec"), timeRemain);
}

void PowermanagementEngine::updateBatteryNames()
{
    uint unnamedBatteries = 0;
    foreach (QString source, m_batterySources) {
        DataContainer *batteryDataContainer = containerForSource(source);
        if (batteryDataContainer) {
            const QString batteryVendor = batteryDataContainer->data()[QStringLiteral("Vendor")].toString();
            const QString batteryProduct = batteryDataContainer->data()[QStringLiteral("Product")].toString();

            // Don't show battery name for primary power supply batteries. They usually have cryptic serial number names.
            const bool showBatteryName = batteryDataContainer->data()[QStringLiteral("Type")].toString() != QLatin1String("Battery")
                || !batteryDataContainer->data()[QStringLiteral("Is Power Supply")].toBool();

            if (!batteryProduct.isEmpty() && batteryProduct != QLatin1String("Unknown Battery") && showBatteryName) {
                if (!batteryVendor.isEmpty()) {
                    setData(source, QStringLiteral("Pretty Name"), QString(batteryVendor + ' ' + batteryProduct));
                } else {
                    setData(source, QStringLiteral("Pretty Name"), batteryProduct);
                }
            } else {
                ++unnamedBatteries;
                if (unnamedBatteries > 1) {
                    setData(source, QStringLiteral("Pretty Name"), i18nc("Placeholder is the battery number", "Battery %1", unnamedBatteries));
                } else {
                    setData(source, QStringLiteral("Pretty Name"), i18n("Battery"));
                }
            }
        }
    }
}

void PowermanagementEngine::updateOverallBattery()
{
    const QList<Solid::Device> listBattery = Solid::Device::listFromType(Solid::DeviceInterface::Battery);
    bool hasCumulative = false;

    double energy = 0;
    double totalEnergy = 0;
    bool allFullyCharged = true;
    bool charging = false;
    bool noCharge = false;
    double totalPercentage = 0;
    int count = 0;

    foreach (const Solid::Device &deviceBattery, listBattery) {
        const Solid::Battery *battery = deviceBattery.as<Solid::Battery>();
        
#if defined (__arm64__) || defined (__aarch64__)
        if(deviceBattery.udi().contains("battery_battery") == false)
            continue;
#endif

        if (battery && battery->isPowerSupply()) {
            hasCumulative = true;

            energy += battery->energy();
            totalEnergy += battery->energyFull();
            totalPercentage += battery->chargePercent();
            allFullyCharged = allFullyCharged && (battery->chargeState() == Solid::Battery::FullyCharged);
            charging = charging || (battery->chargeState() == Solid::Battery::Charging);
            noCharge = noCharge || (battery->chargeState() == Solid::Battery::NoCharge);
            ++count;
        }
    }

    if (count == 1) {
        // Energy is sometimes way off causing us to show rubbish; this is a UPower issue
        // but anyway having just one battery and the tooltip showing strange readings
        // compared to the popup doesn't look polished.
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), totalPercentage);
    } else if (totalEnergy > 0) {
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), qRound(energy / totalEnergy * 100));
    } else if (count > 0) { // UPS don't have energy, see Bug 348588
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), qRound(totalPercentage / static_cast<qreal>(count)));
    } else {
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), 0);
    }

    //if(charging == false)
    checkLowBattery(totalPercentage,charging);

    if (hasCumulative) {
        if (allFullyCharged) {
            setData(QStringLiteral("Battery"), QStringLiteral("State"), "FullyCharged");
        } else if (charging) {
            setData(QStringLiteral("Battery"), QStringLiteral("State"), "Charging");
        } else if (noCharge) {
            setData(QStringLiteral("Battery"), QStringLiteral("State"), "NoCharge");
        } else {
            setData(QStringLiteral("Battery"), QStringLiteral("State"), "Discharging");
        }
    } else {
        setData(QStringLiteral("Battery"), QStringLiteral("State"), "Unknown");
    }

    setData(QStringLiteral("Battery"), QStringLiteral("Has Cumulative"), hasCumulative);
}

void PowermanagementEngine::updateAcPlugState(bool onBattery)
{
    setData(QStringLiteral("AC Adapter"), QStringLiteral("Plugged in"), !onBattery);
}

void PowermanagementEngine::deviceRemoved(const QString &udi)
{
    if (m_batterySources.contains(udi)) {
        Solid::Device device(udi);
        Solid::Battery *battery = device.as<Solid::Battery>();
        if (battery){
            
#if defined (__arm64__) || defined (__aarch64__)
            if(udi.contains("battery_battery") == false)
                return;
#endif


            battery->disconnect();
        }

        const QString source = m_batterySources[udi];
        m_batterySources.remove(udi);
        //[liubangguo]当底层异常发送deviceRemove时，不去removeResource，以防数据获取不到
        //removeSource(source);

        QStringList sourceNames(m_batterySources.values());
        sourceNames.removeAll(source);
        setData(QStringLiteral("Battery"), QStringLiteral("Sources"), sourceNames);
        setData(QStringLiteral("Battery"), QStringLiteral("Has Battery"), !sourceNames.isEmpty());

        updateOverallBattery();
    }
}

void PowermanagementEngine::deviceAdded(const QString &udi)
{
    Solid::Device device(udi);
    if (device.isValid()) {
        const Solid::Battery *battery = device.as<Solid::Battery>();

        if (battery) {
            int index = 0;
            QStringList sourceNames(m_batterySources.values());
            while (sourceNames.contains(QStringLiteral("Battery%1").arg(index))) {
                index++;
            }

            
    #if defined (__arm64__) || defined (__aarch64__)
            if(udi.contains("battery_battery") == false)
                return;
    #endif

            const QString source = QStringLiteral("Battery%1").arg(index);
            sourceNames << source;
            m_batterySources[device.udi()] = source;

            connect(battery, &Solid::Battery::chargeStateChanged, this, &PowermanagementEngine::updateBatteryChargeState);
            connect(battery, &Solid::Battery::chargePercentChanged, this, &PowermanagementEngine::updateBatteryChargePercent);
            connect(battery, &Solid::Battery::energyChanged, this, &PowermanagementEngine::updateBatteryEnergy);
            connect(battery, &Solid::Battery::presentStateChanged, this, &PowermanagementEngine::updateBatteryPresentState);
            connect(battery, &Solid::Battery::powerSupplyStateChanged, this, &PowermanagementEngine::updateBatteryPowerSupplyState);

            // Set initial values
            updateBatteryChargeState(battery->chargeState(), device.udi());
            updateBatteryChargePercent(battery->chargePercent(), device.udi());
            updateBatteryEnergy(battery->energy(), device.udi());
            updateBatteryPresentState(battery->isPresent(), device.udi());
            updateBatteryPowerSupplyState(battery->isPowerSupply(), device.udi());

            setData(source, QStringLiteral("Vendor"), device.vendor());
            setData(source, QStringLiteral("Product"), device.product());
            setData(source, QStringLiteral("Capacity"), battery->capacity());
            setData(source, QStringLiteral("Type"), batteryType(battery));

            setData(QStringLiteral("Battery"), QStringLiteral("Sources"), sourceNames);
            setData(QStringLiteral("Battery"), QStringLiteral("Has Battery"), !sourceNames.isEmpty());

            updateBatteryNames();
            updateOverallBattery();
        }
    }
}

void PowermanagementEngine::batteryRemainingTimeChanged(qulonglong time)
{
    // qDebug() << "Remaining time 2:" << time;
    setData(QStringLiteral("Battery"), QStringLiteral("Remaining msec"), time);
}

void PowermanagementEngine::screenBrightnessChanged(int brightness)
{
    s_brightness = brightness;
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Screen Brightness"), brightness);

}

void PowermanagementEngine::autoBrightnessChanged(bool autoBrightness)
{
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Auto Brightness"), autoBrightness);
}

void PowermanagementEngine::maximumScreenBrightnessChanged(int maximumBrightness)
{
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Maximum Screen Brightness"), maximumBrightness);
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Screen Brightness Available"), maximumBrightness > 0);
}

void PowermanagementEngine::keyboardBrightnessChanged(int brightness)
{
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Keyboard Brightness"), brightness);
}

void PowermanagementEngine::maximumKeyboardBrightnessChanged(int maximumBrightness)
{
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Maximum Keyboard Brightness"), maximumBrightness);
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Keyboard Brightness Available"), maximumBrightness > 0);
}

void PowermanagementEngine::triggersLidActionChanged(bool triggers)
{
    setData(QStringLiteral("PowerDevil"), QStringLiteral("Triggers Lid Action"), triggers);
}

void PowermanagementEngine::inhibitionsChanged(const QList<InhibitionInfo> &added, const QStringList &removed)
{
    for (auto it = removed.constBegin(); it != removed.constEnd(); ++it) {
        removeData(QStringLiteral("Inhibitions"), (*it));
    }

    for (auto it = added.constBegin(); it != added.constEnd(); ++it) {
        const QString &name = (*it).first;
        QString prettyName;
        QString icon;
        const QString &reason = (*it).second;

        populateApplicationData(name, &prettyName, &icon);

        setData(QStringLiteral("Inhibitions"),
                name,
                QVariantMap{{QStringLiteral("Name"), prettyName}, {QStringLiteral("Icon"), icon}, {QStringLiteral("Reason"), reason}});
    }
}

void PowermanagementEngine::populateApplicationData(const QString &name, QString *prettyName, QString *icon)
{
    if (m_applicationInfo.contains(name)) {
        const auto &info = m_applicationInfo.value(name);
        *prettyName = info.first;
        *icon = info.second;
    } else {
        KService::Ptr service = KService::serviceByStorageId(name + ".desktop");
        if (service) {
            *prettyName = service->property(QStringLiteral("Name"), QVariant::Invalid).toString(); // cannot be null
            *icon = service->icon();

            m_applicationInfo.insert(name, qMakePair(*prettyName, *icon));
        } else {
            *prettyName = name;
            *icon = name.section(QLatin1Char('/'), -1).toLower();
        }
    }
}

void PowermanagementEngine::DisplayPowerStateChange(int state,int reason)
{
    if(state == 0)//灭屏
    {
        QDBusConnection::sessionBus().asyncCall(QDBusMessage::createMethodCall("org.freedesktop.ScreenSaver",
                                                                           "/ScreenSaver",
                                                                           "org.freedesktop.ScreenSaver",
                                                                           "Lock"));
    }
}

void PowermanagementEngine::chargeStopThresholdChanged(int threshold)
{
    setData(QStringLiteral("Battery"), QStringLiteral("Charge Stop Threshold"), threshold);
}

void PowermanagementEngine::onRepowerButtonPressed()
{
    if(m_powerButtonTimer.isActive() == true)
        m_powerButtonTimer.stop();

    m_powerButtonTimer.start(1000);
}

void PowermanagementEngine::onRepowerButtonReleased()
{
    if(m_powerButtonTimer.isActive() == true)
        QDBusConnection::sessionBus().asyncCall(QDBusMessage::createMethodCall("org.freedesktop.ScreenSaver",
                                                                           "/ScreenSaver",
                                                                           "org.freedesktop.ScreenSaver",
                                                                           "Lock"));
}


void PowermanagementEngine::onRepowerButtonLongpressed()
{
    QDBusConnection::sessionBus().asyncCall(QDBusMessage::createMethodCall("org.kde.LogoutPrompt",
                                                                           "/LogoutPrompt",
                                                                           "org.kde.LogoutPrompt",
                                                                           "promptLogout"));

}

void PowermanagementEngine::onBatteryPropertiesChanged(QString serviceName,QVariantMap map,QStringList strList)
{

    if(m_pluginTimer.isActive() == true)
        return;
    if(serviceName == "org.freedesktop.UPower.Device")
    {
        int timeToFull = 0;
        auto it = map.find(QStringLiteral("TimeToFull"));
        if (it != map.end()) {
            timeToFull = it.value().toInt();
            setData(QStringLiteral("Battery"), QStringLiteral("Full msec"), timeToFull);
        }

        int timeRemain = 0;
        auto it2 = map.find(QStringLiteral("TimeToEmpty"));
        if (it2 != map.end()) {
            timeRemain = it2.value().toInt();
            setData(QStringLiteral("Battery"), QStringLiteral("Remaining msec"), timeRemain);
        }


        auto it3 = map.find(QStringLiteral("Percentage"));
        if (it3 == map.end()) {
            return;
        }
        double percent = it3.value().toDouble();
        setData(QStringLiteral("Battery"), QStringLiteral("Percent"), percent);

    }
}

void PowermanagementEngine::onPropertiesChanged(QString serviceName,QVariantMap map,QStringList strList)
{
    if(serviceName == "com.jingos.powerd")
    {
        auto it = map.find(QStringLiteral("brightness"));
        if (it == map.end()) {
            return;
        }
        int brightness = it.value().toInt();
        setData(QStringLiteral("PowerDevil"), QStringLiteral("Screen Brightness"), brightness);
        s_brightness = brightness;
    }
}


void PowermanagementEngine::onQuickChargingTimerout()
{

    if(isQuickCharging())
    {
        setData(QStringLiteral("AC Adapter"), QStringLiteral("Quick charging"), true);
    }
    else {
        setData(QStringLiteral("AC Adapter"), QStringLiteral("Quick charging"), false);
    }


}

void PowermanagementEngine::onPluginTimerout()
{

    //updateBatteryFullRemainTime();

}

bool PowermanagementEngine::isQuickCharging()
{
    QFile device("/sys/class/power_supply/eta6937_charger/status");
    if (!device.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         return false;
    }
    QByteArray line = device.readLine(strlen("Charging")+1);
    if(QString(line)=="Charging")
        return true;
    return false;
}

bool PowermanagementEngine::isLowElectric()
{
    QFile device("/sys/class/power_supply/battery/current_now");
    if (!device.open(QIODevice::ReadOnly | QIODevice::Text))
    {
         qWarning()<< "[isLowElectric]Can't open the file!" <<endl;
         return false;
    }
    int electric = device.readLine().toInt();
    if(electric < 0)
        return true;
    return false;
}

void PowermanagementEngine::checkLowBattery(int percent,bool isCharging)
{
    bool bLowBatteryShow = false;
    QString lowBatteryContent = "";
    switch (percent) {
    case 20:
    {
        bLowBatteryShow = true;
        lowBatteryContent = QStringLiteral("Battery power is less than 20%");
    }
        break;
    case 10:
    {
        bLowBatteryShow = true;
        lowBatteryContent = QStringLiteral("Battery power is less than 10%");
    }
        break;
    case 5:
    {
        bLowBatteryShow = true;
        lowBatteryContent = QStringLiteral("Battery power is less than 5%");
    }
        break;
    default:
        if(percent <=3 && isLowElectric())
        {
            //低电关机
            //SessionBackend::self()->shutdown();
            qDebug()<<"Low battery & shut down!!!!!";
            qDebug()<<"Low battery & shut down!!!!!";
            qDebug()<<"Low battery & shut down!!!!!";
            m_session->requestShutdown(SessionManagement::ConfirmationMode::Skip);

        }
        break;
    }
    //[liubangguo]show low battery dialog
    if(bLowBatteryShow == true && !isCharging){
        QDBusMessage message = QDBusMessage::createMethodCall("org.kde.plasmashell",
                                                                           "/org/kde/messageboxService",
                                                                           "org.kde.messageboxService",
                                                                           "showDialog");
        message.setArguments({QStringLiteral("Low Battery Warning"), lowBatteryContent});
        QDBusConnection::sessionBus().asyncCall(message);
    }
}

double PowermanagementEngine::getCurrentBatteryPrecent(){
   QDBusInterface interface("org.freedesktop.UPower", "/org/freedesktop/UPower/devices/battery_battery",
                             "org.freedesktop.UPower.Device",
                             QDBusConnection::systemBus());
    qint64 kValue = 0;
    if (interface.isValid()) {

        QVariant reply = interface.property("Percentage");
        if (reply.isValid()) {
            kValue = reply.toDouble();

        }
    }
    return kValue ;
}

K_EXPORT_PLASMA_DATAENGINE_WITH_JSON(powermanagement, PowermanagementEngine, "plasma-dataengine-powermanagement.json")

#include "powermanagementengine.moc"
