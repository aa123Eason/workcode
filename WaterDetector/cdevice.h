#ifndef CDEVICE_H
#define CDEVICE_H

#include <QMainWindow>
#include "QString"
#include "cfactor.h"
#include "QMap"
#include "cjsonfile.h"
#include "common.h"

class CFactor;

class CDevice : public QMainWindow
{
    Q_OBJECT

public:
    explicit CDevice(QString pType,QString pName,QWidget *parent = nullptr);
    ~CDevice();

    QString get_address(){return m_address;}
    QString get_name(){return m_name;}
    QString get_type(){return m_type;}
    QString get_protoname(){return m_protoname;}
    uint32_t get_timeout(){return m_TimeOut;}
    uint32_t get_processStatus(){return m_ProcessStatus;}
    bool getBit0(){return m_bit0;}
    bool getBit1(){return m_bit1;}
    bool getBit2(){return m_bit2;}
    bool getBit10(){return m_bit10;}
    bool getBit20(){return m_bit20;}
    bool getBit23(){return m_bit23;}

    bool getBitCommErr(){return m_bitCommErr;}

    void set_timeout(uint32_t temp){m_TimeOut = temp;}
    void set_processStatus(uint32_t temp){m_ProcessStatus = temp;}
    void set_address(QString address){m_address = address;}
    void set_name(QString name){m_name = name;}
    void set_type(QString type){m_type = type;}
    void set_protoname(QString protoname){m_protoname = protoname;}
    void map_DevFactors_init(QString pType);

    void setBit0(bool pTemp){m_bit0 = pTemp;}
    void setBit1(bool pTemp){m_bit1 = pTemp;}
    void setBit2(bool pTemp){m_bit2 = pTemp;}
    void setBit10(bool pTemp){m_bit10 = pTemp;}
    void setBit20(bool pTemp){m_bit20 = pTemp;}
    void setBit23(bool pTemp){m_bit23 = pTemp;}

    void setBitCommErr(bool pTemp){m_bitCommErr = pTemp;}

    QMap<QString, CFactor *> get_map_DevFactors(){return map_DevFactors;}

private:
    bool m_bit0;
    bool m_bit1;
    bool m_bit2;
    bool m_bit10;
    bool m_bit20;
    bool m_bit23;

    bool m_bitCommErr;
    /// bit0: 发送测量命令
    /// bit1: 设备异常
    /// bit2: 进入测量流程
    /// m_bit10:
    /// m_bit20:
    /// m_bit23:

    uint32_t m_TimeOut;
    uint16_t m_ProcessStatus;
    QString m_address;
    QString m_name;
    QString m_type;
    QString m_protoname;
    QMap<QString, CFactor *> map_DevFactors;
};

#endif // CDEVICE_H
