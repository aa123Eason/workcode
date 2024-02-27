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

}
//#ifdef Q_OS_LINUX
DeviceCMDCtrlDlg::~DeviceCMDCtrlDlg()
{
    ui->loopread->setChecked(false);

//    if(muartThread->isRunning())
//    {
//        muartThread->quit();
//    }
//    muartThread->deleteLater();
    serialPort->deleteLater();



    delete ui;
}
//#endif

void DeviceCMDCtrlDlg::init()
{
//    this->setWindowFlags(Qt::FramelessWindowHint);
    typecks.clear();
    typecks.append(ui->tmck1);
    typecks.append(ui->tmck2);
    typecks.append(ui->tmck3);
    typecks.append(ui->tmck4);
    list.clear();
    list<<"01 06 00 00 00 0B C8 0D";
    list<<"01 06 00 00 00 01 48 0A";
    list<<"01 06 00 00 00 1E 09 C2";
    list<<"01 06 00 00 00 02 08 0B";

    timeCheckInit(l1,ui->timeset1);
    timeCheckInit(l2,ui->timeset2);
    timeCheckInit(l3,ui->timeset3);
    timeCheckInit(l4,ui->timeset4);

    loadtmcksState();


    move(this->parentWidget()->x(),this->parentWidget()->y());
    setMaximumSize(1280,718);
    setMinimumSize(1280,718);
    ui->sendEdit->clear();
    ui->receiveEdit->clear();
    ui->sendEdit->installEventFilter(this);
    ui->loopread->hide();

    QFile file(":/images/checkboxstyle.qss");
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QByteArray byt = file.readAll();

        ui->loopread->setStyleSheet(byt);
        file.close();
    }
    ui->loopread->setChecked(false);
//    ui->loopread->setCheckable(false);
    map = util.Uart_devicetype();
    namemap = util.Uart_facnameMatch();

    loadFactorAndPort();


//    ontimecks.clear();
    for(QCheckBox *box:timecks)
    {
        box->setCheckable(true);
    }

    serialPort = new SerialPort(this);

    ui->sendEdit->installEventFilter(this);

    connectevent();

    ui->tmck1->setChecked(Qt::Checked);

//kb = new localKeyboard(this);

}

void DeviceCMDCtrlDlg::loadtmcksState()
{
    QFile file(QApplication::applicationDirPath()+DEVICE_CMD_INFO);
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
        QString cmdName = it.key();
        QJsonObject valueObj = it.value().toObject();
        QString seltms = valueObj.value("select_times").toString();
        qDebug()<<__LINE__<<it.key()<<seltms<<endl;
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

        QWidget *w = ui->timesetframe->widget(index);
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

        it++;
    }
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
//                if(!kb->isVisible())
//                     kb->show();
                QProcess pro;
                pro.startDetached("pkill florence");
                pro.close();
                QProcess pro1;
                pro1.startDetached("florence");
                pro1.close();

            }
        }
    }

    return QDialog::eventFilter(obj,e);
}

