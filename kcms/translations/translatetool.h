/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */
#ifndef TranslateTool_H
#define TranslateTool_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include <sessionmanagement.h>

class TranslateTool : public QObject
{
    Q_OBJECT


public:
    explicit TranslateTool();
    ~TranslateTool(){};

    Q_INVOKABLE void confirmDlg(bool result);
    Q_INVOKABLE void restartDevice();
private:
    SessionManagement m_session;

Q_SIGNALS:
    void dlgLanguageChanged(bool result);
};
#endif
