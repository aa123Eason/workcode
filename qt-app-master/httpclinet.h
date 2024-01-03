#ifndef HTTPCLINET_H
#define HTTPCLINET_H

#include "common.h"

class httpclinet
{
public:
    httpclinet();

    bool get(QString api,QJsonObject &pJsonObj);
    bool get(QString api,QJsonArray &pJsonArray);
    bool post(QString api,QJsonObject &pJsonObj);
    bool post(QString api,QJsonObject &pJsonObj,QJsonObject &pJsonReply);
    bool put(QString api,QJsonObject &pJsonObj);
    bool deleteSource(QString api,QString item);
};

#endif // HTTPCLINET_H
