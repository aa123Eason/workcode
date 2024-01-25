#include "widget.h"
#include "ui_widget.h"

QStringList availblePorts;
QJsonObject portsStateObj;
QMap<QString,QString> faccommap;
bool testModeState = false;
QMap<QString,bool> portExistStateMap;
QMap<QString,QString> modbusindexmap;
QMap<QString,SerialPort *> commMap;
QString g_curFacid;

static char ConvertHexChar(char c);

static QByteArray QString2Hex(QString hexStr);

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    connectevent();
    init();

}

Widget::~Widget()
{


    if(m_SignalMapper_Dele) m_SignalMapper_Dele->deleteLater();
    if(m_SignalMapper_Edit) m_SignalMapper_Edit->deleteLater();

    m_httpWorker->stopWork();
    m_thread.quit();
    /* 阻塞线程 2000ms，判断线程是否结束 */
    if (m_thread.wait(2000)) {
        qDebug()<<"自动监控线程结束";
    }

    m_httpWorkerRW->stopWork();
    m_threadRW.quit();
    /* 阻塞线程 2000ms，判断线程是否结束 */
    if (m_threadRW.wait(2000)) {
        qDebug()<<"自动监控线程结束";
    }




    delete ui;
}


void Widget::init()
{
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    device_init();
    widget_init();
}

void Widget::widget_init()
{
    QString mainTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ui->time->setText(mainTime);
    timer.setInterval(1000);
    timer.start();


    headerfont.setBold(true);
    headerfont.setPointSize(16);

    itemfont.setBold(true);
    itemfont.setPointSize(20);

    ckfont.setBold(true);
    ckfont.setPointSize(18);

    ui->time->setFont(itemfont);

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

    page_deviceStatueInit();
    page_deviceEditInit();
    page_deviceAddInit();
    page_deviceLogInit();
}


void Widget::page_deviceStatueInit()
{
    setTableHeader();

}

void Widget::page_deviceEditInit()
{
    ui->editcomBox->clear();
    ui->editcomBox->addItems(portsList);
    ui->editfacBox->clear();
    qDebug()<<"faccommap==>"<<faccommap<<endl;
    for(int i=0;i<faccommap.count();++i)
    {
        QString key = faccommap.keys()[i];
        QString dev,fac;
        if(key.split("-").count()==3)
        {
            dev = key.split("-")[0];
            fac = key.split("-")[1]+"-"+key.split("-")[2];

            ui->editfacBox->addItem(fac,dev);

        }
    }

    ui->editfacBox->setCurrentIndex(0);
    ui->editcmdinfo->setWordWrap(true);

    edittypecks.clear();
    edittypecks.append(ui->editcmd1);
    edittypecks.append(ui->editcmd2);
    edittypecks.append(ui->editcmd3);
    edittypecks.append(ui->editcmd4);
    qDebug()<<__LINE__<<"Edit CMD:"<<edittypecks.count()<<endl;

    timeCheckInit(edittimecks,el1,ui->ep1);
    timeCheckInit(edittimecks,el2,ui->ep2);
    timeCheckInit(edittimecks,el3,ui->ep3);
    timeCheckInit(edittimecks,el4,ui->ep4);

    for(QCheckBox *ck:edittypecks)
    {
        connect(ck,&QCheckBox::stateChanged,this,[=](int state)
        {
            if(state)
            {
                qDebug()<<__LINE__<<"cke:"<<ck->text()<<endl;
                if(ck==ui->editcmd1)
                {
                    ui->edittimeframe->setCurrentWidget(ui->ep1);
                }
                else if(ck==ui->editcmd2)
                {
                    ui->edittimeframe->setCurrentWidget(ui->ep2);
                }
                else if(ck==ui->editcmd3)
                {
                    ui->edittimeframe->setCurrentWidget(ui->ep3);
                }
                else if(ck==ui->editcmd4)
                {
                    ui->edittimeframe->setCurrentWidget(ui->ep4);
                }
            }
        });
    }

    ui->editcmd1->setChecked(true);

    connect(ui->editSelAll,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            QWidget *w = ui->edittimeframe->currentWidget();
            if(w)
            {
                QList<QCheckBox *> cks = w->findChildren<QCheckBox *>();
                for(QCheckBox *box:cks)
                {
                    box->setChecked(true);
                }

            }
        }
        else
        {
            loadtmcksState(ui->editcomBox->currentText());
        }
    });

