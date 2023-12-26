#include "devicecmdctrldlg.h"
#include "ui_devicecmdctrldlg.h"

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
    serial->setPortName(ui->curPort->currentText());
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setParity(QSerialPort::NoParity);

    //open port
    if(serial->open(QIODevice::ReadWrite))
    {
        QString cmd = ui->sendEdit->text().trimmed();
        QByteArray bytArr;
        bytArr.append(cmd.toLatin1().toHex());

        //send cmd
        serial->write(bytArr);

        while(serial->waitForBytesWritten(3000));
        bytArr.clear();
        connect(serial,SIGNAL(readyRead),this,SLOT(readData));

    }
}

void DeviceCMDCtrlDlg::readData()
{
    QByteArray data;

    if(serial!=nullptr)
    {
        if(serial->isOpen())
        {
            data.append(serial->readAll());
            QString str = ui->curDevice->currentText();
            str += data.toHex();
            ui->receiveEdit->setPlainText(str);
            serial->close();
        }
    }


    data.clear();

}
