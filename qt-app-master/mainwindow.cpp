#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextCodec>
#include <QListView>

QJsonObject g_Dcm_Factor;
QJsonArray g_Dcm_SystemCode;
QJsonObject g_Dcm_SupportDevice;
QJsonObject g_ConfObjDevParam;
QString g_Device_ID;
bool g_IsAnalogDevOperated = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnBottomHint);
    setWindowTitle(QTAPP_VER);

    setMaximumSize(1280,800);
    setMinimumSize(1280,800);

    m_SignalMapper = new QSignalMapper(this);
    m_SignalMapper_Te = new QSignalMapper(this);
    m_SignalMapper_Ted = new QSignalMapper(this);
    m_SignalMapper_Up = new QSignalMapper(this);
    m_SignalMapper_Upd = new QSignalMapper(this);

    m_SignalMapper_DevD = new QSignalMapper(this);
    m_SignalMapper_DevM = new QSignalMapper(this);
    m_SignalMapper_DevF = new QSignalMapper(this);

    m_SignalMapper_FaDele = new QSignalMapper(this);
    m_SignalMapper_FaEdit = new QSignalMapper(this);

    ConfFile_Init();

    // 组件初始化
    Widget_Init();

    m_pDateTimer = new QTimer(this);
    m_pDateTimer->setInterval(100);
    connect(m_pDateTimer, SIGNAL(timeout()), this, SLOT(handleDateTimeout()));
    m_pDateTimer->start();

//    QJsonObject pJsonObj;
//    httpclinet hCLient;
//    if(hCLient.get(DCM_REALTIME_DATA,pJsonObj))
//    {
//        handleResults("realtime_data",pJsonObj);
//    }

//    if(hCLient.get(DCM_COONECT_STAT,pJsonObj))
//    {
//        handleResults("connect_stat",pJsonObj);
//    }

    // 1.新建串口处理子线程
    m_httpWorker = new CHttpWork();

    // 将串口和子类一同移入子线程
    m_httpWorker->moveToThread(&m_thread);

    // 2.连接信号和槽
    connect(&m_thread, &QThread::finished,
            m_httpWorker, &QObject::deleteLater);           // 线程结束，自动删除对象
    connect(&m_thread, SIGNAL(finished()),
            &m_thread, SLOT(deleteLater()));
    connect(this, &MainWindow::startWork,
            m_httpWorker, &CHttpWork::doWork1);   // 主线程串口数据发送的信号

    /* 接收到 worker 发送过来的信号 */
    connect(m_httpWorker, SIGNAL(resultReady(QString,QJsonObject)),
            this, SLOT(handleResults(QString,QJsonObject)));

    /* 判断线程是否在运行 */
    if(!m_thread.isRunning()) {
        /* 开启线程 */
        m_thread.start();
    }

    /* 发送正在运行的信号，线程收到信号后执行后返回线程耗时函数 + 此字符串 */
    emit this->startWork();

    map = util.Uart_devicetype();
    namemap = util.Uart_devicetypeNameMatch();

    this->installEvents();
    connect(ui->buttonVer,SIGNAL(clicked()),this,SLOT(OpenSysInfo()));
    connect(ui->interacess,SIGNAL(clicked()),this,SLOT(OpenNeworkSetting()));
    connect(ui->Com,SIGNAL(clicked()),this,SLOT(OpenComDebug()));
    connect(ui->bufa,SIGNAL(clicked()),this,SLOT(OpenBufa()));
    connect(ui->timeset,SIGNAL(clicked()),this,SLOT(OpenTimeSet()));
    connect(ui->master,SIGNAL(clicked()),this,SLOT(OpenMasterSet()));
    connect(ui->Data_Query,SIGNAL(clicked()),this,SLOT(OpenData_Query()));
    connect(ui->dev_Setting,SIGNAL(clicked()),this,SLOT(OpenDev_Setting()));
    connect(ui->pushButton_Teshuzhi,SIGNAL(clicked()),this,SLOT(OpenTeshuzhi()));
    connect(ui->quickInput,&QPushButton::clicked,this,[=]()
    {
        ui->Username->setText("admin");
        ui->Password->setText("lcdcm");
    });
    connect(ui->modbus,SIGNAL(clicked()),this,SLOT(openModbus()));
}

MainWindow::~MainWindow()
{
    /* 打断线程再退出 */
    m_httpWorker->stopWork();
    m_thread.quit();
    /* 阻塞线程 2000ms，判断线程是否结束 */
    if (m_thread.wait(5000)) {
        qDebug()<<"自动监控线程结束";
    }

    qDeleteAll(DATA_list);
    qDeleteAll(UINT_list);
    qDeleteAll(NAME_list);
    qDeleteAll(CODE_list);
    qDeleteAll(BUTTON_list);
    qDeleteAll(widget_list);

    if(hexGroup) hexGroup->deleteLater();
    if(m_SignalMapper) m_SignalMapper->deleteLater();
    if(m_SignalMapper_Te) m_SignalMapper_Te->deleteLater();
    if(m_SignalMapper_Ted) m_SignalMapper_Ted->deleteLater();
    if(m_SignalMapper_Up) m_SignalMapper_Up->deleteLater();
    if(m_SignalMapper_Upd) m_SignalMapper_Upd->deleteLater();

    if(m_SignalMapper_DevD) m_SignalMapper_DevD->deleteLater();
    if(m_SignalMapper_DevF) m_SignalMapper_DevF->deleteLater();
    if(m_SignalMapper_DevM) m_SignalMapper_DevM->deleteLater();

    if(m_SignalMapper_FaDele) m_SignalMapper_FaDele->deleteLater();
    if(m_SignalMapper_FaEdit) m_SignalMapper_FaEdit->deleteLater();

    delete ui;
}

void MainWindow::ConfFile_Init()
{
    QFile file(CONF);
    if (!file.exists()){
        if(!file.open(QIODevice::ReadWrite)) {
            qDebug() << "配置文件创建失败！";
        }
        else
        {
            ConfNode_Init();
        }
    }else
    {
        ConfNode_Init();
    }
}

void MainWindow::ConfNode_Init()
{
    QFile file(CONF);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString value = file.readAll();
    file.close();
    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);

    //  空文件校验不能铜鼓通过 -->{}
    //    if(!(parseJsonErr.error == QJsonParseError::NoError))
//    {
//        qDebug()<< "配置文件Json格式错误！";
//        return;
//    }

    bool pConfChanged = false;
    QJsonObject jsonObject = document.object();
    if(!jsonObject.contains(FACTORS))
    {
        pConfChanged = true;
        QJsonObject QJsonObj_temp;
        jsonObject.insert(FACTORS,QJsonObj_temp);
    }
    if(!jsonObject.contains(TESHUZHI))
    {
        pConfChanged = true;
        QJsonObject QJsonObj_temp;
        jsonObject.insert(TESHUZHI,QJsonObj_temp);
    }

    if(pConfChanged)
    {
        // 使用QJsonDocument设置该json对象
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QJsonDocument jsonDoc;
        jsonDoc.setObject(jsonObject);
        file.write(jsonDoc.toJson());
        file.close();
    }
}

void MainWindow::Widget_Init()
{
    ui->comboBox->setView(new QListView(this));
    ui->comboBox->setView(new QListView(this));

    ui->hexYes->setChecked(true);
    hexGroup = new QButtonGroup(this);
    hexGroup->addButton(ui->hexYes, 0);
    hexGroup->addButton(ui->hexNo, 1);

    ui->dateTimeEdit->setCalendarPopup(true);        // 日历
    ui->dateTimeEdit->setDisplayFormat(QLatin1String("yyyy-MM-dd hh:mm"));  //格式
    ui->dateTimeEdit_2->setCalendarPopup(true);        // 日历
    ui->dateTimeEdit_2->setDisplayFormat(QLatin1String("yyyy-MM-dd hh:mm"));  //格式

    connect(ui->checkBox, SIGNAL(clicked()), this, SLOT(ClearTable()));
    connect(ui->checkBox_2, SIGNAL(clicked()), this, SLOT(ClearTable()));
    connect(ui->checkBox_3, SIGNAL(clicked()), this, SLOT(ClearTable()));
    connect(ui->checkBox_4, SIGNAL(clicked()), this, SLOT(ClearTable()));
    connect(ui->checkBox_9, SIGNAL(clicked()), this, SLOT(ClearTable()));
    connect(ui->checkBox_8, SIGNAL(clicked()), this, SLOT(ClearTable()));

    connect(ui->pushButtonNext, SIGNAL(clicked()), this, SLOT(ShowNextPage()));
    connect(ui->pushButtonLast, SIGNAL(clicked()), this, SLOT(ShowLastPage()));
    connect(ui->pushButtonJump, SIGNAL(clicked()), this, SLOT(ShowNewPage()));

    ui->lineEdit_MN->setEnabled(false);
    ui->lineEdit_Q->setEnabled(false);
    ui->lineEdit_M->setEnabled(false);
    ui->lineEdit_D->setEnabled(false);

    ui->factorBox->setEditable(true);
    // set CheckBox单项选择
    QButtonGroup* pButtonGroup = new QButtonGroup(this);
    pButtonGroup->addButton(ui->checkBox,1);
    pButtonGroup->addButton(ui->checkBox_2, 2);
    pButtonGroup->addButton(ui->checkBox_3, 3);
    pButtonGroup->addButton(ui->checkBox_4, 4);
    pButtonGroup->addButton(ui->checkBox_8, 8);
    pButtonGroup->addButton(ui->checkBox_9, 9);
    ui->checkBox->setChecked(true);

    ui->progressBar->hide();

    ui->label_46->hide();
    ui->comboBox_3->hide();

    ui->payload->setAlignment( Qt::AlignLeft);
    ui->response->setAlignment( Qt::AlignLeft);
    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->closewindow,&QPushButton::clicked,this,[=]()
    {
        this->close();
    });

    connect(ui->keyboard,&QPushButton::clicked,this,[=]()
    {
        QProcess process;
        process.startDetached("pkill florence");
        QThread::sleep(3);
        process.startDetached("florence");
        process.close();
    });

}

void MainWindow::ClearTable()
{
    if(ui->checkBox->isChecked())
    {
        ui->label_46->hide();
        ui->comboBox_3->hide();
        ui->label_48->show();
        ui->factorBox->show();
    }
    else if(ui->checkBox_8->isChecked())    // message package
    {
        ui->label_48->hide();
        ui->factorBox->hide();
        ui->label_46->show();
        ui->comboBox_3->show();
    }
    else
    {
        ui->label_46->hide();
        ui->comboBox_3->hide();
        ui->label_48->show();
        ui->factorBox->show();
    }

    ui->textEditAllPage->clear();
    ui->textEditCurPage->clear();
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);
    ui->progressBar->hide();
    ui->pushButtonExport->setEnabled(true);
}


void MainWindow::installEvents() {

    labelList.append(ui->label_37);
    labelList.append(ui->label_40);
    funcList.append(std::bind(&MainWindow::OpenSysInfo, this));

    labelList.append(ui->label_27);
    labelList.append(ui->label_28);
    funcList.append(std::bind(&MainWindow::OpenNeworkSetting, this));

    labelList.append(ui->label_33);
    labelList.append(ui->label_34);
    funcList.append(std::bind(&MainWindow::OpenComDebug, this));

    labelList.append(ui->label_20);
    labelList.append(ui->label_19);
    funcList.append(std::bind(&MainWindow::OpenBufa, this));

    labelList.append(ui->label_22);
    labelList.append(ui->label_21);
    funcList.append(std::bind(&MainWindow::OpenTimeSet, this));

    labelList.append(ui->label_8);
    labelList.append(ui->label_7);
    funcList.append(std::bind(&MainWindow::OpenMasterSet, this));

    labelList.append(ui->label_16);
    labelList.append(ui->label_15);
    funcList.append(std::bind(&MainWindow::OpenData_Query, this));

    labelList.append(ui->label_6);
    labelList.append(ui->label_5);
    funcList.append(std::bind(&MainWindow::OpenDev_Setting, this));

    labelList.append(ui->label_70);
    labelList.append(ui->label_9);
    funcList.append(std::bind(&MainWindow::OpenTeshuzhi, this));

    labelList.append(ui->label_30);
    labelList.append(ui->label_71);
    funcList.append(std::bind(&MainWindow::onReceiveDeviceCMDCtrl, this));

    labelList.append(ui->label_35);
    labelList.append(ui->label_36);
    funcList.append(std::bind(&MainWindow::openModbus, this));


    for(int i = 0; i < labelList.size(); ++ i) {
        labelList.at(i)->installEventFilter(this);
    }

    ui->pushButton_devicecmdctrl->installEventFilter(this);
    ui->modbus->installEventFilter(this);
}