//    connect(ui->edittimeframe,&QStackedWidget::currentChanged,this,[=](int index)
//    {
//        QWidget *w = ui->edittimeframe->currentWidget();
//        if(w)
//        {
//            QList<QCheckBox *> cks = w->findChildren<QCheckBox *>();
//            QList<QCheckBox *>::iterator ck = cks.begin();
//            while(ck!=cks.end())
//            {
//                if(ck.i->t())
//                {
//                    if(!ck.i->t()->isChecked())
//                    {
//                        ui->editSelAll->setChecked(false);
//                        return;
//                    }
//                }

//                ck++;
//            }
//            ui->editSelAll->setChecked(true);
//        }
//    });

    ui->editcomBox->setEnabled(false);
    ui->editfacBox->setEnabled(false);
    ui->e_modbusindex->setEnabled(false);

}

void Widget::page_deviceAddInit()
{
    ui->btn_add->hide();
    ui->addcomBox->clear();
    ui->addcomBox->addItems(portsList);
    ui->addfacBox->clear();

    for(int i=0;i<faccommap.count();++i)
    {
        QString key = faccommap.keys()[i];
        QString dev,fac;
        if(key.split("-").count()==3)
        {
            dev = key.split("-")[0];
            fac = key.split("-")[1]+"-"+key.split("-")[2];

            ui->addfacBox->addItem(fac,dev);

        }
    }

    ui->addfacBox->setCurrentIndex(0);
    ui->addcmdinfo->setWordWrap(true);


    addtypecks.clear();
    addtypecks.append(ui->addcmd1);
    addtypecks.append(ui->addcmd2);
    addtypecks.append(ui->addcmd3);
    addtypecks.append(ui->addcmd4);

    qDebug()<<__LINE__<<"Add CMD:"<<edittypecks.count()<<endl;


    timeCheckInit(addtimecks,al1,ui->ap1);
    timeCheckInit(addtimecks,al2,ui->ap2);
    timeCheckInit(addtimecks,al3,ui->ap3);
    timeCheckInit(addtimecks,al4,ui->ap4);

    for(QCheckBox *ck:addtypecks)
    {
        connect(ck,&QCheckBox::stateChanged,this,[=](int state)
        {
            if(state)
            {
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

    ui->addcmd1->setChecked(true);

    connect(ui->addSelAll,&QCheckBox::stateChanged,this,[=](int state)
    {
        QWidget *w = ui->addTimeframe->currentWidget();
        if(w)
        {
            QList<QCheckBox *> cks = w->findChildren<QCheckBox *>();
            for(QCheckBox *box:cks)
            {
                box->setChecked(state);
            }

        }
    });

    connect(ui->addTimeframe,&QStackedWidget::currentChanged,this,[=](int index)
    {
        QWidget *w = ui->addTimeframe->currentWidget();
        if(w)
        {
            QList<QCheckBox *> cks = w->findChildren<QCheckBox *>();
            QList<QCheckBox *>::iterator ck = cks.begin();
            while(ck!=cks.end())
            {
                if(ck.i->t())
                {
                    if(!ck.i->t()->isChecked())
                    {
                        ui->addSelAll->setChecked(false);
                        return;
                    }
                }

                ck++;
            }
            ui->addSelAll->setChecked(true);
        }
    });

}

void Widget::page_deviceLogInit()
{

}


void Widget::device_init()
{
    facnameMap = util.Uart_facnameMatch();

    m_SignalMapper_Edit = new QSignalMapper(this);
    m_SignalMapper_Dele = new QSignalMapper(this);

    // 1.新建串口处理子线程
    m_httpWorker = new CHttpWork();

    // 将串口和子类一同移入子线程
    m_httpWorker->moveToThread(&m_thread);

    // 2.连接信号和槽
    connect(&m_thread, &QThread::finished,
            m_httpWorker, &QObject::deleteLater);           // 线程结束，自动删除对象
    connect(&m_thread, SIGNAL(finished()),
            &m_thread, SLOT(deleteLater()));
    connect(this, &Widget::startWork,
            m_httpWorker, &CHttpWork::doWork);   // 主线程串口数据发送的信号

    /* 接收到 worker 发送过来的信号 */
    connect(m_httpWorker, &CHttpWork::resultReady,
            this, &Widget::handleResults);
    connect(m_httpWorker,&CHttpWork::sendisExist,this,[=](QStringList &ports){

            for(int i=0;i<ui->device_table->rowCount();++i)
            {
                if(ui->device_table->item(i,2))
                {
                    QString refPort = ui->device_table->item(i,2)->text();
                    portExistStateMap.insert(refPort,ports.contains(refPort));

                }

            }

    });


    m_httpWorker->sendResultReady();

    /* 判断线程是否在运行 */
    if(!m_thread.isRunning()) {
        /* 开启线程 */
        m_thread.start();
    }


    m_httpWorkerRW = new CHttpWork();

    // 将串口和子类一同移入子线程
    m_httpWorkerRW->moveToThread(&m_threadRW);

    // 2.连接信号和槽
    connect(&m_threadRW, &QThread::finished,
            m_httpWorkerRW, &QObject::deleteLater);           // 线程结束，自动删除对象
    connect(&m_threadRW, SIGNAL(finished()),
            &m_threadRW, SLOT(deleteLater()));
    connect(this, &Widget::startWork,
            m_httpWorkerRW, &CHttpWork::rwWork);   // 主线程串口数据发送的信号

    /* 接收到 worker 发送过来的信号 */
    connect(m_httpWorkerRW, &CHttpWork::sendCMD,
            this, &Widget::onSlotRW);


    m_httpWorkerRW->sendResultReady();

    /* 判断线程是否在运行 */
    if(!m_threadRW.isRunning()) {
        /* 开启线程 */
        m_threadRW.start();
    }



    /* 发送正在运行的信号，线程收到信号后执行后返回线程耗时函数 + 此字符串 */
    emit this->startWork();



}

void Widget::connectevent()
{
    connect(&timer,&QTimer::timeout,this,[=]()
    {
        QString curTime = ui->time->text();
        QDateTime curDT = QDateTime::fromString(curTime,"yyyy-MM-dd HH:mm:ss");
        ui->time->setText(curDT.addSecs(1).toString("yyyy-MM-dd HH:mm:ss"));

    });

    connect(ui->btn_devSta,&QPushButton::clicked,this,[=]()
    {
        ui->mainStack->setCurrentWidget(ui->devicestaus);
    });

    connect(ui->btn_devLog,&QPushButton::clicked,this,[=]()
    {
        ui->mainStack->setCurrentWidget(ui->ctrllog);
    });

    connect(ui->close,&QPushButton::clicked,this,[=](){
        this->close();
    });

    connect(ui->btn_add,&QPushButton::clicked,this,[=]()
    {
        ui->mainStack->setCurrentWidget(ui->deviceadd);
    });

    connect(ui->reback,&QPushButton::clicked,this,[=]()
    {
        ui->mainStack->setCurrentWidget(ui->devicestaus);
    });

    connect(ui->addreback,&QPushButton::clicked,this,[=]()
    {
        ui->mainStack->setCurrentWidget(ui->devicestaus);
    });

    connect(ui->btn_refresh,&QPushButton::clicked,this,[=]()
    {
        if(m_httpWorker)
            m_httpWorker->sendResultReady();
    });

    connect(ui->mainStack,&QStackedWidget::currentChanged,this,[=](int index)
    {
        if(index == ui->mainStack->indexOf(ui->devicestaus))
        {
            if(m_httpWorker)
                m_httpWorker->sendResultReady();
        }
    });


    connect(this,&Widget::sendfacid,this,[=](QString &facid)
    {
        qDebug()<<__LINE__<<facid<<faccommap[facid]<<endl;

        QString dev,fac;
        if(facid.split("-").count()==3)
        {
            dev = facid.split("-")[0];
            fac = facid.split("-")[1]+"-"+facid.split("-")[2];
            ui->editfacBox->setCurrentText(fac);
        }
        qDebug()<<__LINE__<<modbusindexmap<<endl;
        ui->e_modbusindex->setText(modbusindexmap[facid]);

    });

    connect(ui->editfacBox,&QComboBox::currentTextChanged,this,[=](const QString &text)
    {
        QString dev;
        dev = ui->editfacBox->currentData().toString();
        QString key = dev+"-"+text;
        qDebug()<<__LINE__<<key<<faccommap[key]<<endl;
        ui->editcomBox->setCurrentText(faccommap[key]);
        ui->editcmdinfo->setText(key+":"+faccommap[key]);

    });

    connect(ui->addfacBox,&QComboBox::currentTextChanged,this,[=](const QString &text)
    {
        QString dev;
        dev = ui->addfacBox->currentData().toString();
        QString key = dev+"-"+text;
        qDebug()<<__LINE__<<key<<faccommap[key]<<endl;
        ui->addcomBox->setCurrentText(faccommap[key]);
        ui->addcmdinfo->setText(key+":"+faccommap[key]);

    });

    connect(ui->save,&QPushButton::clicked,this,&Widget::onSaveTimeset);
    connect(ui->testMode,&QCheckBox::stateChanged,this,[=](int state)
    {
        testModeState = state;
    });

    connect(ui->clearContent,&QPushButton::clicked,ui->msginfo,&QTextBrowser::clear);


}

void Widget::setTableHeader()
{
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#002570;color:#ffffff}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC; height:30px;  border:1px solid #001014; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #323232); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
//连接状态，因子名称，端口名，操作
headerText << QStringLiteral("连接状态")  << QStringLiteral("因子名称") << QStringLiteral("端口名")<< QStringLiteral("modbus序号")<<QStringLiteral("操作");
int cnt = headerText.count();
ui->device_table->setColumnCount(cnt);
ui->device_table->setHorizontalHeaderLabels(headerText);
ui->device_table->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->device_table->horizontalHeader()->setStretchLastSection(true); //行头自适应表格
int widths[] = {120, 200, 100,150,160};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->device_table->setColumnWidth(i, widths[i]);
}

ui->device_table->horizontalHeader()->setFont(headerfont);
ui->device_table->setStyleSheet(qssTV);
ui->device_table->setWordWrap(true);
ui->device_table->horizontalHeader()->setVisible(true);
ui->device_table->verticalHeader()->setVisible(false);
ui->device_table->setFrameShape(QFrame::NoFrame);
ui->device_table->setTextElideMode(Qt::ElideNone);
ui->device_table->setShowGrid(false);
//ui->device_table->resizeRowsToContents();

}

void Widget::setTableContent(const QJsonObject &pDevice,const QJsonObject &pFactor)
{
    qDebug()<<__LINE__<<"Device:"<<pDevice<<endl;
    qDebug()<<__LINE__<<"Factor:"<<pFactor<<endl;

    QString stylesheet = ":/images/images/checkboxstyle.qss";
    QFile file(stylesheet);
    QByteArray bytSty;
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        bytSty.append(file.readAll());
    }
    ui->device_table->setRowCount(0);
    faccommap.clear();
    disconnect(m_SignalMapper_Edit,SIGNAL(mapped(QString)),this,SLOT(onButtonEdit(QString)));
    disconnect(m_SignalMapper_Dele,SIGNAL(mapped(QString)),this,SLOT(onButtonDele(QString)));

    QJsonObject::const_iterator itDevice = pDevice.begin();
    while(itDevice != pDevice.end())
    {
        qDebug()<<__LINE__<<itDevice.key()<<endl;
        QString device_id = itDevice.key();
        QJsonObject device_info = itDevice.value().toObject();
        QString device_com = util.Uart_Revert(device_info.value("com").toString());

        int row = 0;
        QJsonObject::const_iterator itFactor = pFactor.begin();
        while(itFactor != pFactor.end())
        {
            qDebug()<<__LINE__<<device_id<<"vs"<<itFactor.key()<<endl;
            if(itFactor.key().contains(device_id))
            {

                QJsonObject fac_info = itFactor.value().toObject();

                QString fac_code = fac_info.value("factor_code").toString();
                QString fac_name = facnameMap[fac_code];
                QString fac_FullName = fac_code + "-" + fac_name;
                faccommap.insert(device_id+"-"+fac_FullName,device_com);

                QString modbus_index = QString::number(fac_info.value("modbus_index").toInt());
                modbusindexmap.insert(device_id+"-"+fac_FullName,modbus_index);
                //                qDebug()<<__LINE__<<fac_FullName<<modbus_index<<endl;
                ui->device_table->insertRow(row);

                //state
                QPixmap pixmap;
                if(portsStateObj.contains(device_com))
                {
                    if(portsStateObj.value(device_com).toBool())
                    {
                        pixmap = QPixmap(STATE_ON);
                    }
                    else
                    {
                        pixmap = QPixmap(STATE_OFF);
                    }
                }
                else
                {
                    pixmap = QPixmap(STATE_OFF);
                }
                QLabel *itemState = new QLabel();
                itemState->setPixmap(pixmap.scaled(20,20));
                itemState->setAlignment(Qt::AlignCenter);
                ui->device_table->setCellWidget(row,0,itemState);


                //name
                QTableWidgetItem *itemName = new QTableWidgetItem();
                QFont nameFont;
                nameFont.setBold(true);
                nameFont.setPointSize(14);
                if(fac_FullName.split("-").count()>1)
                {
                    if(fac_FullName.split("-")[1].length()>5)
                    {
                        itemName->setFont(nameFont);
                        ui->device_table->setColumnWidth(1,300);
                    }
                    else
                    {
                        itemName->setFont(itemfont);
                        ui->device_table->setColumnWidth(1,200);
                    }
                }
                else
                {
                    itemName->setFont(itemfont);
                    ui->device_table->setColumnWidth(1,200);
                }

                itemName->setText(fac_FullName);
                itemName->setTextAlignment(Qt::AlignCenter);
                ui->device_table->setItem(row,1,itemName);

                //port
                QTableWidgetItem *itemPort = new QTableWidgetItem();
                itemPort->setFont(itemfont);
                itemPort->setText(device_com);
                itemPort->setTextAlignment(Qt::AlignCenter);
                ui->device_table->setItem(row,2,itemPort);

                //modbus_index
                QTableWidgetItem *itemIndex = new QTableWidgetItem();
                itemIndex->setFont(itemfont);
                itemIndex->setText(modbus_index);
                itemIndex->setTextAlignment(Qt::AlignCenter);
                ui->device_table->setItem(row,3,itemIndex);

//                //test mode
//                QCheckBox *textMode = new QCheckBox();
//                textMode->setStyleSheet(bytSty);
//                ui->device_table->setCellWidget(row,4,textMode);
//                connect(textMode,&QCheckBox::stateChanged,this,[=](int state)
//                {

//                });

                //operate
                QPushButton *pOperEdit = new QPushButton();
                pOperEdit->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
                pOperEdit->setFont(itemfont);
                pOperEdit->setText("详情");

                QPushButton *pOperDele = new QPushButton();
                pOperDele->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
                pOperDele->setFont(itemfont);
                pOperDele->setText("删除");
                pOperDele->hide();

                connect(pOperEdit,SIGNAL(clicked()),m_SignalMapper_Edit,SLOT(map()));
                m_SignalMapper_Edit->setMapping(pOperEdit,device_id+"-"+fac_FullName);
                connect(pOperDele,SIGNAL(clicked()),m_SignalMapper_Dele,SLOT(map()));
                m_SignalMapper_Dele->setMapping(pOperDele,device_com);

                QWidget *btnWidget = new QWidget();
                QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);    // FTIXME：内存是否会随着清空tablewidget而释放
                btnLayout->addWidget(pOperEdit,1);
                btnLayout->addWidget(pOperDele,1);
                btnLayout->setMargin(2);
                btnLayout->setAlignment(Qt::AlignCenter);
                ui->device_table->setCellWidget(row, 4, btnWidget);

                ui->device_table->setRowHeight(row,64);
            }

            row++;
            itFactor++;
        }



        itDevice++;
    }
    bytSty.clear();
    connect(m_SignalMapper_Edit,SIGNAL(mapped(QString)),this,SLOT(onButtonEdit(QString)));
    connect(m_SignalMapper_Dele,SIGNAL(mapped(QString)),this,SLOT(onButtonDele(QString)));
}

void Widget::timeCheckInit(QList<QCheckBox *> &timecks,QGridLayout &timeset,QWidget *w)
{
    QString stylesheet = ":/images/images/checkboxstyle.qss";
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
        box->setFont(ckfont);
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


void Widget::loadtmcksState(QString porName)
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
        QString portName = it.key();
        if(porName == portName)
        {
            QJsonObject portObj = it.value().toObject();
            QJsonObject::iterator itck = portObj.begin();
            while(itck != portObj.end())
            {
                QString cmdName = itck.key();
                QJsonObject valueObj = itck.value().toObject();
                QString seltms = valueObj.value("select_times").toString();
                qDebug()<<__LINE__<<itck.key()<<seltms<<endl;
                QStringList tmlist;
                int index;
                for(QCheckBox *box:edittypecks)
                {
                    if(box)
                    {

                        if(box->text() == cmdName)
                        {

                            index = edittypecks.indexOf(box);
                            qDebug()<<__LINE__<<index<<box->text()<<cmdName<<endl;

                        }
                    }
                }

                if(seltms.split(",").count()>0)
                {
                    tmlist = seltms.split(",");
                }

                QWidget *w = ui->edittimeframe->widget(index);
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



//slot function
void Widget::handleResults(const QJsonObject &pDevice,const QJsonObject &pFactor)
{


    setTableContent(pDevice,pFactor);

}

void Widget::handleDateTimeout()
{

}

void Widget::onSaveTimeset()
{
    QJsonObject mainobj;

    for(int i=0;i<ui->edittimeframe->count();++i)
    {
        QWidget *w = ui->edittimeframe->widget(i);

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

            QCheckBox *typeBox = edittypecks.at(i);
            if(typeBox)
            {
                QString seltype = typeBox->text();
                subObj.insert("cmd",cmdlist.at(i));
                subObj.insert("select_times",seltms);
                mainobj.insert(seltype,subObj);
            }
        }
    }

    QJsonObject jItemObj;
    jItemObj.insert(ui->editcomBox->currentText(),mainobj);

    qDebug()<<__LINE__<<jItemObj<<endl;

    QString filestr = QApplication::applicationDirPath()+CMDINFO;

    QJsonObject jObjR;

    //read
    QFile fileR(filestr);
    if(fileR.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QByteArray bytR;
        bytR.append(fileR.readAll());
        fileR.flush();
        fileR.close();

        QJsonDocument jDocR = QJsonDocument::fromJson(bytR);
        bytR.clear();
        jObjR = jDocR.object();
        qDebug()<<__LINE__<<jObjR<<endl;
        if(jObjR.contains(ui->editcomBox->currentText()))
        {
            jObjR.remove(ui->editcomBox->currentText());

        }
        jObjR.insert(ui->editcomBox->currentText(),mainobj);
    }
    qDebug()<<__LINE__<<jObjR<<endl;

    //write
    QJsonDocument jDoc;
    jDoc.setObject(jObjR);
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


void Widget::onSlotRW(PORT_STYLE style,QMap<QString,SerialPort *> &mapcom)
{
    QString infoStr = ui->time->text();
    QFile file(QApplication::applicationDirPath()+CMDINFO);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return ;
    }

    QByteArray bytArr;
    bytArr.append(file.readAll());
    file.flush();
    QJsonDocument jDoc = QJsonDocument::fromJson(bytArr);
    bytArr.clear();
    QJsonObject jObj = jDoc.object();

    QMap<QString,SerialPort *>::iterator it = mapcom.begin();
    while(it != mapcom.end())
    {
        QString curPortName = it.key();
        QString originName = util.Uart_Convert(curPortName);
        QJsonObject jPort = jObj.value(curPortName).toObject();
        SerialPort *curPort = it.value();

        logStr = curPortName +"("+originName+")" +"已接通!";
        logStr += "波特率:9600,";
        logStr += "数据位:8,";
        logStr += "停止位:1,";
        logStr += "校验位:none,";
        logStr += "数据进制:16,";
        logStr += "通信超时:1min;";
#pragma region SENDCMD{
            if(style == PORT_WRITE)
            {


                QDateTime dtx = QDateTime::fromString(infoStr,"yyyy-MM-dd HH:mm:ss");
                int hour = dtx.time().hour();
                int min = dtx.time().minute();
                int sec = dtx.time().second();
                int msec = dtx.time().msec();

                bool testConf1,testConf2;
                if(!testModeState)
                {
                    testConf1 = (msec==0)&&(sec==0)&&(min==0);
                    testConf2 = hour;
                }
                else
                {
                    testConf1 = (msec==0)&&(sec==0);
                    testConf2 = min;
                }

                if(testConf1)
                {
                    QJsonObject::iterator it_port = jPort.begin();
                    while(it_port != jPort.end())
                    {
                        QString cmdKey = it_port.key();
                        QJsonObject jSubObj = it_port.value().toObject();
                        QString cmd = jSubObj.value("cmd").toString();
                        QString seltms = jSubObj.value("select_times").toString();
                        QStringList tmlist;
                        if(seltms.split(",").count()>0)
                        {
                            for(QString tm:seltms.split(","))
                            {
//                                QString tm_num = tm.remove("点");
                                tmlist<<tm;
                            }

                        }
                        if(tmlist.contains(QString::number(testConf2)))
                        {

                            if(!cmd.isEmpty())
                            {
                                QByteArray bytArr = QString2Hex(cmd.toLatin1().toUpper());
                                int length = cmd.length();
                                curPort->write(QString2Hex(cmd));
                                logStr += infoStr+"[S]"+curPortName+":"+cmdKey+"-"+cmd+"\r\n";
                            }

                        }

                        it_port++;
                    }
                }

            }
#pragma endregion SENDCMD}

#pragma region RECEICMD{
            if(style == PORT_READ)
            {
                connect(curPort,&SerialPort::hasdata,this,[=](QByteArray &bytR)
                {
                    QString recStr = QString(bytR);
                    logStr += infoStr+"[R]"+curPortName+":"+recStr+"\r\n";
                });

            }
#pragma endregion RECEICMD}

            it++;
        }

        ui->msginfo->setText(logStr);
        ui->msginfo->setFont(itemfont);


}

void Widget::onButtonDele(QString id)
{
    qDebug()<<__LINE__<<__FUNCTION__<<id<<endl;

    for(int i=0;i<ui->device_table->rowCount();++i)
    {
        if(ui->device_table->item(i,2))
        {
            if(id == ui->device_table->item(i,2)->text())
            {
                ui->device_table->removeRow(i);
            }
        }
    }


}

void Widget::onButtonEdit(QString id)
{
    qDebug()<<__LINE__<<__FUNCTION__<<id<<endl;
    g_curFacid = id;
    QString portName = faccommap[g_curFacid];
    qDebug()<<__LINE__<<"map:"<<g_curFacid<<portName<<endl;

    loadtmcksState(portName);

    ui->mainStack->setCurrentWidget(ui->deviceedit);
    emit sendfacid(g_curFacid);
}




//httpwork function

CHttpWork::CHttpWork(QObject *parent)
        :QObject(parent)
{

}


void CHttpWork::sendResultReady()
{
    // 获取实时数据
    QJsonObject pDevice,pFactor;
    httpclinet hClient;
    if(!hClient.get(DCM_DEVICE,pDevice))
    {
//        QMessageBox::warning(this,"提示","远端设备同步失败");
        return;
    }


    if(!hClient.get(DCM_DEVICE_FACTOR,pFactor))
    {
//        QMessageBox::warning(this,"提示","远端设备同步失败");
        return;
    }

    availblePorts = getAvailblePorts(pDevice);
    checkPortExistInTable(availblePorts);
    portsStateObj = checkPortState(availblePorts);


    emit resultReady(pDevice,pFactor);

}

QStringList CHttpWork::getAvailblePorts(const QJsonObject &pDevice)
{
    QStringList list;
    QJsonObject::const_iterator it = pDevice.begin();
    while(it != pDevice.end())
    {
        QJsonObject device_info = it.value().toObject();
        list<<util.Uart_Revert(device_info.value("com").toString());
        it++;
    }

    return list;
}

QJsonObject CHttpWork::checkPortState(QStringList ports)
{
    QJsonObject jObj;
    for(QString port:ports)
    {
        SerialPort *s = new SerialPort();
        QString oriname = util.Uart_Convert(port);
        bool isConnected = s->openPort(oriname,BAUD9600,DATA_8,PAR_NONE,STOP_1,FLOW_OFF,60);
        jObj.insert(port,isConnected);
        if(isConnected)
            commMap.insert(port,s);
    }



    qDebug()<<__LINE__<<__FUNCTION__<<jObj<<endl;
    return jObj;

}

void CHttpWork::checkPortExistInTable(QStringList &ports)
{
    emit sendisExist(ports);

    QMap<QString,bool>::iterator it = portExistStateMap.begin();
    while(it != portExistStateMap.end())
    {
        QString refport = it.key();
        bool isExit = it.value();
        if(ports.contains(refport))
        {
            int index = ports.indexOf(refport);
            if(!isExit)
            {
                ports.removeAt(index);
            }
        }
        it++;
    }

}




void CHttpWork::doWork() {



    /* 标志位为真 */
    isCanRun = true;

    /* 死循环 */
    while (isCanRun) {
        /* 此{}作用是QMutexLocker与lock的作用范围，获取锁后，
         * 运行完成后即解锁 */
        {
            QMutexLocker locker(&lock);
            /* 如果标志位不为真 */
            if (!isCanRun) {
                /* 跳出循环 */
                break;
            }
        }


        // 获取实时数据
        QJsonObject pDevice,pFactor;
        httpclinet hClient;
        if(!hClient.get(DCM_DEVICE,pDevice))
        {
    //        QMessageBox::warning(this,"提示","远端设备同步失败");
            return;
        }


        if(!hClient.get(DCM_DEVICE_FACTOR,pFactor))
        {
    //        QMessageBox::warning(this,"提示","远端设备同步失败");
            return;
        }


        availblePorts = getAvailblePorts(pDevice);
        checkPortExistInTable(availblePorts);
        portsStateObj = checkPortState(availblePorts);




        emit resultReady(pDevice,pFactor);


        /* 使用QThread里的延时函数，当作一个普通延时 */
        QThread::sleep(REALTIME_FLUSH_PERIOD);



    }
}

void CHttpWork::rwWork()
{
    /* 标志位为真 */
    isCanRun = true;

    /* 死循环 */
    while (isCanRun) {
        /* 此{}作用是QMutexLocker与lock的作用范围，获取锁后，
         * 运行完成后即解锁 */
        {
            QMutexLocker locker(&lock);
            /* 如果标志位不为真 */
            if (!isCanRun) {
                /* 跳出循环 */
                break;
            }
        }

        emit sendCMD(PORT_WRITE,commMap);
        emit sendCMD(PORT_READ,commMap);
        /* 使用QThread里的延时函数，当作一个普通延时 */
        QThread::sleep(1);



    }
}

