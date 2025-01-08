//
// Created by 58226 on 2025/1/2.
//

#ifndef GLOBALSIGNAL_H
#define GLOBALSIGNAL_H

#include <QObject>
#include "config.h"
class GlobalSignal:public QObject
{
    Q_OBJECT
    public:
    static GlobalSignal *instance();
public:
    signals:
    void requestOpenFullScreenWindow();
    void requestCloseFullScreenWindow();
    void requestOpenSettingWindow();
    void requestCloseSettingWindow();
    void requestQuit();
    void requestShowMessage(const QString& message);
    void requestModifyConfig(ConfigType type,const QVariant& data);
private:
    explicit GlobalSignal(QObject *parent = nullptr):QObject(parent){}
    static GlobalSignal s_signalProxy;
};


#endif //GLOBALSIGNAL_H
