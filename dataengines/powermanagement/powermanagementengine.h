/*
 *   Copyright 2007 Aaron Seigo <aseigo@kde.org>
 *   Copyright 2007-2008 Sebastian Kuegler <sebas@kde.org>
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

#ifndef POWERMANAGEMENTENGINE_H
#define POWERMANAGEMENTENGINE_H

#include <Plasma/DataEngine>

#include <solid/battery.h>

#include <QDBusConnection>
#include <QVariantMap>
#include <QHash>
#include <QPair>
#include <QTimer>
#include <QFileSystemWatcher>
class SessionManagement;

using InhibitionInfo = QPair<QString, QString>;

struct BrightnessParam{
      int dim;
      int minBrightness;
      int maxBrightness;
      int defaultBrightness;
      bool autoBrightness;

   };

/**
 * This class provides runtime information about the battery and AC status
 * for use in power management Plasma applets.
 */
class PowermanagementEngine : public Plasma::DataEngine
{
    Q_OBJECT

public:
    PowermanagementEngine(QObject *parent, const QVariantList &args);
    ~PowermanagementEngine() override;
    QStringList sources() const override;
    Plasma::Service *serviceForSource(const QString &source) override;

protected:
    bool sourceRequestEvent(const QString &name) override;
    bool updateSourceEvent(const QString &source) override;
    void init();
    void initArm();//for repower

private Q_SLOTS:
    void updateBatteryChargeState(int newState, const QString &udi);
    void updateBatteryPresentState(bool newState, const QString &udi);
    void updateBatteryChargePercent(int newValue, const QString &udi);
    void updateBatteryEnergy(double newValue, const QString &udi);
    void updateBatteryPowerSupplyState(bool newState, const QString &udi);
    void updateAcPlugState(bool onBattery);
    void updateBatteryNames();
    void updateOverallBattery();
    void updateBatteryFullRemainTime();

    void deviceRemoved(const QString &udi);
    void deviceAdded(const QString &udi);
    void batteryRemainingTimeChanged(qulonglong time);
    void screenBrightnessChanged(int brightness);
    void autoBrightnessChanged(bool autoBrightness);
    void maximumScreenBrightnessChanged(int maximumBrightness);
    void keyboardBrightnessChanged(int brightness);
    void maximumKeyboardBrightnessChanged(int maximumBrightness);
    void triggersLidActionChanged(bool triggers);
    void inhibitionsChanged(const QList<InhibitionInfo> &added, const QStringList &removed);
    void chargeStopThresholdChanged(int threshold);

    //[liubangguo]repower button
    void onRepowerButtonPressed();
    void onRepowerButtonReleased();
    void onRepowerButtonLongpressed();
    void DisplayPowerStateChange(int,int);
    void onBatteryPropertiesChanged(QString,QVariantMap,QStringList);
    void onPropertiesChanged(QString,QVariantMap,QStringList);
    void onQuickChargingTimerout();
    void onPluginTimerout();

private:
    void checkLowBattery(int percent,bool isCharging);
    bool isLowElectric();
    bool isQuickCharging();
    double getCurrentBatteryPrecent();
    void populateApplicationData(const QString &name, QString *prettyName, QString *icon);
    QString batteryType(const Solid::Battery *battery) const;
    QStringList basicSourceNames() const;
    QString batteryStateToString(int newState) const;

    QStringList m_sources;

    QHash<QString, QString> m_batterySources; // <udi, Battery0>
    QHash<QString, QPair<QString, QString>> m_applicationInfo; // <appname, <pretty name, icon>>

	SessionManagement *m_session;
    //[liubangguo]for repower button
    QTimer m_powerButtonTimer;
    QTimer m_quickChargingTimer;
    QTimer m_pluginTimer;
    //QFileSystemWatcher* m_watcher;
};

#endif
