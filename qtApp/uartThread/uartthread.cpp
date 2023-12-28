#include "uartthread.h"

UartThread::UartThread(QObject *parent) :
    QThread(parent)
{
    connect(this,&UartThread::sendloopon,this,[=](bool flag){
        if(flag)
        {
            qDebug()<<"Thread Start!"<<endl;
            this->quit();
            this->start();
        }
        else
        {
            qDebug()<<"Thread Exit!"<<endl;
            this->quit();
        }
    });
}

UartThread::~UartThread()
{
    this->runControl(false);
    this->quit();
    this->wait();

    delete this->mcom;
}

//初始化串口
/*
 * 参数说明：
 * name：串口名
 * baudrate：波特率
 * delayTime：延时时间
 * timeOut：线程读取串口数据的时间间隔
 */
bool UartThread::initUart(const QString & name, BaudRateType baudRate, long delayTime, unsigned int timeOut)
{
    this->mcom = new Posix_QextSerialPort(name, QextSerialBase::Polling);//定义串口对象，指定串口名和查询模式，这里使用Polling

    if(!this->mcom ->open(QIODevice::ReadWrite|QIODevice::Text))//以读写方式打开串口
            return false;

    this->mcom->setBaudRate(baudRate);//波特率设置

    this->mcom->setDataBits(DATA_8);//数据位设置，我们设置为8位数据位

    this->mcom->setParity(PAR_NONE);//奇偶校验设置，我们设置为无校验

    this->mcom->setStopBits(STOP_1);//停止位设置，我们设置为1位停止位

    this->mcom->setFlowControl(FLOW_OFF);//数据流控制设置，我们设置为无数据流控制

    this->mcom->setTimeout(delayTime); //延时设置

    //connect(this->mcom,SIGNAL(readyRead()),this,SLOT(readComSlot()));//信号和槽函数关联，当串口缓冲区有数据时，进行读串口操作(事件驱动EventDriven模式)

    this->mtimeOut = timeOut;

    this->mrunFlag = false;

    printf("Initialise serial port %s(DAM8081) successfully.\n", name.toLatin1().data());

    return true;
}

//读取串口数据
QByteArray UartThread::readUart()
{
//    if(this->mcom->bytesAvailable() >= 10)  //如果可用数据大于或等于10字节再读取
//    {
        int num = this->mcom->bytesAvailable();//读取读取到的数据的字节数

        if(num>0)
        {
//            qDebug() << "old read: "<<num<<"bytes";
            this->mcom->flush();
            msleep(3000);
        }
        QByteArray readData;
        readData.append(this->mcom->readAll());//存储读取到的数据
        if(readData.length()>0)
        {
            qDebug()<<__LINE__<<"new read"<<readData.length()<<readData.toHex(' ');
            emit sendResData(readData);
        }

        return readData;
//   }
}

//循环读取串口数据的线程
void UartThread::run()
{
    while(this->mrunFlag)
    {
        outByt = this->readUart();
        msleep(this->mtimeOut);//设置读取间隔
    }
}

//设置线程启停标志位
void UartThread::runControl(bool flag)
{
    this->mrunFlag = flag;
    emit sendloopon(this->mrunFlag);
}
//向串口写数据
void UartThread::writeUart(const QByteArray & dataToWrite )
{
    this->mcom->flush();
    this->mcom->write(dataToWrite);
}

//向串口写数据
void UartThread::writeUart(const char *dataToWrite )
{
    this->mcom->flush();
    this->mcom->write(dataToWrite);
}

