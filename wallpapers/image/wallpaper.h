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

#ifndef WALLPAPER
#define WALLPAPER

#include <QObject>

class Wallpaper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString launcherWallpaper READ launcherWallpaper WRITE setLauncherWallpaper NOTIFY launcherWallpaperChanged)
    Q_PROPERTY(QString lockscreenWallpaper READ lockscreenWallpaper WRITE setLockscreenWallpaper NOTIFY lockscreenWallpaperChanged)

public:
    static Wallpaper *instance();

    QString launcherWallpaper();
    QString lockscreenWallpaper();

public slots:
    QString setLauncherWallpaper(QString path);
    QString setLockscreenWallpaper(QString path);

Q_SIGNALS:
    void launcherWallpaperChanged(QString path);
    void lockscreenWallpaperChanged(QString path);

private:
    explicit Wallpaper(QObject* parent = nullptr);
    ~Wallpaper() override;

    static Wallpaper* m_instance;
    QString m_launcherWallpaper;
    QString m_lockscreenWallpaper;
};

#endif
