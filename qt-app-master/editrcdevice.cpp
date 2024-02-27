#include "editrcdevice.h"
#include "ui_editrcdevice.h"

EditRCDevice::EditRCDevice(QString facname,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditRCDevice)
{
    ui->setupUi(this);
    name = facname;
    init();
    connectevent();
}

EditRCDevice::~EditRCDevice()
{
    delete ui;
}

void EditRCDevice::init()
{
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose);

    cmdinfofile = QApplication::applicationDirPath() + CMDINFO;

    qDebug()<<__LINE__<<__FUNCTION__<<endl;
    QJsonDocument jDocR;
    QFile fileR(cmdinfofile);
    qDebug()<<__LINE__<<__FUNCTION__<<cmdinfofile<<endl;
    if(!fileR.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        qDebug()<<__LINE__<<fileR.errorString()<<endl;
        return;
    }

    QByteArray byt;
    byt.append(fileR.readAll());
    fileR.flush();
    fileR.close();

    jDocR = QJsonDocument::fromJson(byt);
    QJsonObject jObj = jDocR.object();
    byt.clear();
    qDebug()<<__LINE__<<jObj<<endl;


    ckFont.setBold(true);
    ckFont.setPointSize(18);

    facnameMap = util.Uart_facnameMatch();
    qDebug()<<__LINE__<<facnameMap<<endl;

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



    ui->editcmdinfo->clear();

    ui->editcomBox->clear();
    ui->editcomBox->addItems(portsList);

    ui->editfacBox->clear();
    for(int i=0;i<facnameMap.count();++i)
    {
        QString code = facnameMap.keys()[i];
        QString name = facnameMap[code];
        QString full_name = code + "-" + name;
        qDebug()<<__LINE__<<full_name<<endl;
        ui->editfacBox->addItem(full_name);
    }

//    ui->e_modbusindex->setText("0");

    typecks.clear();
    typecks.append(ui->editcmd1);
    typecks.append(ui->editcmd2);
    typecks.append(ui->editcmd3);
    typecks.append(ui->editcmd4);

    addTimeCks(timecks,al1,ui->ep1);
    addTimeCks(timecks,al2,ui->ep2);
    addTimeCks(timecks,al3,ui->ep3);
    addTimeCks(timecks,al4,ui->ep4);

    qDebug()<<__LINE__<<name<<endl;

    if(jObj.contains(name))
    {
        QJsonObject jValueObj = jObj.value(name).toObject();
        qDebug()<<__LINE__<<name<<jValueObj<<endl;
        QString modindex = QString::number(jValueObj.value("modbus_index").toInt());
        QString com = jValueObj.value("port").toString();
        QJsonObject cmdObj = jValueObj.value("cmd").toObject();

        qDebug()<<__LINE__<<name<<com<<modindex<<endl;
        qDebug()<<__LINE__<<cmdObj<<endl;
        qDebug()<<__LINE__<<ui->editfacBox->findText(name)<<endl;
        qDebug()<<__LINE__<<ui->editcomBox->findText(com)<<endl;

        ui->editfacBox->setCurrentText(name);
        ui->editcomBox->setCurrentText(com);

        ui->e_modbusindex->setText(modindex);

        loadtmcksState(com);


        ui->editSelAll->setChecked(false);
        for(QCheckBox *ck:typecks)
        {
            connect(ck,&QCheckBox::stateChanged,this,[=](int state)
            {

                if(state)
                {
                    int index = typecks.indexOf(ck);
                    QString cmdName = ck->text();
                    QJsonObject jCmdValueObj = cmdObj.value(cmdName).toObject();
                    QString cmd = jCmdValueObj.value("data").toString();
                    ui->editcmdinfo->setText(ck->text()+":"+cmd);
                    qDebug()<<__LINE__<<"cka:"<<ck->text()<<endl;
                    if(ck==ui->editcmd1)
                    {
                        ui->editTimeframe->setCurrentWidget(ui->ep1);
                    }
                    else if(ck==ui->editcmd2)
                    {
                        ui->editTimeframe->setCurrentWidget(ui->ep2);
                    }
                    else if(ck==ui->editcmd3)
                    {
                        ui->editTimeframe->setCurrentWidget(ui->ep3);
                    }
                    else if(ck==ui->editcmd4)
                    {
                        ui->editTimeframe->setCurrentWidget(ui->ep4);
                    }
                }
            });
        }

        if(typecks[0])
        {
            typecks[0]->setChecked(true);
        }





    }

    ui->editfacBox->setEnabled(false);
    ui->e_modbusindex->installEventFilter(this);
//    kb = new localKeyboard(this);

    QMessageBox::about(this,"操作提示","指定串口号，modbus设备序号，"\
                       "为四个指令指定不同的时间数字组合，"\
                       "非测试模式下，将数字昨晚小时数运行线程，测试模式下，"\
                       "将数字昨晚秒数运行线程，点击确定完成新增");
}

