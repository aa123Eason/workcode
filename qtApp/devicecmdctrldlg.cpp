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

}
