#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>
#include <QTextCodec>
#include <QMessageBox>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);

//    connect(&my_Process,&QProcess::readyRead,this,&Dialog::showResult);
//    connect(&my_Process,&QProcess::stateChanged,this,&Dialog::showState);
//    connect(&my_Process,&QProcess::errorOccurred,this,&Dialog::showError);
//    connect(&my_Process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(showFinished(int,QProcess::ExitStatus)));

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_3_clicked()
{
    this->close();
}

void Dialog::on_pushButton_clicked()
{

   if(ui->comboBox->currentText() == "超级管理员" && ui->lineEdit_2->text() == "888888")
   {
       my_Process.startDetached("D:/QtWork/build-VocGas-Desktop_Qt_5_12_12_MinGW_32_bit-Release/release/VocGas.exe");
       this->close();
   }
   else
   {
       QMessageBox::about(NULL, "提示", "<font color='black'>登录密码错误！</font>");
   }
}

//void Dialog::showState(QProcess::ProcessState state)
//{
//    qDebug()<<"showState: ";
//    if(state == QProcess::NotRunning)
//    {
//        qDebug()<<"没有程序运行";
//    }
//    else if(state==QProcess::Starting)
//    {
//        qDebug()<<"开始中";
//    }
//    else
//    {
//        qDebug()<<"运行中。。。";
//    }
//}
//void Dialog::showResult()
//{
//    QTextCodec *codec=QTextCodec::codecForLocale();
//    qDebug()<<"显示结果:"<<endl<<codec->toUnicode(my_Process.readAll());
//}
//void Dialog::showError()
//{
//    qDebug()<<"显示错误信息："<<endl<<my_Process.errorString();
//}
//void Dialog::showFinished(int exitCode,QProcess::ExitStatus exit_stattus)
//{
//    qDebug()<<"显示完成："<<endl<<"退出代码："<<exitCode<<"，退出状态:"<<exit_stattus;
//}

