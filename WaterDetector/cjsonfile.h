#ifndef CJSONFILE_H
#define CJSONFILE_H

#include "common.h"
#include "cfactor.h"
#include "cuploadsetting.h"
#include "mainwindow.h"


class CCustomSerial;

class CJsonFile
{
public:
    CJsonFile();

    // judge if file exists
    void jsonfile_create();
    QJsonObject get_root();
    bool remove_devnode(QString pStrName);
    bool edit_devnode(QString pStrName, QString pStrAddr, QString pStrTips);
    bool insert_devnode(QString pStrName, QString pStrAddr, QString pStrTips);
    void jsonfile_serial(QString port,QString baud,QString databit,QString stopbit,QString parity);
    void remove_protonode(QString proto);
    void remove_factornode(QString protoStrlist);
    bool insert_protonode(QString protoname, QString prototype);
    void edit_factornode(QString pTempProtoName, CFactor &pfactor);
    void insert_factornode(QString pTempProtoName, CFactor &pfactor);
    QJsonArray get_factors(QString pType);
    QString get_proto_name(QString pType);
    bool insert_uploadnode(CUploadSetting &pUpSetting);
    bool remove_uploadnode(QString pServerAddr,int &index);
    QJsonArray get_level1_array(QString pTemp);
    bool update_KeyValue(QString pTemp,QString Key);
};

#endif // CJSONFILE_H