void DeviceCMDCtrlDlg::timeCheckInit(QGridLayout &timeset,QWidget *w)
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
    QFont font;
    font.setBold(true);
    font.setPointSize(18);



    for(int i=0;i<24;++i)
    {
        QString timeNote = QString::number(i)+"点";
        QCheckBox *box = new QCheckBox(timeNote);
        box->setCheckable(true);
        box->setStyleSheet(bytArr);
        box->setFont(font);
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

void DeviceCMDCtrlDlg::connectevent()
{
    connect(ui->btn_quit,&QPushButton::clicked,[=]()
    {
        ui->loopread->setChecked(false);

        this->close();
    });

    connect(ui->keyboard,&QPushButton::clicked,this,[=]()
    {
//        if(!kb->isVisible())
//             kb->show();
        QProcess pro;
        pro.startDetached("pkill florence");
        pro.close();
        QProcess pro1;
        pro1.startDetached("florence");
        pro1.close();
    });

    connect(ui->curPort,&QComboBox::currentTextChanged,this,[=](const QString &)
    {
        ui->loopread->setChecked(false);

    });

    connect(ui->deleteSend,&QPushButton::clicked,[=]()
    {
        ui->sendEdit->clear();
        ui->cominfo->clear();
    });

    connect(ui->deleteReceive,&QPushButton::clicked,[=]()
    {
        ui->receiveEdit->clear();
        ui->cominfo->clear();
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

//    #ifdef Q_OS_LINUX
    connect(this,&DeviceCMDCtrlDlg::sendCMD,this,&DeviceCMDCtrlDlg::onReceiveCMD);
    connect(this,&DeviceCMDCtrlDlg::sendCMD,this,[=](QString text)
    {
        //str = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")+curPortName+"[S]:"+bytArr+"\r\n";
        QString str = ui->receiveEdit->toPlainText();
        str += QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")+ui->curPort->currentText()+"[S]:"+text+"\r\n";
        qDebug()<<__LINE__<<str<<endl;
        ui->receiveEdit->setPlainText(str);
    });

//    #endif
    connect(ui->sendEdit,&QLineEdit::textChanged,[=](const QString &text)
    {
//        QStringList list;

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




//#ifdef Q_OS_LINUX
    connect(ui->loopread,&QCheckBox::stateChanged,this,[=](int state)
    {

        qDebug()<<__LINE__<<"loopsend"<<endl;
        isLoopOn = state;
        emit sendloopread(isLoopOn);
//        if(state == Qt::Checked)
//        {
////            if(muartThread!=nullptr)
////            {
////                muartThread->runControl(true);
////                connect(muartThread,&UartThread::sendResData,this,[=](QByteArray arrData)
////                {
////                    qDebug()<<__LINE__<<"RECEIVE2:"<<arrData.toHex(' ')<<endl;


////                    str = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")+ui->curPort->currentText()+"[R]:"+arrData.toHex(' ')+"\r\n";


////                    ui->receiveEdit->setPlainText(str);
////                });
////            }
//        }
//        else
//        {
////            if(muartThread!=nullptr)
////            {
////                muartThread->runControl(false);

////            }
//        }
    });
//#endif

//    for(QCheckBox *box:timecks)
//    {
//        if(box!=nullptr)
//        {
//            connect(box,&QCheckBox::stateChanged,this,[=](int state)
//            {
//                if(state)
//                {
//                    if(ontimecks.contains(box->text().split("点")[0]))
//                        ontimecks.removeOne(box->text().split("点")[0]);
//                    ontimecks.append(box->text().split("点")[0]);
//                }

//                qDebug()<<__LINE__<<ontimecks<<endl;
//            });
//        }
//    }

    connect(ui->btn_sendbytime,&QPushButton::clicked,this,[=]()
    {
        emit sendIsSendBytime(isSendBytime);
        if(isSendBytime)
        {

            ui->btn_sendbytime->setStyleSheet("QPushButton\n{\n	\n	background-color: rgb(30, 120, 0);\n	border-radius:5px;\ncolor: rgb(255, 255, 255);\n	font: 75 18pt '微软雅黑';\n}\n\nQPushButton::hover\n{\n	\n	background-color: rgb(129, 194, 0);\n}\n");
            if(QMessageBox::Ok == QMessageBox::information(this,"提示","停止定时发送"))
            {
                ui->btn_sendbytime->setText("启动定时发送");
            }
            isSendBytime = false;
        }
        else
        {

            ui->btn_sendbytime->setStyleSheet("QPushButton\n{\n	\n	background-color: rgb(120, 30, 0);\n	border-radius:5px;\ncolor: rgb(255, 255, 255);\n	font: 75 18pt '微软雅黑';\n}\n\nQPushButton::hover\n{\n	\n	background-color: rgb(194, 129, 0);\n}\n");
            if(QMessageBox::Ok == QMessageBox::information(this,"提示","启动定时发送"))
            {
                 ui->btn_sendbytime->setText("停止定时发送");
            }
            isSendBytime = true;
        }








    });



    connect(this,&DeviceCMDCtrlDlg::sendIsSendBytime,this,[=](bool state)
    {
        QString str = ui->receiveEdit->toPlainText();

        if(state)
        {
            str += "-------停止定时发送-------";
            timer.stop();

        }
        else
        {
            str += "-------启动定时发送-------";
            timer.start(1000);
        }

        ui->receiveEdit->setPlainText(str);

    });

    connect(&timer,&QTimer::timeout,this,&DeviceCMDCtrlDlg::onSendByTime);

    connect(this,&DeviceCMDCtrlDlg::sendloopread,this,[=](bool state)
    {
//        qDebug()<<__LINE__<<"SEND:"<<cmd<<endl;
        QString curPortName = ui->curPort->currentText();
        QString originName = util.Uart_Revert(curPortName);




        if(!serialPort->openPort(originName,BAUD9600,DATA_8,PAR_NONE,STOP_1,FLOW_OFF,60))
        {
            comstr += curPortName +"未接通!";
            ui->loopread->setCheckable(false);
            ui->cominfo->setText(comstr);
            qDebug()<<__LINE__<<comstr<<endl;
            return;
        }


        comstr == curPortName +"已接通!";
        comstr += "串口名:"+curPortName+"\r\n";
        comstr += "波特率:9600\r\n";
        comstr += "数据位:8\r\n";
        comstr += "停止位:1\r\n";
        comstr += "校验位:none\r\n";
        comstr += "数据进制:16\r\n";
        comstr += "通信超时:1min\r\n";


        qDebug()<<__LINE__<<comstr<<endl;
        ui->cominfo->setText(comstr);

        while(state)
        {

            QByteArray resByt = serialPort->read();


            qDebug()<<__LINE__<<"RECEIVE2:"<<resByt<<endl;
            str += ui->receiveEdit->toPlainText()+"\r\n";
            str += QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")+curPortName+"[R]:"+resByt+"\r\n";
            ui->receiveEdit->setPlainText(str);
        }



        serialPort->close();


    });

    for(QCheckBox *ck:typecks)
    {
        connect(ck,&QCheckBox::stateChanged,this,[=](int state)
        {
            if(state)
            {
                if(ck==ui->tmck1)
                {
                    ui->timesetframe->setCurrentWidget(ui->timeset1);
                }
                else if(ck==ui->tmck2)
                {
                    ui->timesetframe->setCurrentWidget(ui->timeset2);
                }
                else if(ck==ui->tmck3)
                {
                    ui->timesetframe->setCurrentWidget(ui->timeset3);
                }
                else if(ck==ui->tmck4)
                {
                    ui->timesetframe->setCurrentWidget(ui->timeset4);
                }
            }
        });
    }

    connect(ui->timesetframe,&QStackedWidget::currentChanged,this,&DeviceCMDCtrlDlg::onStackedCurrentChanged);

    connect(ui->saveset,&QPushButton::clicked,this,&DeviceCMDCtrlDlg::onSaveTimeset);

    connect(ui->resetset,&QPushButton::clicked,this,&DeviceCMDCtrlDlg::onResetTimeset);

    connect(ui->selAll,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            QWidget *w = ui->timesetframe->currentWidget();
            if(w==nullptr)
            {
                return;
            }
            qDebug()<<__LINE__<<w->objectName()<<endl;
        //    QGridLayout *layout = (QGridLayout *)w->layout();
            qDebug()<<__LINE__<<w->layout()->count()<<endl;
            for(int k = 0;k < w->layout()->count();++k)
            {
                QLayoutItem *item = w->layout()->itemAt(k);
                if(item->widget())
                {
                    QCheckBox *box = (QCheckBox *)item->widget();
        //            qDebug()<<__LINE__<<box->text()<<endl;

                    box->setChecked(Qt::Checked);

                }
            }
        }
        else
        {
            loadtmcksState();
        }
    });

}

void DeviceCMDCtrlDlg::onSendByTime()
{
    QString infoStr = curDT.toString("yyyy-MM-dd HH:mm:ss.zzz ")+"启动定时发送\r\n";

    QDateTime dtx = QDateTime::fromString(ui->timertk->text(),"yyyy-MM-dd HH:mm:ss");
    int hour = dtx.time().hour();
    qDebug()<<__LINE__<<hour<<endl;
    if(dtx.time().msec()==0&&dtx.time().second()==0&&dtx.time().minute()==0)
    {

        if(isSendBytime)
        {
            QFile file(QApplication::applicationDirPath()+DEVICE_CMD_INFO);
            if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
            {
                return ;
            }
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
                QString cmdKey = it.key();
                QJsonObject jSubObj = it.value().toObject();
                QString cmd = jSubObj.value("cmd").toString();
                QString seltms = jSubObj.value("select_times").toString();
                QStringList tmlist;
                if(seltms.split(",").count()>0)
                {
                    for(QString tm:seltms.split(","))
                    {
                        QString tm_num = tm.remove("点");
                        tmlist<<tm;
                    }

                }
                if(tmlist.contains(QString::number(hour)))
                {
                    infoStr += QString::number(hour)+"点--"+cmdKey+":"+cmd+"\r\n";
                    emit sendCMD(cmd);
                }

                it++;
            }


        }
    }

    QString oldStr = ui->receiveEdit->toPlainText()+"\r\n";
    ui->receiveEdit->setPlainText(oldStr+infoStr);
    writeloglocal(ui->receiveEdit->toPlainText());
}

void DeviceCMDCtrlDlg::onStackedCurrentChanged(int index)
{
    QWidget *widget = ui->timesetframe->currentWidget();
    if(widget==nullptr)
    {
        return;
    }
//    timeCheckInit(widget);
    ui->selAll->setChecked(false);
    qDebug()<<__LINE__<<"CURRENT PAGE:"<<ui->timesetframe->currentWidget()->objectName()<<endl;

}

void DeviceCMDCtrlDlg::onSaveTimeset()
{
    QJsonObject mainobj;

    for(int i=0;i<ui->timesetframe->count();++i)
    {
        QWidget *w = ui->timesetframe->widget(i);

        QString seltms;
        QStringList tmplist;
        QJsonObject subObj;
        if(w)
        {
            qDebug()<<__LINE__<<w->objectName()<<endl;
            qDebug()<<__LINE__<<w->layout()->count()<<endl;
            for(int k = 0;k < w->layout()->count();++k)
            {
                QLayoutItem *item = w->layout()->itemAt(k);
                if(item->widget())
                {
                    QCheckBox *box = (QCheckBox *)item->widget();
                    //            qDebug()<<__LINE__<<box->text()<<endl;
                    if(box->isChecked())
                    {
                        qDebug()<<__LINE__<<box->text()<<endl;
                        tmplist<<box->text();

                    }
                }
            }

            qDebug()<<__LINE__<<tmplist<<endl;

            for(int p =0;p<tmplist.count();++p)
            {
                seltms += tmplist[p];
                if(p<tmplist.count()-1)
                {
                    seltms += ",";
                }
            }

            QCheckBox *typeBox = typecks.at(i);
            if(typeBox)
            {
                QString seltype = typeBox->text();
                subObj.insert("cmd",list.at(i));
                subObj.insert("select_times",seltms);
                mainobj.insert(seltype,subObj);
            }
        }
    }

    QJsonDocument jDoc;
    jDoc.setObject(mainobj);

    QString filestr = QApplication::applicationDirPath()+DEVICE_CMD_INFO;

    QFile file(filestr);
    QDir dir;
    if(!file.exists())
    {
       QDir dir;
       dir.mkdir(QApplication::applicationDirPath()+"/docu");
    }
    file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate);
    if(file.write(jDoc.toJson()))
    {
        QMessageBox::information(this,"提示","定时时间设置已保存:\r\n"+filestr);

        file.flush();
        file.close();
    }

}

void DeviceCMDCtrlDlg::onResetTimeset()
{

    QWidget *w = ui->timesetframe->currentWidget();
    if(w==nullptr)
    {
        return;
    }
    qDebug()<<__LINE__<<w->objectName()<<endl;
//    QGridLayout *layout = (QGridLayout *)w->layout();
    qDebug()<<__LINE__<<w->layout()->count()<<endl;
    for(int k = 0;k < w->layout()->count();++k)
    {
        QLayoutItem *item = w->layout()->itemAt(k);
        if(item->widget())
        {
            QCheckBox *box = (QCheckBox *)item->widget();
//            qDebug()<<__LINE__<<box->text()<<endl;
            if(box->isChecked())
            {
                qDebug()<<__LINE__<<box->text()<<endl;
                box->setChecked(Qt::Unchecked);
            }
        }
    }
}

void DeviceCMDCtrlDlg::writeloglocal(QString text)
{
    QString path = "/home/rpdzkj/log.txt";
    QFile file(path);
    file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);

    QString str = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm")+text+"\r\n";
    file.write(str.toUtf8());
    file.flush();
    file.close();
}

