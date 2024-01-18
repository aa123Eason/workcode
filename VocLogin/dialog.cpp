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
    ui->lineEdit_2->installEventFilter(this);


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
    if(obj == ui->lineEdit_2)
    {
        qDebug()<<__LINE__<<obj->objectName()<<endl;
        if(e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *me = (QMouseEvent *)e;
            if(me->button()==Qt::LeftButton)
            {
                PVOID OldValue;
                BOOL bRet = Wow64DisableWow64FsRedirection (&OldValue);
                QString csProcess="C:\\Windows\\System32\\osk.exe";
                QString params="";
                ShellExecute(NULL, L"open", (LPCWSTR)csProcess.utf16(), (LPCWSTR)params.utf16(), NULL, SW_SHOWNORMAL);
                if ( bRet )
                {
                    Wow64RevertWow64FsRedirection(OldValue);
                }

            }
        }
    }

    return QDialog::eventFilter(obj,e);
}

void Dialog::on_pushButton_3_clicked()
{
    this->close();
}

QJsonArray Dialog::getUserInfo()
{
    QJsonArray array;
    QString path = QApplication::applicationDirPath()+USERINFO;
    QFile file(path);
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray byt;
        byt.append(file.readAll());
        file.flush();
        file.close();
        QJsonDocument jDoc = QJsonDocument::fromBinaryData(byt);
        byt.clear();
        array = jDoc.array();

    }

    qDebug()<<__LINE__<<array<<endl;
//    QJsonDocument jDoc1;
//    jDoc1.setArray(array);
//    QString path1 = QApplication::applicationDirPath()+"/docu/user.dat";
//    QFile file1(path1);
//    file1.open(QIODevice::WriteOnly|QIODevice::Truncate);
//    file1.write(jDoc1.toBinaryData());
//    file1.flush();
//    file1.close();

    return array;

}

void Dialog::on_pushButton_clicked()
{

    for(int i=0;i<usersArray.count();++i)
    {
        QJsonValue value = usersArray.at(i);
        QString username = value.toObject().keys()[0];
        QString pwd = value.toObject().value(username).toString();

        qDebug()<<"info:"<<username<<":"<<pwd<<endl;
        if(ui->comboBox->currentText() == username && ui->lineEdit_2->text() == pwd)
        {
            qDebug()<<__LINE__<<ui->comboBox->currentText()<<":"<<ui->lineEdit_2->text()<<endl;
            if(!ui->comboBox->currentText().isEmpty())
            {
                if(writeindb(db,ui->comboBox->currentText()))
                {
                    this->close();
                    qDebug()<<__LINE__<<QApplication::applicationDirPath() + "/VocGas.exe"<<endl;
                    my_Process->startDetached(QApplication::applicationDirPath() + "/VocGas.exe");
                    break;
                }
            }



        }
//        else
//        {

//            QMessageBox::about(NULL, "提示", "<font color='black'>用戶名或密码输入错误！</font>");

//        }

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
    usersArray = getUserInfo();

    ui->lineEdit_2->installEventFilter(this);
}

bool Dialog::datebaseinit()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath()+"/VocGas.db");
    qDebug()<<__LINE__<<db.databaseName()<<endl;
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
    QString sqlStr2 = "delete from T_User_Logining;";

    qDebug()<<__LINE__<<sqlStr<<endl;
    qDebug()<<__LINE__<<sqlStr1<<endl;
    qDebug()<<__LINE__<<sqlStr2<<endl;

    QSqlQuery q(db),q1(db),q2(db);

    q1.exec(sqlStr1);  
    q2.exec(sqlStr2);
    q.exec(sqlStr);

    return true;

}

