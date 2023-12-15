#include "cjsonfile.h"


CJsonFile::CJsonFile()
{

}

QJsonObject CJsonFile::get_root()
{
    QJsonObject jsonObject;
    QFile file(SYS_FILE);
    if (file.exists()){
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return jsonObject;
        }
        jsonObject= document.object();
        return jsonObject;
    }
    return jsonObject;
}

QJsonArray CJsonFile::get_level1_array(QString pTemp)
{
    QJsonArray jsonArray;
    QFile file(SYS_FILE);
    if (file.exists()){
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return jsonArray;
        }
        QJsonObject jsonObject= document.object();
        if(jsonObject.contains(pTemp))
        {
            jsonArray = jsonObject.value(pTemp).toArray();
        }
        return jsonArray;
    }
    return jsonArray;
}

QJsonArray CJsonFile::get_factors(QString pType)
{
    QJsonArray jsonObjFactors;
    QFile file(SYS_FILE);
    if (file.exists()){
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return jsonObjFactors;
        }
        QJsonObject jsonObject= document.object();
        if(jsonObject.contains(TEMPLATE))
        {
            QJsonArray jsonArrayProto = jsonObject.value(TEMPLATE).toArray();
            for(int i=0;i<jsonArrayProto.size();i++)
            {
                QJsonObject pJsonProto = jsonArrayProto.at(i).toObject();
                if(pJsonProto.contains(TEMPLATE_NAME))
                {
                    if(pType == pJsonProto.value(TEMPLATE_NAME).toString())
                    {
                        if(pJsonProto.contains(FACTORS))
                        {
                            jsonObjFactors = pJsonProto.value(FACTORS).toArray();
                            return jsonObjFactors;
                        }
                    }
                }
            }
        }
        return jsonObjFactors;
    }
    return jsonObjFactors;
}


QString CJsonFile::get_proto_name(QString pType)
{
    QString pStrProtoName;
    QFile file(SYS_FILE);
    if (file.exists()){
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return pStrProtoName;
        }
        QJsonObject jsonObject= document.object();
        if(jsonObject.contains(TEMPLATE))
        {
            QJsonArray jsonArrayProto = jsonObject.value(TEMPLATE).toArray();
            for(int i=0;i<jsonArrayProto.size();i++)
            {
                QJsonObject pJsonProto = jsonArrayProto.at(i).toObject();
                if(pJsonProto.contains(TEMPLATE_NAME))
                {
                    if(pType == pJsonProto.value(TEMPLATE_NAME).toString())
                    {
                        if(pJsonProto.contains(TEMPLATE_PROTO))
                        {
                            pStrProtoName = pJsonProto.value(TEMPLATE_PROTO).toString();
                            return pStrProtoName;
                        }
                    }
                }
            }
        }
    }
    return pStrProtoName;
}


bool CJsonFile::edit_devnode(QString pStrName, QString pStrAddr, QString pStrTips)
{
    QFile file(SYS_FILE);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return false;
        }

        QJsonObject jsonObject = document.object();
        QJsonValueRef RefVersion = jsonObject.find(DEVICE).value();
        QJsonArray pJsonArray = RefVersion.toArray();
        for(int i=0;i<pJsonArray.size();i++)
        {
            QJsonObject pJsonDev = pJsonArray.at(i).toObject();
            if(pJsonDev.contains(DEV_NAME))
            {
                if(pStrName == pJsonDev.value(DEV_NAME).toString())
                {
                    pJsonArray.removeAt(i);

                    QJsonObject pJsonDev;
                    pJsonDev.insert(DEV_NAME,pStrName);
                    pJsonDev.insert(DEV_ADDR,pStrAddr);
                    pJsonDev.insert(DEV_TEMPLATE,pStrTips);
                    pJsonArray.insert(i,pJsonDev);
                    RefVersion = pJsonArray;

                    file.open(QIODevice::WriteOnly | QIODevice::Text);
                    QJsonDocument jsonDoc;
                    jsonDoc.setObject(jsonObject);
                    file.write(jsonDoc.toJson());
                    file.close();
                    return true;
                }
            }
        }
    }
    return false;
}

