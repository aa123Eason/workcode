#ifndef CFACTOR_H
#define CFACTOR_H

#include "QString"

class CFactor
{
public:
    CFactor();
    ~CFactor();

    QString getFactorCode(){return m_FactorCode;}
    QString getFactorName(){return m_FactorName;}
    QString getFactorFunc(){return m_FactorFunc;}
    QString getDataType(){return m_DataType;}
    QString getByteOrder(){return m_ByteOrder;}
    QString getRegisAddr(){return m_RegisAddr;}
    QString getFactorData(){return m_FactorData;}
    QString getDateTime(){return m_DateTime;}
    QString getTagId(){return m_TagId;}
    QString getFlag(){return m_Flag;}

    QString getProperty(){return m_Property;}

    void setFactorCode(QString pTemp){m_FactorCode = pTemp;}
    void setFactorName(QString pTemp){m_FactorName = pTemp;}
    void setFactorFunc(QString pTemp){m_FactorFunc = pTemp;}
    void setDataType(QString pTemp){m_DataType = pTemp;}
    void setByteOrder(QString pTemp){m_ByteOrder = pTemp;}
    void setRegisAddr(QString pTemp){m_RegisAddr = pTemp;}
    void setFactorData(QString pTemp){m_FactorData = pTemp;}
    void setDateTime(QString pTemp){m_DateTime = pTemp;}
    void setTagId(QString pTemp){m_TagId = pTemp;}
    void setFlag(QString pTemp){m_Flag = pTemp;}

    void setProperty(QString pTemp){m_Property = pTemp;}

private:
    QString m_FactorCode;
    QString m_FactorName;
    QString m_FactorFunc;
    QString m_DataType;
    QString m_ByteOrder;
    QString m_RegisAddr;
    QString m_FactorData;
    QString m_DateTime;
    QString m_TagId;
    QString m_Flag;

    QString m_Property;    // 因子属性： 0 - 测量值（默认）；1-ProcessStatus； 2-告警？
};

#endif // CFACTOR_H