//事件过滤器
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    for(int i = 0; i < labelList.size(); ++ i) {
        if(obj == labelList.at(i)) {
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event); // 事件转换
                if(mouseEvent->button() == Qt::LeftButton) {
                    std::function<void()> fun = funcList.at(i / 2);
                    fun();
                    return true;
                }
            }
            return false;
        }
    }

    if(obj == ui->pushButton_devicecmdctrl)
    {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event); // 事件转换
            if(mouseEvent->button() == Qt::LeftButton) {
                onReceiveDeviceCMDCtrl();
                return true;
            }
        }
        return false;
    }

    for(int i = 0; i < CODE_list.size(); ++ i) {
        if(obj == CODE_list.at(i)) {
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event); // 事件转换
                if(mouseEvent->button() == Qt::LeftButton) {
                    if(!m_LoginStatus)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>请先登录！</font>");
                        ui->stackedWidget->setCurrentIndex(2);
                        return false;
                    }
                    qDebug()<<__LINE__<<"Fac==>"<<CODE_list.at(i)->text()<<endl;
                    ui->stackedWidget->setCurrentWidget(ui->page_9);
                    ui->factorBox->setCurrentText(CODE_list.at(i)->text()+"-"+NAME_list.at(i)->text());
                    ui->dateTimeEdit->setDateTime((QDateTime::currentDateTime()).addSecs(-3600));
                    ui->dateTimeEdit_2->setDateTime(QDateTime::currentDateTime());
                    ui->checkBox->setChecked(true);
                    on_pushButtonFind_clicked();
                    return true;
                }
            }
            return false;
        }
    }

    for(int i = 0; i < NAME_list.size(); ++ i) {
        if(obj == NAME_list.at(i)) {
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event); // 事件转换
                if(mouseEvent->button() == Qt::LeftButton) {
                    if(!m_LoginStatus)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>请先登录！</font>");
                        ui->stackedWidget->setCurrentIndex(2);
                        return false;
                    }
                    qDebug()<<__LINE__<<"Fac==>"<<CODE_list.at(i)->text()<<endl;
                    ui->stackedWidget->setCurrentWidget(ui->page_9);
                    ui->factorBox->setCurrentText(CODE_list.at(i)->text()+"-"+NAME_list.at(i)->text());
                    ui->dateTimeEdit->setDateTime((QDateTime::currentDateTime()).addSecs(-3600));
                    ui->dateTimeEdit_2->setDateTime(QDateTime::currentDateTime());
                    ui->checkBox->setChecked(true);
                    ui->label_46->hide();
                    ui->comboBox_3->hide();
                    ui->label_48->show();
                    ui->factorBox->show();
                    on_pushButtonFind_clicked();
                    return true;
                }
            }
            return false;
        }
    }

    for(int i = 0; i < UINT_list.size(); ++ i) {
        if(obj == UINT_list.at(i)) {
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event); // 事件转换
                if(mouseEvent->button() == Qt::LeftButton) {
                    if(!m_LoginStatus)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>请先登录！</font>");
                        ui->stackedWidget->setCurrentIndex(2);
                        return false;
                    }
                    qDebug()<<__LINE__<<"Fac==>"<<CODE_list.at(i)->text()<<endl;
                    ui->stackedWidget->setCurrentWidget(ui->page_9);
                    ui->factorBox->setCurrentText(CODE_list.at(i)->text()+"-"+NAME_list.at(i)->text());
                    ui->dateTimeEdit->setDateTime((QDateTime::currentDateTime()).addSecs(-3600));
                    ui->dateTimeEdit_2->setDateTime(QDateTime::currentDateTime());
                    ui->checkBox->setChecked(true);
                    ui->label_46->hide();
                    ui->comboBox_3->hide();
                    ui->label_48->show();
                    ui->factorBox->show();
                    on_pushButtonFind_clicked();
                    return true;
                }
            }
            return false;
        }
    }

    for(int i = 0; i < DATA_list.size(); ++ i) {
        if(obj == DATA_list.at(i)) {
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event); // 事件转换
                if(mouseEvent->button() == Qt::LeftButton) {
                    if(!m_LoginStatus)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>请先登录！</font>");
                        ui->stackedWidget->setCurrentIndex(2);
                        return false;
                    }
                    qDebug()<<__LINE__<<"Fac==>"<<CODE_list.at(i)->text()<<endl;
                    ui->stackedWidget->setCurrentWidget(ui->page_9);
                    ui->factorBox->setCurrentText(CODE_list.at(i)->text()+"-"+NAME_list.at(i)->text());
                    ui->dateTimeEdit->setDateTime((QDateTime::currentDateTime()).addSecs(-3600));
                    ui->dateTimeEdit_2->setDateTime(QDateTime::currentDateTime());
                    ui->checkBox->setChecked(true);
                    ui->label_46->hide();
                    ui->comboBox_3->hide();
                    ui->label_48->show();
                    ui->factorBox->show();
                    on_pushButtonFind_clicked();
                    return true;
                }
            }
            return false;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void MainWindow::openModbus()
{
    ui->stackedWidget->setCurrentIndex(13);
    QStringList comlist,baudlist,databitlist,paritylist,stopbitlist;
    comlist<<"COM1"<<"COM2"<<"COM3"<<"COM4"<<"COM5"<<"COM6"<<"COM7"<<"COM8"<<"COM9"<<"COM10"<<"COM11"<<"COM12";
    baudlist<<"9600"<<"4800"<<"115200";
    databitlist<<"7"<<"8";
    paritylist<<"N"<<"E"<<"O";
    stopbitlist<<"1"<<"0";
    ui->com->clear();
    ui->baudrate->clear();
    ui->databit->clear();
    ui->parity->clear();
    ui->stopbit->clear();
    ui->ctrl->clear();
    ui->address->clear();
    ui->isactive->setCheckable(true);


    ui->com->addItems(comlist);
    ui->baudrate->addItems(baudlist);
    ui->databit->addItems(databitlist);
    ui->parity->addItems(paritylist);
    ui->stopbit->addItems(stopbitlist);
    ui->ctrl->addItem("03");

//    "address": 0,
//      "com": "string",
//      "baudrate": 0,
//      "data_bit": 0,
//      "parity": "string",
//      "stop_bit": 0,
//      "active": 0

    httpclinet h;
    QJsonObject jObj;
    if(h.get("/dcm/modbus_server",jObj))
    {
        qDebug()<<__LINE__<<jObj<<endl;
        ui->com->setCurrentText(util.Uart_Revert(jObj.value("com").toString()));
        ui->baudrate->setCurrentText(QString::number(jObj.value("baudrate").toInt()));
        ui->databit->setCurrentText(QString::number(jObj.value("data_bit").toInt()));
        ui->parity->setCurrentText(jObj.value("parity").toString());
        ui->stopbit->setCurrentText(QString::number(jObj.value("stop_bit").toInt()));
        if(jObj.value("active").toBool())
            ui->isactive->setChecked(true);
        else
            ui->isactive->setChecked(false);
        ui->address->setText(QString::number(jObj.value("address").toInt()));
    }

    connect(ui->btn_clr,&QPushButton::clicked,ui->infoedit,&QTextBrowser::clear);

    connect(ui->btn_ok,&QPushButton::clicked,this,[=]()
    {
        QJsonObject jObj0;
        jObj0.insert("com",ui->com->currentText());
        jObj0.insert("baudrate",ui->baudrate->currentText().toInt());
        jObj0.insert("data_bit",ui->databit->currentText().toInt());
        jObj0.insert("parity",ui->parity->currentText());
        jObj0.insert("stop_bit",ui->stopbit->currentText().toInt());
        if(ui->isactive->isChecked())
            jObj0.insert("active",1);
        else
            jObj0.insert("active",0);
        jObj0.insert("address",ui->address->text().toInt());

        qDebug()<<__LINE__<<jObj0<<endl;

        //串口号，波特率，数据位，停止位，校验位，设备地址，是否开启，控制指令
        httpclinet h;
        QString str;
        str = "需要配置的信息:\n";
        str += "串口号:"+ui->com->currentText()+";\n";
        str += "波特率:"+ui->baudrate->currentText()+";\n";
        str += "数据位:"+ui->databit->currentText()+";\n";
        str += "停止位:"+ui->stopbit->currentText()+";\n";
        str += "校验位:"+ui->parity->currentText()+";\n";
        str += "设备地址:"+ui->address->text()+";\n";
        str += "控制指令:"+ui->ctrl->currentText()+";\n";
        if(ui->isactive->isChecked())
            str += "是否开启:是;\n";
        else
            str += "是否开启:否;\n";

        if(h.put("/dcm/modbus_server",jObj0))
        {
            str += "***************配置成功***************\n";
            ui->infoedit->setTextColor(QColor(Qt::darkGreen));

        }
        else
        {
            str += "***************配置失败***************\n";
            ui->infoedit->setTextColor(QColor(Qt::darkRed));

        }
        ui->infoedit->setPlainText(str);


    });




}

void MainWindow::handleDateTimeout()
{
    QDateTime time = QDateTime::currentDateTime();
    emit sendCurDT(time);
    QString str = time.toString("yyyy-MM-dd hh:mm:ss");
    ui->textEditDataTime->setAlignment( Qt::AlignCenter);//文字水平居中
    ui->textEditDataTime->setText(str);

    ui->dateTimeEdit_local->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::on_login_clicked()
{
    if(m_LoginStatus)   // login out
    {
        m_LoginStatus = false;
        ui->login->setText("登录");
        ui->stackedWidget->setCurrentIndex(0);
    }
    else    // login in
    {
        ui->Username->setText("");
        ui->Password->setText("");
        ui->stackedWidget->setCurrentIndex(2);
    }
}

void MainWindow::on_data_clicked()
{
    if(ui->stackedWidget->currentIndex() == 0)
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>当前已经是实时数据主页面！</font>");
        return;
    }

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_main_clicked()
{
    if(ui->stackedWidget->currentIndex() == 1)
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>当前已经是系统配置页面！</font>");
        return;
    }

    if(!m_LoginStatus)
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>请先登录！</font>");
        return;
    }

    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_Login_clicked()
{
    if((QString(SERVER_IP) != QString("localhost")) || (ui->Username->text() == USERNAME && ui->Password->text() == PASSWORD))
    {
        m_LoginStatus = true;
        ui->stackedWidget->setCurrentIndex(1);
        ui->login->setText("退出登录");
    }
    else
    {
        m_LoginStatus = false;
        QMessageBox::about(NULL, "提示", "<font color='m  black'>用户名或密码错误!</font>");
        ui->login->setText("登录");
    }
}

void MainWindow::on_Return_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

//填充页面
void MainWindow::setTableContents(const QJsonObject &pJsonObj,const QJsonObject & pConfJsonTeList)
{
    for(int i=0;i<BUTTON_list.size();i++)
    {
        disconnect(BUTTON_list.at(i),SIGNAL(clicked()),m_SignalMapper,SLOT(map()));
        m_SignalMapper->removeMappings(BUTTON_list.at(i));
    }
    disconnect(m_SignalMapper,SIGNAL(mapped(QString)),this,SLOT(onButtonDevParam(QString)));

    int i = 0;

    QJsonObject::const_iterator it = pJsonObj.constBegin();
    QJsonObject::const_iterator end = pJsonObj.constEnd();
    while(it != end)
    {

        QJsonObject json = it.value().toObject();
        QString pFactorCode = json.value("FactorCode").toString();

        //名称
        if (json.contains("FactorCode")){
            CODE_list.at(i)->setText(pFactorCode);
        }
        CODE_list.at(i)->show();

        BUTTON_list.at(i)->show();
        connect(BUTTON_list.at(i),SIGNAL(clicked()),m_SignalMapper,SLOT(map()));
        m_SignalMapper->setMapping(BUTTON_list.at(i),it.key());

        //别名
        if(g_Dcm_Factor.contains(pFactorCode))
        {
            QString pStrName = g_Dcm_Factor.value(pFactorCode).toObject()[QLatin1String("name")].toString();
            QTextCodec *codec = QTextCodec::codecForName("UTF8");
            QString unidata = codec->toUnicode(pStrName.toUtf8().data());
            NAME_list.at(i)->setText(unidata);
        }
        NAME_list.at(i)->show();

        //单位
        if(g_Dcm_Factor.contains(pFactorCode))
        {
            QString pStrName = g_Dcm_Factor.value(pFactorCode).toObject()[QLatin1String("unit")].toString();
            UINT_list.at(i)->setText(QStringLiteral("单位:")+ pStrName.toStdString().c_str());
        }
        UINT_list.at(i)->show();

        //实时值
        QString html = QLatin1String("");
        if( json.value("Flag").toString() == "N" ){
            html = QLatin1String("<span style='color: green; font-weight: bold;'>N</span>");
        }
        else if( json.value("Flag").toString() == "B" ){
            html = QLatin1String("<span style='color: red; font-weight: bold;'>B</span>");
        }
        QString A = json.value("Data").toString();  // data val

        QString pTeshuzhi;
        if(pConfJsonTeList.contains(pFactorCode))
        {
            QJsonObject pJsonFactor = pConfJsonTeList.value(pFactorCode).toObject();
            QString pKey = pJsonFactor.value(CONF_TE_KEY).toString();
            QString pVal = pJsonFactor.value(CONF_TE_VALUE).toString();

            if(pKey == A)
            {
                pTeshuzhi = "[" + pVal + "]";
            }
        }

        DATA_list.at(i)->setText(A + pTeshuzhi + QLatin1String(" ") + html);
        DATA_list.at(i)->show();

        CODE_list.at(i)->installEventFilter(this);

        NAME_list.at(i)->installEventFilter(this);

        UINT_list.at(i)->installEventFilter(this);

        DATA_list.at(i)->installEventFilter(this);

        i++;
        it++;
    }

    connect(m_SignalMapper,SIGNAL(mapped(QString)),this,SLOT(onButtonDevParam(QString)));

    return;
}

void MainWindow::onButtonDevParam(QString pFactorKey)
{
    DialogDevProp *pDevProp = new DialogDevProp(pFactorKey);
    pDevProp->setWindowModality(Qt::ApplicationModal);
    pDevProp->show();
}

void MainWindow::onButtonUpSaved(QString ipaddr)
{
    // qDebug() << "ipaddr edit==>>" << ipaddr;

    QJsonObject obj;

    // tranverse tablewidget
    for (int i=0;i<ui->tableWidget_Upload->rowCount();i++) //逐行处理
    {
        QTableWidgetItem *cellItem=ui->tableWidget_Upload->item(i,2); //获取单元格的item

        // qDebug() << "====>>" << cellItem->text();

        if(cellItem->text() == ipaddr)
        {
            QTableWidgetItem *cellItem_id = ui->tableWidget_Upload->item(i,0);
            obj.insert(QLatin1String("id"), cellItem_id->text());

            QTableWidgetItem *cellItem_mn = ui->tableWidget_Upload->item(i,1);
            obj.insert(QLatin1String("mn"), cellItem_mn->text());

            obj.insert(QLatin1String("ip_addr_port"), ipaddr);

            QTableWidgetItem *cellItem_iu = ui->tableWidget_Upload->item(i,3);
            obj.insert(QLatin1String("interval_upload"), cellItem_iu->text().toInt());

            QTableWidgetItem *cellItem_imdu = ui->tableWidget_Upload->item(i,4);
            obj.insert(QLatin1String("interval_minute_data_upload"), cellItem_imdu->text().toInt());

            QCheckBox *pCheckHeart = (QCheckBox *)ui->tableWidget_Upload->cellWidget(i,5);
            int is_open_heartbeat = pCheckHeart->isChecked() ? 1 : 0;
            obj.insert(QLatin1String("is_open_heartbeat"), is_open_heartbeat);

            QTableWidgetItem *cellItem_ih = ui->tableWidget_Upload->item(i,6);
            obj.insert(QLatin1String("interval_heartbeat"), cellItem_ih->text().toInt());

            QTableWidgetItem *cellItem_pw = ui->tableWidget_Upload->item(i,7);
            obj.insert(QLatin1String("pw"), cellItem_pw->text());

            QCheckBox *pCheckRtd = (QCheckBox *)ui->tableWidget_Upload->cellWidget(i,8);
            int is_send_rtd = pCheckRtd->isChecked() ? 1 : 0;
            obj.insert(QLatin1String("is_send_rtd"), is_send_rtd);

            QTableWidgetItem *cellItem_ot = ui->tableWidget_Upload->item(i,9);
            obj.insert(QLatin1String("over_time"), cellItem_ot->text().toInt());

            QTableWidgetItem *cellItem_rt = ui->tableWidget_Upload->item(i,10);
            obj.insert(QLatin1String("resend_times"), cellItem_rt->text().toInt());

            QComboBox *pComBox_pv = (QComboBox *)ui->tableWidget_Upload->cellWidget(i,11);
            obj.insert(QLatin1String("protocol_version"), pComBox_pv->currentText());

            qDebug() << "obj==>>" << obj;

            httpclinet pClient;
            if(pClient.put(DCM_MNINFO,obj))
            {
                QMessageBox::about(NULL, "提示", "<font color='black'>保存上传配置成功！</font>");
            }
            else
            {
                QMessageBox::about(NULL, "提示", "<font color='black'>保存上传配置失败！</font>");
            }

            break;
        }
    }
}

