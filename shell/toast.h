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

#ifndef TOAST_H
#define TOAST_H

#include <QObject>
#include <QString>
#include <QUrl>

namespace KDeclarative {
    class QmlObjectSharedEngine;
}
namespace Plasma {
}

class QTimer;
class ShellCorona;

class Toast : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.jingos.toast")
public:
    Toast(ShellCorona *corona);
    ~Toast() override;

public Q_SLOTS:
     void showText(const QString &text);
     void test();

    void hideToast();

private:
    bool init();

    void showProgress(const QString &icon, const int percent, const int maximumPercent, const QString &additionalText = QString());
    void showToast();

    QUrl m_toastUrl;
    KDeclarative::QmlObjectSharedEngine *m_toastObject = nullptr;
    QTimer *m_toastTimer = nullptr;
    int m_timeout = 0;
};

#endif // TOAST_H
