/*
 *  Copyright 2021 Rui Wang <wangrui@jingos.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

#include "wallpaper.h"
#include <QDebug>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusServer>
#include <KConfigGroup>
#include <KSharedConfig>
#include <QDBusConnection>

const QString SERVICE_NAME =  "org.jing.systemui.wallpaper";
const QString SERVICE_PATH =   "/jing/systemui/wallpaper";
const QString SERVICE_INTERFACE =  "org.jing.systemui.wallpaper";

Wallpaper *Wallpaper::m_instance = nullptr;

Wallpaper::Wallpaper(QObject *parent)
    : QObject(parent)
{
    if ( m_instance != nullptr )  
        return;

    auto kdeglobals = KSharedConfig::openConfig("kdeglobals");
    KConfigGroup cfg(kdeglobals, "Wallpapers");
    m_launcherWallpaper = cfg.readEntry("launcherWallpaper", QString());
    if(m_launcherWallpaper.isEmpty() || m_launcherWallpaper.isNull()) {
        m_launcherWallpaper = cfg.readEntry("defaultLauncherWallpaper", QString());
    }

    m_lockscreenWallpaper = cfg.readEntry("lockscreenWallpaper", QString());
    if(m_lockscreenWallpaper.isEmpty() || m_lockscreenWallpaper.isNull()) {
        m_lockscreenWallpaper = cfg.readEntry("defaultLockScreenWallpaper", QString());
    }

    QDBusConnection::sessionBus().connect(SERVICE_NAME,
                                          SERVICE_PATH,
                                          SERVICE_INTERFACE,
                                         "launcherWallpaperChanged",
                                         this,
                                         SLOT(setLauncherWallpaper(QString)));

    QDBusConnection::sessionBus().connect(SERVICE_NAME,
                                          SERVICE_PATH,
                                          SERVICE_INTERFACE,
                                         "lockscreenWallpaperChanged",
                                         this,
                                         SLOT(setLockscreenWallpaper(QString)));
}

Wallpaper* Wallpaper::instance()
{
    if ( m_instance == nullptr )  
        m_instance = new Wallpaper();
    return m_instance;
}

Wallpaper::~Wallpaper()
{

}

QString Wallpaper::launcherWallpaper()
{
    return m_launcherWallpaper;
}
QString Wallpaper::setLauncherWallpaper(QString path)
{
    m_launcherWallpaper = path;
    emit launcherWallpaperChanged(path);
    return path;
}

QString Wallpaper::lockscreenWallpaper()
{
    return m_lockscreenWallpaper;
}

QString Wallpaper::setLockscreenWallpaper(QString path)
{
    m_lockscreenWallpaper = path;
    emit lockscreenWallpaperChanged(path);
    return path;
}