void MainWindow::onButtonFaEdit(QString id)
{
    // qDebug() << "id ==>>" << id;

    FacEdit *pFacEdit = new FacEdit(id,this);
    pFacEdit->show();
    connect(pFacEdit, SIGNAL(editSuccess()),this, SLOT(refresh_AnalogDevParam()));
}

void MainWindow::onButtonFaDele(QString id)
{
    // qDebug() << "id ==>>" << id;

    httpclinet pClient;
    // 获取 设备ID-fcode
    QString pItem = "";
    QJsonObject jsonObjectFas;
    if(pClient.get(DCM_DEVICE_FACTOR,jsonObjectFas))
    {
        // qDebug() << "jsonObjectFa===>>" << jsonObjectFa;
        QJsonObject::const_iterator it = jsonObjectFas.constBegin();
        QJsonObject::const_iterator end = jsonObjectFas.constEnd();
        while(it != end)
        {
            QJsonObject pJsonFa = it.value().toObject();

            if(pJsonFa.value("id").toString() == id)
            {
                pItem = it.key();
                break;
            }
            it++;
        }
    }
    if(pClient.deleteSource(DCM_DEVICE_FACTOR_DELETE,id))
    {
        if(pItem != "")
        {
            if(pClient.deleteSource(DCM_CONF_FACTOR_DELETE,pItem.toLower()))
            {
                QMessageBox::about(NULL, "提示", "<font color='black'>删除因子配置成功！</font>");
            }
        }
    }else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>删除因子配置失败！</font>");
    }
}

void MainWindow::onButtonTeSaved(QString pFactor)
{
    // qDebug() << "pFactor==>>" << pFactor;

    // tranverse tablewidget
    for (int i=0;i<ui->tableWidget_Te->rowCount();i++) //逐行处理
    {
        QTableWidgetItem *cellItem=ui->tableWidget_Te->item(i,0); //获取单元格的item

        // qDebug() << "====>>" << cellItem->text();

        if(cellItem->text() == pFactor)
        {
            QTableWidgetItem *cellItem1=ui->tableWidget_Te->item(i,1);
            QTableWidgetItem *cellItem2=ui->tableWidget_Te->item(i,2);
            QTableWidgetItem *cellItem3=ui->tableWidget_Te->item(i,3);

            QJsonObject obj;
            obj.insert(QLatin1String("name"), cellItem1->text());
            obj.insert(QLatin1String("key"), cellItem2->text());
            obj.insert(QLatin1String("value"), cellItem3->text());

            // qDebug() << "obj==>>" << obj;

            httpclinet pClient;
            if(pClient.post(DCM_SET_TESHUZHI+pFactor,obj))
            {
                QMessageBox::about(NULL, "提示", "<font color='black'>保存特殊值配置成功！</font>");
            }
            else
            {
                QMessageBox::about(NULL, "提示", "<font color='black'>保存特殊值配置失败！</font>");
            }
        }
    }
}

void MainWindow::onButtonUpDele(QString ipaddr)
{
    // qDebug() << "ipaddr dele==>>" << ipaddr;
    httpclinet pClient;
    if(pClient.deleteSource(DCM_MNINFO_DELETE,ipaddr))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>删除上传配置成功！</font>");
    }else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>删除上传配置失败！</font>");
    }

}

void MainWindow::onButtonDevDele(QString id)
{
    // qDebug() << "dev id =dele==>>  " << id;

    httpclinet pClient;
    if(pClient.deleteSource(DCM_DEVICE_DELETE,id))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>删除设备配置成功！</font>");
    }else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>删除设备配置失败！</font>");
    }

}

void MainWindow::onButtonDevMore(QString id)
{
    g_IsAnalogDevOperated = false;
    // qDebug() << "dev id =more==>>  " << id;

    DevEdit *pDevEdit = new DevEdit(id,this);
    pDevEdit->show();

}

void MainWindow::onButtonDevFactor(QString id)
{
    // qDebug() << "dev id =factor==>>  " << id;
    ui->stackedWidget->setCurrentIndex(11);
    g_Device_ID = id;
    FactorGui_Init(id);
}

void MainWindow::onButtonTeDele(QString pFactor)
{
    // qDebug() << "pFactor==>>" << pFactor;
    httpclinet pClient;
    if(pClient.deleteSource(DCM_TESHUZHI_DELETE,pFactor.toLower()))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>删除特殊值配置成功！</font>");
    }else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>删除特殊值配置失败！</font>");
    }
}


void MainWindow::handleResults(QString item,const QJsonObject & results)
{
//    qDebug()<<"item:"<<item;
//    qDebug()<<"results："<<results;

    if(item == "realtime_data")
    {
        QJsonObject pConObj;
        httpclinet pClient;
        if(pClient.get(DCM_DEVICE_FACTOR,pConObj))
        {
            QJsonObject pTempObjFactor;
            g_ConfObjDevParam.empty();
//            if(pConObj.contains(FACTORS))
//            {
//                QJsonObject pConfJsonFactorList = pConObj.value(FACTORS).toObject();
                QJsonObject pConfJsonFactorList = pConObj;
                QJsonObject pConfJsonTeList;
                QJsonObject::const_iterator it = results.constBegin();
                QJsonObject::const_iterator end = results.constEnd();
                while(it != end)
                {
                    bool isDevParam = false;
                    if(pConfJsonFactorList.contains(it.key()))
                    {
                        pConfJsonTeList = it.value().toObject().value(TESHUZHI).toObject();
                        QJsonObject pConfJsonFactor = pConfJsonFactorList.value(it.key()).toObject();
                        if(pConfJsonFactor.contains(CONF_IS_DEVICE_PROPERTY))
                        {
                            if(pConfJsonFactor.value(CONF_IS_DEVICE_PROPERTY).toBool())
                            {
                                g_ConfObjDevParam.insert(it.key(),it.value());
                                isDevParam = true;
                            }
                        }
                    }

                    if(!isDevParam) pTempObjFactor.insert(it.key(),it.value());
                    it++;
                }

                //qDebug() << "g_ConfObjDevParam--->>" << g_ConfObjDevParam;
                //qDebug()<<"pTempObjFactor==>"<<pTempObjFactor << endl;

                // 过滤掉仪器参数
                int differ = widget_list.size() - pTempObjFactor.size();
                if(differ == 0) this->setTableContents(pTempObjFactor,pConfJsonTeList);
                else{
                    if(differ < 0) this->addfactor( abs(differ) , pTempObjFactor,pConfJsonTeList);
                    else this->delfactor( differ, pTempObjFactor,pConfJsonTeList);
                }
//            }
        }
    }
    else if(item == "connect_stat")
    {
        QDateTime time = QDateTime::currentDateTime();
        QString str = time.toString("yyyy-MM-dd hh:mm:ss");
        ui->label_datetime->setAlignment( Qt::AlignCenter);//文字水平居中
        ui->label_datetime->setText(str);

        int i = 0;
        label_clear();

        QJsonObject::const_iterator it = results.constBegin();
        QJsonObject::const_iterator end = results.constEnd();
        while (it != end) {
            QLabel *labelstate = new QLabel(ui->scrollAreaWidgetContents_2);
            labelstate->setGeometry(i*220+10,0,30,30);
            labelstate->setAlignment(Qt::AlignLeft);
            labelstate->setStyleSheet(QLatin1String("font: 9pt"));
            STATE_list.append(labelstate);
            if(it.value().isBool())
            {
                if(it.value().toBool())
                {
                    labelstate->setStyleSheet("border-image: url(:/images/greenC.png);");
                }
                else
                {
                    labelstate->setStyleSheet("border-image: url(:/images/blackC.png);");
                }
            }
            labelstate->show();

            QLabel *labeladdr = new QLabel(ui->scrollAreaWidgetContents_2);
            labeladdr->setGeometry(i*220+40,0,200,30);
            labeladdr->setAlignment(Qt::AlignLeft);
            labeladdr->setAlignment(Qt::AlignVCenter);
            labeladdr->setStyleSheet(QLatin1String("font: 11pt"));
            ADDR_list.append(labeladdr);
            labeladdr->setText(it.key());
            labeladdr->show();

            i++;
            it++;
        }
    }
}

void MainWindow::label_clear()
{
    foreach(QLabel* label,NO_list)
    {
        NO_list.removeOne(label);
        label->deleteLater();
        label = nullptr;
    }
    foreach(QLabel* label,ADDR_list)
    {
        ADDR_list.removeOne(label);
        label->deleteLater();
        label = nullptr;
    }
    foreach(QLabel* label,STATE_list)
    {
        STATE_list.removeOne(label);
        label->deleteLater();
        label = nullptr;
    }
}

//删除因子框
void MainWindow::delfactor(int differ, const QJsonObject & results,const QJsonObject & pConfJsonTeList)
{
    int lower = widget_list.size()-differ-1;
    int upper = widget_list.size()-1;

    for(int i=upper; i>lower; i--)
    {
        QLabel *pLabelC = CODE_list.at(i);
        if(pLabelC)
        {
            CODE_list.removeOne(pLabelC);
            delete pLabelC;
        }
        QLabel *pLabelD = DATA_list.at(i);
        if(pLabelD)
        {
            DATA_list.removeOne(pLabelD);
            delete pLabelD;
        }
        QLabel *pLabelU = UINT_list.at(i);
        if(pLabelU)
        {
            UINT_list.removeOne(pLabelU);
            delete pLabelU;
        }
        QLabel *pLabelN = NAME_list.at(i);
        if(pLabelN)
        {
            NAME_list.removeOne(pLabelN);
            delete pLabelN;
        }
        QPushButton *pButton = BUTTON_list.at(i);
        if(pButton)
        {
            BUTTON_list.removeOne(pButton);
            delete pButton;
        }

        QWidget *pWidget = widget_list.at(i);
        if(pWidget)
        {
            widget_list.removeOne(pWidget);
            pWidget->deleteLater();
        }
    }
    this->setTableContents(results,pConfJsonTeList);
}

//添加因子框
void MainWindow::addfactor( int differ, const QJsonObject & results,const QJsonObject & pConfJsonTeList)
{
    int lower = widget_list.size();
    int upper = widget_list.size()+differ;
    // qDebug() << "lower-->>"<< lower;
    // qDebug() << "upper==>>"<< upper;
    for(int i=lower; i<upper; i++)
    {
        QWidget *TEST = new QWidget(ui->scrollAreaWidgetContents_3);
        TEST->setGeometry(10+(i%6)*203,10+(i/6)*170,200,150);
        TEST->setStyleSheet(QLatin1String("background-color: rgb(186, 189, 182);"));
        TEST->show();
        widget_list.append(TEST);

        QPushButton *button = new QPushButton(TEST);
        button->setGeometry(140,10,55,30);
        button->setText("仪器参数");
        button->setStyleSheet(QLatin1String("font: 10pt"));
        BUTTON_list.append(button);

        QLabel *code = new QLabel(TEST);
        code->setGeometry(5,5,130,35);
        code->setStyleSheet(QLatin1String("font: 13pt"));
        CODE_list.append(code);

        QLabel *name = new QLabel(TEST);
        name->setGeometry(5,45,190,25);
        name->setStyleSheet(QLatin1String("font: 12pt"));
        NAME_list.append(name);

        QLabel *data = new QLabel(TEST);
        data->setGeometry(0,80,200,30);
        data->setAlignment(Qt::AlignCenter);
        data->setStyleSheet(QStringLiteral("font: 15pt 黑体;"));
        DATA_list.append(data);

        QLabel *uint = new QLabel(TEST);
        uint->setGeometry(5,120,190,30);
        uint->setAlignment(Qt::AlignRight);
        UINT_list.append(uint);

    }
    this->setTableContents(results,pConfJsonTeList);
}

void CHttpWork::doWork1() {

    bool pDcmDactorFlag = false;
    bool pDcmSystemCodeFlag = false;
    bool pDcmSupportDeviceFlag = false;

    /* 标志位为真 */
    isCanRun = true;

    /* 死循环 */
    while (1) {
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
        /* 使用QThread里的延时函数，当作一个普通延时 */
        QThread::sleep(REALTIME_FLUSH_PERIOD);

        // 获取实时数据
        httpclinet pClient;
        if(!pDcmDactorFlag)
        {
            if(pClient.get(DCM_FACTOR,g_Dcm_Factor))
            {
                pDcmDactorFlag = true;
            }
        }
        if(!pDcmSystemCodeFlag)
        {
            if(pClient.get(DCM_SYSTEM_CODE,g_Dcm_SystemCode))
            {
                // qDebug() << "g_Dcm_SystemCode==>" << g_Dcm_SystemCode;
                pDcmSystemCodeFlag = true;
            }
        }
        if(!pDcmSupportDeviceFlag)
        {
            if(pClient.get(DCM_SUPPORT_DEVICE,g_Dcm_SupportDevice))
            {
                pDcmSupportDeviceFlag = true;
            }
        }

        QJsonObject pJsonObj;
        if(pClient.get(DCM_REALTIME_DATA,pJsonObj))
        {
            emit resultReady("realtime_data",pJsonObj);
        }

        if(pClient.get(DCM_COONECT_STAT,pJsonObj))
        {
            emit resultReady("connect_stat",pJsonObj);
        }
    }
}

void MainWindow::OpenTeshuzhi()
{
    ui->stackedWidget->setCurrentIndex(12);
    TeshuzhiGui_Init();
}

