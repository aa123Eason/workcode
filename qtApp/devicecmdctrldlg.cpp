#include "devicecmdctrldlg.h"
#include "ui_devicecmdctrldlg.h"

static char ConvertHexChar(char c);

static QByteArray QString2Hex(QString hexStr);

DeviceCMDCtrlDlg::DeviceCMDCtrlDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceCMDCtrlDlg)
{
    ui->setupUi(this);
    init();
    connectevent();
}

DeviceCMDCtrlDlg::~DeviceCMDCtrlDlg()
{
    delete ui;
}


void DeviceCMDCtrlDlg::init()
{
    ui->sendEdit->clear();
    ui->receiveEdit->clear();
    ui->sendEdit->installEventFilter(this);
}

bool DeviceCMDCtrlDlg::eventFilter(QObject *obj,QEvent *e)
{
    if(obj == nullptr || e == nullptr)
        return false;
    if(obj == ui->sendEdit)
    {
        if(e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *me = (QMouseEvent *)e;
            if(me->button() == Qt::LeftButton)
            {
                qDebug()<<"open keyboard"<<endl;
                QProcess::execute("florence");
            }
        }
    }

    return QDialog::eventFilter(obj,e);
}

void DeviceCMDCtrlDlg::connectevent()
{
    connect(ui->btn_quit,&QPushButton::clicked,[=]()
    {
        emit sendReback(this);
    });

    connect(ui->deleteSend,&QPushButton::clicked,[=]()
    {
        ui->sendEdit->clear();
    });

    connect(ui->deleteReceive,&QPushButton::clicked,[=]()
    {
        ui->receiveEdit->clear();
    });

    connect(ui->btnJZ,&QPushButton::clicked,this,[=]()
    {
        ui->sendEdit->setText("01 06 00 00 00 0B C8 0D");
    });

    connect(ui->btntestnow,&QPushButton::clicked,this,[=]()
    {
        ui->sendEdit->setText("01 06 00 00 00 01 48 0A");
    });


    connect(ui->btnfc,&QPushButton::clicked,this,[=]()
    {
        ui->sendEdit->setText("01 06 00 00 00 1E 09 C2");
    });


    connect(ui->btnBDCY,&QPushButton::clicked,this,[=]()
    {
        ui->sendEdit->setText("01 06 00 00 00 02 08 0B");
    });

    connect(ui->btn_sendcmd,&QPushButton::clicked,this,[=]()
    {
        emit sendCMD(ui->sendEdit->text());
    });

    connect(this,&DeviceCMDCtrlDlg::sendCMD,this,&DeviceCMDCtrlDlg::onReceiveCMD);
}

void DeviceCMDCtrlDlg::onReceiveCMD(QString cmd)
{
    qDebug()<<__LINE__<<"SEND:"<<cmd<<endl;

    serial = new QSerialPort(this);

    //set params
    QString realName = UsbUtility::matchDevName(ui->curPort->currentText());

    serial->setPortName(realName);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setParity(QSerialPort::NoParity);

    //串口已接通，波特率，数据位，停止位，校验位，数据进制，通信超时
    //open port
    if(serial->open(QIODevice::ReadWrite))
    {
        str = "串口"+ui->curPort->currentText()+"已接通!\r\n";
        str += "波特率"+QString::number(serial->baudRate())+"\r\n";
        str += "数据位"+QString::number(serial->baudRate())+"\r\n";
        str += "停止位"+QString::number(serial->baudRate())+"\r\n";
        str += "校验位"+QString::number(serial->baudRate())+"\r\n";
        str += "数据进制:16\r\n";
        str += "通信超时:1min\r\n";

        QString cmd = ui->sendEdit->text().trimmed();
        QByteArray bytArr = QByteArray::fromHex(cmd.toUpper().toLatin1()).toHex(' ');

        str += "S:"+bytArr+"\r\n";
        bytArr.clear();

        //send cmd
        serial->flush();
        serial->write(QString2Hex(cmd).data());

        QByteArray data;

        data.append(serial->readAll());
        str += ui->curDevice->currentText()+"R:"+data.toHex(' ')+"\r\n";
        ui->receiveEdit->setPlainText(str);
        serial->close();



    }
    else
    {
        str = "串口"+ui->curPort->currentText()+"未接通!\r\n";
    }

    ui->receiveEdit->setPlainText(str);
}

//将单个字符串转换为hex
//0-F -> 0-15
char ConvertHexChar(char c)
{
    if((c >= '0') && (c <= '9'))
        return c - 0x30;
    else if((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;//'A' = 65;
    else if((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;
    else
        return -1;
}

QByteArray QString2Hex(QString hexStr)
{
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = hexStr.length();
    senddata.resize(len/2);
    char lstr, hstr;
    for(int i = 0; i < len; )
    {
        //将第一个不为' '的字符赋给hstr;
        hstr = hexStr[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        //当i >= len时，跳出循环
        if(i >= len)
            break;
        //当i < len时，将下一个字符赋值给lstr;
        lstr = hexStr[i].toLatin1();
        //将hstr和lstr转换为0-15的对应数值
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        //
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata * 16 + lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}