bool CJsonFile::insert_devnode(QString pStrName, QString pStrAddr, QString pStrTips)
{
    QFile file(SYS_FILE);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return false;
        }

        QJsonObject jsonObject = document.object();
        if(jsonObject.contains(DEVICE))
        {
            QJsonValueRef RefVersion = jsonObject.find(DEVICE).value();
            QJsonArray pJsonArray = RefVersion.toArray();   // FIXME: 系统文件中若不存在该节点，会发生段错误

            QJsonObject pJsonDev;
            pJsonDev.insert(DEV_NAME,pStrName);
            pJsonDev.insert(DEV_ADDR,pStrAddr);
            pJsonDev.insert(DEV_TEMPLATE,pStrTips);
            pJsonArray.insert(pJsonArray.size(),pJsonDev);
            RefVersion = pJsonArray;

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
        else {
            QJsonArray pJsonDevArray;
            QJsonObject pJsonDev;
            pJsonDev.insert(DEV_NAME,pStrName);
            pJsonDev.insert(DEV_ADDR,pStrAddr);
            pJsonDev.insert(DEV_TEMPLATE,pStrTips);
            pJsonDevArray.insert(0,pJsonDev);
            jsonObject.insert(DEVICE,pJsonDevArray);

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
    }
    return false;
}


bool CJsonFile::remove_devnode(QString pStrName)
{
    QJsonObject jsonObject;
    QFile file(SYS_FILE);
    if (file.exists()){
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return false;
        }
        QJsonObject jsonObject = document.object();
        QJsonValueRef RefVersionlist = jsonObject.find(DEVICE).value();
        QJsonArray jsonObjDevList = RefVersionlist.toArray();

        int pCnt = jsonObjDevList.size();
        for(int i=0; i<pCnt; i++)
        {
            QJsonObject pJsonDev = jsonObjDevList.at(i).toObject();
            if(pJsonDev.contains(DEV_NAME))
            {
                if(pStrName == pJsonDev.value(DEV_NAME).toString())
                {
                    jsonObjDevList.removeAt(i);

                    RefVersionlist = jsonObjDevList;
                    file.open(QIODevice::WriteOnly | QIODevice::Text);
                    QJsonDocument jsonDoc;
                    jsonDoc.setObject(jsonObject);
                    file.write(jsonDoc.toJson());
                    file.close();

                    return true;
                }
            }
        }
    }
    return false;
}

bool CJsonFile::remove_uploadnode(QString pServerAddr,int &index)
{
    QJsonObject jsonObject;
    QFile file(SYS_FILE);
    if (file.exists()){
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return false;
        }
        QJsonObject jsonObject = document.object();
        QJsonValueRef RefVersionlist = jsonObject.find(UPLOAD).value();
        QJsonArray jsonObjUpList = RefVersionlist.toArray();

        int pCnt = jsonObjUpList.size();
        for(int i=0; i<pCnt; i++)
        {
            QJsonObject pJsonUp = jsonObjUpList.at(i).toObject();
            if(pJsonUp.contains(UPLOAD_IP) && pJsonUp.contains(UPLOAD_PORT))
            {
                QString pTemp = pJsonUp.value(UPLOAD_IP).toString() + ":" + pJsonUp.value(UPLOAD_PORT).toString();
                if(pTemp == pServerAddr)
                {
                    index = i;
                    jsonObjUpList.removeAt(i);
                    RefVersionlist = jsonObjUpList;
                    file.open(QIODevice::WriteOnly | QIODevice::Text);
                    QJsonDocument jsonDoc;
                    jsonDoc.setObject(jsonObject);
                    file.write(jsonDoc.toJson());
                    file.close();

                    return true;
                }
            }
        }
    }
    return false;
}



