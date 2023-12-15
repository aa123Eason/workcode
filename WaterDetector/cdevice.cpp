#include "cdevice.h"
#include "QDebug"


CDevice::CDevice(QString pType,QString pName,QWidget *parent) :
    QMainWindow(parent)
{
    // 根据当前的系统配置 进行成员map_DevFactors 初始化
    // jsonfile 填充 map_DevFactors
    set_name(pName);
    set_type(pType);

    m_TimeOut = 0;
    m_ProcessStatus = 0;

    CJsonFile jsonfile;
    set_protoname(jsonfile.get_proto_name(pType));

    // map_DevFactors 初始化
    map_DevFactors_init(pType);
}

CDevice::~CDevice()
{
    // 释放 成员map_DevFactors
    // qDebug() << "--> CDevice removed!!" << m_name;
    qDeleteAll(map_DevFactors);
}


// 单个因子的 增删改

void CDevice::map_DevFactors_init(QString pType)
{
    CJsonFile jsonfile;
    QJsonArray pArrayFactors = jsonfile.get_factors(pType);

    for (int i=0; i<pArrayFactors.size(); i++) {

        QString pFactorName, pFactorFunc, pFactorDataType, pFactorAddr, pFactorCode, pFactorByteOrder,pTagId,pProperty;

        QJsonObject pJsonFactor = pArrayFactors.at(i).toObject();
        if(pJsonFactor.contains(FACTOR_NAME)) pFactorName = pJsonFactor.value(FACTOR_NAME).toString();
        if(pJsonFactor.contains(FACTOR_FUNC)) pFactorFunc = pJsonFactor.value(FACTOR_FUNC).toString();
        if(pJsonFactor.contains(DATA_TYPE)) pFactorDataType = pJsonFactor.value(DATA_TYPE).toString();
        if(pJsonFactor.contains(FACTOR_CODE)) pFactorCode = pJsonFactor.value(FACTOR_CODE).toString();
        if(pJsonFactor.contains(FACTOR_ADDR)) pFactorAddr = pJsonFactor.value(FACTOR_ADDR).toString();
        if(pJsonFactor.contains(BYTE_ORDER)) pFactorByteOrder = pJsonFactor.value(BYTE_ORDER).toString();
        if(pJsonFactor.contains(FACTOR_TAG_ID)) pTagId = pJsonFactor.value(FACTOR_TAG_ID).toString();
        if(pJsonFactor.contains(FACTOR_PROPERTY)) pProperty = pJsonFactor.value(FACTOR_PROPERTY).toString();

        CFactor *pFactror = new CFactor();
        pFactror->setByteOrder(pFactorByteOrder);
        pFactror->setDataType(pFactorDataType);
        pFactror->setFactorCode(pFactorCode);
        pFactror->setFactorFunc(pFactorFunc);
        pFactror->setFactorName(pFactorName);
        pFactror->setRegisAddr(pFactorAddr);
        pFactror->setFactorData("-");
        pFactror->setTagId(pTagId);
        pFactror->setProperty(pProperty);

        map_DevFactors.insert(pFactorName,pFactror);
    }

    QLOG_TRACE() <<"map_DevFactors==>  "<< map_DevFactors;
}




