#ifndef UARTTHREAD_H
#define UARTTHREAD_H

#include <QThread>
#include <QDebug>

#include "posix_qextserialport.h"

class UartThread : public QThread
{
    Q_OBJECT
public:
    explicit UartThread(QObject *parent = 0);
    ~UartThread();

    QByteArray readUart();//读取串口数据
    void run();//串口线程函数，在此函数中循环读取串口数据
    void runControl(bool flag);//设置线程启停标志位
    void writeUart(const QByteArray &dataToWrite);//向串口发送数据
    void writeUart(const char *dataToWrite );//向串口发送数据

    //初始化串口
    bool initUart(const QString & name="/dev/ttySAC3", BaudRateType baudRate=BAUD9600, long delayTime=10, unsigned int timeOut=40);

    QByteArray outByt;

signals:
    void sendloopon(bool flag);
    void sendResData(QByteArray data);

private:
    Posix_QextSerialPort *mcom;//串口对象
    unsigned long mtimeOut;//线程读取串口的时间间隔
    bool mrunFlag;//线程启停标志位
};

#endif // UARTTHREAD_H
