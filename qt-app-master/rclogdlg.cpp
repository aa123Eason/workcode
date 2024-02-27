#include "rclogdlg.h"
#include "ui_rclogdlg.h"

RCLogDlg::RCLogDlg(QString str,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RCLogDlg)
{
    ui->setupUi(this);
    curStr = str;
    init();
    connectevent();
}

RCLogDlg::~RCLogDlg()
{
    delete ui;
}


void RCLogDlg::init()
{
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::WindowStaysOnTopHint);
//    kb = new localKeyboard(this);
}

void RCLogDlg::connectevent()
{
    connect(ui->btn_clear,&QPushButton::clicked,ui->logEdit,&QTextBrowser::clear);
    connect(ui->btn_log,&QPushButton::clicked,ui->logEdit,[=]()
    {
        ui->logEdit->setText(curStr);
    });
}

//slot function
void RCLogDlg::onSlotLog(QString str)
{
    QString oldStr = ui->logEdit->toPlainText();
    QString dt = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    curStr = oldStr + "\n" + dt + " " + str + "\n";

}