bool EditRCDevice::eventFilter(QObject *watched, QEvent *event)
{

    if(watched == ui->e_modbusindex) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event); // 事件转换
            if(mouseEvent->button() == Qt::LeftButton) {
//                qDebug()<<__LINE__<<textEditList.at(i)->objectName()<<endl;
//                ui->e_modbusindex->setFocus();
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

void EditRCDevice::addTimeCks(QList<QCheckBox *> &timecks,QGridLayout &timeset,QWidget *w)
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

void EditRCDevice::loadtmcksState(QString porName)
{
    QFile file(QApplication::applicationDirPath()+CMDINFO);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        return;
    QByteArray byt;
    byt.append(file.readAll());
    file.flush();
    file.close();

    QJsonDocument jDoc = QJsonDocument::fromJson(byt);
    byt.clear();
    QJsonObject jObj = jDoc.object();

    QJsonObject::iterator it = jObj.begin();
    while(it != jObj.end())
    {
        qDebug()<<__LINE__<<it.key()<<it.value()<<endl;
        QJsonObject jSubObj = it.value().toObject();
        QString portName = jSubObj.value("port").toString();
        qDebug()<<__LINE__<<porName<<":"<<portName<<endl;
        if(porName == portName)
        {
            QJsonObject portObj = jSubObj.value("cmd").toObject();
            QJsonObject::iterator itck = portObj.begin();
            while(itck != portObj.end())
            {
                QString cmdName = itck.key();
                QJsonObject valueObj = itck.value().toObject();
                QString seltms = valueObj.value("select_times").toString();
                qDebug()<<__LINE__<<itck.key()<<seltms<<endl;
                QStringList tmlist;
                int index;
                for(QCheckBox *box:typecks)
                {
                    if(box)
                    {

                        if(box->text() == cmdName)
                        {

                            index = typecks.indexOf(box);
                            qDebug()<<__LINE__<<index<<box->text()<<cmdName<<endl;

                        }
                    }
                }

                if(seltms.split(",").count()>0)
                {
                    tmlist = seltms.split(",");
                }

                QWidget *w = ui->editTimeframe->widget(index);
                if(w)
                {
                    for(int k = 0;k < w->layout()->count();++k)
                    {
                        QLayoutItem *item = w->layout()->itemAt(k);
                        if(item->widget())
                        {
                            QCheckBox *box = (QCheckBox *)item->widget();
                            qDebug()<<__LINE__<<box->text()<<endl;
                            box->setChecked(tmlist.contains(box->text()));

                        }
                    }
                }
                itck++;
            }
            break;
        }

        it++;
    }
}

void EditRCDevice::connectevent()
{
    connect(ui->editreback,&QPushButton::clicked,this,&EditRCDevice::close);

    connect(ui->editfacBox,&QComboBox::currentTextChanged,ui->editcmdinfo,&QLabel::setText);
    connect(ui->editcomBox,&QComboBox::currentTextChanged,ui->editcmdinfo,&QLabel::setText);
//    ui->editfacBox->setCurrentIndex(0);
//    ui->editcomBox->setCurrentIndex(0);
    connect(ui->editSelAll,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            QWidget *w = ui->editTimeframe->currentWidget();
            if(w)
            {
                QList<QCheckBox *> cks = w->findChildren<QCheckBox *>();
                for(QCheckBox *ck:cks)
                {
                    ck->setChecked(true);
                }
            }

        }
        else
        {
            loadtmcksState(ui->editcomBox->currentText());
        }
    });

    connect(ui->editok,&QPushButton::clicked,this,&EditRCDevice::onSave);

}


//slot function
void EditRCDevice::onSave()
{
    QString modbus_index = ui->e_modbusindex->text().trimmed();
    QString fac = ui->editfacBox->currentText();
    QString com = ui->editcomBox->currentText();

    selckMap[ui->editcmd1->text()].clear();
    selckMap[ui->editcmd2->text()].clear();
    selckMap[ui->editcmd3->text()].clear();
    selckMap[ui->editcmd4->text()].clear();

    for(int i=0;i<ui->ep1->findChildren<QCheckBox *>().count();++i)
    {
        QCheckBox *box = ui->ep1->findChildren<QCheckBox *>()[i];
        if(box->isChecked())
            selckMap[ui->editcmd1->text()].append(box->text());
    }

    for(int i=0;i<ui->ep2->findChildren<QCheckBox *>().count();++i)
    {
        QCheckBox *box = ui->ep2->findChildren<QCheckBox *>()[i];
        if(box->isChecked())
            selckMap[ui->editcmd2->text()].append(box->text());
    }

    for(int i=0;i<ui->ep3->findChildren<QCheckBox *>().count();++i)
    {
        QCheckBox *box = ui->ep3->findChildren<QCheckBox *>()[i];
        if(box->isChecked())
            selckMap[ui->editcmd3->text()].append(box->text());
    }

    for(int i=0;i<ui->ep4->findChildren<QCheckBox *>().count();++i)
    {
        QCheckBox *box = ui->ep4->findChildren<QCheckBox *>()[i];
        if(box->isChecked())
            selckMap[ui->editcmd4->text()].append(box->text());
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
