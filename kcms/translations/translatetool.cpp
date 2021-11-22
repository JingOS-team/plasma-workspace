/*
 * Copyright (C) 2021 Beijing Jingling Information System Technology Co., Ltd. All rights reserved.
 *
 * Authors:
 * Liu Bangguo <liubangguo@jingos.com>
 *
 */
#include "translatetool.h"

#include <QDebug>
#include <QProcess>

TranslateTool::TranslateTool() {

}

void TranslateTool::confirmDlg(bool result){
    emit dlgLanguageChanged(result);
}

void TranslateTool::restartDevice(){
    // QProcess::execute("reboot");
    m_session.requestReboot(SessionManagement::ConfirmationMode::Skip);
}