void MainWindow::setTableDevHeader()
{
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#002570;color:#ffffff}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC; height:30px;  border:1px solid #001014; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #323232); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
headerText << QStringLiteral("端口号")  << QStringLiteral("设备型号") << QStringLiteral("设备地址")<< QStringLiteral("操作");
int cnt = headerText.count();
ui->tableWidget_Dev->setColumnCount(cnt);
ui->tableWidget_Dev->setHorizontalHeaderLabels(headerText);
// ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget_Dev->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget_Dev->horizontalHeader()->setFont(QFont(QLatin1String("song"), 16));
ui->tableWidget_Dev->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget_Dev->horizontalHeader()->font();
font.setBold(true);
ui->tableWidget_Dev->horizontalHeader()->setFont(font);

ui->tableWidget_Dev->setFont(QFont(QLatin1String("Ubuntu Bold"), 16)); // 表格内容的字体为10号宋体

int widths[] = {200, 200, 200, 230};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget_Dev->setColumnWidth(i, widths[i]);
}


ui->tableWidget_Dev->setStyleSheet(qssTV);
ui->tableWidget_Dev->horizontalHeader()->setVisible(true);
ui->tableWidget_Dev->verticalHeader()->setVisible(false);
ui->tableWidget_Dev->verticalHeader()->setDefaultSectionSize(45);
ui->tableWidget_Dev->setFrameShape(QFrame::NoFrame);

}

void MainWindow::setTableFaHeader()
{
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#002570;color:#ffffff}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC; height:30px;  border:1px solid #001014; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #323232); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
headerText  << QStringLiteral("因子编码") << QStringLiteral("因子序号")<< QStringLiteral("操作");
int cnt = headerText.count();
ui->tableWidget_Factor->setColumnCount(cnt);
ui->tableWidget_Factor->setHorizontalHeaderLabels(headerText);
// ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget_Factor->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget_Factor->horizontalHeader()->setFont(QFont(QLatin1String("Ubuntu"), 20,75));
ui->tableWidget_Factor->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget_Factor->horizontalHeader()->font();
font.setBold(true);
ui->tableWidget_Factor->horizontalHeader()->setFont(font);

ui->tableWidget_Factor->setFont(QFont(QLatin1String("Ubuntu"), 20,75)); // 表格内容的字体为10号宋体

int widths[] = {200, 200, 300};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget_Factor->setColumnWidth(i, widths[i]);
}

ui->tableWidget_Factor->setStyleSheet(qssTV);
ui->tableWidget_Factor->horizontalHeader()->setVisible(true);
ui->tableWidget_Factor->verticalHeader()->setVisible(false);
ui->tableWidget_Factor->verticalHeader()->setDefaultSectionSize(45);
ui->tableWidget_Factor->setFrameShape(QFrame::NoFrame);

}


void MainWindow::setTableTeHeader()
{
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#1B89A1}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
headerText << QStringLiteral("因子编码") << QStringLiteral("因子名称") << QStringLiteral("特殊值") << QStringLiteral("显示内容")<< QStringLiteral("操作");
int cnt = headerText.count();
ui->tableWidget_Te->setColumnCount(cnt);
ui->tableWidget_Te->setHorizontalHeaderLabels(headerText);
// ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget_Te->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget_Te->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
ui->tableWidget_Te->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget_Te->horizontalHeader()->font();
font.setBold(true);
ui->tableWidget_Te->horizontalHeader()->setFont(font);

ui->tableWidget_Te->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {150, 200, 150, 300, 130};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget_Te->setColumnWidth(i, widths[i]);
}

ui->tableWidget_Te->setStyleSheet(qssTV);
ui->tableWidget_Te->horizontalHeader()->setVisible(true);
ui->tableWidget_Te->verticalHeader()->setDefaultSectionSize(45);
ui->tableWidget_Te->setFrameShape(QFrame::NoFrame);

}

void MainWindow::setTableUpHeader()
{
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#1B89A1}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
headerText << QStringLiteral("ID") << QStringLiteral("MN") << QStringLiteral("平台地址") << QStringLiteral("上传周期/秒")
           << QStringLiteral("分钟数据上传周期/分钟") << QStringLiteral("是否开启心跳") << QStringLiteral("心跳周期/秒") << QStringLiteral("密码")
           << QStringLiteral("是否上报数据") << QStringLiteral("发送超时/秒") << QStringLiteral("发送失败重试次数") << QStringLiteral("协议版本")<< QStringLiteral("操作");
int cnt = headerText.count();
ui->tableWidget_Upload->setColumnCount(cnt);
ui->tableWidget_Upload->setHorizontalHeaderLabels(headerText);
// ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget_Upload->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget_Upload->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
ui->tableWidget_Upload->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget_Upload->horizontalHeader()->font();
font.setBold(true);
ui->tableWidget_Upload->horizontalHeader()->setFont(font);

ui->tableWidget_Upload->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {130, 150, 200, 100, 170, 110, 100, 80, 110, 100, 150, 130, 130};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget_Upload->setColumnWidth(i, widths[i]);
}

ui->tableWidget_Upload->setStyleSheet(qssTV);
ui->tableWidget_Upload->horizontalHeader()->setVisible(true);
ui->tableWidget_Upload->verticalHeader()->setDefaultSectionSize(45);
ui->tableWidget_Upload->setFrameShape(QFrame::NoFrame);

}

bool MainWindow::DevGui_Init()
{
    disconnect(m_SignalMapper_DevD,SIGNAL(mapped(QString)),this,SLOT(onButtonDevDele(QString)));
    disconnect(m_SignalMapper_DevF,SIGNAL(mapped(QString)),this,SLOT(onButtonDevFactor(QString)));
    disconnect(m_SignalMapper_DevM,SIGNAL(mapped(QString)),this,SLOT(onButtonDevMore(QString)));

    ui->tableWidget_Dev->clear();
    setTableDevHeader();
    QTableWidgetItem *pItemID=nullptr,*pItemName=nullptr,*pItemType=nullptr,*pItemAddr=nullptr;
    QPushButton *pOperFactor=nullptr,*pOperDetail=nullptr,*pOperDele=nullptr;

    QJsonObject pJsonDev;
    httpclinet pClient;
    if(pClient.get(DCM_DEVICE,pJsonDev))
    {
        // table ==>>  ID 设备名称 设备型号 设备地址     操作：详情  因子配置  删除
        ui->tableWidget_Dev->setRowCount(pJsonDev.size());

        int row=0;
        QJsonObject::const_iterator it = pJsonDev.constBegin();
        QJsonObject::const_iterator end = pJsonDev.constEnd();
        while(it != end)
        {
            // qDebug() << "key ==> "<< it.key();

            QJsonObject pJsonDev = it.value().toObject();

            pItemID = new QTableWidgetItem(util.Uart_Revert(pJsonDev.value("com").toString()));
            pItemID->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Dev->setItem(row, 0, pItemID);

//            pItemName = new QTableWidgetItem(pJsonDev.value("dev_name").toString());
//            pItemName->setTextAlignment(Qt::AlignCenter);
//            ui->tableWidget_Dev->setItem(row, 1, pItemName);

            pItemType = new QTableWidgetItem(namemap[pJsonDev.value("dev_type").toString()]);
            pItemType->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Dev->setItem(row, 1, pItemType);

            pItemAddr = new QTableWidgetItem(QString::number(pJsonDev.value("address").toInt()));
            pItemAddr->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Dev->setItem(row, 2, pItemAddr);

            pOperDetail = new QPushButton();
            pOperDetail->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
            pOperDetail->setFont(QFont("Ubuntu",16,75));
            pOperDetail->setText("详情");

            pOperFactor = new QPushButton();
            pOperFactor->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
            pOperFactor->setFont(QFont("Ubuntu",16,75));
            pOperFactor->setText("因子配置");

            pOperDele = new QPushButton();
            pOperDele->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
            pOperDele->setFont(QFont("Ubuntu",16,75));
            pOperDele->setText("删除");

            connect(pOperDetail,SIGNAL(clicked()),m_SignalMapper_DevM,SLOT(map()));
            m_SignalMapper_DevM->setMapping(pOperDetail,it.key());
            connect(pOperFactor,SIGNAL(clicked()),m_SignalMapper_DevF,SLOT(map()));
            m_SignalMapper_DevF->setMapping(pOperFactor,it.key());
            connect(pOperDele,SIGNAL(clicked()),m_SignalMapper_DevD,SLOT(map()));
            m_SignalMapper_DevD->setMapping(pOperDele,it.key());

            QWidget *btnWidget = new QWidget(this);
            QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);    // FTIXME：内存是否会随着清空tablewidget而释放
            btnLayout->addWidget(pOperDetail,1);
            btnLayout->addWidget(pOperFactor,1);
            btnLayout->addWidget(pOperDele,1);
            btnLayout->setMargin(2);
            btnLayout->setAlignment(Qt::AlignCenter);
            ui->tableWidget_Dev->setCellWidget(row, 3, btnWidget);

            ui->tableWidget_Dev->setRowHeight(row,64);

            it++;
            row++;
        }

        connect(m_SignalMapper_DevM,SIGNAL(mapped(QString)),this,SLOT(onButtonDevMore(QString)));
        connect(m_SignalMapper_DevF,SIGNAL(mapped(QString)),this,SLOT(onButtonDevFactor(QString)));
        connect(m_SignalMapper_DevD,SIGNAL(mapped(QString)),this,SLOT(onButtonDevDele(QString)));

        // QMessageBox::about(NULL, "提示", "<font color='black'>获取设备信息成功！</font>");
        return true;
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取设备信息失败！</font>");
    }
    return false;
}

void MainWindow::OpenDev_Setting()
{
    ui->stackedWidget->setCurrentIndex(10);
    DevGui_Init();
}

void MainWindow::OpenData_Query()
{
    ui->stackedWidget->setCurrentIndex(9);
    ClearTable();
    ui->factorBox->clear();
    ui->comboBox_3->clear();
    QDateTime currentTime = QDateTime::currentDateTime();       //获取当前系统时间
    QString current_Time = currentTime.toString("yyyy-MM-dd hh:mm:ss");     //设置时间格式
    QString beforeDayTime = currentTime.addSecs(-3600).toString("yyyy-MM-dd hh:mm:ss");  //获取前一天时间

    ui->dateTimeEdit->setDateTime(QDateTime::fromString(beforeDayTime, "yyyy-MM-dd hh:mm:ss"));
    ui->dateTimeEdit_2->setDateTime(QDateTime::currentDateTime());

    QJsonObject pDev_Factor,pDev_CommomFacs;
    httpclinet pClient;
    if(pClient.get(DCM_DEVICE_FACTOR,pDev_Factor))
    {
        QJsonObject::iterator iter = pDev_Factor.begin();
        for(;iter != pDev_Factor.end(); iter++)
        {
            QString fac_fullName;
            if(pClient.get(DCM_FACTOR,pDev_CommomFacs))
            {
                QJsonObject valueObj = pDev_CommomFacs[iter.key().split("-")[1]].toObject();
                fac_fullName = iter.key().split("-")[1] +"-"+ valueObj.value("name").toString();
            }
            if(!fac_fullName.isEmpty())
                ui->factorBox->addItem(fac_fullName);
        }
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取设备因子信息失败！</font>");
    }

    if(pClient.get(DCM_MNINFO,m_JsonArray))
    {
        for(int i=0;i<m_JsonArray.size();i++)  //获得总数据的个数 做个循环来处理每条数据
        {
            QJsonValue iconArray = m_JsonArray.at(i);
            QJsonObject icon = iconArray.toObject();
            QString b = icon[QLatin1String("ip_addr_port")].toString();
            ui->comboBox_3->addItem(b,icon[QLatin1String("ip_addr_port")].toString());
        }
    }else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取平台地址信息失败！</font>");
    }
}

void MainWindow::OpenMasterSet(){
    ui->stackedWidget->setCurrentIndex(8);

    UploadGui_Init();
}

void MainWindow::OpenTimeSet(){
    ui->stackedWidget->setCurrentIndex(7);

    ui->dateTimeEdit_local->setDisplayFormat(QLatin1String("yyyy-MM-dd hh:mm:ss"));  //格式
    ui->dateTimeEdit_local->setReadOnly(true);

    ui->dateTimeEdit_self->setDisplayFormat(QLatin1String("yyyy-MM-dd hh:mm:ss"));  //格式
    ui->dateTimeEdit_self->setCalendarPopup(true);        // 日历
    ui->dateTimeEdit_self->setDateTime(QDateTime::currentDateTime());
}

void MainWindow::OpenBufa(){
    ui->stackedWidget->setCurrentIndex(6);
    ui->comboBox->clear();

    ui->dateTimeEdit_Beg->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->dateTimeEdit_End->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->dateTimeEdit_Beg->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEdit_End->setDateTime(QDateTime::currentDateTime());

    QJsonArray pJsonArray;
    httpclinet pClient;
    if(pClient.get(DCM_MNINFO,pJsonArray))
    {
        for(int i=0;i<pJsonArray.size();i++)  //获得总数据的个数 做个循环来处理每条数据
        {
            QJsonValue iconArray = pJsonArray.at(i);
            QJsonObject icon = iconArray.toObject();
            QString b = icon[QLatin1String("mn")].toString() + QLatin1String("[") +icon[QLatin1String("ip_addr_port")].toString() + QLatin1String("]");
            ui->comboBox->addItem(b,icon[QLatin1String("ip_addr_port")].toString());
        }

    }else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取MN和平台地址信息失败！</font>");
    }

}

void MainWindow::OpenComDebug(){
    ui->stackedWidget->setCurrentIndex(5);

}