//#ifdef Q_OS_LINUX
void DeviceCMDCtrlDlg::onReceiveCMD(QString cmd)
{
    qDebug()<<__LINE__<<"SEND:"<<cmd<<endl;
    QString curPortName = ui->curPort->currentText();
    QString originName = util.Uart_Convert(curPortName);

    if(!serialPort->openPort(originName,BAUD9600,DATA_8,PAR_NONE,STOP_1,FLOW_OFF,60))
    {
        comstr = curPortName+"("+originName+")" +"未接通!";
        ui->loopread->setCheckable(false);
        ui->cominfo->setText(comstr);
        qDebug()<<__LINE__<<comstr<<endl;
        return;
    }

//    muartThread = new UartThread;
//    if(!muartThread->initUart(originName,BAUD9600,3,5))
//    {
//        comstr = curPortName +"未接通!";
////        ui->loopread->setChecked(false);
//        ui->loopread->setCheckable(false);
//        ui->cominfo->setText(comstr);
//        qDebug()<<__LINE__<<comstr<<endl;
//        return;
//    }

    ui->loopread->setCheckable(true);



//    ui->btn_sendcmd->setText("执行");
    //串口已接通，波特率，数据位，停止位，校验位，数据进制，通信超时
    comstr = curPortName +"("+originName+")" +"已接通!";
    comstr += "串口名:"+curPortName+"\r\n";
    comstr += "波特率:9600\r\n";
    comstr += "数据位:8\r\n";
    comstr += "停止位:1\r\n";
    comstr += "校验位:none\r\n";
    comstr += "数据进制:16\r\n";
    comstr += "通信超时:1min\r\n";

    qDebug()<<__LINE__<<comstr<<endl;
    ui->cominfo->setText(comstr);

    if(!cmd.isEmpty())
    {
        QByteArray bytArr = QString2Hex(cmd.toLatin1().toUpper());
        int length = cmd.length();

        serialPort->write(QString2Hex(cmd));
//        muartThread->writeUart(QString2Hex(cmd));
        str = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")+curPortName+"[S]:"+bytArr.toHex(' ')+"\r\n";

    }





    QByteArray resByt = serialPort->read();

    qDebug()<<__LINE__<<"RECEIVE1:"<<resByt<<endl;

    str = ui->receiveEdit->toPlainText();
    str += QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")+curPortName+"[R]:"+resByt+"\r\n";


    ui->receiveEdit->setPlainText(str);



    serialPort->close();



}