//// 判断文件是否存在 判断结果通过返回值来通知调用方
void CJsonFile::jsonfile_serial(QString port,QString baud,QString databit,QString stopbit,QString parity)
{

    QFile file(SYS_FILE);

    if (file.exists()){
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QMessageBox::about(NULL, "提示", "<font color='black'>系统文件格式错误！</font>");
            return;
        }
        QJsonObject jsonObject = document.object();
        QJsonObject::iterator jsonDevListIt = jsonObject.find(SERIAL);
        if(jsonObject.end() == jsonDevListIt)
        {
            // 不存在 serial 节点
            QJsonObject jsonObjectSer;
            jsonObjectSer.insert(SERIAL_NAME, port);
            jsonObjectSer.insert(SERIAL_PARITY, parity);
            jsonObjectSer.insert(SERIAL_STOPBIT, stopbit);
            jsonObjectSer.insert(SERIAL_DATABIT, databit);
            jsonObjectSer.insert(SERIAL_BAUDRATE, baud);
            jsonObject.insert(SERIAL,jsonObjectSer);

            // 使用QJsonDocument设置该json对象
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();
        }
        else
        {
            QJsonValueRef RefVersion = jsonObject.find(SERIAL).value();
            QJsonObject jsonObjectNode = RefVersion.toObject();

            QJsonValueRef ElementOneValueRefName = jsonObjectNode.find(SERIAL_NAME).value();
            ElementOneValueRefName = QJsonValue(port);

            QJsonValueRef ElementOneValueRefParity = jsonObjectNode.find(SERIAL_PARITY).value();
            ElementOneValueRefParity = QJsonValue(parity);

            QJsonValueRef ElementOneValueRefSB = jsonObjectNode.find(SERIAL_STOPBIT).value();
            ElementOneValueRefSB = QJsonValue(stopbit);

            QJsonValueRef ElementOneValueRefDB = jsonObjectNode.find(SERIAL_DATABIT).value();
            ElementOneValueRefDB = QJsonValue(databit);

            QJsonValueRef ElementOneValueRefBaud = jsonObjectNode.find(SERIAL_BAUDRATE).value();
            ElementOneValueRefBaud = QJsonValue(baud);

            RefVersion = jsonObjectNode;
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();
        }
    }
    else
    {
        if(!file.open(QIODevice::ReadWrite)) {
            QMessageBox::about(NULL, "提示", "<font color='black'>创建系统文件失败！</font>");
        } else {
            QJsonObject jsonObject;
            QJsonObject jsonObjectSer;
            jsonObjectSer.insert(SERIAL_NAME, port);
            jsonObjectSer.insert(SERIAL_PARITY, parity);
            jsonObjectSer.insert(SERIAL_STOPBIT, stopbit);
            jsonObjectSer.insert(SERIAL_DATABIT, databit);
            jsonObjectSer.insert(SERIAL_BAUDRATE, baud);

            jsonObjectSer.insert(SERIAL, jsonObjectSer);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();

        }
    }
    QMessageBox::about(NULL, "提示", "<font color='black'>保存串口配置成功，重启生效！</font>");

    return;
}

void CJsonFile::remove_protonode(QString proto)
{
    QJsonObject jsonObject;
    QFile file(SYS_FILE);
    if (file.exists()){
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return ;
        }
        QJsonObject jsonObject = document.object();
        QJsonValueRef RefVersionlist = jsonObject.find(TEMPLATE).value();
        QJsonArray jsonObjProtoList = RefVersionlist.toArray();

        int pCnt = jsonObjProtoList.size();
        for(int i=0; i<pCnt; i++)
        {
            QJsonObject pJsonDev = jsonObjProtoList.at(i).toObject();
            if(pJsonDev.contains(TEMPLATE_NAME))
            {
                if(proto == pJsonDev.value(TEMPLATE_NAME).toString())
                {
                    jsonObjProtoList.removeAt(i);

                    RefVersionlist = jsonObjProtoList;
                    file.open(QIODevice::WriteOnly | QIODevice::Text);
                    QJsonDocument jsonDoc;
                    jsonDoc.setObject(jsonObject);
                    file.write(jsonDoc.toJson());
                    file.close();

                    return;
                }
            }
        }
    }
}

bool CJsonFile::insert_protonode(QString protoName, QString protoType)
{
    QFile file(SYS_FILE);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return false;
        }

        QJsonObject jsonObject = document.object();
        if(jsonObject.contains(TEMPLATE))
        {
            QJsonValueRef RefVersion = jsonObject.find(TEMPLATE).value();
            QJsonArray pJsonArray = RefVersion.toArray();

            QJsonObject pJsonProto;
            QJsonArray pJsonTempArray;
            pJsonProto.insert(TEMPLATE_NAME,protoName);
            pJsonProto.insert(TEMPLATE_PROTO,protoType);
            pJsonProto.insert(FACTORS,pJsonTempArray);
            pJsonArray.insert(pJsonArray.size(),pJsonProto);
            RefVersion = pJsonArray;

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
        else
        {
            QJsonArray pJsonArray;
            QJsonObject pJsonProto;
            QJsonArray pJsonTempArray;
            pJsonProto.insert(TEMPLATE_NAME,protoName);
            pJsonProto.insert(TEMPLATE_PROTO,protoType);
            pJsonProto.insert(FACTORS,pJsonTempArray);
            pJsonArray.insert(pJsonArray.size(),pJsonProto);
            jsonObject.insert(TEMPLATE,pJsonArray);
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
    }
    return false;
}

