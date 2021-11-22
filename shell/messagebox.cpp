/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */
#include "messagebox.h"

#include "shellcorona.h"

#include <QDBusConnection>
#include <QDebug>
#include <QTimer>
#include <QWindow>
#include <QUrl>

#include <KDeclarative/QmlObjectSharedEngine>
#include <Plasma/Package>
#include <klocalizedstring.h>

MessageBox::MessageBox(const KSharedConfig::Ptr &config, ShellCorona *corona)
    : QObject(corona)
    , m_qmlUrl(corona->lookAndFeelPackage().fileUrl("messageboxmainscript"))
{
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/org/kde/messageboxService"),
                                                 this,
                                                 QDBusConnection::ExportAllSlots | QDBusConnection::ExportAllSignals);

    init();
}

MessageBox::~MessageBox()
{
}

void MessageBox::init()
{
    if (!m_qmlObject) {
        m_qmlObject = new KDeclarative::QmlObjectSharedEngine(this);
    }

    m_qmlObject->setSource(m_qmlUrl);

    if (m_qmlObject->status() != QQmlComponent::Ready) {
        qWarning() << "Failed to load messagebox QML file" ;
    }
}

void MessageBox::showDialog(const QString &title, const QString &text)
{
    auto *rootObject = m_qmlObject->rootObject();

    rootObject->setProperty("title", title);
    rootObject->setProperty("content", text);
    bool isVisible = rootObject->property("visible").toBool();
    if(isVisible == true)
    {
        rootObject->setProperty("visible", false);
    }

    rootObject->setProperty("visible", true);
}


void MessageBox::hideDialog()
{
    auto *rootObject = m_qmlObject->rootObject();
    bool isVisible = rootObject->property("visible").toBool();
    if(isVisible == true)
    {
        rootObject->setProperty("visible", false);
    }
}