void MainWindow::OpenNeworkSetting(){
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::OpenSysInfo(){
    ui->stackedWidget->setCurrentIndex(3);

    ui->lineEdit_Q->setText(QTAPP_VER);
    ui->lineEdit_Q->setAlignment(Qt::AlignCenter);

    QJsonObject pJsonObj;
    httpclinet pClient;
    if(pClient.get(DCM_VERSION,pJsonObj))
    {
        // qDebug() << "==>>" << pJsonObj;
        if(pJsonObj.contains("sn"))
        {
            ui->lineEdit_MN->setText(pJsonObj.value("sn").toString());
            ui->lineEdit_MN->setAlignment(Qt::AlignCenter);
        }
        if(pJsonObj.contains("主程序版本"))
        {
            ui->lineEdit_M->setText(pJsonObj.value("主程序版本").toString());
            ui->lineEdit_M->setAlignment(Qt::AlignCenter);
        }
        if(pJsonObj.contains("驱动版本"))
        {
            ui->lineEdit_D->setText(pJsonObj.value("驱动版本").toString());
            ui->lineEdit_D->setAlignment(Qt::AlignCenter);
        }
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>主程序和驱动查询失败！</font>");
    }
}

void MainWindow::on_pushButtonQuery_clicked()
{
    QJsonObject pJsonObj;
    httpclinet pClient;
    if(pClient.get(DCM_GET_IPADDR,pJsonObj))
    {
        //qDebug() << "DCM_GET_IPADDR===>>" << pJsonObj;
        if(pJsonObj.contains("ipaddr") && pJsonObj.contains("gateway") && pJsonObj.contains("netmask"))
        {
            ui->lineEditIP->setText(pJsonObj.value("ipaddr").toString());
            ui->lineEditGW->setText(pJsonObj.value("gateway").toString());
            ui->lineEditNM->setText(pJsonObj.value("netmask").toString());
            QMessageBox::about(NULL, "提示", "<font color='black'>获取网口配置信息成功！</font>");
        }
        else
        {
            QMessageBox::about(NULL, "提示", "<font color='black'>未能获取到网口配置信息！</font>");
        }
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取网口配置信息失败！</font>");
    }
}

static bool net_address_check(QString currentIp)
{
    QRegExp rxp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if(!rxp.exactMatch(currentIp))
    {
        return false;
    }
    return true;
}

void MainWindow::on_pushButtonModify_clicked()
{
    if(!net_address_check(ui->lineEditIP->text()))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>IP地址错误！</font>");
        return;
    }
    if(!net_address_check(ui->lineEditGW->text()))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>网关地址错误！</font>");
        return;
    }
    if(!net_address_check(ui->lineEditNM->text()))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>子网掩码地址错误！</font>");
        return;
    }

    QJsonObject obj;
    obj.insert(QLatin1String("dhcp"), 0);
    obj.insert(QLatin1String("ipaddr"), ui->lineEditIP->text());
    obj.insert(QLatin1String("gateway"), ui->lineEditGW->text());
    obj.insert(QLatin1String("netmask"), ui->lineEditNM->text());

    httpclinet pClient;
    if(pClient.post(DCM_SET_IPADDR,obj))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>更改网口配置成功，系统即将重启！</font>");
        return;
    }
}

void MainWindow::on_confirmBtn_clicked()
{
    if(ui->comboBox_7->currentText() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>端口号不可为空！</font>");
        return;
    }
    if(ui->payload->toPlainText() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>发送内容不可为空！</font>");
        return;
    }

    int is_hex = hexGroup->checkedId() == 0;
    int baudrate = ui->baudBox->currentText().toInt();
    int data_bit = ui->dataBitBox->currentText().toInt();

    QString pComTex = ui->comboBox_7->currentText();
    Util pUtil;
    QString pTex = pUtil.Uart_Convert(pComTex);

    int stop_bit = ui->stopBitBox->currentText().toInt();
    QString parity = ui->checkBitBox->currentText();
    QString payload = ui->payload->toPlainText();

    QJsonObject obj;
    obj.insert(QLatin1String("time_out"), 0);
    obj.insert(QLatin1String("baudrate"), baudrate);
    if(pTex == "") obj.insert(QLatin1String("com"), pComTex);
    else obj.insert(QLatin1String("com"), pTex);
    obj.insert(QLatin1String("data_bit"), data_bit);
    obj.insert(QLatin1String("is_hex"), is_hex);
    obj.insert(QLatin1String("parity"),parity);
    obj.insert(QLatin1String("payload"), payload);
    obj.insert(QLatin1String("stop_bit"), stop_bit);

    // qDebug() << "obj==>>" << obj;

    httpclinet pClient;
    if(pClient.post(DCM_SERIAL_DEBUG,obj))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>发送成功！</font>");
        return;
    }
}

void MainWindow::on_pushButton_Bufa_clicked()
{
    QString endt =ui->dateTimeEdit_End->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"));
    QString startt =ui->dateTimeEdit_Beg->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"));
    QString port = ui->comboBox->currentData().toString();

    QString pRequest = DCM_MANUAL_MSG
            +QUrl::toPercentEncoding(port)
            +QLatin1String("&startt=")+QUrl::toPercentEncoding(startt)
            +QLatin1String("&endt=")+QUrl::toPercentEncoding(endt);

    QJsonObject obj;

    httpclinet pClient;
    if(pClient.post(pRequest,obj))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>发送成功！</font>");
        return;
    }
}

void MainWindow::on_pushButton_setlocal_clicked()
{
    QString pTimeSet = QDateTime::currentDateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"));
    QNetworkRequest request;
    request.setUrl(QUrl(DCM_SET_TIME+pTimeSet));

    QJsonObject obj;
    httpclinet pClient;
    if(pClient.post(DCM_SET_TIME+pTimeSet,obj))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>同步系统时间成功！</font>");
        return;
    }
}

void MainWindow::on_pushButton_setself_clicked()
{
    QString pTimeSet = ui->dateTimeEdit_self->dateTime().toString( QLatin1String("yyyy-MM-dd hh:mm:ss") );

    QNetworkRequest request;
    request.setUrl(QUrl(DCM_SET_TIME+pTimeSet));

    QJsonObject obj;
    httpclinet pClient;
    if(pClient.post(DCM_SET_TIME+pTimeSet,obj))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>同步自定义时间成功！</font>");
        return;
    }
}

void MainWindow::on_pushButton_clicked()
{
    UpsetAdd *pUpsetAdd = new UpsetAdd();
    pUpsetAdd->show();
}

void MainWindow::on_pushButton_3_clicked()
{
    // fresh
    if(UploadGui_Init())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取上传配置信息成功！</font>");
    }
}

void MainWindow::on_pushButtonFind_clicked()
{




    if(ui->checkBox->isChecked())
    {
        this->qingqiu(0);
    }
    else if(ui->checkBox_2->isChecked())
    {
        this->qingqiuHisData(0,"minute");
    }
    else if(ui->checkBox_4->isChecked())
    {
        this->qingqiuHisData(0,"hour");
    }
    else if(ui->checkBox_3->isChecked())
    {
        this->qingqiuHisData(0,"daily");
    }
    else if(ui->checkBox_9->isChecked())
    {
        this->qingqiuHisData(0,"month");
    }
    else if(ui->checkBox_8->isChecked())
    {
        this->qingqiuMessage(0);
    }
}

void MainWindow::on_pushButtonExport_clicked()
{
    QMessageBox::about(NULL, "提示", "<font color='black'>暂不支持该功能！</font>");
    return;

//    m_XlsxFactorCols = 0;
//    m_HisFactorList.clear();

//    m_UDiskPath = "";
//    foreach (const QStorageInfo &storage, QStorageInfo::mountedVolumes())
//    {
//        if (storage.isValid() && storage.isReady())
//        {
//             // qDebug() << "storage.rootPath-------------->>" << storage.rootPath();
//             // qDebug() << "storage.device-------------->>" << storage.device();
//            if(storage.device().contains("/dev/sd"))
//            {
//                if(storage.rootPath().contains("/media/") || (storage.rootPath() == "/mnt/usb1") ||(storage.rootPath() == "/mnt/sdb1"))
//                {
//                    m_UDiskPath = storage.rootPath();
//                    qDebug() << "m_UDiskPath===>>" << m_UDiskPath;
//                }
//            }
//        }
//    }

//    if(m_UDiskPath != "")
//    {
//        m_ExportPage = 0;
//        m_ExportProgress = 0;
//        ui->pushButtonExport->setEnabled(false);
//        ui->progressBar->setValue(0);
//        ui->progressBar->show();

//        if(ui->checkBox->isChecked())
//        {
//            qingqiu(m_ExportPage);
//        }
//        else if(ui->checkBox_2->isChecked())
//        {
//            this->qingqiuHisData(m_ExportPage,"minute");
//        }
//        else if(ui->checkBox_4->isChecked())
//        {
//            this->qingqiuHisData(m_ExportPage,"hour");
//        }
//        else if(ui->checkBox_3->isChecked())
//        {
//            this->qingqiuHisData(m_ExportPage,"daily");
//        }
//        else if(ui->checkBox_9->isChecked())
//        {
//            this->qingqiuHisData(m_ExportPage,"month");
//        }
//    }
//    else {
//        msgBox::information(QStringLiteral("通知"), QStringLiteral("未监测到U盘!"));
//        return;
//    }
}

void MainWindow::ShowNextPage()
{
//    // fixme: curpage > totalpage
    if(ui->checkBox->isChecked())
    {
        this->qingqiu(m_CurPage+1);
    }
    else if(ui->checkBox_2->isChecked())
    {
        this->qingqiuHisData(m_CurPage+1,"minute");
    }
    else if(ui->checkBox_4->isChecked())
    {
        this->qingqiuHisData(m_CurPage+1,"hour");
    }
    else if(ui->checkBox_3->isChecked())
    {
        this->qingqiuHisData(m_CurPage+1,"daily");
    }
    else if(ui->checkBox_9->isChecked())
    {
        this->qingqiuHisData(m_CurPage+1,"month");
    }
    else if(ui->checkBox_8->isChecked())
    {
        this->qingqiuMessage(m_CurPage+1);
    }
//    else if(ui->checkBox_6->isChecked())
//    {
//        this->ShowSysOperTable(gCurPage+1);
//    }
//    else if(ui->checkBox_7->isChecked())
//    {
//        this->ShowInspTable(gCurPage+1);
//    }
}

void MainWindow::ShowNewPage()
{
    int qSkipPage = ui->textEditToPage->toPlainText().toInt()-1;

    if(qSkipPage < 0 || qSkipPage >= m_TotalPage)
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>输入错误，请重新输入！</font>");
        return;
    }

    if(ui->checkBox->isChecked())
    {
        this->qingqiu(qSkipPage);
    }
    else if(ui->checkBox_2->isChecked())
    {
        this->qingqiuHisData(qSkipPage,"minute");
    }
    else if(ui->checkBox_4->isChecked())
    {
        this->qingqiuHisData(qSkipPage,"hour");
    }
    else if(ui->checkBox_3->isChecked())
    {
        this->qingqiuHisData(qSkipPage,"daily");
    }
    else if(ui->checkBox_9->isChecked())
    {
        this->qingqiuHisData(qSkipPage,"month");
    }
    else if(ui->checkBox_8->isChecked())
    {
        this->qingqiuMessage(qSkipPage);
    }
//    else if(ui->checkBox_6->isChecked())
//    {
//        this->ShowSysOperTable(qSkipPage);
//    }
//    else if(ui->checkBox_7->isChecked())
//    {
//        this->ShowInspTable(qSkipPage);
//    }
}

void MainWindow::ShowLastPage()
{

//    // fixme: curpage < 1
    if(ui->checkBox->isChecked())
    {
        this->qingqiu(m_CurPage-1);
    }
    else if(ui->checkBox_2->isChecked())
    {
        this->qingqiuHisData(m_CurPage-1,"minute");
    }
    else if(ui->checkBox_4->isChecked())
    {
        this->qingqiuHisData(m_CurPage-1,"hour");
    }
    else if(ui->checkBox_3->isChecked())
    {
        this->qingqiuHisData(m_CurPage-1,"daily");
    }
    else if(ui->checkBox_9->isChecked())
    {
        this->qingqiuHisData(m_CurPage-1,"month");
    }
    else if(ui->checkBox_8->isChecked())
    {
        this->qingqiuMessage(m_CurPage-1);
    }
//    else if(ui->checkBox_6->isChecked())
//    {
//        this->ShowSysOperTable(gCurPage-1);
//    }
//    else if(ui->checkBox_7->isChecked())
//    {
//        this->ShowInspTable(gCurPage-1);
//    }
}

