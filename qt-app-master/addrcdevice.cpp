#include "addrcdevice.h"
#include "ui_addrcdevice.h"



AddRCDevice::AddRCDevice(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddRCDevice)
{
    ui->setupUi(this);
    init();
    connectevent();
}

AddRCDevice::~AddRCDevice()
{
    this->deleteLater();
    delete ui;
}

void AddRCDevice::init()
{
   setWindowFlags(Qt::WindowStaysOnTopHint);
//   setAttribute(Qt::WA_DeleteOnClose);

   cmdinfofile = QApplication::applicationDirPath() + CMDINFO;

   ckFont.setBold(true);
   ckFont.setPointSize(18);

   facnameMap = util.Uart_facnameMatch();

   portsList<<"COM1"<<"COM2"<<"COM3"<<"COM4"
            <<"COM5"<<"COM6"<<"COM7"<<"COM8"
            <<"COM9"<<"COM10"<<"COM11"<<"COM12"
            <<"XR-COM1"<<"XR-COM2"<<"XR-COM3"<<"XR-COM4"
            <<"USB-COM1"<<"USB-COM2"<<"USB-COM3"<<"USB-COM4";
   cmdlist.clear();
   cmdlist<<"01 06 00 00 00 0B C8 0D";
   cmdlist<<"01 06 00 00 00 01 48 0A";
   cmdlist<<"01 06 00 00 00 1E 09 C2";
   cmdlist<<"01 06 00 00 00 02 08 0B";



   ui->addcmdinfo->clear();

   ui->addcomBox->clear();
   ui->addcomBox->addItems(portsList);

   ui->addfacBox->clear();
   for(int i=0;i<facnameMap.count();++i)
   {
       QString code = facnameMap.keys()[i];
       QString name = facnameMap[code];
       QString full_name = code + "-" + name;
       ui->addfacBox->addItem(full_name);
   }

   ui->a_modbusindex->setText("0");


   typecks.clear();
   typecks.append(ui->addcmd1);
   typecks.append(ui->addcmd2);
   typecks.append(ui->addcmd3);
   typecks.append(ui->addcmd4);


   addTimeCks(timecks,al1,ui->ap1);
   addTimeCks(timecks,al2,ui->ap2);
   addTimeCks(timecks,al3,ui->ap3);
   addTimeCks(timecks,al4,ui->ap4);

   ui->a_modbusindex->installEventFilter(this);
}

bool AddRCDevice::eventFilter(QObject *watched, QEvent *event)
{

    if(watched == ui->a_modbusindex) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event); // 事件转换
            if(mouseEvent->button() == Qt::LeftButton) {
//                qDebug()<<__LINE__<<textEditList.at(i)->objectName()<<endl;
//                ui->a_modbusindex->setFocus();
//                if(!kb)
//                    kb = new localKeyboard(this);
//                else
//                    kb->show();
                QProcess pro;
                pro.startDetached("pkill florence");
                pro.close();
                QProcess pro1;
                pro1.startDetached("florence");
                pro1.close();


                return true;
            }
        }
        return false;
    }


    return QWidget::eventFilter(watched,event);
}

void AddRCDevice::connectevent()
{
    connect(ui->addreback,&QPushButton::clicked,this,[=]()
    {
        if(this->isVisible())
            this->close();
    });

    for(QCheckBox *ck:typecks)
    {
        connect(ck,&QCheckBox::stateChanged,this,[=](int state)
        {
            if(state)
            {
                int index = typecks.indexOf(ck);
                QString cmd = cmdlist[index];
                ui->addcmdinfo->setText(ck->text()+":"+cmd);
                qDebug()<<__LINE__<<"cka:"<<ck->text()<<endl;
                if(ck==ui->addcmd1)
                {
                    ui->addTimeframe->setCurrentWidget(ui->ap1);
                }
                else if(ck==ui->addcmd2)
                {
                    ui->addTimeframe->setCurrentWidget(ui->ap2);
                }
                else if(ck==ui->addcmd3)
                {
                    ui->addTimeframe->setCurrentWidget(ui->ap3);
                }
                else if(ck==ui->addcmd4)
                {
                    ui->addTimeframe->setCurrentWidget(ui->ap4);
                }
            }
        });
    }
    if(typecks[0])
    {
        typecks[0]->setChecked(true);
    }

    connect(ui->addfacBox,&QComboBox::currentTextChanged,ui->addcmdinfo,&QLabel::setText);
    connect(ui->addcomBox,&QComboBox::currentTextChanged,ui->addcmdinfo,&QLabel::setText);
    ui->addfacBox->setCurrentIndex(0);
    ui->addcomBox->setCurrentIndex(0);

    connect(ui->addok,&QPushButton::clicked,this,&AddRCDevice::onAdd);

    connect(ui->addSelAll,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            QWidget *w = ui->addTimeframe->currentWidget();
            if(w)
            {
                QList<QCheckBox *> cks = w->findChildren<QCheckBox *>();
                for(QCheckBox *ck:cks)
                {
                    ck->setChecked(true);
                }
            }

        }

    });


}

