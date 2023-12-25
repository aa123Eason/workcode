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

    init();

    my_Process = new QProcess(this);

    connect(my_Process,&QProcess::readyRead,this,&Dialog::showResult);
    connect(my_Process,&QProcess::stateChanged,this,&Dialog::showState);
    connect(my_Process,&QProcess::errorOccurred,this,&Dialog::showError);
    connect(my_Process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(showFinished(int,QProcess::ExitStatus)));
//    connect(ui->pushButton,&QPushButton::clicked,this,&Dialog::on_pushButton_clicked);
}

Dialog::~Dialog()
{
    delete ui;
    delete my_Process;
    if(db.isOpen())
        db.close();
}

bool Dialog::eventFilter(QObject *obj, QEvent *e)
{
    if(obj != ui->lineEdit_2)
    {
        if(e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *me = (QMouseEvent *)e;
            if(me->button()==Qt::LeftButton)
            {
                void* keyBoard = nullptr;
                bool m_b = Wow64DisableWow64FsRedirection(&keyBoard);
                QString boardExe = "C:/Windows/System32/osk.exe";

                QString params = "";
                ShellExecute(nullptr, L"open", (LPCWSTR)boardExe.utf16(), (LPCWSTR)params.utf16(), nullptr, SW_SHOWNORMAL);
                if (m_b)
                {
                    Wow64RevertWow64FsRedirection(keyBoard);
                }

            }
        }
    }

    return QWidget::eventFilter(obj,e);
}

void Dialog::on_pushButton_3_clicked()
{
    this->close();
}

void Dialog::on_pushButton_clicked()
{

   if(ui->comboBox->currentText() == "超级管理员" && ui->lineEdit_2->text() == "888888")
   {


       if(!ui->comboBox->currentText().isEmpty())
       {
           if(writeindb(db,ui->comboBox->currentText()))
           {
               this->close();
               my_Process->startDetached(QApplication::applicationDirPath() + "/VocGas.exe");
           }
       }



   }
   else
   {
       QMessageBox::about(NULL, "提示", "<font color='black'>登录密码错误！</font>");
   }
}

void Dialog::showState(QProcess::ProcessState state)
{
    qDebug()<<"showState: ";
    if(state == QProcess::NotRunning)
    {
        qDebug()<<"没有程序运行";
    }
    else if(state==QProcess::Starting)
    {
        qDebug()<<"开始中";
    }
    else
    {
        qDebug()<<"运行中。。。";
    }
}
void Dialog::showResult()
{
    QTextCodec *codec=QTextCodec::codecForLocale();
    qDebug()<<"显示结果:"<<endl<<codec->toUnicode(my_Process->readAll());
}
void Dialog::showError()
{
    qDebug()<<"显示错误信息："<<endl<<my_Process->errorString();
}
void Dialog::showFinished(int exitCode,QProcess::ExitStatus exit_stattus)
{
    qDebug()<<"显示完成："<<endl<<"退出代码："<<exitCode<<"，退出状态:"<<exit_stattus;
}

void Dialog::init()
{
    datebaseinit();

    ui->lineEdit_2->installEventFilter(this);
}

bool Dialog::datebaseinit()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath()+"/VocGas.db");
    if(db.open())
    {
        qDebug()<<__LINE__<<"数据库打开成功"<<endl;
        return true;
    }
    else
    {
        QString text = db.lastError().text();
        qDebug()<<__LINE__<<"错误："<<text<<endl;
        return false;
    }
}

bool Dialog::writeindb(QSqlDatabase &db,QString name)
{
    if(!db.isOpen())
        return false;
    QString sqlStr = "insert into T_User_Logining(Id,UserName,LoginTime) values(";
    sqlStr += QString::number(1)+",\'"+name+"\',\'"+QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
    sqlStr += "\');";

    QString sqlStr1 = "select UserName from T_User_Logining;";
    QString sqlStr2 = "delete * from T_User_Logining;";

    qDebug()<<__LINE__<<sqlStr<<endl;
    qDebug()<<__LINE__<<sqlStr1<<endl;
    qDebug()<<__LINE__<<sqlStr2<<endl;

    QSqlQuery q(db),q1(db),q2(db);

    q1.exec(sqlStr1);


    while(q1.next())
    {
        if(name == q1.value("UserName").toString())
        {
            q2.exec(sqlStr2);
            break;

        }
    }

    q.exec(sqlStr);

    return true;

}

