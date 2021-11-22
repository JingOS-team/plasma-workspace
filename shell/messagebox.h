/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */
#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QObject>
#include <QString>
#include <QUrl>

#include <KSharedConfig>
#include <KConfigGroup>

namespace KDeclarative
{
class QmlObjectSharedEngine;
}
namespace Plasma
{
}

class QTimer;
class ShellCorona;

class MessageBox : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.messageboxService")
public:
    MessageBox(const KSharedConfig::Ptr &config, ShellCorona *corona);
    ~MessageBox() override;

public Q_SLOTS:
    void showDialog(const QString &title, const QString &content);
    void hideDialog();

private:
    void init();


    KDeclarative::QmlObjectSharedEngine *m_qmlObject = nullptr;
    QUrl m_qmlUrl;
};

#endif