void CJsonFile::remove_factornode(QString protoStrlist)
{
    if(!protoStrlist.contains("+"))
    {
        return;
    }
    QStringList protoList = protoStrlist.split("+");
    if(protoList.size() != 2)
    {
        return;
    }

    QFile file(SYS_FILE);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return ;
        }

        QJsonObject jsonObject = document.object();
        QJsonValueRef RefVersion = jsonObject.find(TEMPLATE).value();
        QJsonArray pJsonArray = RefVersion.toArray();

        QJsonArray::iterator ArrayIterator = pJsonArray.begin();

        for(;ArrayIterator != pJsonArray.end(); ArrayIterator++)
        {
            QJsonValueRef ElementOneValueRef = ArrayIterator[0];
            QJsonObject pJsonProtoObj = ElementOneValueRef.toObject();
            if(pJsonProtoObj.contains(TEMPLATE_NAME))
            {
                QString protoName = pJsonProtoObj.value(TEMPLATE_NAME).toString();
                if(protoName == protoList[0])
                {
                    QJsonValueRef RefVersionFaList = pJsonProtoObj.find(FACTORS).value();
                    QJsonArray jsonObjectFalist = RefVersionFaList.toArray();
                    QJsonArray::iterator ArrayIteratorFa = jsonObjectFalist.begin();
                    for(;ArrayIteratorFa != jsonObjectFalist.end(); ArrayIteratorFa++)
                    {
                        //factor_code
                        QJsonValueRef ElementOneValueRefFa = ArrayIteratorFa[0];
                        QJsonObject pJsonFactorObj = ElementOneValueRefFa.toObject();
                        if(protoList[1] == pJsonFactorObj.value(FACTOR_CODE).toString())
                        {
                            jsonObjectFalist.erase(ArrayIteratorFa);
                            RefVersionFaList = jsonObjectFalist;
                            ElementOneValueRef = pJsonProtoObj;
                            RefVersion = pJsonArray;

                            if(file.open(QIODevice::WriteOnly | QIODevice::Text))
                            {
                                qDebug("系统配置文件为只读文件!");
                            }
                            else
                            {
                                qDebug("failed!");
                                QMessageBox::about(NULL, "提示", "<font color='black'>删除因子配置失败！</font>");
                            }

                            QJsonDocument jsonDoc;
                            jsonDoc.setObject(jsonObject);
                            file.write(jsonDoc.toJson());
                            file.close();
                            QMessageBox::about(NULL, "提示", "<font color='black'>删除因子配置成功！</font>");
                            return;
                        }
                    }
                }
            }
        }
    }
    return;
}