void MainWindow::qingqiu(int page)
{
    if(ui->pushButtonExport->isEnabled())
    {
        ui->progressBar->setValue(0);
        ui->progressBar->show();
    }

    m_CurPage = page;

    QJsonObject obj;
    obj.insert(QLatin1String("until"), ui->dateTimeEdit_2->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm")) );
    obj.insert(QLatin1String("pages_index"),page);
    obj.insert(QLatin1String("page_size"),PAGE_SIZE);
    obj.insert(QLatin1String("since"), ui->dateTimeEdit->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm")) );

    QJsonObject pJsonReply;
    httpclinet pClient;
    if(pClient.post(DCM_HISTORY_RTD+ui->factorBox->currentText().split("-")[0],obj,pJsonReply))
    {
        // qDebug() << "post reply: " << pJsonReply;

        if(pJsonReply.contains(QLatin1String("data"))) {
            m_TotalPage = pJsonReply[QLatin1String("pages")].toInt();
            QJsonValue arrayValue = pJsonReply.value(QLatin1String("data"));
            if(arrayValue.isArray()) {
                m_history_rtd = arrayValue.toArray(); // 保存下来
            }
        }

        if(m_history_rtd.size() <= 0)
        {
            QMessageBox::about(NULL, "提示", "<font color='black'>未查询到任何数据，请重新选择起始时间！</font>");
            ui->progressBar->setValue(0);
            ui->progressBar->hide();
            return;
        }

        // 通过导出按钮的状态来分辨 是 导出还好是展示
        if(ui->pushButtonExport->isEnabled())
        {
            ui->progressBar->setValue(70+qrand()%9);
            usleep(200000);
            this->setTableHeader();
            this->setTableContents(m_history_rtd);
        }
//        else
//        {
//            // qDebug() << "m_ExportPage=====>" << m_ExportPage;
//            Export(history_real_time_data);
//            if((m_ExportPage+1)!= gTotalPage)                               //修改点
//            {
//                qingqiu(m_ExportPage++);
//                // qDebug() << "------------>>>" << (m_ExportPage*100)/gTotalPage;
//                ui->progressBar->setValue((m_ExportPage*100)/gTotalPage);
//            }
//            else    //export done
//            {
//                is_first = 1;   // for table head

//                QString path=QString("%5/Real-time-data_%1_%2_%3%4")
//                        .arg(ui->factorBox->currentText())
//                        .arg(ui->dateTimeEdit->dateTime().toString(QLatin1String("yyyy-MM-dd")))
//                        .arg(ui->dateTimeEdit_2->dateTime().toString(QLatin1String("yyyy-MM-dd")))
//                        .arg(QLatin1String(".xlsx"))
//                        .arg(m_UDiskPath);
//                xlsx.saveAs(path);

//                ui->progressBar->setValue(100);
//                usleep(200000);
//                ui->progressBar->hide();
//                ui->pushButtonExport->setEnabled(true);
//                msgBox::information(QStringLiteral("通知"), QStringLiteral("数据已保存到U盘!"));
//            }
//        }
        return;
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取实时数据失败！</font>");
        return;
    }
}

//初始化表头
void MainWindow::setTableHeader()
{
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#1B89A1}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
"QTableWidget{background-color:white;border:none;}");

//设置表头
QStringList headerText;
headerText << QStringLiteral("序号") << QStringLiteral("采集时间")
           << QStringLiteral("因子编码") << QStringLiteral("实时值") <<  QStringLiteral("数据标识") ;
int cnt = headerText.count();
ui->tableWidget->setColumnCount(cnt);
ui->tableWidget->setHorizontalHeaderLabels(headerText);
ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget->horizontalHeader()->font();

ui->tableWidget->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {100, 245,245,245,245};                  //1080
for (int i = 0;i < cnt; i++ ){ //列编号从0开始
    ui->tableWidget->setColumnWidth(i, widths[i]);
}

ui->tableWidget->setStyleSheet(qssTV);
ui->tableWidget->horizontalHeader()->setVisible(true);
ui->tableWidget->verticalHeader()->setDefaultSectionSize(45);
}

//填充表格
void MainWindow::setTableContents(QJsonArray &history_real_time_data)
{
    ui->progressBar->setValue(100);
    usleep(100000);
    if(m_CurPage == 0) ui->pushButtonLast->setEnabled(false);
    else ui->pushButtonLast->setEnabled(true);
    if(m_CurPage+1 == m_TotalPage) ui->pushButtonNext->setEnabled(false);
    else ui->pushButtonNext->setEnabled(true);

    ui->textEditCurPage->setText("当前：第" + QString::number(m_CurPage+1) + "页");
    ui->textEditCurPage->setAlignment(Qt::AlignCenter);
    ui->textEditToPage->setAlignment(Qt::AlignCenter);
    ui->textEditAllPage->setText("共：" + QString::number(m_TotalPage) + "页");
    ui->textEditAllPage->setAlignment(Qt::AlignCenter);

    ui->tableWidget->verticalHeader()->setVisible(false);//表头不可见
    ui->tableWidget->clearContents(); //只清除工作区，不清除表头
    if(history_real_time_data.isEmpty()) return ;

    int cnt = history_real_time_data.size();
    ui->tableWidget->setRowCount(cnt);

    QJsonObject json;
    QTableWidgetItem *item;
    for(int i = 0; i < cnt; i++)
    {
        json = history_real_time_data.at(i).toObject();

        // 序号
        item = new QTableWidgetItem( QString::number(i+1,10) );
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,0,item);

        //采集时间
        QString a = json[QLatin1String("CollectAt")].toString();
        a.replace("T"," ");a.replace("Z"," ");
        item = new QTableWidgetItem( a );
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,1,item);

        //因子名称
        item = new QTableWidgetItem( ui->factorBox->currentText() );
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,2,item);

        //实时值
        item = new QTableWidgetItem( json[QLatin1String("Data")].toString() );
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,3,item);

        //数据标识
        item = new QTableWidgetItem( json[QLatin1String("Flag")].toString() );
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,4,item);
    }

    ui->progressBar->hide();
}

//查询历史shishi数据
void MainWindow::qingqiuHisData(int page, QString type)
{
    if(ui->pushButtonExport->isEnabled())
    {
        ui->progressBar->setValue(0);
        ui->progressBar->show();
    }

    m_CurPage = page;

    if(ui->pushButtonExport->isEnabled())
    {
        ui->progressBar->setValue(30+qrand()%9);
        usleep(200000);
    }

    QJsonObject obj;
    obj.insert(QLatin1String("until"), ui->dateTimeEdit_2->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm")) );
    obj.insert(QLatin1String("pages_index"),page);
    obj.insert(QLatin1String("page_size"),200);
    obj.insert(QLatin1String("since"), ui->dateTimeEdit->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm")) );

    QJsonObject pJsonReply;
    httpclinet pClient;
    if(pClient.post(DCM_HISTORY+type,obj,pJsonReply))
    {
        qDebug() << "post reply: " << pJsonReply;

        if(pJsonReply.contains(QLatin1String("data"))) {
            qDebug()<<__LINE__<<endl;
            m_TotalPage = pJsonReply[QLatin1String("pages")].toInt();
            QJsonValue arrayValue = pJsonReply.value(QLatin1String("data"));
            qDebug()<<__LINE__<<endl;
            if(arrayValue.isArray()) {
                qDebug()<<__LINE__<<endl;
                QJsonArray resArray = arrayValue.toArray();
                qDebug()<<__LINE__<<resArray<<endl;
                int num = 0;
                for(auto res:resArray)
                {
                    QJsonObject jObj = res.toObject();
                    qDebug()<<__LINE__<<num<<jObj<<endl;
                    if(jObj.value("FactorCode").toString() == ui->factorBox->currentText().split("-")[0])
                    {
                        qDebug()<<__LINE__<<num<<arrayValue.toArray()[num]<<endl;
                        m_history_data.append(res); // 保存下来
                    }
                }


            }
        }

        qDebug()<<__LINE__<<endl;

        if(m_history_data.size() <= 0)
        {
            QMessageBox::about(NULL, "提示", "<font color='black'>未查询到任何数据，请重新选择起始时间！</font>");
            ui->progressBar->setValue(0);
            ui->progressBar->hide();
            ui->pushButtonExport->setEnabled(true);
            return;
        }

        qDebug()<<__LINE__<<endl;
        // 通过导出按钮的状态来分辨 是 导出还好是展示
        if(ui->pushButtonExport->isEnabled())
        {
            ui->progressBar->setValue(70+qrand()%9);
            usleep(200000);
            qDebug()<<__LINE__<<endl;

            this->setHisTableContents(m_history_data);
        }
        else
        {
//            // qDebug()<< "history_real_time_data======>>" << history_real_time_data;
//            // qDebug() << "m_ExportPage=====>" << m_ExportPage;
//            ExportHis(history_real_time_data);
//            if((m_ExportPage+1)!= gTotalPage)                               //修改点
//            {
//                if(ui->checkBox_2->isChecked())
//                {
//                    this->qingqiuHisData(m_ExportPage++,"minute");
//                }
//                else if(ui->checkBox_4->isChecked())
//                {
//                    this->qingqiuHisData(m_ExportPage++,"hour");
//                }
//                else if(ui->checkBox_3->isChecked())
//                {
//                    this->qingqiuHisData(m_ExportPage++,"daily");
//                }
//                else if(ui->checkBox_9->isChecked())
//                {
//                    this->qingqiuHisData(m_ExportPage++,"month");
//                }

//                // qDebug() << "------------>>>" << (m_ExportPage*100)/gTotalPage;
//                ui->progressBar->setValue((m_ExportPage*100)/gTotalPage);
//            }
//            else    //export done
//            {
//                is_first = 1;   // for table head
//                QString pExportType;
//                if(ui->checkBox_2->isChecked())
//                {
//                    pExportType = "minute";
//                }
//                else if(ui->checkBox_4->isChecked())
//                {
//                    pExportType = "hour";
//                }
//                else if(ui->checkBox_3->isChecked())
//                {
//                    pExportType = "daily";
//                }
//                else if(ui->checkBox_9->isChecked())
//                {
//                    pExportType = "month";
//                }

//                QString path=QString("%5/History-Data_%1_%2_%3%4")
//                        .arg(pExportType)
//                        .arg(ui->dateTimeEdit->dateTime().toString(QLatin1String("yyyy-MM-dd")))
//                        .arg(ui->dateTimeEdit_2->dateTime().toString(QLatin1String("yyyy-MM-dd")))
//                        .arg(QLatin1String(".xlsx"))
//                        .arg(m_UDiskPath);
//                xlsx.saveAs(path);

//                ui->progressBar->setValue(100);
//                usleep(200000);
//                ui->progressBar->hide();
//                ui->pushButtonExport->setEnabled(true);
//                msgBox::information(QStringLiteral("通知"), QStringLiteral("数据已保存到U盘!"));
//            }
        }
    }
}

void MainWindow::setHisTableContents(QJsonArray &history_real_time_data)
{
    ui->progressBar->setValue(100);
    usleep(200000);

    if(m_CurPage == 0) ui->pushButtonLast->setEnabled(false);
    else ui->pushButtonLast->setEnabled(true);
    if(m_CurPage+1 == m_TotalPage) ui->pushButtonNext->setEnabled(false);
    else ui->pushButtonNext->setEnabled(true);

    // InsertAt
    QStringList pFactorList;
    QMap<QString, mapStrString> pMapTimeFacDataList;

    for(int i=0;i<history_real_time_data.size();i++)
    {
        QJsonObject pFactorObj = history_real_time_data.at(i).toObject();
        QString pFactID = pFactorObj.value("FactorCode").toString();
        if(!pFactorList.contains(pFactID))
        {
            pFactorList.clear();
            pFactorList << pFactID;
        }

        QString pTimestamp = pFactorObj.value("InsertAt").toString();
        // fill pMapTimeFacDataList
        QString pMax = pFactorObj.value("Max").toString();
        QString pMin = pFactorObj.value("Min").toString();
        QString pAvg = pFactorObj.value("Avg").toString();
        QString pSum = pFactorObj.value("Sum").toString();
        QString pFlag = pFactorObj.value("Flag").toString();
        QString pData = pMax + "-" + pMin + "-" + pAvg + "-" +pSum + "-" + pFlag;
        if(pMapTimeFacDataList.contains(pTimestamp))
        {
            pMapTimeFacDataList[pTimestamp].insert(pFactID,pData);
        }
        else
        {
            mapStrString pStrStrList;
            pStrStrList.insert(pFactID,pData);
            pMapTimeFacDataList.insert(pTimestamp,pStrStrList);
        }
    }

    // qDebug() << "pMapTimeFacDataList============>>" << pMapTimeFacDataList;
    ui->textEditCurPage->setText("当前：第" + QString::number(m_CurPage+1) + "页");
    ui->textEditCurPage->setAlignment(Qt::AlignCenter);
    ui->textEditToPage->setAlignment(Qt::AlignCenter);
    ui->textEditAllPage->setText("共：" + QString::number(m_TotalPage) + "页");
    ui->textEditAllPage->setAlignment(Qt::AlignCenter);

    ui->tableWidget->verticalHeader()->setVisible(false);//表头不可见
    ui->tableWidget->horizontalHeader()->setVisible(false);//表头不可见
    ui->tableWidget->clearContents(); //只清除工作区，不清除表头
    if(history_real_time_data.isEmpty()) return ;

    int pRowNum = pMapTimeFacDataList.size();
    int pColNum = pFactorList.size();

    ui->tableWidget->setRowCount(2+pRowNum);
    ui->tableWidget->setColumnCount(2+pColNum*5);
    ui->tableWidget->clearContents();

    ui->tableWidget->setColumnWidth(0, 50); // xuhao
    ui->tableWidget->setColumnWidth(1, 200); // timestamp
    for (int i = 0;i < pColNum; i++ ){
        ui->tableWidget->setColumnWidth(i+2, 100);
    }

    // hebing danyuange
    ui->tableWidget->setSpan(0,0,2,1);  // xuhao
    ui->tableWidget->setSpan(0,1,2,1);  // timestamp

    QTableWidgetItem * item0 = new QTableWidgetItem( "序号" );
    item0->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(0,0,item0);
    item0->setBackground(Qt::lightGray);

    QTableWidgetItem * itemStamp = new QTableWidgetItem( "时间戳" );
    itemStamp->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(0,1,itemStamp);
    itemStamp->setBackground(Qt::darkGray);

    for(int i=0;i<pColNum;i++)
    {
        ui->tableWidget->setSpan(0,2+i*5,1,5);


        QString fac_fullName;
        QJsonObject pDev_CommomFacs;
        httpclinet pClinet;
        if(pClinet.get(DCM_FACTOR,pDev_CommomFacs))
        {
            QJsonObject valueObj = pDev_CommomFacs[pFactorList.at(i)].toObject();
            fac_fullName = pFactorList.at(i) +"-"+ valueObj.value("name").toString();
        }

        QTableWidgetItem * itemFactorName = new QTableWidgetItem( fac_fullName );
        itemFactorName->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(0,2+i*5,itemFactorName);
        itemFactorName->setBackground(Qt::darkYellow);

        QTableWidgetItem * itemMax = new QTableWidgetItem( "最大值" );
        itemMax->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(1,2+i*5,itemMax);
        itemMax->setBackground(Qt::red);

        QTableWidgetItem * itemMin = new QTableWidgetItem( "最小值" );
        itemMin->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(1,3+i*5,itemMin);
        itemMin->setBackground(Qt::yellow);

        QTableWidgetItem * itemAvg = new QTableWidgetItem( "平均值" );
        itemAvg->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(1,4+i*5,itemAvg);
        itemAvg->setBackground(Qt::cyan);

        QTableWidgetItem * itemSum = new QTableWidgetItem( "累计值" );
        itemSum->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(1,5+i*5,itemSum);
        itemSum->setBackground(Qt::darkCyan);

        QTableWidgetItem * itemFlag = new QTableWidgetItem( "数据标记" );
        itemFlag->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(1,6+i*5,itemFlag);
        itemFlag->setBackground(Qt::magenta);
    }

    int pRow = 2;
    QTableWidgetItem *item = NULL;
    QMap<QString, mapStrString>::const_iterator i = pMapTimeFacDataList.constBegin();
    while (i != pMapTimeFacDataList.constEnd()) { // tranverse row

        // 序号
        item = new QTableWidgetItem( QString::number(pRow-1) ); //fixme:  ======>>next page???
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(pRow,0,item);
        //采集时间
        item = new QTableWidgetItem( i.key() );
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(pRow,1,item);

        mapStrString pStrString = i.value();
        for(int j=0;j<pColNum;j++)
        {
            if(pStrString.contains(pFactorList.at(j)))
            {
                QString pData = pStrString[pFactorList.at(j)]; // max min avg sum flag
                QStringList pDataList = pData.split("-");
                if(pDataList.size() == 5)
                {
                    if(pDataList[0] == "") pDataList[0] = "-";
                    item = new QTableWidgetItem( pDataList[0] );
                    item->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidget->setItem(pRow,2+j*5,item);

                    if(pDataList[1] == "") pDataList[1] = "-";
                    item = new QTableWidgetItem( pDataList[1] );
                    item->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidget->setItem(pRow,3+j*5,item);

                    if(pDataList[2] == "") pDataList[2] = "-";
                    item = new QTableWidgetItem( pDataList[2] );
                    item->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidget->setItem(pRow,4+j*5,item);

                    if(pDataList[3] == "") pDataList[3] = "-";
                    item = new QTableWidgetItem( pDataList[3] );
                    item->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidget->setItem(pRow,5+j*5,item);

                    if(pDataList[4] == "") pDataList[4] = "-";
                    item = new QTableWidgetItem( pDataList[4] );
                    item->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidget->setItem(pRow,6+j*5,item);
                }
            }
            else
            {
                item = new QTableWidgetItem( "-" );
                item->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(pRow,2+j*5,item);

                item = new QTableWidgetItem( "-" );
                item->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(pRow,3+j*5,item);

                item = new QTableWidgetItem( "-" );
                item->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(pRow,4+j*5,item);

                item = new QTableWidgetItem( "-" );
                item->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(pRow,5+j*5,item);

                item = new QTableWidgetItem( "-" );
                item->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(pRow,6+j*5,item);
            }
        }
        ++pRow;
        ++i;
    }
    ui->progressBar->hide();
}

void MainWindow::qingqiuMessage(int page)
{
    ui->progressBar->setValue(0);
    ui->progressBar->show();

    QJsonObject obj;
    obj.insert(QLatin1String("until"), ui->dateTimeEdit_2->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm")) );
    obj.insert(QLatin1String("pages_index"),page);
    obj.insert(QLatin1String("page_size"),200);
    obj.insert(QLatin1String("since"), ui->dateTimeEdit->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm")) );

    QJsonObject pJsonReply;
    httpclinet pClient;
    if(pClient.post(DCM_UP_MESSAGE+ui->comboBox_3->currentText(),obj,pJsonReply))
    {
        //qDebug() << "post reply: " << pJsonReply;
        if(pJsonReply.contains(QLatin1String("data"))) {
            m_TotalPage = pJsonReply[QLatin1String("pages")].toInt();
            QJsonValue arrayValue = pJsonReply.value(QLatin1String("data"));
            if(arrayValue.isArray()) {
                m_history_message = arrayValue.toArray(); // 保存下来
            }
        }

        this->setMsgTableHeader();
        this->setMsgTableContents(m_history_message);
    }
}

//初始化表头
void MainWindow::setMsgTableHeader()
{
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#1B89A1}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
"QTableWidget{background-color:white;border:none;}");

//设置表头
QStringList headerText;
headerText << QStringLiteral("序号")
           << QStringLiteral("日期时间") << QStringLiteral("数据方向") <<  QStringLiteral("服务器地址") << QStringLiteral("数据包内容");
int cnt = headerText.count();
ui->tableWidget->setColumnCount(cnt);
// ui->tableWidget->setRowCount(10);
ui->tableWidget->setHorizontalHeaderLabels(headerText);
ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget->horizontalHeader()->font();

ui->tableWidget->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {50,200,120,200,600};                  //1080
for (int i = 0;i < cnt; i++ ){ //列编号从0开始
    ui->tableWidget->setColumnWidth(i, widths[i]);
}

ui->tableWidget->setStyleSheet(qssTV);
ui->tableWidget->horizontalHeader()->setVisible(true);
ui->tableWidget->verticalHeader()->setDefaultSectionSize(45);
}

void MainWindow::setMsgTableContents(QJsonArray &history_real_time_data)
{
    ui->progressBar->setValue(100);
    usleep(200000);
    ui->progressBar->hide();

    if(m_CurPage == 0) ui->pushButtonLast->setEnabled(false);
    else ui->pushButtonLast->setEnabled(true);
    if(m_CurPage+1 == m_TotalPage) ui->pushButtonNext->setEnabled(false);
    else ui->pushButtonNext->setEnabled(true);

    ui->textEditCurPage->setText("当前：第" + QString::number(m_CurPage+1) + "页");
    ui->textEditCurPage->setAlignment(Qt::AlignCenter);
    ui->textEditToPage->setAlignment(Qt::AlignCenter);
    ui->textEditAllPage->setText("共：" + QString::number(m_TotalPage) + "页");
    ui->textEditAllPage->setAlignment(Qt::AlignCenter);

    ui->tableWidget->verticalHeader()->setVisible(false);//表头不可见
    ui->tableWidget->clearContents(); //只清除工作区，不清除表头
    if(history_real_time_data.isEmpty())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>未查询到任何数据，请重新选择起始时间！</font>");
        return ;
    }

    int cnt = history_real_time_data.size();
    ui->tableWidget->setRowCount(cnt);

    if(cnt == 0)
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>未查询到任何数据，请重新选择起始时间！</font>");
    }

    QJsonObject json;
    QTableWidgetItem *item;
    for(int i = 0; i < cnt; i++)
    {
        json = history_real_time_data.at(i).toObject();

        // qDebug() << "json===xxx=>>" << json;

        // 序号
        item = new QTableWidgetItem( QString::number(i+1,10) );
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,0,item);

        //因子名称
        item = new QTableWidgetItem( json[QLatin1String("timestamp")].toString() );
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,1,item);

        //实时值
        item = new QTableWidgetItem( "--->" );
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,2,item);

        //数据标识
        item = new QTableWidgetItem( json[QLatin1String("target_addr")].toString() );
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,3,item);

        //数据 content  fixme: ... yingwenzimu zidonghuanhang
        item = new QTableWidgetItem( json[QLatin1String("message")].toString() );
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,4,item);
    }
}


