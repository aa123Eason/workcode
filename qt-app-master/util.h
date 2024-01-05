#ifndef UTIL_H
#define UTIL_H

#include "common.h"
#include "httpclinet.h"
#include <QMap>
#include <QString>
#include <QStringList>

class Util
{
public:
    Util();

    QString Uart_Convert(QString comboxTex);
    QString Uart_Revert(QString pTex);
    QMap<QString,QStringList> Uart_devicetype();
    QMap<QString,QString> Uart_devicetypeNameMatch();

};

#endif // UTIL_H
