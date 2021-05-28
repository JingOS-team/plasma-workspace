#ifndef TranslateTool_H
#define TranslateTool_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>


class TranslateTool : public QObject
{
    Q_OBJECT


public:
    explicit TranslateTool();
    ~TranslateTool(){};

    Q_INVOKABLE void confirmDlg(bool result);
    Q_INVOKABLE void restartDevice();


Q_SIGNALS:
    void dlgLanguageChanged(bool result);
};
#endif
