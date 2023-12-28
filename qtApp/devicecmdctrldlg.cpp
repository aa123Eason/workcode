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
    ui->loopread->setChecked(false);
    if(muartThread->isRunning())
    {
        muartThread->quit();
    }
    muartThread->deleteLater();
    delete ui;
}


void DeviceCMDCtrlDlg::init()
{
    ui->sendEdit->clear();
    ui->receiveEdit->clear();
    ui->sendEdit->installEventFilter(this);

    QFile file(":/new/images/image/checkboxstyle.qss");
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QByteArray byt = file.readAll();

        ui->loopread->setStyleSheet(byt);
        file.close();
    }
    ui->loopread->setChecked(false);
    ui->loopread->setCheckable(false);

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
        ui->loopread->setChecked(false);
        emit sendReback(this);
    });

    connect(ui->curPort,&QComboBox::currentTextChanged,this,[=](const QString &)
    {
        ui->loopread->setChecked(false);

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
    connect(ui->sendEdit,&QLineEdit::textChanged,[=](const QString &text)
    {
        QStringList list;
        list<<"01 06 00 00 00 0B C8 0D";
        list<<"01 06 00 00 00 01 48 0A";
        list<<"01 06 00 00 00 1E 09 C2";
        list<<"01 06 00 00 00 02 08 0B";
        //执行，发送
        if(list.contains(text))
        {
            ui->btn_sendcmd->setText("执行");
        }
        else
        {
            ui->btn_sendcmd->setText("发送");
        }
    });




    connect(ui->loopread,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state == Qt::Checked)
        {
            if(muartThread!=nullptr)
            {
                muartThread->runControl(true);
                connect(muartThread,&UartThread::sendResData,this,[=](QByteArray arrData)
                {
                    qDebug()<<__LINE__<<"RECEIVE2:"<<arrData.toHex(' ')<<endl;


                    str = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")+ui->curPort->currentText()+"[R]:"+arrData.toHex(' ')+"\r\n";


                    ui->receiveEdit->setPlainText(str);
                });
            }
        }
        else
        {
            if(muartThread!=nullptr)
            {
                muartThread->runControl(false);

            }
        }
    });


}

void DeviceCMDCtrlDlg::onReceiveCMD(QString cmd)
{
    qDebug()<<__LINE__<<"SEND:"<<cmd<<endl;
    QString curPortName = ui->curPort->currentText();
    QString originName = UsbUtility::matchDevName(curPortName);
    muartThread = new UartThread;
    if(!muartThread->initUart(originName,BAUD9600,3,5))
    {
        comstr = curPortName +"未接通!";
//        ui->loopread->setChecked(false);
        ui->loopread->setCheckable(false);

        return;
    }

    ui->loopread->setCheckable(true);



//    ui->btn_sendcmd->setText("执行");
    //串口已接通，波特率，数据位，停止位，校验位，数据进制，通信超时
    comstr = curPortName +"已接通!";
    comstr += "波特率:9600\r\n";
    comstr += "数据位:8\r\n";
    comstr += "停止位:1\r\n";
    comstr += "校验位:none\r\n";
    comstr += "数据进制:16\r\n";
    comstr += "通信超时:1min\r\n";

    ui->cominfo->setText(comstr);

    if(!cmd.isEmpty())
    {
        QByteArray bytArr = QString2Hex(cmd.toLatin1().toUpper()).toHex(' ');

        muartThread->writeUart(QString2Hex(cmd));
        str = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")+curPortName+"[S]:"+bytArr+"\r\n";

    }

    QByteArray resByt = muartThread->readUart();
    qDebug()<<__LINE__<<"RECEIVE1:"<<resByt<<endl;


    str += QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")+curPortName+"[R]:"+resByt+"\r\n";


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