void MainWindow::on_pushButton_AddDev_clicked()
{
    DevAdd *pDevAdd = new DevAdd();
    pDevAdd->show();
    return;
}

void MainWindow::refresh_AnalogDevParam()
{
    if(g_IsAnalogDevOperated)
    {
        g_IsAnalogDevOperated = false;

        int index = 0;
        // lineEdit_ID
        QString pDevParam = "";

        QJsonObject jsonObject;
        httpclinet pClient;

        if(pClient.get(DCM_CONF,jsonObject))
        {
            // analog_max_1=15,analog_min_1=0,upper_limit_1=5,lower_limit_1=1

            // g_Device_ID
            QJsonObject pJsonFactor = jsonObject.value(FACTORS).toObject();

            QJsonObject::const_iterator it = pJsonFactor.constBegin();
            QJsonObject::const_iterator end = pJsonFactor.constEnd();
            while(it != end)
            {
                if(it.key().contains(g_Device_ID))
                {
                    QJsonObject QJsonObjectFator = it.value().toObject();
                    qDebug() << "QJsonObjectFator---> " << QJsonObjectFator;
                    if(QJsonObjectFator.contains(CONF_IS_ANALOG_PARAM))
                    {
                        if(QJsonObjectFator.value(CONF_IS_ANALOG_PARAM).toBool())
                        {
                            QString pAlias,pAU1,pAD1,pAU2,pAD2;
                            if(QJsonObjectFator.contains(CONF_ANALOG_PARAM_AU1))
                                pAU1 = QString::number(QJsonObjectFator.value(CONF_ANALOG_PARAM_AU1).toVariant().toDouble());
                            if(QJsonObjectFator.contains(CONF_ANALOG_PARAM_AD1))
                                pAD1 = QString::number(QJsonObjectFator.value(CONF_ANALOG_PARAM_AD1).toVariant().toDouble());
                            if(QJsonObjectFator.contains(CONF_ANALOG_PARAM_AU2))
                                pAU2 = QString::number(QJsonObjectFator.value(CONF_ANALOG_PARAM_AU2).toVariant().toDouble());
                            if(QJsonObjectFator.contains(CONF_ANALOG_PARAM_AD2))
                                pAD2 = QString::number(QJsonObjectFator.value(CONF_ANALOG_PARAM_AD2).toVariant().toDouble());

                            if(QJsonObjectFator.contains(CONF_FACTOR_ALIAS))
                                pAlias = QString::number(QJsonObjectFator.value(CONF_FACTOR_ALIAS).toVariant().toInt());

                            // qDebug() << "pAlias---->>>>>" <<pAlias;

                            if(index == 0)
                            {
                                 // analog_max_1=15,analog_min_1=0,upper_limit_1=5,lower_limit_1=1
                                pDevParam += "analog_max_" + pAlias + "=" + pAU1 + ",analog_min_" + pAlias + "=" + pAD1 + ",upper_limit_" + pAlias + "=" + pAU2 + ",lower_limit_" + pAlias + "=" + pAD2;
                            }
                            else
                            {
                                pDevParam += ",analog_max_" + pAlias + "=" + pAU1 + ",analog_min_" + pAlias + "=" + pAD1 + ",upper_limit_" + pAlias + "=" + pAU2 + ",lower_limit_" + pAlias + "=" + pAD2;
                            }
                            index++;
                        }
                    }
                }
                it++;
            }
        }
        if(pDevParam != "")
        {
            // get & update
            QJsonObject pJsonDevs;
            httpclinet pClient;
            if(pClient.get(DCM_DEVICE,pJsonDevs))
            {
                QJsonObject::const_iterator it1 = pJsonDevs.constBegin();
                QJsonObject::const_iterator end1 = pJsonDevs.constEnd();
                while(it1 != end1)
                {
                    if(it1.key() == g_Device_ID)
                    {
                        QJsonObject pJsonDev = it1.value().toObject();

                        QJsonObject obj;
                        obj.insert(QLatin1String("id"), pJsonDev.value("id").toString());
                        obj.insert(QLatin1String("address"), pJsonDev.value("address").toInt());
                        obj.insert(QLatin1String("baudrate"), pJsonDev.value("baudrate").toInt());
                        obj.insert(QLatin1String("com"), pJsonDev.value("com").toString());

                        obj.insert(QLatin1String("data_bit"), pJsonDev.value("data_bit").toInt());
                        obj.insert(QLatin1String("dev_name"), pJsonDev.value("dev_name").toString());
                        obj.insert(QLatin1String("dev_params"), pDevParam);
                        obj.insert(QLatin1String("dev_type"), pJsonDev.value("dev_type").toString());

                        obj.insert(QLatin1String("ip_addr"), pJsonDev.value("ip_addr").toString());
                        obj.insert(QLatin1String("parity"), pJsonDev.value("parity").toString());
                        obj.insert(QLatin1String("stop_bit"), pJsonDev.value("stop_bit").toString());

                        // qDebug() << "dev param obj==>>" << obj;

                        httpclinet pClient;
                        if(pClient.put(DCM_DEVICE,obj))
                        {
                            QMessageBox::about(NULL, "提示", "<font color='black'>更新设备参数成功！</font>");
                        }
                        else
                        {
                            QMessageBox::about(NULL, "提示", "<font color='black'>更新设备参数失败！</font>");
                        }

                        break;
                    }
                    it1++;
                }
            }
        }
    }
}

void MainWindow::on_pushButton_Addf_clicked()
{
    FactorAdd *pFactorAdd = new FactorAdd(g_Device_ID);
    pFactorAdd->show();
    connect(pFactorAdd, SIGNAL(addSuccess()),this, SLOT(refresh_AnalogDevParam()));
    return;
}

void MainWindow::on_pushButton_Updatef_clicked()
{
    // g_Device_ID

    if(FactorGui_Init(g_Device_ID))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取因子信息成功！</font>");
    }
}


void MainWindow::on_pushButton_AddFresh_clicked()
{
    if(DevGui_Init())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取设备信息成功！</font>");
    }
}


void MainWindow::on_pushButton_AddT_clicked()
{
    TeshuzhiAdd *pTeshuzhiAdd = new TeshuzhiAdd();
    pTeshuzhiAdd->show();
    return;
}

void MainWindow::on_pushButton_UpdateT_clicked()
{
    if(TeshuzhiGui_Init())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取特殊值配置成功!</font>");
    }else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取特殊值配置失败!</font>");
    }
}

