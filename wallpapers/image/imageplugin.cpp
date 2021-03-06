/*
 *   Copyright 2013 by Marco Martin <mart@kde.org>
 *             2021 by Rui Wang <wangrui@jingos.com>
 * 
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
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

#include "imageplugin.h"
#include "image.h"
#include "wallpaper.h"

#include <QQmlContext>
#include <QQmlEngine>


static Wallpaper *wallpaper_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    qDebug() << " Wallpaper  Wallpaper_singletontype_provider ";
    return Wallpaper::instance();
}

void ImagePlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.wallpapers.image"));

    qmlRegisterType<Image>(uri, 2, 0, "Image");
    qmlRegisterType<QAbstractItemModel>();
    qmlRegisterSingletonType<Wallpaper>(uri, 2, 0, "Wallpaper", wallpaper_singletontype_provider);
}



