#include "translatetool.h"

#include <QDebug>
#include <QProcess>

TranslateTool::TranslateTool() {

}

void TranslateTool::confirmDlg(bool result){
    emit dlgLanguageChanged(result);
}

void TranslateTool::restartDevice(){
    QProcess::execute("reboot");
}