void CJsonFile::edit_factornode(QString pTempProtoName, CFactor &pfactor)
{
    QFile file(SYS_FILE);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return ;
        }

        QJsonObject jsonObject = document.object();
        QJsonValueRef RefVersion = jsonObject.find(TEMPLATE).value();
        QJsonArray pJsonArray = RefVersion.toArray();

        QJsonArray::iterator ArrayIterator = pJsonArray.begin();

        for(;ArrayIterator != pJsonArray.end(); ArrayIterator++)
        {
            QJsonValueRef ElementOneValueRef = ArrayIterator[0];
            QJsonObject pJsonProtoObj = ElementOneValueRef.toObject();
            if(pJsonProtoObj.contains(TEMPLATE_NAME))
            {
                QString protoName = pJsonProtoObj.value(TEMPLATE_NAME).toString();
                if(protoName == pTempProtoName)
                {
                    QJsonValueRef RefVersionFaList = pJsonProtoObj.find(FACTORS).value();
                    QJsonArray jsonObjectFalist = RefVersionFaList.toArray();
                    QJsonArray::iterator ArrayIteratorFa = jsonObjectFalist.begin();
                    for(;ArrayIteratorFa != jsonObjectFalist.end(); ArrayIteratorFa++)
                    {
                        //factor_code
                        QJsonValueRef ElementOneValueRefFa = ArrayIteratorFa[0];
                        QJsonObject pJsonFactorObj = ElementOneValueRefFa.toObject();

                        if(pJsonFactorObj.contains(FACTOR_CODE))
                        {
                            QString pFactCode = pJsonFactorObj.value(FACTOR_CODE).toString();
                            if(pfactor.getFactorCode() == pFactCode)
                            {
                                if(pJsonFactorObj.contains(BYTE_ORDER))
                                {
                                    QJsonValueRef ElementOneValueRefBO = pJsonFactorObj.find(BYTE_ORDER).value();
                                    ElementOneValueRefBO = QJsonValue(pfactor.getByteOrder());
                                }
                                if(pJsonFactorObj.contains(DATA_TYPE))
                                {
                                    QJsonValueRef ElementOneValueRDT = pJsonFactorObj.find(DATA_TYPE).value();
                                    ElementOneValueRDT = QJsonValue(pfactor.getDataType());
                                }
                                if(pJsonFactorObj.contains(FACTOR_FUNC))
                                {
                                    QJsonValueRef ElementOneValueRefFF = pJsonFactorObj.find(FACTOR_FUNC).value();
                                    ElementOneValueRefFF = QJsonValue(pfactor.getFactorFunc());
                                }
                                if(pJsonFactorObj.contains(FACTOR_NAME))
                                {
                                    QJsonValueRef ElementOneValueRefNAME = pJsonFactorObj.find(FACTOR_NAME).value();
                                    ElementOneValueRefNAME = QJsonValue(pfactor.getFactorName());
                                }
                                if(pJsonFactorObj.contains(FACTOR_ADDR))
                                {
                                    QJsonValueRef ElementOneValueRefFA = pJsonFactorObj.find(FACTOR_ADDR).value();
                                    ElementOneValueRefFA = QJsonValue(pfactor.getRegisAddr());
                                }
                                if(pJsonFactorObj.contains(FACTOR_TAG_ID))
                                {
                                    QJsonValueRef ElementOneValueRefTI = pJsonFactorObj.find(FACTOR_TAG_ID).value();
                                    ElementOneValueRefTI = QJsonValue(pfactor.getTagId());
                                }
                                if(pJsonFactorObj.contains(FACTOR_PROPERTY))
                                {
                                    QJsonValueRef ElementOneValueRefProp = pJsonFactorObj.find(FACTOR_PROPERTY).value();
                                    ElementOneValueRefProp = QJsonValue(pfactor.getProperty());
                                }

                                ElementOneValueRefFa = pJsonFactorObj;
                                RefVersionFaList = jsonObjectFalist;
                                ElementOneValueRef = pJsonProtoObj;
                                RefVersion = pJsonArray;

                                file.open(QIODevice::WriteOnly | QIODevice::Text);
                                QJsonDocument jsonDoc;
                                jsonDoc.setObject(jsonObject);
                                file.write(jsonDoc.toJson());
                                file.close();
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
    return;
}



void CJsonFile::insert_factornode(QString pTempProtoName, CFactor &pfactor)
{
    QFile file(SYS_FILE);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return ;
        }

        QJsonObject jsonObject = document.object();
        QJsonValueRef RefVersion = jsonObject.find(TEMPLATE).value();
        QJsonArray pJsonArray = RefVersion.toArray();

        QJsonArray::iterator ArrayIterator = pJsonArray.begin();

        for(;ArrayIterator != pJsonArray.end(); ArrayIterator++)
        {
            QJsonValueRef ElementOneValueRef = ArrayIterator[0];
            QJsonObject pJsonProtoObj = ElementOneValueRef.toObject();
            if(pJsonProtoObj.contains(TEMPLATE_NAME))
            {
                QString protoName = pJsonProtoObj.value(TEMPLATE_NAME).toString();
                if(protoName == pTempProtoName)
                {
                    QJsonValueRef RefVersionFaList = pJsonProtoObj.find(FACTORS).value();
                    QJsonArray jsonObjectFalist = RefVersionFaList.toArray();

                    QJsonObject pJsonFaObj;
                    pJsonFaObj.insert(BYTE_ORDER,pfactor.getByteOrder());
                    pJsonFaObj.insert(FACTOR_ADDR,pfactor.getRegisAddr());
                    pJsonFaObj.insert(FACTOR_CODE,pfactor.getFactorCode());
                    pJsonFaObj.insert(FACTOR_NAME,pfactor.getFactorName());
                    pJsonFaObj.insert(DATA_TYPE,pfactor.getDataType());
                    pJsonFaObj.insert(FACTOR_FUNC,pfactor.getFactorFunc());
                    pJsonFaObj.insert(FACTOR_TAG_ID,pfactor.getTagId());
                    pJsonFaObj.insert(FACTOR_PROPERTY,pfactor.getProperty());

                    jsonObjectFalist.insert(jsonObjectFalist.size(),pJsonFaObj);

                    RefVersionFaList = jsonObjectFalist;
                    ElementOneValueRef = pJsonProtoObj;
                    RefVersion = pJsonArray;

                    file.open(QIODevice::WriteOnly | QIODevice::Text);
                    QJsonDocument jsonDoc;
                    jsonDoc.setObject(jsonObject);
                    file.write(jsonDoc.toJson());
                    file.close();

                    QMessageBox::about(NULL, "提示", "<font color='black'>新增因子配置成功！</font>");
                    return;

                }
            }
        }
    }
    return;
}

void CJsonFile::jsonfile_create()
{
    QFile file(SYS_FILE);
    if (!file.exists()){
        if(!file.open(QIODevice::ReadWrite)) {
            QMessageBox::about(NULL, "提示", "<font color='black'>创建系统文件失败！</font>");
        } else {
            QJsonObject jsonObject;
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();
        }
    }
}

bool CJsonFile::insert_uploadnode(CUploadSetting &pUpSetting)
{
    QFile file(SYS_FILE);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return false;
        }

        QJsonObject jsonObject = document.object();
        if(jsonObject.contains(UPLOAD))
        {
            QJsonValueRef RefVersion = jsonObject.find(UPLOAD).value();
            QJsonArray pJsonArray = RefVersion.toArray();

            QJsonObject pJsonUpload;
            pJsonUpload.insert(UPLOAD_MN,pUpSetting.get_mn());
            pJsonUpload.insert(UPLOAD_IP,pUpSetting.get_ip());
            pJsonUpload.insert(UPLOAD_PORT,pUpSetting.get_port());
            pJsonUpload.insert(UPLOAD_PASSWORD,pUpSetting.get_passwd());
            pJsonUpload.insert(UPLOAD_RETRY,pUpSetting.get_retry());
            pJsonUpload.insert(UPLOAD_TIMEOUT,pUpSetting.get_timeout());
            pJsonUpload.insert(UPLOAD_MIN_INTERNAL,pUpSetting.get_min_internal());
            pJsonUpload.insert(UPLOAD_INTERNAL,pUpSetting.get_upload_internal());
            pJsonUpload.insert(UPLOAD_HEARTBREAK,pUpSetting.get_heartbreak());
            pJsonUpload.insert(UPLOAD_RT_UPLOAD,pUpSetting.get_rt_upload());

            pJsonArray.insert(pJsonArray.size(),pJsonUpload);
            RefVersion = pJsonArray;

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
        else
        {
            QJsonArray pJsonArray;
            QJsonObject pJsonUpload;
            pJsonUpload.insert(UPLOAD_MN,pUpSetting.get_mn());
            pJsonUpload.insert(UPLOAD_IP,pUpSetting.get_ip());
            pJsonUpload.insert(UPLOAD_PORT,pUpSetting.get_port());
            pJsonUpload.insert(UPLOAD_PASSWORD,pUpSetting.get_passwd());
            pJsonUpload.insert(UPLOAD_RETRY,pUpSetting.get_retry());
            pJsonUpload.insert(UPLOAD_TIMEOUT,pUpSetting.get_timeout());
            pJsonUpload.insert(UPLOAD_MIN_INTERNAL,pUpSetting.get_min_internal());
            pJsonUpload.insert(UPLOAD_INTERNAL,pUpSetting.get_upload_internal());
            pJsonUpload.insert(UPLOAD_HEARTBREAK,pUpSetting.get_heartbreak());
            pJsonUpload.insert(UPLOAD_RT_UPLOAD,pUpSetting.get_rt_upload());

            pJsonArray.insert(pJsonArray.size(),pJsonUpload);
            jsonObject.insert(UPLOAD,pJsonArray);
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
    }
    return false;
}

bool CJsonFile::update_KeyValue(QString pTemp,QString Key)
{
    QFile file(SYS_FILE);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            qDebug() << "Err: system.json bad format";
            return false;
        }

        QJsonObject jsonObject = document.object();
        if(jsonObject.contains(Key))
        {
            QJsonValueRef ElementOneValueRef = jsonObject.find(Key).value();
            ElementOneValueRef = QJsonValue(pTemp);

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
        else {
            jsonObject.insert(Key,pTemp);

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
    }
    return false;
}