void AddRCDevice::addTimeCks(QList<QCheckBox *> &timecks,QGridLayout &timeset,QWidget *w)
{
    QString stylesheet = ":/images/checkboxstyle.qss";
    QFile file(stylesheet);
    QByteArray bytArr;
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        bytArr.append(file.readAll());
    }

    timecks.clear();
    QHBoxLayout layout;

    for(int i=0;i<24;++i)
    {
        QString timeNote = QString::number(i);
        QCheckBox *box = new QCheckBox(timeNote);
        box->setCheckable(true);
        box->setStyleSheet(bytArr);
        box->setFont(ckFont);
//        box->resize(widthck,64);
//        box->setAutoExclusive(true);
        box->setChecked(false);
        box->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
        timecks.append(box);



    }

    int row = 0,col = 0;

    for(int i = 0;i<timecks.count();++i)
    {
        QCheckBox *box = (QCheckBox *)timecks.at(i);
        if(box!=nullptr)
        {
            qDebug()<<__LINE__<<row<<col<<box->text()<<endl;
            timeset.addWidget(box,row,col,Qt::AlignCenter);
            timeset.setRowStretch(row,1);
            timeset.setColumnStretch(col,1);
            if(col==5)
            {
                col = 0;
                row++;
            }
            else
            {
                col++;
            }

        }

    }

    timeset.setSpacing(4);


    w->setLayout(&timeset);



    bytArr.clear();
}

//slot function
void AddRCDevice::onAdd()
{
    QString modbus_index = ui->a_modbusindex->text().trimmed();
    QString fac = ui->addfacBox->currentText();
    QString com = ui->addcomBox->currentText();

    selckMap[ui->addcmd1->text()].clear();
    selckMap[ui->addcmd2->text()].clear();
    selckMap[ui->addcmd3->text()].clear();
    selckMap[ui->addcmd4->text()].clear();

    for(int i=0;i<ui->ap1->findChildren<QCheckBox *>().count();++i)
    {
        QCheckBox *box = ui->ap1->findChildren<QCheckBox *>()[i];
        if(box->isChecked())
            selckMap[ui->addcmd1->text()].append(box->text());
    }

    for(int i=0;i<ui->ap2->findChildren<QCheckBox *>().count();++i)
    {
        QCheckBox *box = ui->ap2->findChildren<QCheckBox *>()[i];
        if(box->isChecked())
            selckMap[ui->addcmd2->text()].append(box->text());
    }

    for(int i=0;i<ui->ap3->findChildren<QCheckBox *>().count();++i)
    {
        QCheckBox *box = ui->ap3->findChildren<QCheckBox *>()[i];
        if(box->isChecked())
            selckMap[ui->addcmd3->text()].append(box->text());
    }

    for(int i=0;i<ui->ap4->findChildren<QCheckBox *>().count();++i)
    {
        QCheckBox *box = ui->ap4->findChildren<QCheckBox *>()[i];
        if(box->isChecked())
            selckMap[ui->addcmd4->text()].append(box->text());
    }
    qDebug()<<__LINE__<<selckMap<<endl;

    QMap<QString,QStringList>::iterator it = selckMap.begin();


    QJsonObject jMainObj,jBaseObj,jCmdObj,jSubobj;

    while(it != selckMap.end())
    {
        QString cmdName = it.key();
        int index = selckMap.keys().indexOf(cmdName);
        QString cmd = cmdlist[index];

        QString seltks;
        QStringList tks = it.value();
        for(auto tk:tks)
        {
            seltks += tk;
            if(tk!=tks.last())
            {
                seltks += ",";
            }
        }

        jSubobj.insert("data",cmd);
        jSubobj.insert("select_times",seltks);

        jCmdObj.insert(cmdName,jSubobj);

        it++;
    }


    jBaseObj.insert("port",com);
    jBaseObj.insert("cmd",jCmdObj);
    jBaseObj.insert("modbus_index",modbus_index.toInt());

    jMainObj.insert(fac,jBaseObj);

    qDebug()<<__LINE__<<jMainObj<<endl;

    QJsonDocument jDocW,jDocR;

    QFile fileR(cmdinfofile);
    if(fileR.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QByteArray byt;
        byt.append(fileR.readAll());
        fileR.flush();
        fileR.close();

        jDocR = QJsonDocument::fromJson(byt);
        QJsonObject jtmpObj = jDocR.object();
        if(jtmpObj.contains(fac))
        {
            jtmpObj.remove(fac);
        }
        jtmpObj.insert(fac,jBaseObj);
        jMainObj = jtmpObj;
        byt.clear();
    }

        jDocW.setObject(jMainObj);
        QFile fileW(cmdinfofile);
        qDebug()<<__LINE__<<cmdinfofile<<endl;
        fileW.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate);
        fileW.write(jDocW.toJson());
        fileW.flush();
        fileW.close();

        if(QMessageBox::Ok == QMessageBox::information(this,"提示","新增设备成功！"))
        {
             emit sendFinished(true);
        }
        this->close();

}