//#endif

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

void DeviceCMDCtrlDlg::loadFactorAndPort()
{
    httpclinet pClient;
    QJsonObject jDeviceFac,jDevice;

    ui->curDevice->clear();

    pClient.get(DCM_DEVICE,jDevice);
    QMap<QString,QString> deviceportmap;

    if(pClient.get(DCM_DEVICE_FACTOR,jDeviceFac))
    {
        QString portName;
        QJsonObject::iterator it = jDeviceFac.begin();
        while(it != jDeviceFac.end())
        {
            QString key = it.key();
            if(key.split("-").count()==2)
            {
                QString deviceid = key.split("-")[0];
                QString faccode = key.split("-")[1];
                QString fullname = faccode + "-"+ namemap[faccode];
                qDebug()<<__LINE__<<deviceid<<fullname<<endl;

                if(jDevice.count()>0)
                {
                    QJsonObject devObj = jDevice.value(deviceid).toObject();
                    QString oriName = devObj.value("com").toString();
                    portName = util.Uart_Revert(oriName);
                    qDebug()<<__LINE__<<oriName<<"==>"<<portName<<endl;
                    deviceportmap.insert(deviceid,portName);
                }

                ui->curDevice->addItem(fullname,deviceid);


            }
            it++;
        }

        qDebug()<<__LINE__<<deviceportmap<<endl;

        connect(ui->curDevice,&QComboBox::currentTextChanged,this,[=](const QString &text)
        {
            int curIndex;
            for(int i=0;i<ui->curDevice->count();++i)
            {
                if(text == ui->curDevice->itemText(i))
                {
                    curIndex = i;
                    break;
                }
            }

            QString devid = ui->curDevice->itemData(curIndex).toString();
            QString port = deviceportmap[devid];

            ui->curPort->setCurrentText(port);
        });

        ui->curDevice->setCurrentIndex(0);
    }
}

void DeviceCMDCtrlDlg::onReceivecurDT(QDateTime &dt)
{
    curDT = dt;
//    qDebug()<<__LINE__<<__FUNCTION__<<dt.toString("yyyy-MM-dd HH:mm:ss")<<endl;
    if(!ui->test_sendintime->isChecked())
        ui->timertk->setText(dt.toString("yyyy-MM-dd HH:mm:ss"));
    else
    {

        ui->timertk->setText("2024-01-17 14:58:00");
        QTimer timer0;
        timer0.setInterval(1000);
        timer0.start();
//        timer0.setSingleShot(true);
        connect(&timer0,&QTimer::timeout,this,&DeviceCMDCtrlDlg::onTimeout);

    }


}

void DeviceCMDCtrlDlg::onTimeout()
{
    QDateTime dt = QDateTime::fromString(ui->timertk->text(),"yyyy-MM-dd HH:mm:ss");
    QDateTime dt1 = dt.addSecs(1);
    ui->timertk->setText(dt1.toString("yyyy-MM-dd HH:mm:ss"));
}
