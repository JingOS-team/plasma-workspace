/*
 *  Copyright 2014 (c) Martin Klapetek <mklapetek@kde.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "toast.h"
#include "shellcorona.h"

#include <QDBusConnection>
#include <QTimer>
#include <QWindow>
#include <QDebug>
#include <QUrl>

#include <Plasma/Package>
#include <KDeclarative/QmlObjectSharedEngine>
#include <klocalizedstring.h>

Toast::Toast(ShellCorona *corona)
    : QObject(corona)
    , m_toastUrl(corona->lookAndFeelPackage().fileUrl("toastmainscript"))
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    if(!connection.registerService(QStringLiteral("org.jingos.toast"))) {
        qWarning() << "Toast Failed to register service!"<< connection.lastError().message();
    }

    if(!connection.registerObject(QStringLiteral("/org/jingos/toast"), this, QDBusConnection::ExportAllSlots)) {
        qWarning() << "Toast Failed to register object!"<< connection.lastError().message();
    }
}

Toast::~Toast()
{
}

bool Toast::init()
{
    if (m_toastObject && m_toastObject->rootObject()) {
        return true;
    }

    if (m_toastUrl.isEmpty()) {
        return false;
    }

    if (!m_toastObject) {
        m_toastObject = new KDeclarative::QmlObjectSharedEngine(this);
    }

    m_toastObject->setSource(m_toastUrl);

    if (m_toastObject->status() != QQmlComponent::Ready) {
        qWarning() << "Failed to load Toast QML file" << m_toastUrl;
        return false;
    }

    m_timeout = m_toastObject->rootObject()->property("timeout").toInt();

    if (!m_toastTimer) {
        m_toastTimer = new QTimer(this);
        m_toastTimer->setSingleShot(true);
        m_toastTimer->setInterval(1000);
        connect(m_toastTimer, &QTimer::timeout, this, &Toast::hideToast);
    }

    return true;
}

void Toast::showText(const QString &text)
{
    if (!init()) {
        return;
    }

    auto *rootObject = m_toastObject->rootObject();
    rootObject->setProperty("toastContent", text);

    showToast();
}

void Toast::test()
{

}

void Toast::showToast()
{
    m_toastTimer->stop();

    auto *rootObject = m_toastObject->rootObject();

    // if our TOAST understands animating the opacity, do it;
    // otherwise just show it to not break existing lnf packages
    if (rootObject->property("animateOpacity").isValid()) {
        rootObject->setProperty("animateOpacity", false);
        rootObject->setProperty("opacity", 1);
        rootObject->setProperty("visible", true);
        rootObject->setProperty("animateOpacity", true);
        rootObject->setProperty("opacity", 0);
    } else {
        rootObject->setProperty("flags", Qt::Dialog);
        rootObject->setProperty("visible", true);
        rootObject->setProperty("display", true);
    }

    m_toastTimer->start();
}

void Toast::hideToast()
{
    auto *rootObject = m_toastObject->rootObject();
    if (!rootObject) {
        return;
    }

    rootObject->setProperty("display", false);
}