bool MainWindow::UploadGui_Init()
{
    disconnect(m_SignalMapper_Up,SIGNAL(mapped(QString)),this,SLOT(onButtonUpSaved(QString)));
    disconnect(m_SignalMapper_Upd,SIGNAL(mapped(QString)),this,SLOT(onButtonUpDele(QString)));

    ui->tableWidget_Upload->clear();
    setTableUpHeader();
    QTableWidgetItem *pItemID,*pItemMN,*pItemAddr,*pItemIR,*pItemIMDR,*pItemIH,*pItemPW,*pItemOT,*pItemRT;
    QCheckBox *pHeart,*pUpload;
    QPushButton *pOperSaved,*pOperDele;
    QComboBox *pVersion;

    httpclinet pClient;
    if(pClient.get(DCM_MNINFO,m_JsonArray))
    {
        int pRow = m_JsonArray.size();
        ui->tableWidget_Upload->setRowCount(pRow);

        for(int i=0;i<pRow;i++)  //获得总数据的个数 做个循环来处理每条数据
        {
            // table mn
            QJsonValue iconArray = m_JsonArray.at(i);
            QJsonObject icon = iconArray.toObject();

            pItemID = new QTableWidgetItem(icon[QLatin1String("id")].toString());
            pItemID->setTextAlignment(Qt::AlignCenter);
            pItemID->setFlags(Qt::ItemIsEditable);
            ui->tableWidget_Upload->setItem(i, 0, pItemID);

            pItemMN = new QTableWidgetItem(icon[QLatin1String("mn")].toString());
            pItemMN->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Upload->setItem(i, 1, pItemMN);

            pItemAddr = new QTableWidgetItem(icon[QLatin1String("ip_addr_port")].toString());
            pItemAddr->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Upload->setItem(i, 2, pItemAddr);

            pItemIR = new QTableWidgetItem(QString::number(icon[QLatin1String("interval_upload")].toInt()));
            pItemIR->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Upload->setItem(i, 3, pItemIR);

            pItemIMDR = new QTableWidgetItem(QString::number(icon[QLatin1String("interval_minute_data_upload")].toInt()));
            pItemIMDR->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Upload->setItem(i, 4, pItemIMDR);

            pHeart = new QCheckBox();
            if(icon[QLatin1String("is_open_heartbeat")].toInt()) pHeart->setCheckState(Qt::Checked);
            else pHeart->setCheckState(Qt::Unchecked);
            pHeart->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
            ui->tableWidget_Upload->setCellWidget(i, 5, pHeart);

            pItemIH = new QTableWidgetItem(QString::number(icon[QLatin1String("interval_heartbeat")].toInt()));
            pItemIH->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Upload->setItem(i, 6, pItemIH);

            pItemPW = new QTableWidgetItem(icon[QLatin1String("pw")].toString());
            pItemPW->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Upload->setItem(i, 7, pItemPW);

            pUpload = new QCheckBox();
            if(icon[QLatin1String("is_open_heartbeat")].toInt()) pUpload->setCheckState(Qt::Checked);
            else pUpload->setCheckState(Qt::Unchecked);
            pUpload->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
            ui->tableWidget_Upload->setCellWidget(i, 8, pUpload);

            pItemOT = new QTableWidgetItem(QString::number(icon[QLatin1String("over_time")].toInt()));
            pItemOT->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Upload->setItem(i, 9, pItemOT);

            pItemRT = new QTableWidgetItem(QString::number(icon[QLatin1String("resend_times")].toInt()));
            pItemRT->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_Upload->setItem(i, 10, pItemRT);

            pVersion = new QComboBox();
            pVersion->addItem("HJ212-2017");
            pVersion->addItem("HJ212-2005");
            pVersion->setCurrentText(icon[QLatin1String("protocol_version")].toString());
            ui->tableWidget_Upload->setCellWidget(i, 11, pVersion);

            pOperSaved = new QPushButton();
            pOperSaved->setText("保存");

            pOperDele = new QPushButton();
            pOperDele->setText("删除");

            connect(pOperSaved,SIGNAL(clicked()),m_SignalMapper_Up,SLOT(map()));
            m_SignalMapper_Up->setMapping(pOperSaved,icon[QLatin1String("ip_addr_port")].toString());
            connect(pOperDele,SIGNAL(clicked()),m_SignalMapper_Upd,SLOT(map()));
            m_SignalMapper_Upd->setMapping(pOperDele,icon[QLatin1String("ip_addr_port")].toString());

            QWidget *btnWidget = new QWidget(this);
            QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);    // FTIXME：内存是否会随着清空tablewidget而释放
            btnLayout->addWidget(pOperSaved);
            btnLayout->addWidget(pOperDele);
            btnLayout->setMargin(5);
            btnLayout->setAlignment(Qt::AlignCenter);
            ui->tableWidget_Upload->setCellWidget(i, 12, btnWidget);
        }

        connect(m_SignalMapper_Up,SIGNAL(mapped(QString)),this,SLOT(onButtonUpSaved(QString)));
        connect(m_SignalMapper_Upd,SIGNAL(mapped(QString)),this,SLOT(onButtonUpDele(QString)));

        return true;
    }else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取上传配置信息失败！</font>");
    }

    return false;
}

bool MainWindow::FactorGui_Init(QString pDev_ID)
{
    disconnect(m_SignalMapper_FaEdit,SIGNAL(mapped(QString)),this,SLOT(onButtonFaEdit(QString)));
    disconnect(m_SignalMapper_FaDele,SIGNAL(mapped(QString)),this,SLOT(onButtonFaDele(QString)));

    ui->tableWidget_Factor->clear();
    setTableFaHeader();
    QTableWidgetItem *pItemID = NULL,*pItemCode = NULL,*pItemAlias = NULL,*pItemTagID = NULL;
    QPushButton *pOperSaved = NULL,*pOperDele = NULL;

    // get..

    QJsonObject jsonObjectFa;
    httpclinet pClient;

    if(pClient.get(DCM_DEVICE_FACTOR,jsonObjectFa))
    {
        // qDebug() << "jsonObjectFa===>>" << jsonObjectFa;

        int row=0;
        ui->tableWidget_Factor->setRowCount(0);
        QJsonObject::const_iterator it = jsonObjectFa.constBegin();
        QJsonObject::const_iterator end = jsonObjectFa.constEnd();
        while(it != end)
        {
            if(it.key().contains(pDev_ID))
            {
                QJsonObject pJsonFa = it.value().toObject();

                // qDebug() << "row===>>" << row;
                ui->tableWidget_Factor->insertRow(row);

                QString pFactor_ID = pJsonFa.value("id").toString();

//                pItemID = new QTableWidgetItem(pFactor_ID);
//                pItemID->setTextAlignment(Qt::AlignCenter);
//                ui->tableWidget_Factor->setItem(row, 0, pItemID);

                pItemCode = new QTableWidgetItem(pJsonFa.value("factor_code").toString());
                pItemCode->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget_Factor->setItem(row, 0, pItemCode);

                pItemAlias = new QTableWidgetItem(pJsonFa.value("factor_alias").toString());
                pItemAlias->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget_Factor->setItem(row, 1, pItemAlias);

//                pItemTagID = new QTableWidgetItem(pJsonFa.value("tag_id").toString());
//                pItemTagID->setTextAlignment(Qt::AlignCenter);
//                ui->tableWidget_Factor->setItem(row, 0, pItemTagID);

                pOperSaved = new QPushButton();
                pOperSaved->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
                pOperSaved->setFont(QFont("Ubuntu",20,75));
                pOperSaved->setText("详情");

                pOperDele = new QPushButton();
                pOperDele->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
                pOperDele->setFont(QFont("Ubuntu",20,75));
                pOperDele->setText("删除");

                connect(pOperSaved,SIGNAL(clicked()),m_SignalMapper_FaEdit,SLOT(map()));
                m_SignalMapper_FaEdit->setMapping(pOperSaved,pFactor_ID);
                connect(pOperDele,SIGNAL(clicked()),m_SignalMapper_FaDele,SLOT(map()));
                m_SignalMapper_FaDele->setMapping(pOperDele,pFactor_ID);

                QWidget *btnWidget = new QWidget(this);
                QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);    // FTIXME：内存是否会随着清空tablewidget而释放
                btnLayout->addWidget(pOperSaved,1);
                btnLayout->addWidget(pOperDele,1);
                btnLayout->setMargin(3);
                btnLayout->setAlignment(Qt::AlignCenter);
                ui->tableWidget_Factor->setCellWidget(row, 2, btnWidget);
                ui->tableWidget_Factor->setRowHeight(row,64);

                row++;
            }
            it++;
        }
        connect(m_SignalMapper_FaEdit,SIGNAL(mapped(QString)),this,SLOT(onButtonFaEdit(QString)));
        connect(m_SignalMapper_FaDele,SIGNAL(mapped(QString)),this,SLOT(onButtonFaDele(QString)));

        return true;
    }

//    QMessageBox::about(NULL, "提示", "<font color='black'>获取因子配置失败!</font>");
    return false;
}

bool MainWindow::TeshuzhiGui_Init()
{
    disconnect(m_SignalMapper_Te,SIGNAL(mapped(QString)),this,SLOT(onButtonTeSaved(QString)));
    disconnect(m_SignalMapper_Ted,SIGNAL(mapped(QString)),this,SLOT(onButtonTeDele(QString)));

    ui->tableWidget_Te->clear();
    setTableTeHeader();
    QTableWidgetItem *pItem1,*pItem2,*pItem3,*pItem4;
    QPushButton *pOperSaved,*pOperDele;

    // get..

    QJsonObject jsonObject;
    httpclinet pClient;

    if(pClient.get(DCM_CONF,jsonObject))
    {
        // qDebug() << "jsonObject==>" << jsonObject;

        if(jsonObject.contains(TESHUZHI))
        {
            QJsonObject pTeshuzhi = jsonObject.value(TESHUZHI).toObject();
            ui->tableWidget_Te->setRowCount(pTeshuzhi.size());

            int row=0;
            QJsonObject::const_iterator it = pTeshuzhi.constBegin();
            QJsonObject::const_iterator end = pTeshuzhi.constEnd();
            while(it != end)
            {
                QJsonObject pJsonTe = it.value().toObject();
                if(pJsonTe.contains(CONF_TE_NAME))
                {
                    pItem1 = new QTableWidgetItem(it.key());
                    pItem1->setTextAlignment(Qt::AlignCenter);
                    pItem1->setFlags(Qt::ItemIsEditable);
                    ui->tableWidget_Te->setItem(row, 0, pItem1);

                    QString pTeName = pJsonTe.value(CONF_TE_NAME).toString();
                    QString pTeKey = pJsonTe.value(CONF_TE_KEY).toString();
                    QString pTeVal = pJsonTe.value(CONF_TE_VALUE).toString();

                    pItem2 = new QTableWidgetItem(pTeName);
                    pItem2->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidget_Te->setItem(row, 1, pItem2);

                    pItem3 = new QTableWidgetItem(pTeKey);
                    pItem3->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidget_Te->setItem(row, 2, pItem3);

                    pItem4 = new QTableWidgetItem(pTeVal);
                    pItem4->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidget_Te->setItem(row, 3, pItem4);

                    pOperSaved = new QPushButton();
                    pOperSaved->setText("保存");

                    pOperDele = new QPushButton();
                    pOperDele->setText("删除");

                    connect(pOperSaved,SIGNAL(clicked()),m_SignalMapper_Te,SLOT(map()));
                    m_SignalMapper_Te->setMapping(pOperSaved,it.key());
                    connect(pOperDele,SIGNAL(clicked()),m_SignalMapper_Ted,SLOT(map()));
                    m_SignalMapper_Ted->setMapping(pOperDele,it.key());

                    QWidget *btnWidget = new QWidget(this);
                    QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);    // FTIXME：内存是否会随着清空tablewidget而释放
                    btnLayout->addWidget(pOperSaved);
                    btnLayout->addWidget(pOperDele);
                    btnLayout->setMargin(5);
                    btnLayout->setAlignment(Qt::AlignCenter);
                    ui->tableWidget_Te->setCellWidget(row, 4, btnWidget);

                }
                it++;
                row++;
            }

            connect(m_SignalMapper_Te,SIGNAL(mapped(QString)),this,SLOT(onButtonTeSaved(QString)));
            connect(m_SignalMapper_Ted,SIGNAL(mapped(QString)),this,SLOT(onButtonTeDele(QString)));
        }
    }

    return true;
}


void MainWindow::Analog_RenewDevParam()
{
    // lineEdit_ID
    QString pDevParam;

    // read from CONF & setText
    QFile file(CONF);
    if (file.exists()){
        // update
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            // log: QJsonParseError
            return;
        }

        // analog_max_1=15,analog_min_1=0,upper_limit_1=5,lower_limit_1=1
        int index = 0;
        QJsonObject jsonObject = document.object();
        if(jsonObject.contains(FACTORS))
        {
            QJsonObject pJsonFactorList = jsonObject.value(FACTORS).toObject();

            QJsonObject::const_iterator it = pJsonFactorList.constBegin();
            QJsonObject::const_iterator end = pJsonFactorList.constEnd();
            while(it != end)
            {
                QString pKey = it.key();
                if(pKey.contains("-"))
                {
                    QString pKey0 = pKey.split("-").at(0);
                    // if(pKey0 == ui->lineEdit_ID->text())
                    {
                        QJsonObject QJsonObjectFator = it.value().toObject();
                        if(QJsonObjectFator.contains(CONF_IS_ANALOG_PARAM))
                        {
                            if(QJsonObjectFator.value(CONF_IS_ANALOG_PARAM).toBool())
                            {
                                QString pAlias,pAU1,pAD1,pAU2,pAD2;
                                if(QJsonObjectFator.contains(CONF_ANALOG_PARAM_AU1))
                                    pAU1 = QString::number(QJsonObjectFator.value(CONF_ANALOG_PARAM_AU1).toDouble());
                                if(QJsonObjectFator.contains(CONF_ANALOG_PARAM_AD1))
                                    pAD1 = QString::number(QJsonObjectFator.value(CONF_ANALOG_PARAM_AD1).toDouble());
                                if(QJsonObjectFator.contains(CONF_ANALOG_PARAM_AU2))
                                    pAU2 = QString::number(QJsonObjectFator.value(CONF_ANALOG_PARAM_AU2).toDouble());
                                if(QJsonObjectFator.contains(CONF_ANALOG_PARAM_AD2))
                                    pAD2 = QString::number(QJsonObjectFator.value(CONF_ANALOG_PARAM_AD2).toDouble());

                                if(QJsonObjectFator.contains(CONF_FACTOR_ALIAS))
                                    pAlias = QJsonObjectFator.value(CONF_FACTOR_ALIAS).toString();

                                if(index == 0)
                                {
                                     // analog_max_1=15,analog_min_1=0,upper_limit_1=5,lower_limit_1=1
                                    pDevParam += "analog_max_" + pAlias + "=" + pAU1 + ",analog_min_" + pAlias + "=" + pAD1 + ",upper_limit_" + pAlias + "=" + pAU2 + ",lower_limit_" + pAlias + "=" + pAD2;
                                }
                                else
                                {
                                    pDevParam += ",analog_max_" + pAlias + "=" + pAU1 + ",analog_min_" + pAlias + "=" + pAD1 + ",upper_limit_" + pAlias + "=" + pAU2 + ",lower_limit_" + pAlias + "=" + pAD2;
                                }
                                index++;
                            }
                        }
                    }
                }
                it++;
            }
        }

        if(index)
        {
            // qDebug() << "pDevParam===>>" << pDevParam;
           // ui->textEdit_devParams->setText(pDevParam);
            QMessageBox::about(NULL, "提示", "<font color='black'>更新设备配置参数成功!</font>");
        }
    }
}

QJsonObject MainWindow::Conf_RootObjGet()
{
    QJsonObject jsonObject;
    QFile file(CONF);
    if (file.exists()){
        // update
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            // log: QJsonParseError
            return jsonObject;
        }

        jsonObject = document.object();
    }
    return jsonObject;
}

bool MainWindow::Conf_TeshuzhiUpdate()
{
    QFile file(CONF);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            return false;
        }

        QJsonObject jsonObject = document.object();

        QJsonValueRef RefVersionTlist = jsonObject.find(TESHUZHI).value();
        QJsonObject jsonObjTelist = RefVersionTlist.toObject();

    }
    return false;
}

void MainWindow::on_pushButton_ReturnDev_clicked()
{
    ui->stackedWidget->setCurrentIndex(10);
}

void MainWindow::onReceiveDeviceCMDCtrl()
{
    qDebug()<<__LINE__<<__FUNCTION__<<endl;
    devicecmddlg = new DeviceCMDCtrlDlg(this);
    connect(this,&MainWindow::sendCurDT,devicecmddlg,&DeviceCMDCtrlDlg::onReceivecurDT);
    devicecmddlg->show();
}
