#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Crc16Class.h"
#include "vector"

uint64_t g_linux_ts;
string g_MinTime;
string g_DateTime;
string g_StrQN;
QMap<QString,CDevice *> g_StrDevNodeMap;

void setFactorFlag();
CDevice *get_CDevice(QString pName);
CFactor *get_CFactor(CDevice *pDevice, QString pProperty);
int SignedHex2Int(QByteArray array);
static char ConvertHexChar(char c);
static QByteArray QString2Hex(QString hexStr);
bool get_WorkMode_BtnStatus(uint16_t t);
bool GetCollectBtnStatus(uint16_t t);
bool GetCleanBtnStatus(uint16_t t);
void toggle_WorkMode_BtnStatus(uint16_t t, bool toggle);
void ToggleCleanBtnStatus(uint16_t t, bool toggle);
void ToggleCollectBtnStatus(uint16_t t, bool toggle);

static uint32_t Step = 0;

const uint32_t BIT0 = 0x00000001;
const uint32_t BIT1 = 0x00000002;
const uint32_t BIT10 = 0x00000400;
const uint32_t BIT31 = 0x80000000;


#define _ADDRESS_AUTO_REG_PLC_ "200"
#define _ADDRESS_MAN1_REG_PLC_ "100"
#define _ADDRESS_MAN2_REG_PLC_ "120"

// start
// Process
/// 是否以手动x模式运行 0:自动 1:手动1 2:手动2
uint16_t IsToRunManulaMode = 0;

/// 运行模式 连续模式/间隙模式
uint16_t _MEA_COV_RUN_MODE_ = 0x0000;
uint16_t _MEA_UNCOV_RUN_MODE_ = 0x0001;
uint16_t MeasureRunMode = 0xffff;

uint32_t NowSeconds = 0;
bool Lijiceliang = false;
bool StartRunAutoMode = false;
bool AutoLoopStatus = false;

// 流程是否结束 0:手动1 1:手动2 2:自动
bool IsLastLoopEnd[3] = { false, false, false };

/// <summary>
/// 0:全面清洗完成
/// 1:清洗流路1完成
/// 2:清洗流路2完成
/// 3:清洗沉淀池完成
/// 4:清洗测量杯完成
/// </summary>
bool IsCleanDone[5] = { false, false, false, false, false };

/// <summary>
/// 0:完整采水1
/// 1:完整采水2
/// 2:沉淀池采水1
/// 3:沉淀池采水2
/// 4:测量杯采水
/// </summary>
bool IsCollectDone[5] = { false, false, false, false, false };

int StartTime = 0;
int IntervalTime = 240;

// 工作模式
const uint16_t _B_AUTO_MODE_ = 0x01;
const uint16_t _B_DEBUG_MODE_ = 0x02;
const uint16_t _B_HALT_MODE_ = 0x04;
const uint16_t _B_MANUAL_MODE1_ = 0x08;
const uint16_t _B_MANUAL_MODE2_ = 0x10;
uint16_t SysWorkMode = 0;

// 清洗选择
const uint16_t _B_CLEANTUBE1_STAT_ = 0x01; // 清洗外管路1
const uint16_t _B_CLEANTUBE2_STAT_ = 0x02; // 清洗外管路2
const uint16_t _B_CLEANCUP_STAT_ = 0x04;   // 清洗测量杯
const uint16_t _B_CLEAN_SETTLING_TANK_ = 0x08; // 清洗沉淀池
// const uint16_t _B_ALGAE_REMOVAL_ = 0x10;   // 除藻
const uint16_t _B_CLEAN_ALL_ = 0x20;   // 全面清洗
uint16_t CleanMode = 0;

// 采水
const uint16_t _S_COMPLETECOLLECT1_STAT_ = 0x01;   // 完整采水1
const uint16_t _S_COMPLETECOLLECT2_STAT_ = 0x02;   // 完整采水2
const uint16_t _S_TANKCOLLECT1_STAT_ = 0x04;   // 沉淀池采水1
const uint16_t _S_CUPCOLLECT_STAT_ = 0x08;     // 测量杯 采水
const uint16_t _S_TANKCOLLECT2_STAT_ = 0x10;   // 沉淀池采水2
uint16_t CollectionMode = 0;

// 测量五参，沉淀池满
static bool IsToMeasureFive = false;
static bool IsEnableSkipSetting = false; // 跳过沉淀

// 测量剩余参数，测量杯满
static bool IsToMeasureFour = false;

// BIT0:测量完成发送
// BIT1:流程完成发送
// BIT2:防止重复发送
// BIT10:是否采集到水
static uint32_t TransmitFlag = 0;

const int TimeoutThreshhold = 3600 * 1000 / 8000;  // 1h 3600s 3600*1000ms 开始总磷、氨氮、高指流程后 每次轮询通讯良好情况下 大概8s

QString PlcRunState;

bool IsBtnInManualMode1 = false;
bool IsBtnInManualMode2 = false;

//bool IsBtnInCleanTube1 = false;
//bool IsBtnInCleanTube2 = false;
//bool IsBtnInCleanCup = false;
//bool IsBtnInCleanSettingTank = false;
//bool IsBtnInCleanAll = false;

//bool IsBtnInComplelte1 = false;
//bool IsBtnInComplelte2 = false;
//bool IsBtnInTankCollect1 = false;
//bool IsBtnInTankCollect2 = false;
//bool IsBtnInCupCollect = false;

bool IsBtnInWaterValve1 = false;
bool IsBtnInWaterValve2 = false;
bool IsBtnInWaterValve3 = false;
bool IsBtnInWaterValve4 = false;
bool IsBtnInWaterValve5 = false;
bool IsBtnInWaterValve6 = false;
bool IsBtnInWaterValve7 = false;
bool IsBtnInWaterValve8 = false;

bool IsBtnInRunWaterValve = false;
bool IsIsBtnInAirValve = false;

bool IsBtnInInputPump1 = false;
bool IsBtnInInputPump2 = false;
bool IsBtnInPeristalticPump = false;

uint16_t PumpSelected = 0xffff;    // 泵模式 :泵A / 泵B / 交替运行 / 自动切换
uint16_t LastProcessStep = 0xffff;

QString AddressDevPLC;  // PLC 地址

QString AddressDevIMn;  // 高指
QString AddressDevNHx;  // 氨氮1
QString AddressDevTP; // 总磷

bool IsValveXOpen[9];
bool IsBuoyTriggered[2];

bool PLCWarningInfo[11];
bool PLCProcessInfo[16];

// PROCESS END

QT_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(SYS_VERSION);

    initLogger();   //初始化日志
    QLOG_INFO() << "开始运行水站上位机...";

    ui->pushButton_W3_2->setDisabled(true); // 立即测量 失能

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath()+"/WaterDetector.dat");    //如果本目录下没有该文件,则会在本目录下生成,否则连接该文件
    if (!db.open()) {
        QLOG_ERROR() << "Database Error: " << db.lastError().text();
    }

    QSqlQuery query(db);
    query.exec("CREATE TABLE packages ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "timestamp Datetime NOT NULL, "
               "property TEXT NOT NULL, "
               "address TEXT NOT NULL, "
               "package TEXT NOT NULL)");   // property: 0-心跳上送；1-数据报文；2-平台回复 3-补发数据

    query.exec("CREATE TABLE historydata ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "timestamp DateTime NOT NULL, "
               "key TEXT NOT NULL, "
               "keyName TEXT NOT NULL, "
               "value TEXT NOT NULL, "
               "flag TEXT NOT NULL)");

    groupButtonLog=new QButtonGroup(this);
    groupButtonLog->addButton(ui->radioButton_Trace,0);
    groupButtonLog->addButton(ui->radioButton_Debug,1);
    groupButtonLog->addButton(ui->radioButton_Info,2);
    groupButtonLog->addButton(ui->radioButton_Err,3);

    //绑定信号和槽函数
    connect(ui->radioButton_Trace,SIGNAL(clicked(bool)),
            this,SLOT(slots_RadioButton_Logger()));
    connect(ui->radioButton_Debug,SIGNAL(clicked(bool)),
            this,SLOT(slots_RadioButton_Logger()));
    connect(ui->radioButton_Info,SIGNAL(clicked(bool)),
            this,SLOT(slots_RadioButton_Logger()));
    connect(ui->radioButton_Err,SIGNAL(clicked(bool)),
            this,SLOT(slots_RadioButton_Logger()));

    ui->radioButton_Info->setChecked(true); //默认选中apple_radioButton

    CJsonFile jsonfile;
    jsonfile.jsonfile_create(); // if not exist then create json file
    QJsonObject pJsonRootObj = jsonfile.get_root();

    // 串口 配置 赋值 展示
    serialinfo_display(pJsonRootObj);

    if(OpenCom())
    {
        // 1.新建串口处理子线程
        serialWorker = new SerialWorker(m_pSerialCom);

        // 将串口和子类一同移入子线程
        serialWorker->moveToThread(&serialThread_1);

        // 2.连接信号和槽
        connect(&serialThread_1, &QThread::finished,
                serialWorker, &QObject::deleteLater);           // 线程结束，自动删除对象
        connect(&serialThread_1, SIGNAL(finished()),
                &serialThread_1, SLOT(deleteLater()));
        connect(this, &MainWindow::startWork,
                serialWorker, &SerialWorker::doWork1);   // 主线程串口数据发送的信号

        /* 接收到 worker 发送过来的信号 */
        connect(serialWorker, SIGNAL(resultReady(QString)),
                this, SLOT(handleResults(QString)));
    }

    this->installEvents();
    m_DevBtnEditMap.clear();
    m_DevBtnDeleMap.clear();

    // PLC地址初始化
    get_KeyValue(pJsonRootObj,PLC_ADDR);
    get_KeyValue(pJsonRootObj,STARTTIME);
    get_KeyValue(pJsonRootObj,INTERNALTIME);

    // 信号转发
    m_SignalMapperEdit = new QSignalMapper(this);
    m_SignalMapperDele = new QSignalMapper(this);
    connect(m_SignalMapperEdit,SIGNAL(mapped(QString)),this,SLOT(btnClicked_DevEdit(QString)));
    connect(m_SignalMapperDele,SIGNAL(mapped(QString)),this,SLOT(btnClicked_DevDele(QString)));

    m_SignalMapperUpDele = new QSignalMapper(this);
    connect(m_SignalMapperUpDele,SIGNAL(mapped(QString)),this,SLOT(btnClicked_UploadDele(QString)));

    setDeviceInit(pJsonRootObj);
    setUploadInit(pJsonRootObj);

    // 解析系统配置文件 获取设备配置信息 加载到字典 setmapper

    radioButton_Init();

    setTableHeader();
    setTableContents(pJsonRootObj);

    setUploadTableHeader();
    setUpTableContents(pJsonRootObj);

    m_pDateTimer = new QTimer(this);
    m_pDateTimer->setInterval(1000);
    connect(m_pDateTimer, SIGNAL(timeout()), this, SLOT(HandleDateTimeout()));
    m_pDateTimer->start();

    QButtonGroup *m_GroupMember = new QButtonGroup();
    //设置互斥属性
    m_GroupMember->setExclusive(true);
    m_GroupMember->addButton(ui->pushButtonJC, MODULE_JC);
    m_GroupMember->addButton(ui->pushButtonSB, MODULE_SB);
    m_GroupMember->addButton(ui->pushButtonSC, MODULE_SC);
    m_GroupMember->addButton(ui->pushButtonQX, MODULE_QX);
    m_GroupMember->addButton(ui->pushButtonSJ, MODULE_SJ);
    m_GroupMember->addButton(ui->pushButtonRZ, MODULE_RZ);
    m_GroupMember->addButton(ui->pushButtonKZ, MODULE_KZ);
    m_GroupMember->addButton(ui->pushButtonMS, MODULE_MS);
    m_GroupMember->addButton(ui->pushButtonPLC, MODULE_PLC);
    connect(m_GroupMember, SIGNAL(buttonClicked(int)), this,SLOT(SetIndex(int)));
    this->SetIndex(0);

    QJsonArray pArrayDevList = jsonfile.get_level1_array(DEVICE);
    for (int i=0; i<pArrayDevList.size(); i++) {
        // dev factors

        QString pName,pType,pAddr;
        QJsonObject pJsonDev = pArrayDevList.at(i).toObject();

        if(pJsonDev.contains(DEV_NAME)) pName = pJsonDev.value(DEV_NAME).toString();
        if(pJsonDev.contains(DEV_ADDR)) pAddr = pJsonDev.value(DEV_ADDR).toString();
        if(pJsonDev.contains(DEV_TEMPLATE)) pType = pJsonDev.value(DEV_TEMPLATE).toString();

        CDevice *pDevice = new CDevice(pType,pName);
        pDevice->set_address(pAddr);

        g_StrDevNodeMap.insert(pName,pDevice);
    }

    // 首页实时数据初始化
    factor_display();

    // tcp connect 解析当前系统文件的upload节点 socket初始化
    upload_init();

    // CheckNetworkStatus
    m_TimerCheck = new QTimer();
    m_TimerCheck->stop();
    connect(m_TimerCheck,&QTimer::timeout,this,&MainWindow::CheckNetworkStatus);
    m_TimerCheck->start(500);

    //ui->tabWidget->setTabShape(QTabWidget::Triangular);
    //ui->tabWidget_2->setTabShape(QTabWidget::Triangular);

    m_TimerHeartbeat = new QTimer();
    m_TimerHeartbeat->stop();
    connect(m_TimerHeartbeat,&QTimer::timeout,this,&MainWindow::heartbeat_work);
    m_TimerHeartbeat->start(1000* 60 * 5);  // 5分钟 一次
    // m_TimerHeartbeat->start(1000 * 60);

    ProcessCtrlLogic_Init();
}

MainWindow::~MainWindow()
{
    /* 打断线程再退出 */
    serialWorker->stopWork();
    serialThread_1.quit();
    /* 阻塞线程 2000ms，判断线程是否结束 */
    if (serialThread_1.wait(10000)) {
        QLOG_INFO()<<"自动监控线程结束"<<endl;
    }

    if(m_pSerialCom)
    {
        m_pSerialCom->close();
        delete m_pSerialCom;
        m_pSerialCom = nullptr;
    }
    factor_widget_clear();
    device_widget_clear();

    if(m_pDateTimer) delete m_pDateTimer;
    if(m_GroupMember) delete m_GroupMember;
    if(m_SignalMapperEdit) delete m_SignalMapperEdit;
    if(m_SignalMapperDele) delete m_SignalMapperDele;
    if(m_SignalMapperUpDele) delete m_SignalMapperUpDele;
    if(m_RealtimeUpload) delete m_RealtimeUpload;
    if(m_HeartBreak) delete m_HeartBreak;
    if(m_TimerHeartbeat) delete m_TimerHeartbeat;
    if(m_TimerCheck) delete m_TimerCheck;

    qDeleteAll(m_DevBtnEditMap);
    qDeleteAll(m_DevBtnDeleMap);
    qDeleteAll(m_UploadBtnDeleMap);
    qDeleteAll(g_StrDevNodeMap);

    delete ui;
}

void MainWindow::heartbeat_work()
{
    qint8 i = 0;
    for(i = 0; i < IPlist.count(); i++)
    {
        if (SocketList[i]->state() == SocketList[i]->ConnectedState) {
            g_StrQN = "";   // must (为了区分测试包，显示在textbrowser)
            HJ212_PARAM_SET hj212_param;
            HJ212_DATA_PARAM hj212_data_param;
            hj212_data_param.DataTime = current_datetime() + ";";

            hj212_param = hj212_set_params(HJ212_DEF_VAL_ST, HJ212_CN_RTD_DATA, HJ212_FLAG_ACK_EN, &hj212_data_param, Passwordlist[i].toStdString(), m_MN.toStdString());
            string rep = hj212_frame_assemble(&hj212_param);
            this->sendMessages(SocketList[i], QString::fromStdString(rep));

            QString pText = "To " + IPlist.at(i).toString() +": " + QString::fromStdString(rep);
            QLOG_DEBUG() << pText;

            QString pAddress = IPlist.at(i).toString() +":" + QString::number(Portlist.at(i));
            QLOG_INFO() << "发送心跳包至 " << pAddress;

            QString sql = QString("INSERT INTO packages (timestamp,property,address,package) "
                          "VALUES ('%1','0','%2','%3')").arg(QString::fromStdString(current_formattime()),pAddress,QString::fromStdString(rep));

            // qDebug() << "sql===>>" << sql;
            QSqlQuery query(db);
            query.exec(sql);
        }
    }
}

void MainWindow::slots_RadioButton_Logger()
{

    switch(groupButtonLog->checkedId())
    {
    case 0:
    {
        logger->setLoggingLevel(QsLogging::TraceLevel);
        break;
    }

    case 1:
    {
        logger->setLoggingLevel(QsLogging::DebugLevel);
        break;
    }

    case 2:
    {
        logger->setLoggingLevel(QsLogging::InfoLevel);
        break;
    }

    case 3:
    {
        logger->setLoggingLevel(QsLogging::ErrorLevel);
        break;
    }

    }
}

void MainWindow::get_KeyValue(QJsonObject &pJsonRootObj,QString Key)
{
    if(Key == PLC_ADDR)
    {
        if(pJsonRootObj.contains(PLC_ADDR))
        {
            AddressDevPLC = pJsonRootObj.value(PLC_ADDR).toString();
            ui->lineEditPLCAddr->setText(AddressDevPLC);
        }
        else
        {
            AddressDevPLC = ui->lineEditPLCAddr->text();
        }
    }
    else if(Key == STARTTIME)
    {
        if(pJsonRootObj.contains(STARTTIME))
        {
            QString pStartTime = pJsonRootObj.value(STARTTIME).toString();
            StartTime = pStartTime.toUInt();
            ui->lineEdit_39->setText(pStartTime);
        }
    }
    else if(Key == INTERNALTIME)
    {
        if(pJsonRootObj.contains(INTERNALTIME))
        {
            QString pIntervalTime = pJsonRootObj.value(INTERNALTIME).toString();
            IntervalTime = pIntervalTime.toUInt();
            ui->lineEdit_40->setText(pIntervalTime);
        }
    }
    return;
}

void MainWindow::handleResults(const QString & results)
{
    /* 打印线程的状态 */
    QLOG_DEBUG()<< endl <<"线程的状态："<<results<<endl;
    if(results.contains("测量值:"))
    {
        QStringList pTempList = results.split(":");
        if(pTempList.size() >= 3)
        {
            for(int i=0;i<NAME_list.size();i++)
            {
                if(NAME_list.at(i)->text() == pTempList.at(1))
                {
                    DATA_list.at(i)->setText(pTempList.at(2));
                    break;
                }
            }
        }
    }
    else if(results.contains("自来水压:"))
    {
        QStringList pTempList = results.split(":");
        if(pTempList.size() >= 2)
        {
            ui->label_ZLSY->setText(pTempList[1]+" KPa");
        }
    }
    else if(results.contains("采样水压:"))
    {
        QStringList pTempList = results.split(":");
        if(pTempList.size() >= 2)
        {
            ui->label_CYSY->setText(pTempList[1]+" KPa");
        }
    }
    else if(results.contains("实时数据上传"))
    {
        upload_work();
    }
    else if(results.contains("实时数据入库"))
    {
        store_work();
    }
    else if(results.contains("立即测量灭灯"))
    {
        ui->label_W3_2->setStyleSheet("border-image: url(:/images/blackC.png);");
    }
}

CDevice *get_CDevice(QString pName)
{
    // 遍历
    QMap<QString,CDevice *>::iterator it;
    for (it=g_StrDevNodeMap.begin();it!= g_StrDevNodeMap.end() ; it++) {
        CDevice *pDevice = it.value();  //
        if(pDevice->get_name() == pName)
        {
            return pDevice;
        }
    }
    return nullptr;
}

CFactor *get_CFactor(CDevice *pDevice, QString pProperty)   // 0-测量因子；1-状态值；2-错误码
{
    QMap<QString,CFactor *> mapFactorList = pDevice->get_map_DevFactors();
    QMap<QString,CFactor *>::iterator itFa;
    for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {

        CFactor *pFactor = itFa.value();
        if(pFactor->getProperty() == pProperty)
        {
            return pFactor;
        }
    }
    return nullptr;
}


void MainWindow::WriteSingleCoil(QString pAddr,QString pRegAddr,QString pFunc,QString pSetVal)
{
    if(m_pSerialCom->isOpen())
    {
        QString pRealData;
        pAddr = QString("%1").arg(pAddr.toUInt(),2,16,QChar('0'));

        pFunc = QString("%1").arg(pFunc.toUInt(),2,16,QChar('0'));
        pRegAddr = QString("%1").arg(pRegAddr.toUInt(),4,16,QChar('0'));    //str = "0064"
        pRegAddr.insert(2," ");

        pSetVal = QString("%1").arg(pSetVal.toUInt(),4,16,QChar('0'));   //str = "ff00"
        pSetVal.insert(2," ");

        QString pCommText = pAddr + pFunc + pRegAddr + pSetVal;

        Crc16Class crc16;
        QString pTx = crc16.crcCalculation(pCommText).toUpper();

        QLOG_DEBUG() << "WriteSingleCoil TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');
        m_pSerialCom->flush();
        m_pSerialCom->writeData(QString2Hex(pTx).data(),8);

        char data[100];
        int pRetVal = m_pSerialCom->readData(data,8);
        if(pRetVal > 0)
        {
            QString strTmp;
            for(int i = 0; i<8; i++)
            {
                strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
            }
            if(crc16.crc_Checking(strTmp))
            {
                QLOG_DEBUG() << "WriteSingleCoil RX: "<<strTmp;
                // parse
                QByteArray buf = QString2Hex(strTmp);

                QByteArray pFunc;
                pFunc.append(buf.at(1));
                if(pFunc.toHex().toInt() == 0x05)
                {
                    QByteArray arrReg,arrVal;
                    bool ok;
                    arrReg[0] = buf.at(2);
                    arrReg[1] = buf.at(3);

                    arrVal[0] = buf.at(4);
                    arrVal[1] = buf.at(5);
                    // qDebug() << "-->>" << arrReg.toHex().toUShort(&ok, 16) << "===>" <<arrVal.toHex().toUShort(&ok, 16);

                    if(arrReg.toHex().toUShort(&ok, 16) == 200)
                    {
                        if(arrVal.toHex().toUShort(&ok, 16) == 0xff00)
                        {
                            QLOG_INFO() << "启动PLC自动测量。";
                        }
                        if(arrVal.toHex().toUShort(&ok, 16) == 0x0000)
                        {
                            QLOG_INFO() << "关闭PLC自动测量。";
                        }
                    }
                }
            }
            else
            {
                QLOG_DEBUG() << "WriteSingleCoil RX CRC check Err...";
            }
        }
        else
        {
            QLOG_DEBUG() << "WriteSingleCoil RX TimtOut...";
        }
    }
}

bool MainWindow::WriteMultiRegister(QString pAddr,QString pRegAddr,QString pFunc,QString pSetValList)
{
    if(m_pSerialCom->isOpen())
    {
        uint16_t pTempReg_Addr = pRegAddr.toUInt();

        pAddr = QString("%1").arg(pAddr.toUInt(),2,16,QChar('0'));

        pFunc = QString("%1").arg(pFunc.toUInt(),2,16,QChar('0'));
        pRegAddr = QString("%1").arg(pRegAddr.toUInt(),4,16,QChar('0'));    //str = "0064"
        pRegAddr.insert(2," ");

        QString pRegAmt,pByteAmt,pSetVal;
        int pRegSize = 0;
        if(pSetValList.contains("-"))
        {
            QStringList pItemList = pSetValList.split("-");
            pRegSize = pItemList.size();
            if(!pRegSize) return false;
            pRegAmt = QString("%1").arg(pRegSize,4,16,QChar('0'));
            pByteAmt = QString("%1").arg(pRegSize*2,2,16,QChar('0'));

            for(int i=0;i<pRegSize;i++)
            {
                pSetVal += QString("%1").arg(pItemList.at(i).toUInt(),4,16,QChar('0'));
            }
        }
        else
        {
            pRegSize = 1;
            pRegAmt = QString("%1").arg(pRegSize,4,16,QChar('0'));
            pByteAmt = QString("%1").arg(pRegSize*2,2,16,QChar('0'));

            pSetVal += QString("%1").arg(pSetValList.toUInt(),4,16,QChar('0'));
        }

        QString pCommText = pAddr + pFunc + pRegAddr + pRegAmt + pByteAmt + pSetVal;

        Crc16Class crc16;
        QString pTx = crc16.crcCalculation(pCommText).toUpper();

        QLOG_DEBUG() << "WriteMultiRegister TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');
        m_pSerialCom->flush();
        m_pSerialCom->writeData(QString2Hex(pTx).data(),9+pRegSize*2);
        char data[100];
        int64_t pRetVal = m_pSerialCom->readData(data,8);
        if(pRetVal > 0)
        {
            QString strTmp;
            for(int i = 0; i<8; i++)
            {
                strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
            }

            if(crc16.crc_Checking(strTmp))
            {
                QLOG_DEBUG() << "WriteMultiRegister RX: "<<strTmp;

                // parse
                QByteArray buf = QString2Hex(strTmp);

                QByteArray pFunc;
                pFunc.append(buf.at(1));
                bool ok;
                // qDebug() << "Recv Func: " << pFunc.toHex().toUShort(&ok,16);

                if(pFunc.toHex().toUShort(&ok,16) == 0x10)
                {
                    if(pTempReg_Addr == 0x1200)
                    {
                        return true;
                    }
                }
            }
            else
            {
                QLOG_DEBUG() << "WriteMultiRegister RX CRC check Err...";
            }
        }
        else
        {
            QLOG_DEBUG() << "WriteMultiRegister RX TimeOut...";
        }
    }
    return false;
}


bool MainWindow::WriteSingleRegister(QString pAddr,QString pRegAddr,QString pFunc,QString pSetVal)
{
    if(m_pSerialCom->isOpen())
    {
        uint16_t pTempReg_Addr = pRegAddr.toUInt();

        pAddr = QString("%1").arg(pAddr.toUInt(),2,16,QChar('0'));

        pFunc = QString("%1").arg(pFunc.toUInt(),2,16,QChar('0'));
        pRegAddr = QString("%1").arg(pRegAddr.toUInt(),4,16,QChar('0'));    //str = "0064"
        pRegAddr.insert(2," ");

        pSetVal = QString("%1").arg(pSetVal.toUInt(),4,16,QChar('0'));    //str = "ff00"
        pSetVal.insert(2," ");

        QString pCommText = pAddr + pFunc + pRegAddr + pSetVal;

        Crc16Class crc16;
        QString pTx = crc16.crcCalculation(pCommText).toUpper();

        QLOG_INFO() << "==>> WriteSingleRegister TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');
        m_pSerialCom->flush();
        m_pSerialCom->writeData(QString2Hex(pTx).data(),8);
        char data[100];
        int64_t qRetVal = m_pSerialCom->readData(data,8);
        if(qRetVal>=0)
        {
            QString strTmp;
            for(int i = 0; i<8; i++)
            {
                strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
            }

            if(crc16.crc_Checking(strTmp))
            {
                QLOG_INFO() << "WriteSingleRegister RX: "<<strTmp;
                // parse
                QByteArray buf = QString2Hex(strTmp);

                QByteArray pFunc;
                pFunc.append(buf.at(1));
                if(pFunc.toHex().toInt() == 0x06)
                {
                    if(pTempReg_Addr == 0)
                    {
                        QLOG_INFO() << "蓝创A 成功发送测量指令！";
                        return true;
                    }

                    // pTempReg_Addr 0
                    if(pTempReg_Addr == 40001)
                    {
                        QLOG_INFO() << "成功发送测量指令！";
                        return true;
                    }

                    if(pTempReg_Addr == 11)
                    {
                        TransmitFlag |= BIT1; // 流程完成发送
                        QLOG_INFO() << "向PLC发送测量完成信号！";
                    }
                    else if(pTempReg_Addr == 41102 || pTempReg_Addr == 41103 || pTempReg_Addr == 41104 || pTempReg_Addr == 41105)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>设置成功！</font>");
                    }
                    else if(pTempReg_Addr == 41098 || pTempReg_Addr == 41099 || pTempReg_Addr == 41100 || pTempReg_Addr == 41101)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>设置成功！</font>");
                    }
                    else if(pTempReg_Addr == 41088 || pTempReg_Addr == 41124 || pTempReg_Addr == 41094 || pTempReg_Addr == 41125)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>设置成功！</font>");
                    }
                    else if(pTempReg_Addr == 41188 || pTempReg_Addr == 41189 || pTempReg_Addr == 41095 || pTempReg_Addr == 41096)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>设置成功！</font>");
                    }
                    else if(pTempReg_Addr == 41089 || pTempReg_Addr == 41090 || pTempReg_Addr == 41091 || pTempReg_Addr == 41092 || pTempReg_Addr == 41093)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>设置成功！</font>");
                    }
                    else if(pTempReg_Addr == 41097)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>设置泵模式成功！</font>");
                    }

                    // pTempReg_Addr 40001

                    return true;
                }
            }
            else
            {
                QLOG_DEBUG() << "WriteSingleRegister RX CRC check Err...";
            }
        }
        else
        {
            QLOG_TRACE() << "WriteSingleRegister TimeOut";
        }
    }
    return false;
}


void MainWindow::ReadHoldingRegisters(QString pAddr,QString pRegAddr,uint16_t len,QString pRegs)
{

    if(m_pSerialCom->isOpen())
    {

        QString pTemp_Addr = pAddr;
        QString pRealData;

        uint16_t pReg_Bakup = pRegAddr.toUInt();

        pAddr = QString("%1").arg(pAddr.toUInt(),2,16,QChar('0'));

        QString pFunc = QString("%1").arg(0x03,2,16,QChar('0'));
        pRegAddr = QString("%1").arg(pRegAddr.toUInt(),4,16,QChar('0'));    //str = "0064"
        pRegAddr.insert(2," ");

        pRegs = QString("%1").arg(pRegs.toUInt(),4,16,QChar('0'));    //str = "0001"
        pRegs.insert(2," ");

        QString pCommText = pAddr + pFunc + pRegAddr + pRegs;
        qDebug()<<__LINE__<<__FUNCTION__<<"pAddr:"<<pAddr<<endl;
        qDebug()<<__LINE__<<__FUNCTION__<<"pFunc:"<<pFunc<<endl;
        qDebug()<<__LINE__<<__FUNCTION__<<"pRegAddr:"<<pRegAddr<<endl;
        qDebug()<<__LINE__<<__FUNCTION__<<"pRegs:"<<pRegs<<endl;
        qDebug()<<__LINE__<<__FUNCTION__<<"pCommText:"<<pCommText<<endl;

        Crc16Class crc16;
        QString pTx = crc16.crcCalculation(pCommText).toUpper();

        QLOG_DEBUG() << "ReadHoldingRegisters TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');
        m_pSerialCom->flush();
        m_pSerialCom->writeData(QString2Hex(pTx).data(),8);

        char data[100];
        int pRetVal = m_pSerialCom->readData(data,len);
        qDebug()<<__LINE__<<__FUNCTION__<<"pRetVal:"<<pRetVal<<endl;
        if(pRetVal > 0)
        {
            QString strTmp;
            for(int i = 0; i<len; i++)
            {
                strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
            }
            qDebug()<<__LINE__<<__FUNCTION__<<strTmp<<endl;
            if(crc16.crc_Checking(strTmp))
            {
                QLOG_DEBUG() << "ReadHoldingRegisters RX: "<<strTmp;
                // parse
                QByteArray buf = QString2Hex(strTmp);

                QByteArray pFunc;
                pFunc.append(buf.at(1));
                if(pFunc.toHex().toInt() == 0x03)
                {
                    QByteArray arr;
                    if(pReg_Bakup == 41088)   // 自来水压力延时检测时间——HD0
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_8->setText(QString::number(pVal));
                        ui->lineEdit_21->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }

                    else if(pReg_Bakup == 41089)  // 自来水阀延时关闭时长——HD1
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_10->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41090)  // 阀3关闭延时时长——HD2
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_11->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41091)  // 泵1延时打开时长——HD3
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_12->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41092)  // 进水压力延时检测时间——HD4
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_13->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41093)  // 进水压力延时检测时间——HD4
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_15->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41094)  // 水样沉淀时间——HD6
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10 / 60;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_17->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41095)
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_18->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41096)
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_19->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41097)    // 读取泵模式
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        // qDebug() << "--->>pRealData: " << pRealData;
                        uint16_t pVal = pRealData.toUInt();
                        PumpSelected = pVal;

                        ui->comboBox_8->setCurrentIndex(pVal-1);
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取泵模式信息成功！</font>");
                    }
                    else if(pReg_Bakup == 41098)    // 阀3开启时长 41098  lineEdit_23
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_23->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41099)
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_24->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41100)
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_25->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41101)
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_26->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41102)
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_27->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41103)
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_28->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41104)
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_29->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41105)
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_30->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41124)  // 沉淀池最大进水时长——HD200
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_16->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41125)
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        uint16_t pVal = pRealData.toUInt() / 10;
                        // qDebug() << "pRealData--->>" << pVal;

                        ui->lineEdit_20->setText(QString::number(pVal));
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41188)  // 自来水压力阈值——HD100
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        ui->lineEdit_9->setText(pRealData);
                        ui->lineEdit_22->setText(pRealData);
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                    else if(pReg_Bakup == 41189)  // 水样压力阈值——HD101
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));

                        ui->lineEdit_14->setText(pRealData);
                        QMessageBox::about(NULL, "提示", "<font color='black'>读取成功！</font>");
                    }
                }
            }
            else
            {
                QLOG_DEBUG() << "ReadHoldingRegisters RX CRC check Err...";
            }
        }
        else
        {
            QLOG_DEBUG() << "ReadHoldingRegisters TimeOut";
        }
    }
}

void MainWindow::ProcessCtrlLogic_Init(){

    ui->pushButton_W1->setDisabled(true);
    ui->pushButton_W2->setDisabled(true);
    ui->pushButton_W3->setDisabled(true);
    ui->pushButton_W4->setDisabled(true);
    ui->pushButton_W5->setDisabled(true);
    ui->pushButton_W6->setDisabled(true);
    ui->pushButton_W7->setDisabled(true);
    ui->pushButton_W8->setDisabled(true);
    ui->pushButton_W9->setDisabled(true);
    ui->pushButton_W10->setDisabled(true);
    ui->pushButton_W11->setDisabled(true);
    ui->pushButton_W12->setDisabled(true);
    ui->pushButton_W13->setDisabled(true);
    ui->pushButton_W14->setDisabled(true);
    ui->pushButton_W15->setDisabled(true);
    ui->pushButton_W16->setDisabled(true);
    ui->pushButton_W17->setDisabled(true);
    ui->pushButton_W18->setDisabled(true);
    ui->pushButton_W19->setDisabled(true);
    ui->pushButton_W20->setDisabled(true);
    ui->pushButton_W21->setDisabled(true);
    ui->pushButton_W21_1->setDisabled(true);
    ui->pushButton_W22->setDisabled(true);
    ui->pushButton_W23->setDisabled(true);
    ui->pushButton_W24->setDisabled(true);
    ui->pushButton_W25->setDisabled(true);
    ui->pushButton_W26->setDisabled(true);

    for(int i=0;i<9;i++)
    {
        IsValveXOpen[i] = false;
    }

    for(int i=0;i<2;i++)
    {
        IsBuoyTriggered[i] = false;
    }

    for(int i=0;i<11;i++)
    {
        PLCWarningInfo[i] = false;
    }

    for(int i=0;i<16;i++)
    {
        PLCProcessInfo[i] = false;
    }

    CJsonFile jsonfile;
    QJsonObject pJsonRootObj = jsonfile.get_root();
    if(pJsonRootObj.contains(STARTTIME)) StartTime = pJsonRootObj.value(STARTTIME).toString().toUInt();
    if(pJsonRootObj.contains(INTERNALTIME)) IntervalTime = pJsonRootObj.value(INTERNALTIME).toString().toUInt();

    if(m_AutoManulModeTimer == nullptr)
    {
        QLOG_INFO() << "自动手动模式定时器启动!!!";
        AutoManualMeasureThreadCallback();

        m_AutoManulModeTimer = new QTimer();
        m_AutoManulModeTimer->stop();
        connect(m_AutoManulModeTimer,&QTimer::timeout,this,&MainWindow::AutoManualModeTimerCallback);
    }

    IsCollectDone[4] = true;     //FIXME: 为什么[4]置位true? 4:测量杯采水

    MeasureRunMode = 1; // 默认间隙模式
    ui->comboBox_6->setCurrentIndex(1);
    ui->comboBox_8->setCurrentIndex(-1);// 泵模式初始显示为空
}

void MainWindow::AutoManualModeTimerCallback()
{

    m_AutoManulModeTimer->stop();
    if (IsToRunManulaMode == 0) // 是否为 自动模式
    {
        if (MeasureRunMode == _MEA_UNCOV_RUN_MODE_) // 运行模式是否为间隙模式
        {
            QTime current_time = QTime::currentTime();
            NowSeconds = current_time.hour() * 3600 + current_time.minute() * 60 + current_time.second();

            if (!AutoLoopStatus)
            {
                int offset = (NowSeconds - (StartTime * 60))   %   (IntervalTime * 60); // IntervalTime 240min *60 ==> sec
                QLOG_TRACE() << "offset：" << offset << "IntervalTime" << IntervalTime*60;
                if ((offset >= 0) && (offset <= 2))
                {
                    if(!Lijiceliang)    // 立即测量 未在 执行
                    {

                        AutoLoopStatus = true;

                        g_DateTime = current_datetime();
                        g_MinTime = current_minutetime();
                        QLOG_INFO() << "间隙模式定时器触发！时间戳：" << QString::fromStdString(g_MinTime);

                        ui->groupBox->setTitle(QString("实时数据  ")+QString::fromStdString(g_MinTime));
                        StartRunAutoMode = true;

                        // 测量因子 flag  ==> 'B'
                        factor_flag_init();
                    }
                }
            }
        }
        else if (MeasureRunMode == _MEA_COV_RUN_MODE_)
        {
            if (IsLastLoopEnd[2] && !StartRunAutoMode)
            {
                StartRunAutoMode = true;

                // 测量因子 flag  ==> 'B'
                factor_flag_init();

                g_DateTime = current_datetime();
                g_MinTime = current_minutetime();
                QLOG_INFO() << "连续模式触发！时间戳：" << QString::fromStdString(g_MinTime);

                ui->groupBox->setTitle(QString("实时数据  ")+QString::fromStdString(g_MinTime));
            }
        }
    }

    if (IsLastLoopEnd[0] || IsLastLoopEnd[1])
    {
        if (IsToRunManulaMode != 0)
        {
            if (IsToRunManulaMode == 1)
            {
                toggle_WorkMode_BtnStatus(_B_MANUAL_MODE1_, false); // true
            }
            if (IsToRunManulaMode == 2)
            {
                toggle_WorkMode_BtnStatus(_B_MANUAL_MODE2_, false); // true
            }
            return;
        }
    }

    m_AutoManulModeTimer->start();
}

void MainWindow::factor_flag_init()
{
    QMap<QString,CDevice *>::iterator it;
    for (it=g_StrDevNodeMap.begin();it!= g_StrDevNodeMap.end() ; it++) {
        CDevice *pDevice = it.value();  //
        QMap<QString,CFactor *> mapFactorList = pDevice->get_map_DevFactors();
        QMap<QString,CFactor *>::iterator itFa;
        for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {

            CFactor *pFactor = itFa.value();
            pFactor->setFlag("B");
        }
    }
}

void MainWindow::FlushTipStatusTimerCallback()
{
    if (IsEnableSkipSetting)
        ui->pushButton_W3->setDisabled(false);
    else
        ui->pushButton_W3->setDisabled(true);

    // 自动
    if(get_WorkMode_BtnStatus(_B_AUTO_MODE_))
    {
        ui->label_57->setStyleSheet("border-image: url(:/images/greenC.png);");
        ui->label_YX->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
    else
    {
        ui->label_57->setStyleSheet("border-image: url(:/images/blackC.png);");
        ui->label_YX->setStyleSheet("border-image: url(:/images/blackC.png);");
    }

    // 手动
    if(get_WorkMode_BtnStatus(_B_MANUAL_MODE1_) || get_WorkMode_BtnStatus(_B_MANUAL_MODE2_))
    {
        ui->label_69->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
    else
    {
        ui->label_69->setStyleSheet("border-image: url(:/images/blackC.png);");
    }

    // 调试模式
    if(get_WorkMode_BtnStatus(_B_DEBUG_MODE_))
    {
        ui->label_58->setStyleSheet("border-image: url(:/images/greenC.png);");
        ui->label_WH->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
    else
    {
        ui->label_58->setStyleSheet("border-image: url(:/images/blackC.png);");
        ui->label_WH->setStyleSheet("border-image: url(:/images/blackC.png);");
    }

    // 停机
    if(get_WorkMode_BtnStatus(_B_HALT_MODE_)) ui->label_55->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_55->setStyleSheet("border-image: url(:/images/blackC.png);");

    // 手动1
    if(get_WorkMode_BtnStatus(_B_MANUAL_MODE1_)) ui->label_W1->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W1->setStyleSheet("border-image: url(:/images/blackC.png);");

    // 手动2
    if(get_WorkMode_BtnStatus(_B_MANUAL_MODE2_)) ui->label_W2->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W2->setStyleSheet("border-image: url(:/images/blackC.png);");

    // 采水
    if(GetCollectBtnStatus(_S_COMPLETECOLLECT1_STAT_ | _S_COMPLETECOLLECT2_STAT_ | _S_TANKCOLLECT1_STAT_ | _S_TANKCOLLECT2_STAT_ | _S_CUPCOLLECT_STAT_))
        ui->label_CSCS->setStyleSheet("border-image: url(:/images/greenC.png);");
    else
        ui->label_CSCS->setStyleSheet("border-image: url(:/images/blackC.png);");

    if(GetCollectBtnStatus(_S_COMPLETECOLLECT1_STAT_)) ui->label_W10->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W10->setStyleSheet("border-image: url(:/images/blackC.png);");

    if(GetCollectBtnStatus(_S_COMPLETECOLLECT2_STAT_)) ui->label_W11->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W11->setStyleSheet("border-image: url(:/images/blackC.png);");

    if(GetCollectBtnStatus(_S_TANKCOLLECT1_STAT_)) ui->label_W12->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W12->setStyleSheet("border-image: url(:/images/blackC.png);");

    if(GetCollectBtnStatus(_S_TANKCOLLECT2_STAT_)) ui->label_W13->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W13->setStyleSheet("border-image: url(:/images/blackC.png);");

    if(GetCollectBtnStatus(_S_CUPCOLLECT_STAT_)) ui->label_W14->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W14->setStyleSheet("border-image: url(:/images/blackC.png);");

    // 清洗
    if(GetCleanBtnStatus(_B_CLEANTUBE1_STAT_)) ui->label_W4->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W4->setStyleSheet("border-image: url(:/images/blackC.png);");

    if(GetCleanBtnStatus(_B_CLEANTUBE2_STAT_)) ui->label_W5->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W5->setStyleSheet("border-image: url(:/images/blackC.png);");

    if(GetCleanBtnStatus(_B_CLEANCUP_STAT_)) ui->label_W6->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W6->setStyleSheet("border-image: url(:/images/blackC.png);");

    if(GetCleanBtnStatus(_B_CLEAN_SETTLING_TANK_)) ui->label_W7->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W7->setStyleSheet("border-image: url(:/images/blackC.png);");

//    if(GetCleanBtnStatus(_B_ALGAE_REMOVAL_)) ui->label_W8->setStyleSheet("border-image: url(:/images/greenC.png);");
//    else ui->label_W8->setStyleSheet("border-image: url(:/images/blackC.png);");

    if(GetCleanBtnStatus(_B_CLEAN_ALL_))
    {
        ui->label_W9->setStyleSheet("border-image: url(:/images/greenC.png);");
        ui->label_QMQX->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
    else
    {
        ui->label_W9->setStyleSheet("border-image: url(:/images/blackC.png);");
        ui->label_QMQX->setStyleSheet("border-image: url(:/images/blackC.png);");
    }

    // 水阀
//    if(IsValveXOpen[0]) ui->label_W15->setStyleSheet("border-image: url(:/images/greenC.png);");
//    else ui->label_W15->setStyleSheet("border-image: url(:/images/blackC.png);");

//    if(IsValveXOpen[1]) ui->label_W16->setStyleSheet("border-image: url(:/images/greenC.png);");
//    else ui->label_W16->setStyleSheet("border-image: url(:/images/blackC.png);");

//    if(IsValveXOpen[2]) ui->label_W17->setStyleSheet("border-image: url(:/images/greenC.png);");
//    else ui->label_W17->setStyleSheet("border-image: url(:/images/blackC.png);");

//    if(IsValveXOpen[3]) ui->label_W18->setStyleSheet("border-image: url(:/images/greenC.png);");
//    else ui->label_W18->setStyleSheet("border-image: url(:/images/blackC.png);");

//    if(IsValveXOpen[4]) ui->label_W19->setStyleSheet("border-image: url(:/images/greenC.png);");
//    else ui->label_W19->setStyleSheet("border-image: url(:/images/blackC.png);");

//    if(IsValveXOpen[5]) ui->label_W20->setStyleSheet("border-image: url(:/images/greenC.png);");
//    else ui->label_W20->setStyleSheet("border-image: url(:/images/blackC.png);");

//    if(IsValveXOpen[6]) ui->label_W21->setStyleSheet("border-image: url(:/images/greenC.png);");
//    else ui->label_W21->setStyleSheet("border-image: url(:/images/blackC.png);");

//    if(IsValveXOpen[7]) ui->label_W21_1->setStyleSheet("border-image: url(:/images/greenC.png);");
//    else ui->label_W21_1->setStyleSheet("border-image: url(:/images/blackC.png);");

//    //FIXME：  需要检验 对应关系
//    if(IsValveXOpen[8]) ui->label_W22->setStyleSheet("border-image: url(:/images/greenC.png);");
//    else ui->label_W22->setStyleSheet("border-image: url(:/images/blackC.png);");

    if(IsBtnInRunWaterValve) ui->label_W22->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W22->setStyleSheet("border-image: url(:/images/blackC.png);");

    if(IsIsBtnInAirValve) ui->label_W23->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W23->setStyleSheet("border-image: url(:/images/blackC.png);");

    // 水泵控制
    if(IsBtnInInputPump1) ui->label_W24->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W24->setStyleSheet("border-image: url(:/images/blackC.png);");
    if(IsBtnInInputPump2) ui->label_W25->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W25->setStyleSheet("border-image: url(:/images/blackC.png);");
    if(IsBtnInPeristalticPump) ui->label_W26->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_W26->setStyleSheet("border-image: url(:/images/blackC.png);");

    // 水压浮标
    if(IsBuoyTriggered[0]) ui->label_61->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_61->setStyleSheet("border-image: url(:/images/blackC.png);");
    if(IsBuoyTriggered[1]) ui->label_62->setStyleSheet("border-image: url(:/images/greenC.png);");
    else ui->label_62->setStyleSheet("border-image: url(:/images/blackC.png);");

    ui->label_State->setText("当前状态：" + PlcRunState);
}


void MainWindow::upload_init()
{
    CJsonFile jsonfile;
    QJsonArray pJsonArray_Upload = jsonfile.get_level1_array(UPLOAD);

    for(int i = 0; i < pJsonArray_Upload.size(); i++)
    {
        QJsonObject pJsonUpload = pJsonArray_Upload.at(i).toObject();

        if(pJsonUpload.contains(UPLOAD_IP))
        {
            IPlist << QHostAddress(pJsonUpload[QLatin1String(UPLOAD_IP)].toString());
        }

        if(pJsonUpload.contains(UPLOAD_PORT))
        {
            Portlist << atoi(pJsonUpload[QLatin1String(UPLOAD_PORT)].toString().toStdString().data());
        }

        if(pJsonUpload.contains(UPLOAD_PASSWORD))
        {
            Passwordlist << pJsonUpload[QLatin1String(UPLOAD_PASSWORD)].toString();
        }

        if(pJsonUpload.contains(UPLOAD_INTERNAL))
        {
            m_UploadInternal = pJsonUpload[QLatin1String(UPLOAD_INTERNAL)].toString().toUInt();
        }

        if(pJsonUpload.contains(UPLOAD_MN))
        {
            m_MN = pJsonUpload[QLatin1String(UPLOAD_MN)].toString();
        }

        // qDebug() << "IPlist: " << IPlist;
        // qDebug() << "Portlist: " << Portlist;
        // qDebug() << "Passwordlist: " << Passwordlist;

        QTcpSocket* socket = new QTcpSocket();
        SocketList << socket;
        connect(socket, SIGNAL(connected()),this, SLOT(connected()));
        connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
        connect(socket, SIGNAL(readyRead()),this, SLOT(receiveMessages()));
        connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),this, SLOT(socketStateChange(QAbstractSocket::SocketState)));
        this->toConnect();
    }
}

void MainWindow::toConnect()
{
    /* 如果连接状态还没有连接 */
    qint8 socket_num = 0;
    for(socket_num = 0; socket_num < IPlist.count(); socket_num++)
    {
        if (SocketList[socket_num]->state() != SocketList[socket_num]->ConnectedState) {
            /* 指定IP地址和端口连接 */
            SocketList[socket_num]->connectToHost(IPlist[socket_num], Portlist[socket_num]);
            QLOG_DEBUG() << "IP:" << IPlist[socket_num] << "Port:" << Portlist[socket_num] << "password:" << Passwordlist[socket_num];
        }
    }
}

void MainWindow::connected()
{
    QLOG_INFO() << "连接到平台成功...";
}

void MainWindow::toDisConnect()
{
    qint8 socket_num = 0;
    for(socket_num = 0; socket_num < IPlist.count(); socket_num++)
    {
        /* 断开连接 */
        SocketList[socket_num]->disconnectFromHost();
        /* 关闭socket*/
        SocketList[socket_num]->close();
    }
}

void MainWindow::reconnect()
{
    qint8 socket_num = 0;
    for(socket_num = 0; socket_num < IPlist.count(); socket_num++)
    {
        if (SocketList[socket_num]->state() != SocketList[socket_num]->ConnectedState && SocketList[socket_num]->state() != SocketList[socket_num]->ConnectingState) {
            /* 断开连接 */
            SocketList[socket_num]->disconnectFromHost();
            /* 关闭socket*/
            SocketList[socket_num]->close();
            /* 指定IP地址和端口连接 */
            SocketList[socket_num]->connectToHost(IPlist[socket_num], Portlist[socket_num]);
        }
    }
}

void MainWindow::disconnected()
{
    QLOG_ERROR() << "网络连接断开...";
}

/* 客户端状态改变 */
void MainWindow::socketStateChange(QAbstractSocket::SocketState state)
{
    QTcpSocket *receivSocket = qobject_cast<QTcpSocket*>(sender());
    if(receivSocket == nullptr) {
        return;
    }

    QLOG_INFO() << "平台IP: " << receivSocket->peerName() << " Tcp 连接状态: " << state;

    switch (state) {
    case QAbstractSocket::UnconnectedState:
        break;
    case QAbstractSocket::ConnectedState:
        break;
    case QAbstractSocket::ConnectingState:
        break;
    case QAbstractSocket::HostLookupState:
        break;
    case QAbstractSocket::ClosingState:
        break;
    case QAbstractSocket::ListeningState:
        break;
    case QAbstractSocket::BoundState:
        break;
    default:
        break;
    }
}

void MainWindow::radioButton_Init()
{
    m_RealtimeUpload = new QButtonGroup(this);
    m_RealtimeUpload->addButton(ui->radioButtonRTY, 0);
    m_RealtimeUpload->addButton(ui->radioButtonRTN, 1);
    ui->radioButtonRTN->setChecked(true);

    m_HeartBreak = new QButtonGroup(this);
    m_HeartBreak->addButton(ui->radioButtonHY, 0);
    m_HeartBreak->addButton(ui->radioButtonHN, 1);
    ui->radioButtonHN->setChecked(true);
}

void MainWindow::device_widget_clear()
{
//    foreach(QWidget* widget,DevWidget_list)
//    {
//        DevWidget_list.removeOne(widget);
//        widget->deleteLater();
//        widget = nullptr;
//    }

    foreach(QLabel* label,DevKey_list)
    {
        DevKey_list.removeOne(label);
        label->deleteLater();
        label = nullptr;
    }
    foreach(QLabel* label,DevVal_list)
    {
        DevVal_list.removeOne(label);
        label->deleteLater();
        label = nullptr;
    }

    foreach(QPushButton* button,DevButton_list)
    {
        DevButton_list.removeOne(button);
        button->deleteLater();
        button = nullptr;
    }
}

void MainWindow::device_info_widget_clear()
{
    foreach(QLabel* label,DevKey_list)
    {
        DevKey_list.removeOne(label);
        label->deleteLater();
        label = nullptr;
    }
    foreach(QLabel* label,DevVal_list)
    {
        DevVal_list.removeOne(label);
        label->deleteLater();
        label = nullptr;
    }
}

void MainWindow::factor_widget_clear()
{
    foreach(QWidget* widget,widget_list)
    {
        widget_list.removeOne(widget);
        widget->deleteLater();
        widget = nullptr;
    }

    foreach(QLabel* label,NAME_list)
    {
        NAME_list.removeOne(label);
        label->deleteLater();
        label = nullptr;
    }
    foreach(QLabel* label,DATA_list)
    {
        DATA_list.removeOne(label);
        label->deleteLater();
        label = nullptr;
    }
}

void MainWindow::start_measure()
{
    // 发送测量指令
    CDevice *pDevice = get_CDevice(ui->comboBoxDev->currentText());
    if(pDevice)
    {
        if(SendMeasureCmd(pDevice->get_address(),pDevice->get_type()))
        {
            QMessageBox::about(NULL, "提示", "<font color='black'>发送启动测量指令成功！</font>");
            device_info_widget_clear();
            device_display();
        }
        else
            QMessageBox::about(NULL, "提示", "<font color='black'>发送启动测量指令失败！</font>");
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>发送启动测量指令-失败-！</font>");
    }

    return;
}

void MainWindow::read_value()
{
    // 读取测量值
    CDevice *pDevice = get_CDevice(ui->comboBoxDev->currentText());
    if(pDevice)
    {
        if(ReadMeasureResult(pDevice))
        {
            device_info_widget_clear();
            device_display();

            QMessageBox::about(NULL, "提示", "<font color='black'>读取测量值成功！</font>");
        }
        else
        {
            QMessageBox::about(NULL, "提示", "<font color='black'>读取测量值失败！</font>");
        }
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>读取测量值-失败-！</font>");
    }

    return;
}

void MainWindow::device_display()
{

    QString pDateTime = "";

    int i = 0;
    int j = 20;
    QMap<QString,CDevice *>::iterator it;
    for (it=g_StrDevNodeMap.begin();it!= g_StrDevNodeMap.end() ; it++) {
        CDevice *pDevice = it.value();

        if("--选择仪器--" == ui->comboBoxDev->currentText())
        {
            QMessageBox::about(NULL, "提示", "<font color='black'>请先选择需要控制的仪器！</font>");
            return;
        }
        else if(pDevice->get_name() != ui->comboBoxDev->currentText())
        {
            continue;
        }

        if(!pDevice->get_type().contains("五参"))
        {
            QPushButton *button = new QPushButton(ui->widget_Struct);
            button->setGeometry(50,j,120,40);
            button->setStyleSheet(QLatin1String("font: 9pt"));
            DevButton_list.append(button);
            button->setText("启动测量");
            connect(button, SIGNAL(clicked()),this, SLOT(start_measure()));
            button->show();
            j+=60;
        }

        QPushButton *button1 = new QPushButton(ui->widget_Struct);
        button1->setGeometry(50,j,120,40);
        button1->setStyleSheet(QLatin1String("font: 9pt"));
        DevButton_list.append(button1);
        button1->setText("读取测量值");
        connect(button1, SIGNAL(clicked()),this, SLOT(read_value()));
        button1->show();
        j+=60;

        QMap<QString,CFactor *> mapFactorList = pDevice->get_map_DevFactors();
        QMap<QString,CFactor *>::iterator itFa;
        for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {

            CFactor *pFactor = itFa.value();
            if(pFactor)
            {
                if(pFactor->getProperty().toUInt() == 0)
                {

                    QLabel *name = new QLabel(ui->widget_Info);
                    name->setGeometry(0,i*35+5,100,30);
                    name->setAlignment(Qt::AlignLeft);
                    name->setStyleSheet(QLatin1String("font: 9pt"));
                    DevKey_list.append(name);
                    name->setText(pFactor->getFactorName() + "测量值");
                    name->show();

                    QLabel *data = new QLabel(ui->widget_Info);
                    data->setGeometry(105,i*35+5,120,30);
                    data->setAlignment(Qt::AlignLeft);
                    data->setStyleSheet(QStringLiteral("font: 9pt 黑体;"));
                    DevVal_list.append(data);
                    data->setText(pFactor->getFactorData());
                    data->show();

                    pDateTime = pFactor->getDateTime();

                    if(!pDevice->get_type().contains("五参"))
                    {
                        i++;

                        QLabel *key = new QLabel(ui->widget_Info);
                        key->setGeometry(0,i*35+5,100,30);
                        key->setAlignment(Qt::AlignLeft);
                        key->setStyleSheet(QLatin1String("font: 9pt"));
                        DevKey_list.append(key);
                        key->setText(pFactor->getFactorName() + "测量时间");
                        key->show();

                        QLabel *val = new QLabel(ui->widget_Info);
                        val->setGeometry(105,i*35+5,120,30);
                        val->setAlignment(Qt::AlignLeft);
                        val->setStyleSheet(QStringLiteral("font: 9pt 黑体;"));
                        DevVal_list.append(val);
                        val->setText(pDateTime);
                        val->show();
                    }

                    i++;
                }
            }
        }
    }

}

void MainWindow::factor_display()
{
    factor_widget_clear();

    int i= 0;
    QMap<QString,CDevice *>::iterator it;
    for (it=g_StrDevNodeMap.begin();it!= g_StrDevNodeMap.end() ; it++) {
        CDevice *pDevice = it.value();

        if(pDevice)
        {
            ui->comboBoxDev->addItem(pDevice->get_name());
        }

        QMap<QString,CFactor *> mapFactorList = pDevice->get_map_DevFactors();
        QMap<QString,CFactor *>::iterator itFa;
        for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {

            CFactor *pFactor = itFa.value();
            if(pFactor)
            {
                if(pFactor->getProperty().toUInt() == 0)
                {
                    if((i+1) >= 9)  // i start from 0
                    {
                        if((i+1) % 4 == 0)
                        {
                            ui->scrollAreaWidgetContents->setFixedHeight(55 * ((i+1)/4));
                        }
                        else
                        {
                            ui->scrollAreaWidgetContents->setFixedHeight(55 * ((i+1)/4 + 1));
                        }
                    }

                    QWidget *widget = new QWidget(ui->scrollAreaWidgetContents);
                    widget->setGeometry(10+(i%4)*95,(i/4)*55,90,50);

                    widget->setStyleSheet(QLatin1String("background-color: rgb(234, 234, 234);"));
                    widget_list.append(widget);

                    QLabel *name = new QLabel(widget);
                    name->setGeometry(0,0,90,25);
                    name->setAlignment(Qt::AlignCenter);
                    name->setStyleSheet(QLatin1String("font: 9pt"));
                    NAME_list.append(name);

                    if(pFactor->getTagId().toUInt())
                    {
                        NAME_list.at(i)->setText(pFactor->getFactorName()+"-"+pFactor->getTagId());
                    }
                    else    // tagID 为0
                    {
                        NAME_list.at(i)->setText(pFactor->getFactorName());
                    }
                    NAME_list.at(i)->show();

                    QLabel *data = new QLabel(widget);
                    data->setGeometry(0,25,90,25);
                    data->setAlignment(Qt::AlignCenter);
                    data->setStyleSheet(QStringLiteral("font: 9pt 黑体;"));
                    DATA_list.append(data);
                    DATA_list.at(i)->setText(pFactor->getFactorData());
                    DATA_list.at(i)->show();

                    // 查询历史数据 因子名称 下拉 填充
                    if(pFactor->getTagId().toUInt() == 0)
                    {
                        ui->comboBox_FC->addItem(pFactor->getFactorCode()+"  "+pFactor->getFactorName());
                    }
                    else
                    {
                        ui->comboBox_FC->addItem(pFactor->getFactorCode()+"-"+pFactor->getTagId() + "  " +pFactor->getFactorName());
                    }

                    i++;
                }
            }
        }
    }
}

void MainWindow::receiveMessages()
{
    QTcpSocket *receivSocket = qobject_cast<QTcpSocket*>(sender());
    if(receivSocket == nullptr) {
        return;
    }
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(utf8);

    QTextCodec *gbk = QTextCodec::codecForName("GBK");
    QString messages = gbk->toUnicode(receivSocket->readAll());

    QByteArray utf8_bytes = utf8->fromUnicode(messages);
    messages = utf8_bytes;

    // 0077QN=20220908153950000;ST=21;C
    if(messages.contains(";"))
    {
        QStringList pStr1List = messages.split(";");
        QString pStrQN = pStr1List[0];
        if(pStrQN.contains("QN="))
        {
            QStringList pStr2List = pStrQN.split("QN=");
            if(pStr2List.size() == 2)
            {
                QString pStrQN_Val = pStr2List[1];
                if(pStrQN_Val == QString::fromStdString(g_StrQN))
                {
                    QString pText = "From " + receivSocket->peerName() +": " + messages;
                    ui->textBrowserBF->append(pText);
                }
            }
        }
    }

    QString pTextInfo = "From " + receivSocket->peerName() +": " + messages;
    QLOG_DEBUG() << pTextInfo;

    QString pAddress = receivSocket->peerName() +":" + QString::number(receivSocket->peerPort());

    QString sql = QString("INSERT INTO packages (timestamp,property,address,package) "
                  "VALUES ('%1','2','%2','%3')").arg(QString::fromStdString(current_formattime()),pAddress,messages);

    // qDebug() << "sql===>>" << sql;
    QSqlQuery query(db);
    query.exec(sql);
}

void MainWindow::sendMessages(QTcpSocket* socket, QString message)
{
    if(NULL == socket)
    {
        return;
    }

    if(socket->state() == socket->ConnectedState) {
        /* 发送消息 */
        socket->write(message.toUtf8().data());
    }
}

void MainWindow::store_work()
{
    int m = 0;
    QMap<QString,CDevice *>::iterator it;
    for (it=g_StrDevNodeMap.begin();it!= g_StrDevNodeMap.end() ; it++) {
        m++;
        CDevice *pDevice = it.value();
        QMap<QString,CFactor *> mapFactorList = pDevice->get_map_DevFactors();
        QMap<QString,CFactor *>::iterator itFa;

        int n=0;
        for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {
            n++;
            CFactor *pFactor = itFa.value();

            if(pFactor)
            {
                QString prop = pFactor->getProperty();
                if(prop.toUInt() == 0)
                {
                    QString pFactorCode =  pFactor->getFactorCode();
                    QString pFactorName =  pFactor->getFactorName();
                    QString pTagID =  pFactor->getTagId();
                    QString pVal = pFactor->getFactorData();
                    QString pFlag = pFactor->getFlag();

                    QString sql;
                    if(pTagID.toUInt() == 0)
                    {
                        sql = QString("INSERT INTO historydata (timestamp,key,keyName,value,flag) "
                                      "VALUES ('%1','%2','%3','%4','%5')").arg(QString::fromStdString(current_formattime()),pFactorCode,pFactorName,pVal,pFlag);
                    }
                    else
                    {
                        sql = QString("INSERT INTO historydata (timestamp,key,keyName,value,flag) "
                                      "VALUES ('%1','%2','%3','%4','%5')").arg(QString::fromStdString(current_formattime()),pFactorCode+"-"+pTagID,pFactorName,pVal,pFlag);
                    }

                    // qDebug() << "sql===>>" << sql;
                    QSqlQuery query(db);
                    query.exec(sql);

                }
            }
        }
    }
}

void MainWindow::CheckNetworkStatus()
{
    // 平台tcp连接

    this->reconnect();

    FlushTipStatusTimerCallback();
}

void MainWindow::upload_work()
{
    // qDebug() << "IPlist.count()===>>" << IPlist.count();

    qint8 i = 0;
    for(i = 0; i < IPlist.count(); i++)
    {
        if (SocketList[i]->state() == SocketList[i]->ConnectedState) {
            g_StrQN = "";
            HJ212_PARAM_SET hj212_param;
            HJ212_DATA_PARAM hj212_data_param;

            hj212_data_param.DataTime = g_DateTime;
            QString pRtnData;

            int m = 0;
            QMap<QString,CDevice *>::iterator it;
            for (it=g_StrDevNodeMap.begin();it!= g_StrDevNodeMap.end() ; it++) {
                m++;
                CDevice *pDevice = it.value();
                QMap<QString,CFactor *> mapFactorList = pDevice->get_map_DevFactors();
                QMap<QString,CFactor *>::iterator itFa;

                int n=0;
                for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {
                    n++;
                    CFactor *pFactor = itFa.value();

                    if(pFactor)
                    {
                        QString prop = pFactor->getProperty();
                        if(prop.toUInt() == 0)
                        {
                            QString pFactorCode =  pFactor->getFactorCode();
                            // qDebug() << "pFactorCode===>>" << pFactorCode;
                            if(pFactor->getFactorData() == "-")
                            {
                                pRtnData += pFactorCode+HJ212_FIELD_NAME_DATA_RTD + "0" + "," + pFactorCode+HJ212_FIELD_NAME_DATA_FLAG + HJ212_DATA_FLAG_COMM;
                            }
                            else
                            {
                                pRtnData += pFactorCode+HJ212_FIELD_NAME_DATA_RTD + pFactor->getFactorData()+"," + pFactorCode+HJ212_FIELD_NAME_DATA_FLAG + HJ212_DATA_FLAG_RUN;
                            }

                            bool flag = false;
                            if(m == g_StrDevNodeMap.size())
                            {
                                if(pDevice->get_type().contains("五参"))
                                {
                                    if(n == mapFactorList.size())
                                    {
                                        flag = true;
                                    }
                                }
                                else    // 非五参仪器只有一个测量因子
                                {
                                    flag = true;
                                }
                            }
                            if(!flag) pRtnData += ";";
                        }
                    }
                }
            }
            // qDebug() << "pRtnData===>>" << pRtnData;

            hj212_data_param.Rtd = pRtnData.toStdString();

            hj212_param = hj212_set_params(HJ212_DEF_VAL_ST, HJ212_CN_RTD_DATA, HJ212_FLAG_ACK_EN, &hj212_data_param, Passwordlist[i].toStdString(), m_MN.toStdString());
            string rep = hj212_frame_assemble(&hj212_param);
            this->sendMessages(SocketList[i], QString::fromStdString(rep));

            QString pTextInfo = "To " + IPlist.at(i).toString() +": " + QString::fromStdString(rep);
            QLOG_DEBUG() << pTextInfo;

            QString pAddress = IPlist.at(i).toString() +":" + QString::number(Portlist.at(i));

            QString sql = QString("INSERT INTO packages (timestamp,property,address,package) "
                          "VALUES ('%1','1','%2','%3')").arg(QString::fromStdString(current_formattime()),pAddress,QString::fromStdString(rep));

            // qDebug() << "sql===>>" << sql;
            QSqlQuery query(db);
            query.exec(sql);
        }
    }
}

int SignedHex2Int(QByteArray array){
    QString qnum =array.toHex().data();
    int num;
    bool ok;
    num = qnum.toInt(&ok,16);
    if(!(qnum.at(0)>='0'&&qnum.at(0)<='7')){//非正数
        num = num & 0x7FFF;       //清除符号位
        num=32768-num;                   //反码
        num = num * -1;                //符号位
    }
    return num;
}

void MainWindow::setDeviceInit(QJsonObject &pJsonRootObj)
{

    if(pJsonRootObj.contains(DEVICE))
    {
        QJsonValue pVlaue = pJsonRootObj.value(DEVICE);
        if(pVlaue.isArray())
        {
            QJsonArray pDevArray = pVlaue.toArray();
            int pCnt = pDevArray.size();
            for(int i=0; i<pCnt; i++)
            {
                QJsonObject pJsonDev = pDevArray.at(i).toObject();
                if(pJsonDev.contains(DEV_NAME))
                {
                    QString pDevName = pJsonDev.value(DEV_NAME).toString();

                    if(m_DevBtnEditMap.contains(pDevName) || m_DevBtnDeleMap.contains(pDevName))
                    {
                        continue;
                    }

                    QPushButton *pEditBtn = new QPushButton(QStringLiteral("保存"));
                    pEditBtn->setFixedSize(QSize(60, 30));
                    pEditBtn->setStyleSheet(QStringLiteral("QPushButton{color:white;background-color:rgb(51,204,255);font-family:黑体;font-size: 18px; border-radius: 10px;}"
                                                           "QPushButton:pressed{background-color:rgb(51,129,172)}"));

                    QPushButton *pDeleBtn = new QPushButton(QStringLiteral("删除"));
                    pDeleBtn->setFixedSize(QSize(60, 30));
                    pDeleBtn->setStyleSheet(QStringLiteral("QPushButton{color:white;background-color:red;font-family:黑体;font-size: 18px; border-radius: 10px;}"
                                                           "QPushButton:pressed{background-color:rgb(51,129,172)}"));

                    m_DevBtnEditMap.insert(pDevName,pEditBtn);
                    m_DevBtnDeleMap.insert(pDevName,pDeleBtn);

                    connect(pEditBtn, SIGNAL(clicked(bool)), m_SignalMapperEdit, SLOT(map()));
                    // 设置signalmapper的转发规则, 转发为参数为QString类型的信号， 并把textList[i]的内容作为实参传递。
                    m_SignalMapperEdit->setMapping(pEditBtn, pDevName);

                    connect(pDeleBtn, SIGNAL(clicked(bool)), m_SignalMapperDele, SLOT(map()));
                    // 设置signalmapper的转发规则, 转发为参数为QString类型的信号， 并把textList[i]的内容作为实参传递。
                    m_SignalMapperDele->setMapping(pDeleBtn, pDevName);
                }
            }
        }
    }
}

void MainWindow::setUploadInit(QJsonObject &pJsonRootObj)
{

    if(pJsonRootObj.contains(UPLOAD))
    {
        QJsonValue pVlaue = pJsonRootObj.value(UPLOAD);
        if(pVlaue.isArray())
        {
            QJsonArray pUpArray = pVlaue.toArray();
            int pCnt = pUpArray.size();
            for(int i=0; i<pCnt; i++)
            {
                QJsonObject pJsonUpSetting = pUpArray.at(i).toObject();
                if(pJsonUpSetting.contains(UPLOAD_IP) && pJsonUpSetting.contains(UPLOAD_PORT))
                {
                    QString pServerAddr = pJsonUpSetting.value(UPLOAD_IP).toString() + ":" + pJsonUpSetting.value(UPLOAD_PORT).toString();

                    if(m_UploadBtnDeleMap.contains(pServerAddr))
                    {
                        continue;
                    }

                    QPushButton *pDeleBtn = new QPushButton(QStringLiteral("删除"));
                    pDeleBtn->setFixedSize(QSize(60, 30));
                    pDeleBtn->setStyleSheet(QStringLiteral("QPushButton{color:white;background-color:red;font-family:黑体;font-size: 18px; border-radius: 10px;}"
                                                           "QPushButton:pressed{background-color:rgb(51,129,172)}"));

                    m_UploadBtnDeleMap.insert(pServerAddr,pDeleBtn);

                    connect(pDeleBtn, SIGNAL(clicked(bool)), m_SignalMapperUpDele, SLOT(map()));
                    // 设置signalmapper的转发规则, 转发为参数为QString类型的信号， 并把textList[i]的内容作为实参传递。
                    m_SignalMapperUpDele->setMapping(pDeleBtn, pServerAddr);
                }
            }
        }
    }
}

void MainWindow::btnClicked_DevEdit(QString pDevName)
{
    // qDebug() << "pDevName edit====>>" << pDevName;

    for(int row=0; row<ui->tableWidget->rowCount(); row++)
    {
        QTableWidgetItem* item = ui->tableWidget->item(row,0);
        if(item != nullptr)
        {
            if(item->text() == pDevName)
            {

                QTableWidgetItem* pItemAddr = ui->tableWidget->item(row,1);
                QTableWidgetItem* pItemTemp = ui->tableWidget->item(row,2);
                if(pItemAddr != nullptr && pItemTemp != nullptr)
                {
                    CJsonFile jsonfile;
                    jsonfile.edit_devnode(pDevName,pItemAddr->text(),pItemTemp->text());
                    QMessageBox::about(NULL, "提示", "<font color='black'>保存设备配置成功！</font>");
                    return;
                }
            }
        }
    }
}


void MainWindow::btnClicked_DevDele(QString pDevName)
{
    qDebug() << "pDevName dele====>>" << pDevName;
    // 从系统配置中删除pDevName节点
    CJsonFile jsonfile;
    if(jsonfile.remove_devnode(pDevName))
    {
        QJsonObject pJsonRootObj = jsonfile.get_root();
        m_SignalMapperEdit->removeMappings(m_DevBtnEditMap[pDevName]);
        m_SignalMapperDele->removeMappings(m_DevBtnDeleMap[pDevName]);

        if(m_DevBtnEditMap.contains(pDevName))
        {
            QPushButton *button = m_DevBtnEditMap[pDevName];
            if(button) delete button;
            m_DevBtnEditMap.remove(pDevName);
        }

        if(m_DevBtnDeleMap.contains(pDevName))
        {
            QPushButton *button = m_DevBtnDeleMap[pDevName];
            if(button) delete button;
            m_DevBtnDeleMap.remove(pDevName);
        }

        setTableContents(pJsonRootObj);
        QMessageBox::about(NULL, "提示", "<font color='black'>删除成功！</font>");
    }
}


void MainWindow::btnClicked_UploadDele(QString pServerAddr)
{
    qDebug() << "pServerAddr dele====>>" << pServerAddr;
    // 从系统配置中删除pDevName节点

    int index;
    CJsonFile jsonfile;
    if(jsonfile.remove_uploadnode(pServerAddr,index))
    {
        qDebug() << "dele tcpclient index ==> " << index;
        IPlist.removeAt(index);
        Portlist.removeAt(index);
        Passwordlist.removeAt(index);
        /* 断开连接 */
        SocketList[index]->disconnectFromHost();
        /* 关闭socket*/
        SocketList[index]->close();
        delete SocketList[index];
        SocketList[index] = nullptr;
        SocketList.removeAt(index);

        QJsonObject pJsonRootObj = jsonfile.get_root();
        m_SignalMapperUpDele->removeMappings(m_UploadBtnDeleMap[pServerAddr]);

        if(m_UploadBtnDeleMap.contains(pServerAddr))
        {
            QPushButton *button = m_UploadBtnDeleMap[pServerAddr];
            if(button) delete button;
            m_UploadBtnDeleMap.remove(pServerAddr);
        }

        setUpTableContents(pJsonRootObj);

        QMessageBox::about(NULL, "提示", "<font color='black'>删除成功！</font>");
    }
}

void MainWindow::serialinfo_display(QJsonObject &pJsonRootObj)
{
    if(pJsonRootObj.contains(SERIAL))
    {
        QJsonObject pJsonSerObj = pJsonRootObj.value(SERIAL).toObject();
        if(pJsonSerObj.contains(SERIAL_NAME))
        {
            m_port = pJsonSerObj.value(SERIAL_NAME).toString();
            ui->comboBox_name->setCurrentText(m_port);
        }
        if(pJsonSerObj.contains(SERIAL_BAUDRATE))
        {
            m_baudrate = pJsonSerObj.value(SERIAL_BAUDRATE).toString();
            ui->comboBox_baud->setCurrentText(m_baudrate);
        }
        if(pJsonSerObj.contains(SERIAL_DATABIT))
        {
            m_data_bit = pJsonSerObj.value(SERIAL_DATABIT).toString();
            ui->comboBox_databit->setCurrentText(m_data_bit);
        }
        if(pJsonSerObj.contains(SERIAL_STOPBIT))
        {
            m_stop_bit = pJsonSerObj.value(SERIAL_STOPBIT).toString();
            ui->comboBox_stopbit->setCurrentText(m_stop_bit);
        }
        if(pJsonSerObj.contains(SERIAL_PARITY))
        {
            m_parity = pJsonSerObj.value(SERIAL_PARITY).toString();
            ui->comboBox_parity->setCurrentText(m_parity);
        }
    }
}

void MainWindow::HandleDateTimeout()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss");
    ui->textEditDataTime->setText(str);
}

void MainWindow::SetIndex(int id)
{
    ui->stackedWidget->setCurrentIndex(id);
}


void MainWindow::installEvents() {
    labelList.append(ui->label_01t);
    labelList.append(ui->label_02t);

    labelList.append(ui->label_11t);
    labelList.append(ui->label_12t);

    labelList.append(ui->label_11t_2);
    labelList.append(ui->label_12t_2);

    labelList.append(ui->label_21t);
    labelList.append(ui->label_22t);

    labelList.append(ui->label_31t);
    labelList.append(ui->label_32t);

    labelList.append(ui->label_41t);
    labelList.append(ui->label_42t);

    labelList.append(ui->label_51t);
    labelList.append(ui->label_52t);

    labelList.append(ui->label_61t);
    labelList.append(ui->label_62t);

    labelList.append(ui->label_71t);
    labelList.append(ui->label_72t);

    for(int i = 0; i < labelList.size(); ++ i) {
        labelList.at(i)->installEventFilter(this);
    }
}

//事件过滤器
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    for(int i = 0; i < labelList.size(); ++ i) {
        if(obj == labelList.at(i)) {
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event); // 事件转换
                if(mouseEvent->button() == Qt::LeftButton) {
                    this->SetIndex(i / 2);
                    return true;
                }
            }
            return false;
        }
    }
    return QWidget::eventFilter(obj, event);
}


// 串口信息保存 槽函数
void MainWindow::on_pushButtonSerialSaved_clicked()
{
    QString port = ui->comboBox_name->currentText();
    QString baud = ui->comboBox_baud->currentText();
    QString databit = ui->comboBox_databit->currentText();
    QString stopbit = ui->comboBox_stopbit->currentText();
    QString parity = ui->comboBox_parity->currentText();

    CJsonFile jsonfile;
    jsonfile.jsonfile_serial(port,baud,databit,stopbit,parity);
}

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
headerText << QStringLiteral("设备名") << QStringLiteral("设备地址") << QStringLiteral("设备模板")
           << QStringLiteral("操作");
int cnt = headerText.count();
ui->tableWidget->setColumnCount(cnt);
ui->tableWidget->setHorizontalHeaderLabels(headerText);
// ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget->horizontalHeader()->font();
font.setBold(true);
ui->tableWidget->horizontalHeader()->setFont(font);

ui->tableWidget->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {150, 100, 150, 150};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget->setColumnWidth(i, widths[i]);
}

ui->tableWidget->setStyleSheet(qssTV);
ui->tableWidget->horizontalHeader()->setVisible(true);
ui->tableWidget->verticalHeader()->setDefaultSectionSize(45);
}

// 设置表格内容
void MainWindow::setTableContents(QJsonObject &pJsonRootObj)
{
    setDeviceInit(pJsonRootObj);
    ui->tableWidget->clearContents(); //只清除工作区，不清除表头

    // qDebug() << "pJsonRootObj===>>" << pJsonRootObj;
    QTableWidgetItem *pItemName,*pItemAddr,*pItemTemplate;
    if(pJsonRootObj.contains(DEVICE))
    {
        QJsonValue pVlaue = pJsonRootObj.value(DEVICE);
        if(pVlaue.isArray())
        {
            QJsonArray pDevArray = pVlaue.toArray();
            int pCnt = pDevArray.size();
            if(pCnt <= 6) ui->tableWidget->setRowCount(6);
            else ui->tableWidget->setRowCount(pCnt);
            for(int i=0; i<pCnt; i++)
            {
                QJsonObject pJsonDev = pDevArray.at(i).toObject();
                if(pJsonDev.contains(DEV_NAME))
                {

                    QString pDevName = pJsonDev.value(DEV_NAME).toString();
                    // dev_name

                    pItemName = new QTableWidgetItem(pDevName);
                    pItemName->setTextAlignment(Qt::AlignCenter);
                    pItemName->setFlags(Qt::ItemIsEditable);
                    ui->tableWidget->setItem(i, 0, pItemName);

                    if(pJsonDev.contains(DEV_ADDR))
                    {
                        // dev_addr

                        pItemAddr = new QTableWidgetItem(pJsonDev.value(DEV_ADDR).toString());
                        pItemAddr->setTextAlignment(Qt::AlignCenter);
                        ui->tableWidget->setItem(i, 1, pItemAddr);

                    }
                    if(pJsonDev.contains(DEV_TEMPLATE))
                    {
                        // dev_temp

                        pItemTemplate = new QTableWidgetItem(pJsonDev.value(DEV_TEMPLATE).toString());
                        pItemTemplate->setTextAlignment(Qt::AlignCenter);
                        ui->tableWidget->setItem(i, 2, pItemTemplate);

                    }
                    // 按钮

                    QWidget *btnWidget = new QWidget(this);
                    QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);    // FTIXME：<内存是否会随着清空tablewidget而释放>
                    btnLayout->addWidget(m_DevBtnEditMap[pDevName]);
                    btnLayout->addWidget(m_DevBtnDeleMap[pDevName]);
                    btnLayout->setMargin(5);
                    btnLayout->setAlignment(Qt::AlignCenter);
                    ui->tableWidget->setCellWidget(i, 3, btnWidget);
                }
            }
        }
    }
}



// 设置表格内容
void MainWindow::setUpTableContents(QJsonObject &pJsonRootObj)
{
    setUploadInit(pJsonRootObj);
    ui->tableWidget_UpLoad->clearContents(); //只清除工作区，不清除表头

    // qDebug() << "pJsonRootObj===>>" << pJsonRootObj;
    QTableWidgetItem *pItemMN,*pItemIP,*pItemPort;
    if(pJsonRootObj.contains(UPLOAD))
    {
        QJsonValue pVlaue = pJsonRootObj.value(UPLOAD);
        if(pVlaue.isArray())
        {
            QJsonArray pDevArray = pVlaue.toArray();
            int pCnt = pDevArray.size();
            if(pCnt <= 6) ui->tableWidget_UpLoad->setRowCount(6);
            else ui->tableWidget_UpLoad->setRowCount(pCnt);
            for(int i=0; i<pCnt; i++)
            {
                QJsonObject pJsonServer = pDevArray.at(i).toObject();
                if(pJsonServer.contains(UPLOAD_MN))
                {

                    QString pUpload_MN = pJsonServer.value(UPLOAD_MN).toString();
                    // dev_name

                    pItemMN = new QTableWidgetItem(pUpload_MN);
                    pItemMN->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidget_UpLoad->setItem(i, 0, pItemMN);

                    if(pJsonServer.contains(UPLOAD_IP))
                    {
                        pItemIP = new QTableWidgetItem(pJsonServer.value(UPLOAD_IP).toString());
                        pItemIP->setTextAlignment(Qt::AlignCenter);
                        ui->tableWidget_UpLoad->setItem(i, 1, pItemIP);
                    }
                    if(pJsonServer.contains(UPLOAD_PORT))
                    {
                        pItemPort = new QTableWidgetItem(pJsonServer.value(UPLOAD_PORT).toString());
                        pItemPort->setTextAlignment(Qt::AlignCenter);
                        ui->tableWidget_UpLoad->setItem(i, 2, pItemPort);
                    }
                    // 按钮
                    QString pServerAddr = pJsonServer.value(UPLOAD_IP).toString() + ":" + pJsonServer.value(UPLOAD_PORT).toString();
                    QLOG_INFO() << "上传平台地址： " << pServerAddr;

                    QWidget *btnWidget = new QWidget(this);
                    QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);    // FTIXME：<内存是否会随着清空tablewidget而释放>
                    btnLayout->addWidget(m_UploadBtnDeleMap[pServerAddr]);
                    btnLayout->setMargin(5);
                    btnLayout->setAlignment(Qt::AlignCenter);
                    ui->tableWidget_UpLoad->setCellWidget(i, 3, btnWidget);
                }
            }
        }
    }
}


void MainWindow::on_pushButtonAddDev_clicked()
{
    m_DialogNewDev = new DialogNewDev();
    m_DialogNewDev->show();
    connect(m_DialogNewDev, SIGNAL(exitWin()),this, SLOT(newDev_ExitWin()));
}

void MainWindow::newDev_ExitWin()
{
    CJsonFile jsonfile;
    QJsonObject pJsonRootObj = jsonfile.get_root();
    setTableContents(pJsonRootObj);
    QMessageBox::about(NULL, "提示", "<font color='black'>新增设备配置成功！</font>");
}

void MainWindow::on_pushButtonFreshDev_clicked()
{
    CJsonFile jsonfile;
    QJsonObject pJsonRootObj = jsonfile.get_root();
    setTableContents(pJsonRootObj);
    QMessageBox::about(NULL, "提示", "<font color='black'>刷新设备配置成功！</font>");
}


void MainWindow::on_pushButtonProtoDev_clicked()
{
    // 因子配置
    // 通讯规约、设备类型

    // 寄存器地址、数据类型、字节序abcd、功能码、
    m_DialogProtoDev = new DialogProtocol();
    m_DialogProtoDev->show();
}


bool MainWindow::OpenCom()
{
    if(m_port != "")
    {
        if(m_pSerialCom == NULL) m_pSerialCom = new Win_QextSerialPort(m_port,QextSerialBase::Polling);
        if (m_pSerialCom->open(QIODevice::ReadWrite))
        {
            QLOG_INFO()<<"打开串口成功...";

            m_pSerialCom->flush();

            int pIndexBaud = ui->comboBox_baud->currentIndex(); //设置波特率
            switch (pIndexBaud) {
            case 0:
                m_pSerialCom->setBaudRate(BAUD1200);
                break;
            case 1:
                m_pSerialCom->setBaudRate(BAUD2400);
                break;
            case 2:
                m_pSerialCom->setBaudRate(BAUD4800);
                break;
            case 3:
                m_pSerialCom->setBaudRate(BAUD9600);
                break;
            case 4:
                m_pSerialCom->setBaudRate(BAUD19200);
                break;
            case 5:
                m_pSerialCom->setBaudRate(BAUD38400);
                break;
            case 6:
                m_pSerialCom->setBaudRate(BAUD57600);
                break;
            case 7:
                m_pSerialCom->setBaudRate(BAUD115200);
                break;
            default:
                break;
            }

            int pIndexData = ui->comboBox_databit->currentIndex(); //设置数据位
            switch (pIndexData) {
            case 0:
                m_pSerialCom->setDataBits(DATA_5);
                break;
            case 1:
                m_pSerialCom->setDataBits(DATA_6);
                break;
            case 2:
                m_pSerialCom->setDataBits(DATA_7);
                break;
            case 3:
                m_pSerialCom->setDataBits(DATA_8);
                break;
            default:
                break;
            }

            int pIndexParity= ui->comboBox_parity->currentIndex(); //设置奇偶校验
            switch (pIndexParity) {
            case 0:
                m_pSerialCom->setParity(PAR_NONE);
                break;
            case 1:
                m_pSerialCom->setParity(PAR_EVEN);
                break;
            case 2:
                m_pSerialCom->setParity(PAR_ODD);
                break;
            default:
                break;
            }

            int pIndexStop= ui->comboBox_stopbit->currentIndex(); //设置停止位
            switch (pIndexStop) {
            case 0:
                m_pSerialCom->setStopBits(STOP_1);
                break;
            case 1:
                m_pSerialCom->setStopBits(STOP_2);
                break;
            default:
                break;
            }

            m_pSerialCom->setFlowControl(FLOW_OFF); //设置数据流控制，我们使用无数据流控制的默认设置
            m_pSerialCom->setTimeout(500);  // 串口 超时 毫秒
        }
    }
    else
    {
        QLOG_ERROR() << "通讯串口未配置...";
        return false;
    }

    return true;
}


void MainWindow::setUploadTableHeader()
{
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#1B89A1}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
headerText << QStringLiteral("MN") << QStringLiteral("服务器地址") << QStringLiteral("端口号")
         << QStringLiteral("操作");
int cnt = headerText.count();
ui->tableWidget_UpLoad->setColumnCount(cnt);
ui->tableWidget_UpLoad->setHorizontalHeaderLabels(headerText);
// ui->tableWidget_UpLoad->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget_UpLoad->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget_UpLoad->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
ui->tableWidget_UpLoad->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget_UpLoad->horizontalHeader()->font();
font.setBold(true);
ui->tableWidget_UpLoad->horizontalHeader()->setFont(font);

ui->tableWidget_UpLoad->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {200, 200, 100, 200};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget_UpLoad->setColumnWidth(i, widths[i]);
}

ui->tableWidget_UpLoad->setStyleSheet(qssTV);
ui->tableWidget_UpLoad->horizontalHeader()->setVisible(true);
ui->tableWidget_UpLoad->verticalHeader()->setDefaultSectionSize(45);
}

void MainWindow::on_pushButtonUpReset_clicked()
{
    ui->lineEdit_MN->setText("");
    ui->lineEdit_PW->setText("");
    ui->lineEdit_IP->setText("");
    ui->lineEdit_Port->setText("");
    ui->lineEdit_TO->setText("");
    ui->lineEdit_UpInternal->setText("");
    ui->lineEdit_MinInternal->setText("");
    ui->lineEdit_Retry->setText("");

    ui->radioButtonHN->setChecked(true);
    ui->radioButtonRTN->setChecked(true);
}

void MainWindow::on_pushButtonUpSaved_clicked()
{
    CUploadSetting pUpSetting;
    if(ui->lineEdit_MN->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>mn不能为空！</font>");
        return;
    }
    if(ui->lineEdit_IP->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>ip不能为空！</font>");
        return;
    }
    if(ui->lineEdit_Port->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>端口不能为空！</font>");
        return;
    }

    pUpSetting.set_mn(ui->lineEdit_MN->text());
    pUpSetting.set_ip(ui->lineEdit_IP->text());
    pUpSetting.set_port(ui->lineEdit_Port->text());
    pUpSetting.set_passwd(ui->lineEdit_PW->text());
    pUpSetting.set_min_internal(ui->lineEdit_MinInternal->text());
    pUpSetting.set_uplaod_internal(ui->lineEdit_UpInternal->text());
    pUpSetting.set_retry(ui->lineEdit_Retry->text());
    pUpSetting.set_timeout(ui->lineEdit_TO->text());

    if(m_HeartBreak->checkedId() == 0)  pUpSetting.set_heartbreak(true);
    else pUpSetting.set_heartbreak(false);

    if(m_RealtimeUpload->checkedId() == 0)  pUpSetting.set_rt_upload(true);
    else pUpSetting.set_rt_upload(false);

    CJsonFile jsonfile;
    jsonfile.insert_uploadnode(pUpSetting);

    QJsonObject pJsonRootObj = jsonfile.get_root();
    this->setUpTableContents(pJsonRootObj);

    // new add

    IPlist << QHostAddress(ui->lineEdit_IP->text());
    Portlist << atoi(ui->lineEdit_Port->text().toStdString().data());
    Passwordlist << ui->lineEdit_PW->text();

    qDebug() << "IPlist: " << IPlist;
    qDebug() << "Portlist: " << Portlist;
    qDebug() << "Passwordlist: " << Passwordlist;

    QTcpSocket* socket = new QTcpSocket();
    SocketList << socket;
    connect(socket, SIGNAL(connected()),this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()),this, SLOT(receiveMessages()));
    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),this, SLOT(socketStateChange(QAbstractSocket::SocketState)));
    this->toConnect();

}

void MainWindow::on_pushButton_63_clicked()
{
    if(get_WorkMode_BtnStatus(_B_AUTO_MODE_))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>当前系统处于自动模式，请先退出！</font>");
        return;
    }

    QString pTips = "";
    if(!get_WorkMode_BtnStatus(_B_DEBUG_MODE_))
    {
        pTips = "是否进入维护模式?";
    }
    else
    {
        pTips = "是否退出维护模式?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    if (IsEnableSkipSetting)
        ui->pushButton_W3->setDisabled(false);
    else
        ui->pushButton_W3->setDisabled(true);

    if(get_WorkMode_BtnStatus(_B_DEBUG_MODE_))
    {
        toggle_WorkMode_BtnStatus(_B_DEBUG_MODE_,false);
        ui->pushButton_W1->setDisabled(true);
        ui->pushButton_W2->setDisabled(true);
        ui->pushButton_W4->setDisabled(true);
        ui->pushButton_W5->setDisabled(true);
        ui->pushButton_W6->setDisabled(true);
        ui->pushButton_W7->setDisabled(true);
        ui->pushButton_W8->setDisabled(true);
        ui->pushButton_W9->setDisabled(true);
        ui->pushButton_W10->setDisabled(true);
        ui->pushButton_W11->setDisabled(true);
        ui->pushButton_W12->setDisabled(true);
        ui->pushButton_W13->setDisabled(true);
        ui->pushButton_W14->setDisabled(true);
        ui->pushButton_W15->setDisabled(true);
        ui->pushButton_W16->setDisabled(true);
        ui->pushButton_W17->setDisabled(true);
        ui->pushButton_W18->setDisabled(true);
        ui->pushButton_W19->setDisabled(true);
        ui->pushButton_W20->setDisabled(true);
        ui->pushButton_W21->setDisabled(true);
        ui->pushButton_W21_1->setDisabled(true);
        ui->pushButton_W22->setDisabled(true);
        ui->pushButton_W23->setDisabled(true);
        ui->pushButton_W24->setDisabled(true);
        ui->pushButton_W25->setDisabled(true);
        ui->pushButton_W26->setDisabled(true);

        ui->label_58->setStyleSheet("border-image: url(:/images/blackC.png);");
    }
    else
    {
        toggle_WorkMode_BtnStatus(_B_DEBUG_MODE_,true);
        ui->pushButton_W1->setDisabled(false);
        ui->pushButton_W2->setDisabled(false);
        ui->pushButton_W4->setDisabled(false);
        ui->pushButton_W5->setDisabled(false);
        ui->pushButton_W6->setDisabled(false);
        ui->pushButton_W7->setDisabled(false);
        ui->pushButton_W8->setDisabled(false);
        ui->pushButton_W9->setDisabled(false);
        ui->pushButton_W10->setDisabled(false);
        ui->pushButton_W11->setDisabled(false);
        ui->pushButton_W12->setDisabled(false);
        ui->pushButton_W13->setDisabled(false);
        ui->pushButton_W14->setDisabled(false);
        ui->pushButton_W15->setDisabled(false);
        ui->pushButton_W16->setDisabled(false);
        ui->pushButton_W17->setDisabled(false);
        ui->pushButton_W18->setDisabled(false);
        ui->pushButton_W19->setDisabled(false);
        ui->pushButton_W20->setDisabled(false);
        ui->pushButton_W21->setDisabled(false);
        ui->pushButton_W21_1->setDisabled(false);
        ui->pushButton_W22->setDisabled(false);
        ui->pushButton_W23->setDisabled(false);
        ui->pushButton_W24->setDisabled(false);
        ui->pushButton_W25->setDisabled(false);
        ui->pushButton_W26->setDisabled(false);

        ui->label_58->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
}

bool get_WorkMode_BtnStatus(uint16_t t)
{
    if ((SysWorkMode & t) != 0)
        return true;

    return false;
}

bool GetCollectBtnStatus(uint16_t t)
{
    if ((CollectionMode & t) != 0)
        return true;

    return false;
}

bool GetCleanBtnStatus(uint16_t t)
{
    if ((CleanMode & t) != 0)
        return true;

    return false;
}

void toggle_WorkMode_BtnStatus(uint16_t t, bool toggle)
{
    if (!toggle)
        SysWorkMode &= (uint16_t)~t;
    else
        SysWorkMode |= t;
}

void ToggleCleanBtnStatus(uint16_t t, bool toggle)
{
    if (!toggle)
        CleanMode &= (uint16_t)~t;
    else
        CleanMode |= t;
}

void ToggleCollectBtnStatus(uint16_t t, bool toggle)
{
    if (!toggle)
        CollectionMode &= (uint16_t)~t;
    else
        CollectionMode |= t;
}

void MainWindow::AutoManualMeasureThreadCallback()
{
    /* 判断线程是否在运行 */
    if(!serialThread_1.isRunning()) {
        /* 开启线程 */
        serialThread_1.start();
    }

    /* 发送正在运行的信号，线程收到信号后执行后返回线程耗时函数 + 此字符串 */
    emit this->startWork();
}

void setFactorFlag()
{
    // 遍历
    QMap<QString,CDevice *>::iterator it;
    for (it=g_StrDevNodeMap.begin();it!= g_StrDevNodeMap.end() ; it++) {
        CDevice *pDevice = it.value();  //遍历
        if(pDevice)
        {
            QMap<QString,CFactor *> mapFactorList = pDevice->get_map_DevFactors();
            QMap<QString,CFactor *>::iterator itFa;
            for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {
                CFactor *pFactor = itFa.value();
                if(pFactor) pFactor->setFlag("B");
            }
        }
    }
}


void SerialWorker::doWork1() {

    /* 标志位为真 */
    isCanRun = true;

    /* 死循环 */
    while (1) {

        QMutexLocker locker(&lock);
        /* 如果标志位不为真 */
        if (!isCanRun) {
            /* 跳出循环 */
            break;
        }

        if(AutoLoopStatus)
        {
            QThread::sleep(3);
            AutoLoopStatus = false;
        }

        // --- PROCESS ---
        if (StartRunAutoMode)
        {
            QLOG_INFO() << "运行自动测量流程...";
            setFactorFlag();

            StartRunAutoMode = false;
            IsLastLoopEnd[2] = false;
            IsToMeasureFour = false;
            IsToMeasureFive = false;

            for(int i=0;i<16;i++)
            {
                PLCProcessInfo[i] = false;
            }
            TransmitFlag = BIT31; // BIT31

            StartStopRunMode(_ADDRESS_AUTO_REG_PLC_, false);// 0x05 200  ==>自动采样清洗流路1清洗启动按钮
            StartStopRunMode(_ADDRESS_AUTO_REG_PLC_, true);
            QThread::sleep(1);
        }

        ReadWaterPressure("202","1");   // PLC
        ReadWaterPressure("200","1");   // PLC
        QThread::sleep(1);
        ReadPLCValveStat("0","34");     // PLC
        QThread::sleep(1);
        if (CheckWarningSignalFromPLC())
        {
            SendMeasuredDoneSignalToPlc("11",0x0f); // 0x0b
        }

        /// BIT0:测量完成发送
        /// BIT1:流程完成发送
        /// BIT2:防止重复发送
        /// BIT10:是否采集到水
        if ((TransmitFlag & BIT31) != 0)
        {
            if (((TransmitFlag & BIT1) != 0) || (((TransmitFlag & BIT1) == 0) && ((TransmitFlag & BIT0) != 0)))
            {
                if (((TransmitFlag & BIT1) == 0) && ((TransmitFlag & BIT0) != 0))
                {
                    QLOG_INFO() << "测量完成,流程未完成...";
                    setFactorFlag();
                }

                if ((TransmitFlag & BIT10) == 0)//TransmitFlag-BIT10 是否采集到水
                {
                    QLOG_INFO() << "当前流程没有采集到原水！！！";
                    PlcRunState = "当前流程没有采集到原水！！！";
                }

                if(Lijiceliang)
                {
                    QLOG_INFO() << "立即测量 流程结束...";
                    Lijiceliang = false;    // 立即测量还原初始值
                    emit resultReady("立即测量灭灯");
                }

                emit resultReady("实时数据上传");
                emit resultReady("实时数据入库");

                TransmitFlag = 0;
            }
        }
    }
}

bool MainWindow::CommonReadRegister(QString byte_order,QString data_type, QString addr,QString func,QString regisAddr,QString mbLen,QString &pRealData) // 0x03 & 0x04
{
    uint16_t len = 0;
    if(mbLen.toUInt() == 1) len = 7;
    else if(mbLen.toUInt() == 2) len =9;

    QString pAddr = QString("%1").arg(addr.toUInt(),2,16,QChar('0'));
    QString pFunc = QString("%1").arg(func.toUInt(),2,16,QChar('0'));
    QString pRegAddr = QString("%1").arg(regisAddr.toUInt(),4,16,QChar('0'));
    QString pMbLen = QString("%1").arg(mbLen.toUInt(),4,16,QChar('0'));    //str = "0064"
    QString pCommText = pAddr + pFunc + pRegAddr + pMbLen;
    // qDebug() << "pCommText===>>" << pCommText;
    Crc16Class crc16;
    QString pTx = crc16.crcCalculation(pCommText).toUpper();
    QLOG_DEBUG() << "ReadHoldingRegisters TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');

    m_pSerialCom->flush();
    m_pSerialCom->writeData(QString2Hex(pTx).data(),8);
    char data[100];
    int64_t pRetVal = m_pSerialCom->readData(data,len);
    if(pRetVal > 0)
    {
        QString strTmp;
        for(int i = 0; i<len; i++)
        {
            strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
        }

        if(crc16.crc_Checking(strTmp))
        {
            QLOG_DEBUG() << "ReadHoldingRegisters RX: "<<strTmp;
            // parse
            QByteArray buf = QString2Hex(strTmp);

            QByteArray pFunc;
            pFunc.append(buf.at(1));
            if(pFunc.toHex().toInt() != func.toInt())
            {
                QLOG_TRACE() << "modbus 响应帧错误码：" << pFunc.toHex().toInt();
                return false;
            }

            QByteArray arr;
            if(data_type == QT_INT16)
            {
                arr[0] = buf.at(3);
                arr[1] = buf.at(4);
                pRealData = QString::number(SignedHex2Int(arr));
            }
            else if(data_type == QT_UINT16)
            {
                arr[0] = buf.at(3);
                arr[1] = buf.at(4);
                bool ok = false;
                pRealData = QString::number(arr.toHex().toUShort(&ok, 16));
            }
            else if(data_type == QT_FLOAT)
            {
                if(byte_order == "ABCD")
                {
                    arr[0] = buf.at(6);
                    arr[1] = buf.at(5);
                    arr[2] = buf.at(4);
                    arr[3] = buf.at(3);
                }
                else if(byte_order == "CDAB")
                {
                    arr[0] = buf.at(4);
                    arr[1] = buf.at(3);
                    arr[2] = buf.at(6);
                    arr[3] = buf.at(5);
                }
                else if(byte_order == "BADC")
                {
                    arr[0] = buf.at(5);
                    arr[1] = buf.at(6);
                    arr[2] = buf.at(3);
                    arr[3] = buf.at(4);
                }
                else if(byte_order == "DCBA")
                {
                    arr[0] = buf.at(3);
                    arr[1] = buf.at(4);
                    arr[2] = buf.at(5);
                    arr[3] = buf.at(6);
                }

                float acc;
                memcpy(&acc, arr.data(), 4);
                pRealData = QString::number(acc,'f',3);
            }
            else
            {
                QLOG_ERROR() << "数据类型不支持：" << data_type;
            }
            return true;
        }
        else
        {
            QLOG_DEBUG() << "ReadHoldingRegisters RX CRC check Err...";
        }
    }
    else
    {
        // timeout
        QLOG_TRACE() << "仪器设备读寄存器超时。";
        return false;
    }
    return false;
}

bool SerialWorker::CommonReadRegister(QString byte_order,QString data_type, QString addr,QString func,QString regisAddr,QString mbLen,QString &pRealData) // 0x03 & 0x04
{
    uint16_t len = 0;
    if(mbLen.toUInt() == 1) len = 7;
    else if(mbLen.toUInt() == 2) len =9;

    QString pAddr = QString("%1").arg(addr.toUInt(),2,16,QChar('0'));
    QString pFunc = QString("%1").arg(func.toUInt(),2,16,QChar('0'));
    QString pRegAddr = QString("%1").arg(regisAddr.toUInt(),4,16,QChar('0'));
    QString pMbLen = QString("%1").arg(mbLen.toUInt(),4,16,QChar('0'));    //str = "0064"
    QString pCommText = pAddr + pFunc + pRegAddr + pMbLen;
    // qDebug() << "pCommText===>>" << pCommText;
    Crc16Class crc16;
    QString pTx = crc16.crcCalculation(pCommText).toUpper();
    QLOG_DEBUG() << "ReadHoldingRegisters TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');

    serial->flush();
    serial->writeData(QString2Hex(pTx).data(),8);
    char data[100];
    int64_t pRetVal = serial->readData(data,len);
    if(pRetVal > 0)
    {
        QString strTmp;
        for(int i = 0; i<len; i++)
        {
            strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
        }

        if(crc16.crc_Checking(strTmp))
        {
            QLOG_DEBUG() << "ReadHoldingRegisters RX: "<<strTmp;
            // parse
            QByteArray buf = QString2Hex(strTmp);

            QByteArray pFunc;
            pFunc.append(buf.at(1));
            if(pFunc.toHex().toInt() != func.toInt())
            {
                QLOG_TRACE() << "modbus 响应帧错误码：" << pFunc.toHex().toInt();
                return false;
            }

            QByteArray arr;
            if(data_type == QT_INT16)
            {
                arr[0] = buf.at(3);
                arr[1] = buf.at(4);
                pRealData = QString::number(SignedHex2Int(arr));
            }
            else if(data_type == QT_UINT16)
            {
                arr[0] = buf.at(3);
                arr[1] = buf.at(4);
                bool ok = false;
                pRealData = QString::number(arr.toHex().toUShort(&ok, 16));
            }
            else if(data_type == QT_FLOAT)
            {
                if(byte_order == "ABCD")
                {
                    arr[0] = buf.at(6);
                    arr[1] = buf.at(5);
                    arr[2] = buf.at(4);
                    arr[3] = buf.at(3);
                }
                else if(byte_order == "CDAB")
                {
                    arr[0] = buf.at(4);
                    arr[1] = buf.at(3);
                    arr[2] = buf.at(6);
                    arr[3] = buf.at(5);
                }
                else if(byte_order == "BADC")
                {
                    arr[0] = buf.at(5);
                    arr[1] = buf.at(6);
                    arr[2] = buf.at(3);
                    arr[3] = buf.at(4);
                }
                else if(byte_order == "DCBA")
                {
                    arr[0] = buf.at(3);
                    arr[1] = buf.at(4);
                    arr[2] = buf.at(5);
                    arr[3] = buf.at(6);
                }

                float acc;
                memcpy(&acc, arr.data(), 4);
                pRealData = QString::number(acc,'f',3);
            }
            else
            {
                QLOG_ERROR() << "数据类型不支持：" << data_type;
            }
            return true;
        }
        else
        {
            QLOG_DEBUG() << "ReadHoldingRegisters RX CRC check Err...";
        }
    }
    else
    {
        // timeout
        QLOG_TRACE() << "仪器设备读寄存器超时。";
        return false;
    }
    return false;
}

void SerialWorker::MeasureFive()
{
    QMap<QString,CDevice *>::iterator it;
    for (it=g_StrDevNodeMap.begin();it!= g_StrDevNodeMap.end() ; it++) {
        CDevice *pDevice = it.value();  //

        if(pDevice)
        {
            QString pType = pDevice->get_type();
            if(pType.contains("五参数"))
            {
                QMap<QString,CFactor *> mapFactorList = pDevice->get_map_DevFactors();
                QMap<QString,CFactor *>::iterator itFa;
                for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {

                    CFactor *pFactor = itFa.value();
                    if(pFactor)
                    {
                        int retry = 3;
                        while(retry-- > 0)
                        {
                            // 采集
                            QString pFunc = pFactor->getFactorFunc();
                            QString pRegAddr = pFactor->getRegisAddr();
                            QString pDataType = pFactor->getDataType();
                            QString pMbLen;
                            if(pDataType == QT_INT32 || pDataType == QT_UINT32 || pDataType == QT_FLOAT)
                            {
                                pMbLen = "2";
                            }
                            else
                            {
                                pMbLen = "1";
                            }

                            QString pRealData = "";
                            if(CommonReadRegister(pFactor->getByteOrder(),pDataType,pDevice->get_address(),pFunc,pRegAddr,pMbLen,pRealData))
                            {
                                if(pRealData != "")
                                {
                                    QLOG_INFO() << "五参数-测量因子：" << pFactor->getFactorName() <<"测量值：" << pRealData;

                                    pFactor->setFactorData(pRealData);
                                    pFactor->setFlag("N");
                                    emit resultReady("测量值:" + pFactor->getFactorName() + ":" + pRealData);
                                    break;
                                }
                            }
                            QThread::sleep(1);
                        }
                    }
                }
            }
        }
    }
}


bool SerialWorker::CheckWarningSignalFromPLC()
{
    ReadPLCWarningInfo("1500","11");// PLC 0X01: 1500==>  阀/泵 故障
    QThread::sleep(1);
    ReadPLCProcessInfo("1000","16");// PLC UInt16 0x01: 1000 ReadPLCProcessInfo
    ReadPLCProgramRunStep("0","1");// PLC 0x03: 0==>>  PLC 流程 ...

    /// 手动流程1完成
    if (!IsLastLoopEnd[0] && ((PLCProcessInfo[2]) != 0))
    {
        IsLastLoopEnd[0] = true;
        TransmitFlag |= BIT0;
        toggle_WorkMode_BtnStatus(_B_MANUAL_MODE1_, false);
    }
    /// 手动流程2完成
    if (!IsLastLoopEnd[1] && ((PLCProcessInfo[3]) != 0))
    {
        IsLastLoopEnd[1] = true;
        TransmitFlag |= BIT0;
        toggle_WorkMode_BtnStatus(_B_MANUAL_MODE2_, false);
    }
    /// 自动流程完成
    if (!IsLastLoopEnd[2] && ((PLCProcessInfo[4]) != 0))
    {
        IsLastLoopEnd[2] = true;
        TransmitFlag |= BIT0;

        QLOG_INFO() << "自动测量流程完成。";
    }

    /// 全面清洗流程完成
    if (!IsCleanDone[0] && ((PLCProcessInfo[5]) != 0))
    {
        IsCleanDone[0] = true;
        ToggleCleanBtnStatus(_B_CLEAN_ALL_, false);
    }
    /// 流路1清洗完成
    if (!IsCleanDone[1] && ((PLCProcessInfo[6]) != 0))
    {
        IsCleanDone[1] = true;
        ToggleCleanBtnStatus(_B_CLEANTUBE1_STAT_, false);
    }
    /// 流路2清洗完成
    if (!IsCleanDone[2] && ((PLCProcessInfo[7]) != 0))
    {
        IsCleanDone[2] = true;
        ToggleCleanBtnStatus(_B_CLEANTUBE2_STAT_, false);
    }
    /// 清洗沉淀池完成
    if (!IsCleanDone[3] && ((PLCProcessInfo[8]) != 0))
    {
        IsCleanDone[3] = true;
        ToggleCleanBtnStatus(_B_CLEAN_SETTLING_TANK_, false);
    }
    /// 清洗测量杯完成
    if (!IsCleanDone[4] && ((PLCProcessInfo[9]) != 0))
    {
        IsCleanDone[4] = true;
        ToggleCleanBtnStatus(_B_CLEANCUP_STAT_, false);
    }
    /// 完整采水1完成
    if (!IsCollectDone[0] && ((PLCProcessInfo[10]) != 0))
    {
        IsCollectDone[0] = true;
        ToggleCollectBtnStatus(_S_COMPLETECOLLECT1_STAT_, false);
    }
    /// 完整采水2完成
    if (!IsCollectDone[1] && ((PLCProcessInfo[11]) != 0))
    {
        IsCollectDone[1] = true;
        ToggleCollectBtnStatus(_S_COMPLETECOLLECT2_STAT_, false);
    }
    /// 沉淀池采水1完成
    if (!IsCollectDone[2] && ((PLCProcessInfo[12]) != 0))
    {
        IsCollectDone[2] = true;
        ToggleCollectBtnStatus(_S_TANKCOLLECT1_STAT_, false);
    }
    /// 沉淀池采水2完成
    if (!IsCollectDone[3] && ((PLCProcessInfo[13]) != 0))
    {
        IsCollectDone[3] = true;
        ToggleCollectBtnStatus(_S_TANKCOLLECT2_STAT_, false);
    }
    /// 测量杯采水完成
    if (!IsCollectDone[4] && ((PLCProcessInfo[14]) != 0))
    {
        IsCollectDone[4] = true;
        ToggleCollectBtnStatus(_S_CUPCOLLECT_STAT_, false);
    }


    /// 测量多参数
    if (!IsToMeasureFive && ((PLCProcessInfo[0]) != 0))
    {
        uint64_t p_linux_ts = QDateTime::currentMSecsSinceEpoch() / 1000; // 秒级时间戳
        if((p_linux_ts - g_linux_ts) > (5*60))
        {
            string pMinTimeEnd = current_minutetime();
            QLOG_INFO() << "开始读取五参数据...";

            IsToMeasureFive = true;
            IsEnableSkipSetting = true;
            MeasureFive();
            TransmitFlag |= BIT10;
        }
    }

    QLOG_TRACE() <<"Step*************>> " << QString::number(Step,2).rightJustified(32,'0').insert(16,' ');

    ///测量高指、氨氮、总磷
    if (!IsToMeasureFour && ((PLCProcessInfo[1]) != 0))
    {
        QLOG_INFO() << "开始测量高指、氨氮、总磷...";
        IsEnableSkipSetting = false;
        IsToMeasureFour = true;
        Step = BIT31;       // 开始测量

        QMap<QString,CDevice *>::iterator it0;
        for (it0=g_StrDevNodeMap.begin();it0!= g_StrDevNodeMap.end() ; it0++) {
            CDevice *pDevice0 = it0.value();  //
            if(pDevice0)
            {
                pDevice0->set_timeout(0);
                pDevice0->set_processStatus(0);

                pDevice0->setBit0(false);
                pDevice0->setBit1(false);
                pDevice0->setBit2(false);
                pDevice0->setBit10(false);
                pDevice0->setBit20(false);
                pDevice0->setBit23(false);

                pDevice0->setBitCommErr(false);
            }
        }
    }

    if ((Step & BIT31) != 0)
    {
        QMap<QString,CDevice *>::iterator it;
        for (it=g_StrDevNodeMap.begin();it!= g_StrDevNodeMap.end() ; it++) {
            CDevice *pDevice = it.value();  //
            if(pDevice)
            {
                pDevice->set_timeout(pDevice->get_timeout()+1);

                QString pType = pDevice->get_type();
                if(!pType.contains("五参数"))
                {
                    QStringList pItemList;
                    if(pType.contains("-"))
                    {
                        pItemList = pType.split("-");
                        if(pItemList.size() >= 2)
                        {
                            ReadDeviceStatusInfo(pDevice);
                            uint16_t process_status = pDevice->get_processStatus();
                            QLOG_DEBUG() << "当前通信设备：" << pType <<  "设备ID:" << pDevice->get_name() <<"状态值：" << process_status;

                            // STEP1-2 等待高指进入测量流程
                            if(pDevice->getBit0() && !pDevice->getBit1() && !pDevice->getBit2())
                            {
                                if(pItemList[0] == "高指" && pItemList[1] == "云璟")
                                {
                                    if ((process_status == 0x0008) || (process_status == 0x0004))
                                    {
                                        QLOG_INFO() << "高指-云璟 进入测量流程。";
                                        pDevice->setBit2(true);
                                    }
                                }
                                else if(pItemList[0] == "高指" && pItemList[1] == "聚光")
                                {
                                    if (process_status == 0x0001)
                                    {
                                        QLOG_INFO() << "高指-聚光 进入测量流程。";
                                        pDevice->setBit2(true);
                                    }
                                }
                                else if(pItemList[0] == "高指" && pItemList[1] == "鸿恺")
                                {
                                    if (process_status == 0x0001)   // ??
                                    {
                                        QLOG_INFO() << "高指-鸿恺 进入测量流程。";
                                        pDevice->setBit2(true);
                                    }
                                }
                                else if(pItemList[0] == "总磷" && pItemList[1] == "蓝创")
                                {
                                    if ((process_status == 0x0008) || (process_status == 0x0004))
                                    {
                                        QLOG_INFO() << "总磷-蓝创 进入测量流程。";
                                        pDevice->setBit2(true);
                                    }
                                }
                                else if(pItemList[0] == "氨氮" && pItemList[1] == "蓝创")
                                {
                                    if ((process_status == 0x0008) || (process_status == 0x0004))
                                    {
                                        QLOG_INFO() << "氨氮-蓝创 进入测量流程。";
                                        pDevice->setBit2(true);
                                    }
                                }
                                else if(pItemList[0] == "总磷" && pItemList[1] == "蓝创A")
                                {
                                    if ((process_status == 0x0008) || (process_status == 0x0004)) // ??
                                    {
                                        QLOG_INFO() << "总磷-蓝创A 进入测量流程。";
                                        pDevice->setBit2(true);
                                    }
                                }
                                else if(pItemList[0] == "氨氮" && pItemList[1] == "蓝创A")
                                {
                                    if ((process_status == 0x0008) || (process_status == 0x0004)) // ??
                                    {
                                        QLOG_INFO() << "氨氮-蓝创A 进入测量流程。";
                                        pDevice->setBit2(true);
                                    }
                                }
                            }

                            // STEP1-3 高指进入测量流程
                            if(pDevice->getBit2() && !pDevice->getBit1() && !pDevice->getBit10())
                            {

                                if(pItemList[0] == "高指" && pItemList[1] == "云璟")
                                {
                                    if (process_status == 0x0004)
                                    {
                                        QLOG_INFO() << "高指-云璟 测量中。";
                                        pDevice->setBit10(true);
                                    }
                                }
                                else if(pItemList[0] == "高指" && pItemList[1] == "聚光")
                                {
                                    QLOG_INFO() << "高指-聚光 测量中。";
                                    pDevice->setBit10(true);
                                }
                                else if(pItemList[0] == "高指" && pItemList[1] == "鸿恺")
                                {
                                    if (process_status == 0x0004)   // ??
                                    {
                                        QLOG_INFO() << "高指-鸿恺 测量中。";
                                        pDevice->setBit10(true);
                                    }
                                }
                                else if(pItemList[0] == "总磷" && pItemList[1] == "蓝创"){
                                    if (process_status == 0x0004)
                                    {
                                        QLOG_INFO() << "总磷-蓝创 测量中。";
                                        pDevice->setBit10(true);
                                    }
                                }
                                else if(pItemList[0] == "氨氮" && pItemList[1] == "蓝创")
                                {
                                    if (process_status == 0x0004)
                                    {
                                        QLOG_INFO() << "氨氮-蓝创 测量中。";
                                        pDevice->setBit10(true);
                                    }
                                }
                                else if(pItemList[0] == "总磷" && pItemList[1] == "蓝创A"){
                                    if (process_status == 0x0004)   // ??
                                    {
                                        QLOG_INFO() << "总磷-蓝创A 测量中。";
                                        pDevice->setBit10(true);
                                    }
                                }
                                else if(pItemList[0] == "氨氮" && pItemList[1] == "蓝创A")
                                {
                                    if (process_status == 0x0004) // ??
                                    {
                                        QLOG_INFO() << "氨氮-蓝创A 测量中。";
                                        pDevice->setBit10(true);
                                    }
                                }
                            }

                            // STEP1-4 等待高指测量完成读取测量数据，或读取报警信息
                            if(pDevice->getBit10() && !pDevice->getBit20() && !pDevice->getBit1())
                            {
                                if(pItemList[0] == "高指" && pItemList[1] == "云璟")
                                {
                                    if (process_status == 0x0001)
                                    {
                                        pDevice->setBit20(true);
                                        QLOG_INFO() << "高指-云璟 读取测量值。";
                                        if(ReadMeasureResult(pDevice))pDevice->setBit23(true);
                                    }
                                    if (process_status == 0x0006)
                                    {
                                        QLOG_INFO() << "高指-云璟 读取错误码。";
                                        ReadDeviceWarningInfo(pDevice);
                                        pDevice->setBit1(true);
                                    }
                                }
                                else if(pItemList[0] == "高指" && pItemList[1] == "聚光")
                                {
                                    if (process_status == 0x0000)
                                    {
                                        pDevice->setBit20(true);
                                        QLOG_INFO() << "高指-聚光 读取测量值。";
                                        if(ReadMeasureResult(pDevice))pDevice->setBit23(true);
                                    }
                                    if (process_status == 0x0006)
                                    {
                                        QLOG_INFO() << "高指-聚光 读取错误码。";
                                        ReadDeviceWarningInfo(pDevice);
                                        pDevice->setBit1(true);
                                    }
                                }
                                else if(pItemList[0] == "高指" && pItemList[1] == "鸿恺")
                                {
                                    if (process_status == 0x0000)   // ??
                                    {
                                        pDevice->setBit20(true);
                                        QLOG_INFO() << "高指-鸿恺 读取测量值。";
                                        if(ReadMeasureResult(pDevice))pDevice->setBit23(true);
                                    }
                                    if (process_status == 0x0006)  // ??
                                    {
                                        QLOG_INFO() << "高指-鸿恺 读取错误码。";
                                        ReadDeviceWarningInfo(pDevice);
                                        pDevice->setBit1(true);
                                    }
                                }
                                else if(pItemList[0] == "总磷" && pItemList[1] == "蓝创")
                                {
                                    if (process_status == 0x0001)
                                    {
                                        QLOG_INFO() << "总磷-蓝创 读取测量值。";
                                        pDevice->setBit20(true);
                                        if(ReadMeasureResult(pDevice))pDevice->setBit23(true);
                                    }
                                    if (process_status == 0x0006)
                                    {
                                        QLOG_INFO() << "总磷-蓝创 读取错误码。";
                                        ReadDeviceWarningInfo(pDevice);
                                        pDevice->setBit1(true);
                                    }
                                }
                                else if(pItemList[0] == "氨氮" && pItemList[1] == "蓝创")
                                {
                                    if (process_status == 0x0001)
                                    {
                                        QLOG_INFO() << "氨氮-蓝创 读取测量值。";
                                        pDevice->setBit20(true);
                                        if(ReadMeasureResult(pDevice))pDevice->setBit23(true);
                                    }
                                    if (process_status == 0x0006)
                                    {
                                        QLOG_INFO() << "氨氮-蓝创 读取错误码。";
                                        ReadDeviceWarningInfo(pDevice);
                                        pDevice->setBit1(true);
                                    }
                                }
                                else if(pItemList[0] == "总磷" && pItemList[1] == "蓝创A")
                                {
                                    if (process_status == 0x0001||process_status == 0)   // ??
                                    {
                                        QLOG_INFO() << "总磷-蓝创A 读取测量值。";
                                        pDevice->setBit20(true);
                                        if(ReadMeasureResult(pDevice))pDevice->setBit23(true);
                                    }
                                    if (process_status == 0x0006)   // ??
                                    {
                                        QLOG_INFO() << "总磷-蓝创A 读取错误码。";
                                        ReadDeviceWarningInfo(pDevice);
                                        pDevice->setBit1(true);
                                    }
                                }
                                else if(pItemList[0] == "氨氮" && pItemList[1] == "蓝创A")
                                {
                                    if (process_status == 0x0001||process_status == 0)   // ??
                                    {
                                        QLOG_INFO() << "氨氮-蓝创A 读取测量值。";
                                        pDevice->setBit20(true);
                                        if(ReadMeasureResult(pDevice))pDevice->setBit23(true);
                                    }
                                    if (process_status == 0x0006)      // ??
                                    {
                                        QLOG_INFO() << "氨氮-蓝创A 读取错误码。";
                                        ReadDeviceWarningInfo(pDevice);
                                        pDevice->setBit1(true);
                                    }
                                }
                            }

                            // STEP1-5 等待高指测量流程完成
                            if(pDevice->getBit20())
                            {
                                // 测量因子 置 flag 'N'
                                CFactor* pFactor = get_CFactor(pDevice,"0");
                                if(pFactor)
                                {
                                    pFactor->setFlag("N");
                                }else{
                                    QLOG_ERROR() << "设备ID:" << pDevice->get_name() << "未查询测量因子";
                                }
                            }
                            else
                            {
                                if(pDevice->getBit1())
                                {
                                    if(!pDevice->getBitCommErr())   // 单次流程 只播报一次 未读取到数据
                                    {
                                        QLOG_INFO() << "当前通信设备：," << pType <<  "设备ID:" << pDevice->get_name() <<"未读取到数据";
                                        pDevice->setBit23(true);
                                        // 测量因子 置 flag 'B'
                                        CFactor* pFactor = get_CFactor(pDevice,"0");
                                        if(pFactor)
                                        {
                                            pFactor->setFlag("B");
                                        }else{
                                            QLOG_ERROR() << "设备ID:" << pDevice->get_name() << "未查询测量因子";
                                        }

                                        pDevice->setBitCommErr(true);
                                    }
                                }
                            }

                            // STEP1-1 发送高指测量命令
                            if(!pDevice->getBit0() && !pDevice->getBit1())
                            {
                                if(pItemList[0] == "高指" && pItemList[1] == "云璟")
                                {
                                    if ((process_status == 0x0001) || (process_status == 0x0006))
                                    {

                                        QLOG_INFO() << "高指-云璟 发送开始测量命令";
                                        if(SendMeasureCmd(pDevice->get_address(),pType))
                                        {
                                            pDevice->setBit0(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                    else
                                    {
                                        if (pDevice->get_timeout() > 3)
                                        {
                                            QLOG_INFO() << "高指-云璟 未能发送开始测量命令";
                                            pDevice->setBit1(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                }
                                else if(pItemList[0] == "高指" && pItemList[1] == "聚光")
                                {
                                    if (process_status == 0x0000)
                                    {

                                        QLOG_INFO() << "高指-聚光 发送开始测量命令";
                                        if(SendMeasureCmd(pDevice->get_address(),pType))   //0x10 // 0001 0200 0100
                                        {
                                            pDevice->setBit0(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                    else
                                    {
                                        if (pDevice->get_timeout() > 3)
                                        {
                                            QLOG_INFO() << "高指-聚光 未能发送开始测量命令";
                                            pDevice->setBit1(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                }
                                else if(pItemList[0] == "高指" && pItemList[1] == "鸿恺")
                                {
                                    if (process_status == 0x0000)   // ??
                                    {

                                        QLOG_INFO() << "高指-鸿恺 发送开始测量命令";
                                        if(SendMeasureCmd(pDevice->get_address(),pType))   //0x10 // 0001 0200 0100
                                        {
                                            pDevice->setBit0(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                    else
                                    {
                                        if (pDevice->get_timeout() > 3)
                                        {
                                            QLOG_INFO() << "高指-鸿恺 未能发送开始测量命令";
                                            pDevice->setBit1(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                }
                                else if(pItemList[0] == "总磷" && pItemList[1] == "蓝创")
                                {
                                    if ((process_status == 0x0001) || (process_status == 0x0006))
                                    {

                                        QLOG_INFO() << "总磷-蓝创 发送开始测量命令";
                                        if(SendMeasureCmd(pDevice->get_address(),pType))
                                        {
                                            pDevice->setBit0(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                    else
                                    {
                                        if (pDevice->get_timeout() > 3)
                                        {
                                            QLOG_INFO() << "总磷-蓝创 未能发送开始测量命令";
                                            pDevice->setBit1(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                }
                                else if(pItemList[0] == "氨氮" && pItemList[1] == "蓝创")
                                {
                                    if ((process_status == 0x0001) || (process_status == 0x0006))
                                    {

                                        QLOG_INFO() << "氨氮-蓝创 发送开始测量命令";
                                        if(SendMeasureCmd(pDevice->get_address(),pType))
                                        {
                                            pDevice->setBit0(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                    else
                                    {
                                        if (pDevice->get_timeout() > 3)
                                        {
                                            QLOG_INFO() << "氨氮-蓝创 未能发送开始测量命令";
                                            pDevice->setBit1(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                }
                                else if(pItemList[0] == "总磷" && pItemList[1] == "蓝创A")
                                {
                                    if ((process_status == 0x0001) || (process_status == 0x0000)) // ??
                                    {

                                        QLOG_INFO() << "总磷-蓝创A 发送开始测量命令";
                                        if(SendMeasureCmd(pDevice->get_address(),pType))
                                        {
                                            pDevice->setBit0(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                    else
                                    {
                                        if (pDevice->get_timeout() > 3)
                                        {
                                            QLOG_INFO() << "总磷-蓝创A 未能发送开始测量命令";
                                            pDevice->setBit1(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                }
                                else if(pItemList[0] == "氨氮" && pItemList[1] == "蓝创A")
                                {
                                    if ((process_status == 0x0001) || (process_status == 0x0000))  // ??
                                    {

                                        QLOG_INFO() << "氨氮-蓝创A 发送开始测量命令";
                                        if(SendMeasureCmd(pDevice->get_address(),pType))
                                        {
                                            pDevice->setBit0(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                    else
                                    {
                                        if (pDevice->get_timeout() > 3)
                                        {
                                            QLOG_INFO() << "氨氮-蓝创A 未能发送开始测量命令";
                                            pDevice->setBit1(true);
                                            pDevice->set_timeout(0);
                                        }
                                    }
                                }
                            }

                            // 超时
                            if (pDevice->get_timeout() >= TimeoutThreshhold)
                            {
                                QLOG_INFO() << "当前通信设备：," << pType <<  "设备ID:" << pDevice->get_name() << "测量超时。";
                                pDevice->set_timeout(0);
                                pDevice->setBit20(false);
                                pDevice->setBit1(true);
                            }
                        }
                    }
                }
            }
        }

        bool IsProcessCompleted = true;
        QMap<QString,CDevice *>::iterator it1;
        for (it1=g_StrDevNodeMap.begin();it1!= g_StrDevNodeMap.end() ; it1++) {
            CDevice *pDevice1 = it1.value();  //
            if(pDevice1)
            {
                QString type = pDevice1->get_type();
                if(!type.contains("五参数"))
                {
                    if(!pDevice1->getBit23())
                    {
                        IsProcessCompleted = false;
                    }
                }
            }
        }

        // 高指、总磷、氨氮测量完成，退出
        if(IsProcessCompleted)
        {
            QLOG_INFO() << "非五参仪器测量完成，退出";
            Step = 0;
            return true;    // 唯一 流程完成 返回
        }
    }
    return false;
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

//字符串转Hex(QByteArray)类型
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

void MainWindow::on_pushButton_8_clicked()      // 自来水压力延时检测时间——HD0 read
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41088", 7, "1"); // addr + func + len + regs*2 + CRC*2
}

void MainWindow::on_pushButton_9_clicked()      // 自来水压力延时检测时间——HD0 write
{
    if(ui->lineEdit_8->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_8->text();

    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41088", "6", QString::number(pVal * 10));
}

void MainWindow::on_pushButton_clicked()
{
    QString ptempAddr = ui->lineEditPLCAddr->text();
    if(ptempAddr.toUInt() && ptempAddr.toUInt()<255)
    {
        AddressDevPLC = QString::number(ptempAddr.toUInt());

        // 保存到 system.json
        // PLC_ADDR
        CJsonFile jsonfile;
        if(jsonfile.update_KeyValue(AddressDevPLC,PLC_ADDR))
            QMessageBox::about(NULL, "提示", "<font color='black'>保存PLC地址成功！</font>");
    }else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>PLC地址有误！</font>");
    }

}

void MainWindow::on_pushButton_7_clicked()
{
    ui->lineEditPLCAddr->setText(AddressDevPLC);
}

void MainWindow::on_pushButton_10_clicked() // 自来水压力阈值——HD100 R
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41188", 7, "1");
}

void MainWindow::on_pushButton_11_clicked() // 自来水压力阈值——HD100 W
{
    if(ui->lineEdit_9->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_9->text();
    WriteSingleRegister(AddressDevPLC, "41188", "6", pSetVal);
}

void MainWindow::on_pushButton_13_clicked() // 自来水阀延时关闭时长——HD1 0x03 41089 1 R
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41089", 7, "1");
}

void MainWindow::on_pushButton_12_clicked() // 自来水阀延时关闭时长——HD1 0x03 41089 1 W
{
    if(ui->lineEdit_10->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_10->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41089", "6", QString::number(pVal * 10));
}

void MainWindow::on_pushButton_14_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41090", 7, "1");
}

void MainWindow::on_pushButton_15_clicked()
{
    if(ui->lineEdit_11->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_11->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41090", "6", QString::number(pVal * 10));
}

void MainWindow::on_pushButton_16_clicked() // 泵1延时打开时长——HD3 R
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41091", 7, "1");
}

void MainWindow::on_pushButton_17_clicked() //泵1延时打开时长——HD3 W
{
    if(ui->lineEdit_12->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_12->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41091", "6", QString::number(pVal * 10));
}

void MainWindow::on_pushButton_2_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41092", 7, "1");
}

void MainWindow::on_pushButton_6_clicked()
{
    if(ui->lineEdit_13->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_13->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41092", "6", QString::number(pVal * 10));
}

void MainWindow::on_pushButton_18_clicked() // 水样压力阈值——HD101 41189 R
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41189", 7, "1");
}

void MainWindow::on_pushButton_19_clicked() // 水样压力阈值——HD101 41189 W
{
    if(ui->lineEdit_14->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_14->text();

    if(pSetVal.toUInt() > 0xffff)
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值有误！</font>");
        return;
    }
    WriteSingleRegister(AddressDevPLC, "41189", "6", pSetVal);
}


void MainWindow::on_pushButton_20_clicked() // //  41093, 1, "03 Read Holding Registers"); 阀6开启时长——HD5 R
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41093", 7, "1");
}

void MainWindow::on_pushButton_21_clicked() //  41093, 1, "03 Read Holding Registers"); 阀6开启时长——HD5 W
{
    if(ui->lineEdit_15->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_15->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41093", "6", QString::number(pVal * 10));
}

// 41124 沉淀池最大进水时长——HD200
void MainWindow::on_pushButton_22_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41124", 7, "1");
}

void MainWindow::on_pushButton_23_clicked()
{
    if(ui->lineEdit_16->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_16->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41124", "6", QString::number(pVal * 10));
}

void MainWindow::on_pushButton_24_clicked() // 41094 水样沉淀时间——HD6 R
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41094", 7, "1");
}

void MainWindow::on_pushButton_25_clicked() // 41094 水样沉淀时间——HD6 W
{
    if(ui->lineEdit_17->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_17->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41094", "6", QString::number(pVal * 10 * 60));
}

// 41095 采样泵打开延时时长——HD7
void MainWindow::on_pushButton_26_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41095", 7, "1");
}

void MainWindow::on_pushButton_27_clicked()
{
    if(ui->lineEdit_18->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_18->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41095", "6", QString::number(pVal * 10));
}

// 41096 阀8开启时长——HD8
void MainWindow::on_pushButton_28_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41096", 7, "1");
}

void MainWindow::on_pushButton_29_clicked()
{
    if(ui->lineEdit_19->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_19->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41096", "6", QString::number(pVal * 10));
}

// 41125 测量杯最大进水时长
void MainWindow::on_pushButton_30_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41125", 7, "1");
}

void MainWindow::on_pushButton_31_clicked()
{
    if(ui->lineEdit_20->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_20->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41125", "6", QString::number(pVal * 10));
}

// 阀3开启时长 41098  lineEdit_23
void MainWindow::on_pushButton_36_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41098", 7, "1");
}

void MainWindow::on_pushButton_37_clicked()
{
    if(ui->lineEdit_23->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_23->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41098", "6", QString::number(pVal * 10));
}


// 阀5延时打开时长——HD11 41099  lineEdit_24
void MainWindow::on_pushButton_38_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41099", 7, "1");
}

void MainWindow::on_pushButton_39_clicked()
{
    if(ui->lineEdit_24->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_24->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41099", "6", QString::number(pVal * 10));
}

// 进气阀开启时长——HD12 41100 lineEdit_25
void MainWindow::on_pushButton_40_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41100", 7, "1");
}

void MainWindow::on_pushButton_41_clicked()
{
    if(ui->lineEdit_25->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_25->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41100", "6", QString::number(pVal * 10));
}

// 阀6延时关闭时长——HD13 41101 lineEdit_26
void MainWindow::on_pushButton_42_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41101", 7, "1");
}

void MainWindow::on_pushButton_43_clicked()
{
    if(ui->lineEdit_26->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_26->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41101", "6", QString::number(pVal * 10));
}

// 阀7延时打开时长——HD14  41102 lineEdit_27
void MainWindow::on_pushButton_44_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41102", 7, "1");
}

void MainWindow::on_pushButton_45_clicked()
{
    if(ui->lineEdit_27->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_27->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41102", "6", QString::number(pVal * 10));
}

// 气阀延时关闭时长——HD15 41103 lineEdit_28
void MainWindow::on_pushButton_46_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41103", 7, "1");
}

void MainWindow::on_pushButton_47_clicked()
{
    if(ui->lineEdit_28->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_28->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41103", "6", QString::number(pVal * 10));
}

// 阀8延时关闭时长——HD16 41104 lineEdit_29
void MainWindow::on_pushButton_48_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41104", 7, "1");
}

void MainWindow::on_pushButton_49_clicked()
{
    if(ui->lineEdit_29->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_29->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41104", "6", QString::number(pVal * 10));
}

// 阀7延时关闭时间——HD17 41105 lineEdit_30
void MainWindow::on_pushButton_51_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41105", 7, "1");
}

void MainWindow::on_pushButton_50_clicked()
{
    if(ui->lineEdit_30->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值不能为空！</font>");
        return;
    }
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_30->text();
    uint16_t pVal = pSetVal.toUInt();
    WriteSingleRegister(AddressDevPLC, "41105", "6", QString::number(pVal * 10));
}

void MainWindow::initLogger()
{
    // 1. 启动日志记录机制
    logger = &Logger::instance();

    logger->setLoggingLevel(QsLogging::InfoLevel);
    //设置log位置为exe所在目录
    const QString sLogPath(QDir(QCoreApplication::applicationDirPath()).filePath("log.txt"));

    // 2. 添加两个destination
    DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
      sLogPath, EnableLogRotation, MaxSizeBytes(10485760), MaxOldLogCount(0))); // 10Mb
    DestinationPtr debugDestination(DestinationFactory::MakeDebugOutputDestination());
    //DestinationPtr functorDestination(DestinationFactory::MakeFunctorDestination(&logFunction));

    //这样和槽函数连接
    DestinationPtr sigsSlotDestination(DestinationFactory::MakeFunctorDestination(this, SLOT(logSlot(QString,int))));

    logger->addDestination(debugDestination);
    logger->addDestination(fileDestination);
    // logger->addDestination(functorDestination);
    logger->addDestination(sigsSlotDestination);
}

//析构
void MainWindow::destroyLogger()
{
    QsLogging::Logger::destroyInstance();
}

void MainWindow::logSlot(const QString &message, int level)
{
    if(level){}

    if(message.contains("ERROR")){
        const QString ss="<font color=\"#FF0000\">";
        ui->textBrowserLog->append(ss + qUtf8Printable(message) + "</font> ");//显示红色的字体
    }
    else if(message.contains("INFO")){
        const QString ss="<font color=\"#0000FF\">";
        ui->textBrowserLog->append(ss + qUtf8Printable(message) + "</font> ");//显示绿色色的字体
    }else if(message.contains("DEBUG")){
        ui->textBrowserLog->append(qUtf8Printable(message));
    }
}


void MainWindow::on_pushButton_62_clicked()
{
    if(get_WorkMode_BtnStatus(_B_DEBUG_MODE_))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>当前系统处于维护模式，请先退出！</font>");
        return;
    }

    QString pTips = "";
    if(get_WorkMode_BtnStatus(_B_AUTO_MODE_))
    {
        pTips = "是否退出自动模式?";
    }
    else
    {
        pTips = "是否进入自动模式?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    if(get_WorkMode_BtnStatus(_B_AUTO_MODE_))
    {
        toggle_WorkMode_BtnStatus(_B_AUTO_MODE_,false);
        m_AutoManulModeTimer->stop();

        ui->pushButton_W3_2->setDisabled(true);
    }
    else
    {
        toggle_WorkMode_BtnStatus(_B_AUTO_MODE_,true);

        IsToRunManulaMode = 0;
        Reset();
        IsLastLoopEnd[2] = true;
        StartRunAutoMode = false;
        m_AutoManulModeTimer->start(1000);

        ui->pushButton_W3_2->setDisabled(false);
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    QMessageBox::about(NULL, "提示", "<font color='black'>停机模式未投入使用！</font>");
}

void MainWindow::on_pushButton_54_clicked()
{
    QMessageBox::about(NULL, "提示", "<font color='black'>待机模式未投入使用！</font>");
}

void MainWindow::Reset()
{
    IsToMeasureFive = false;
    IsToMeasureFour = false;
    IsLastLoopEnd[0] = IsLastLoopEnd[1] = IsLastLoopEnd[2] = false;
    StartRunAutoMode = true;
}

bool MainWindow::ManualMode1(bool stat)
{
    if (stat)
    {
        IsToRunManulaMode = 1;
        TransmitFlag = BIT31;
        Reset();

        m_AutoManulModeTimer->stop();
        m_AutoManulModeTimer->start(1000);

        g_DateTime = current_datetime();
    }
    return true;
}

bool MainWindow::ManualMode2(bool stat)
{
    if (stat)
    {
        IsToRunManulaMode = 2;
        TransmitFlag = BIT31;
        Reset();

        m_AutoManulModeTimer->stop();
        m_AutoManulModeTimer->start(1000);
        g_DateTime = current_datetime();
    }
    return true;
}

void MainWindow::on_pushButton_W1_clicked()
{

    QString pTips = "";
    if(get_WorkMode_BtnStatus(_B_MANUAL_MODE1_))
    {
        pTips = "是否退出手动模式1?";
    }
    else
    {
        pTips = "是否进入手动模式1?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsLastLoopEnd[0] = true;

    if(get_WorkMode_BtnStatus(_B_MANUAL_MODE1_))
    {
        toggle_WorkMode_BtnStatus(_B_MANUAL_MODE1_,false);
        m_AutoManulModeTimer->stop();
    }
    else
    {
        toggle_WorkMode_BtnStatus(_B_MANUAL_MODE1_,true);

        ManualMode1(true);
        IsLastLoopEnd[0] = false;
        // 测量因子 flag  ==> 'B'
        factor_flag_init();
    }

}

void MainWindow::on_pushButton_W2_clicked()
{
    QString pTips = "";
    if(get_WorkMode_BtnStatus(_B_MANUAL_MODE2_))
    {
        pTips = "是否退出手动模式2?";
    }
    else
    {
        pTips = "是否进入手动模式2?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsLastLoopEnd[1] = true;

    if(get_WorkMode_BtnStatus(_B_MANUAL_MODE2_))
    {
        toggle_WorkMode_BtnStatus(_B_MANUAL_MODE2_,false);
        m_AutoManulModeTimer->stop();
    }
    else
    {
        toggle_WorkMode_BtnStatus(_B_MANUAL_MODE2_,true);

        ManualMode2(true);
        IsLastLoopEnd[1] = false;
        // 测量因子 flag  ==> 'B'
        factor_flag_init();
    }
}

void MainWindow::on_pushButton_W3_clicked()
{
    WriteSingleCoil(AddressDevPLC, "1100", "5", QString::number(0xff00));

    ui->pushButton_W3->setDisabled(true);
    IsEnableSkipSetting = false;
}

// 清洗-6
void MainWindow::on_pushButton_W4_clicked()
{
    QString pTips = "";
    if(!GetCleanBtnStatus(_B_CLEANTUBE1_STAT_))
    {
        pTips = "是否开始清洗外管路1?";
    }
    else
    {
        pTips = "是否结束清洗外管路1?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    /// 不允许获取流程状态
    IsCleanDone[4] = true;

    if(GetCleanBtnStatus(_B_CLEANTUBE1_STAT_))
    {
        ToggleCleanBtnStatus(_B_CLEANTUBE1_STAT_,false);
    }
    else
    {
        ToggleCleanBtnStatus(_B_CLEANTUBE1_STAT_,true);

        WriteSingleCoil(AddressDevPLC, "140", "5", QString::number(0x0000));
        WriteSingleCoil(AddressDevPLC, "140", "5", QString::number(0xff00));
        // FIXME： 结果弹出显示到界面？

        /// 流程执行后监测流程状态
        IsCleanDone[1] = false;
    }
}

void MainWindow::on_pushButton_W5_clicked()
{

    QString pTips = "";
    if(!GetCleanBtnStatus(_B_CLEANTUBE2_STAT_))
    {
        pTips = "是否开始清洗外管路2?";
    }
    else
    {
        pTips = "是否结束清洗外管路2?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsCleanDone[2] = true;

    if(GetCleanBtnStatus(_B_CLEANTUBE2_STAT_))
    {
        ToggleCleanBtnStatus(_B_CLEANTUBE2_STAT_,false);
    }
    else
    {
        ToggleCleanBtnStatus(_B_CLEANTUBE2_STAT_,true);

        WriteSingleCoil(AddressDevPLC, "141", "5", QString::number(0x0000));
        WriteSingleCoil(AddressDevPLC, "141", "5", QString::number(0xff00));
        // FIXME： 结果弹出显示到界面？

        IsCleanDone[2] = false;
    }
}

void MainWindow::on_pushButton_W6_clicked()
{

    QString pTips = "";
    if(!GetCleanBtnStatus(_B_CLEANCUP_STAT_))
    {
        pTips = "是否开始清洗测量杯?";
    }
    else
    {
        pTips = "是否结束清洗测量杯?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsCleanDone[4] = true;

    if(GetCleanBtnStatus(_B_CLEANCUP_STAT_))
    {
        ToggleCleanBtnStatus(_B_CLEANCUP_STAT_,false);
    }
    else
    {
        ToggleCleanBtnStatus(_B_CLEANCUP_STAT_,true);

        WriteSingleCoil(AddressDevPLC, "151", "5", QString::number(0x0000));
        WriteSingleCoil(AddressDevPLC, "151", "5", QString::number(0xff00));
        // FIXME： 结果弹出显示到界面？

        IsCleanDone[4] = false;
    }

}

void MainWindow::on_pushButton_W7_clicked()
{
    QString pTips = "";
    if(!GetCleanBtnStatus(_B_CLEAN_SETTLING_TANK_))
    {
        pTips = "是否开始清洗沉淀池?";
    }
    else
    {
        pTips = "是否结束清洗沉淀池?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsCleanDone[3] = true;

    if(GetCleanBtnStatus(_B_CLEAN_SETTLING_TANK_))
    {
        ToggleCleanBtnStatus(_B_CLEAN_SETTLING_TANK_,false);
    }
    else
    {
        ToggleCleanBtnStatus(_B_CLEAN_SETTLING_TANK_,true);

        WriteSingleCoil(AddressDevPLC, "150", "5", QString::number(0x0000));
        WriteSingleCoil(AddressDevPLC, "150", "5", QString::number(0xff00));
        // FIXME： 结果弹出显示到界面？

        IsCleanDone[3] = false;
    }
}

void MainWindow::on_pushButton_W8_clicked()
{
    QMessageBox::about(NULL, "提示", "<font color='black'>除藻功能未投入使用！</font>");
}

void MainWindow::on_pushButton_W9_clicked()
{
    QString pTips = "";
    if(!GetCleanBtnStatus(_B_CLEAN_ALL_))
    {
        pTips = "是否开始全面清洗?";
    }
    else
    {
        pTips = "是否结束全面清洗?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsCleanDone[0] = true;

    if(GetCleanBtnStatus(_B_CLEAN_ALL_))
    {
        ToggleCleanBtnStatus(_B_CLEAN_ALL_,false);
    }
    else
    {
        ToggleCleanBtnStatus(_B_CLEAN_ALL_,true);

        WriteSingleCoil(AddressDevPLC, "130", "5", QString::number(0x0000));
        WriteSingleCoil(AddressDevPLC, "130", "5", QString::number(0xff00));
        // FIXME： 结果弹出显示到界面？

        IsCleanDone[0] = false;
    }
}

// 采水-5
void MainWindow::on_pushButton_W10_clicked()
{

    QString pTips = "";
    if(!GetCollectBtnStatus(_S_COMPLETECOLLECT1_STAT_))
    {
        pTips = "是否开始完整采水1?";
    }
    else
    {
        pTips = "是否结束完整采水1?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsCollectDone[0] = true;

    if(GetCollectBtnStatus(_S_COMPLETECOLLECT1_STAT_))
    {
        ToggleCollectBtnStatus(_S_COMPLETECOLLECT1_STAT_,false);
    }
    else
    {
        ToggleCollectBtnStatus(_S_COMPLETECOLLECT1_STAT_,true);

        WriteSingleCoil(AddressDevPLC, "152", "5", QString::number(0x0000));
        WriteSingleCoil(AddressDevPLC, "152", "5", QString::number(0xff00));
        // FIXME： 结果弹出显示到界面？

        IsCollectDone[0] = false;
    }
}

void MainWindow::on_pushButton_W11_clicked()
{
    QString pTips = "";
    if(!GetCollectBtnStatus(_S_COMPLETECOLLECT2_STAT_))
    {
        pTips = "是否开始完整采水2?";
    }
    else
    {
        pTips = "是否结束完整采水2?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsCollectDone[1] = true;

    if(GetCollectBtnStatus(_S_COMPLETECOLLECT2_STAT_))
    {
        ToggleCollectBtnStatus(_S_COMPLETECOLLECT2_STAT_,false);
    }
    else
    {
        ToggleCollectBtnStatus(_S_COMPLETECOLLECT2_STAT_,true);

        WriteSingleCoil(AddressDevPLC, "153", "5", QString::number(0x0000));
        WriteSingleCoil(AddressDevPLC, "153", "5", QString::number(0xff00));
        // FIXME： 结果弹出显示到界面？

        IsCollectDone[1] = false;
    }
}

void MainWindow::on_pushButton_W12_clicked()
{
    QString pTips = "";
    if(!GetCollectBtnStatus(_S_TANKCOLLECT1_STAT_))
    {
        pTips = "是否开始沉淀池采水1?";
    }
    else
    {
        pTips = "是否结束沉淀池采水1?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsCollectDone[2] = true;

    if(GetCollectBtnStatus(_S_TANKCOLLECT1_STAT_))
    {
        ToggleCollectBtnStatus(_S_TANKCOLLECT1_STAT_,false);
    }
    else
    {
        ToggleCollectBtnStatus(_S_TANKCOLLECT1_STAT_,true);

        WriteSingleCoil(AddressDevPLC, "154", "5", QString::number(0x0000));
        WriteSingleCoil(AddressDevPLC, "154", "5", QString::number(0xff00));
        // FIXME： 结果弹出显示到界面？

        IsCollectDone[2] = false;
    }
}

void MainWindow::on_pushButton_W13_clicked()
{
    QString pTips = "";
    if(!GetCollectBtnStatus(_S_TANKCOLLECT2_STAT_))
    {
        pTips = "是否开始沉淀池采水2?";
    }
    else
    {
        pTips = "是否结束沉淀池采水2?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsCollectDone[3] = true;

    if(GetCollectBtnStatus(_S_TANKCOLLECT2_STAT_))
    {
        ToggleCollectBtnStatus(_S_TANKCOLLECT2_STAT_,false);
    }
    else
    {
        ToggleCollectBtnStatus(_S_TANKCOLLECT2_STAT_,true);

        WriteSingleCoil(AddressDevPLC, "155", "5", QString::number(0x0000));
        WriteSingleCoil(AddressDevPLC, "155", "5", QString::number(0xff00));
        // FIXME： 结果弹出显示到界面？

        IsCollectDone[3] = false;
    }
}

void MainWindow::on_pushButton_W14_clicked()
{
    QString pTips = "";
    if(!GetCollectBtnStatus(_S_CUPCOLLECT_STAT_))
    {
        pTips = "是否开始测量杯采水?";
    }
    else
    {
        pTips = "是否结束测量杯采水?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsCollectDone[4] = true;

    if(GetCollectBtnStatus(_S_CUPCOLLECT_STAT_))
    {
        ToggleCollectBtnStatus(_S_CUPCOLLECT_STAT_,false);
    }
    else
    {
        ToggleCollectBtnStatus(_S_CUPCOLLECT_STAT_,true);

        WriteSingleCoil(AddressDevPLC, "156", "5", QString::number(0x0000));
        WriteSingleCoil(AddressDevPLC, "156", "5", QString::number(0xff00));
        // FIXME： 结果弹出显示到界面？

        IsCollectDone[4] = false;
    }
}

// 水阀
void MainWindow::on_pushButton_W15_clicked()
{
    QString pTips = "";
    if(!IsBtnInWaterValve1)
    {
        pTips = "是否打开阀1?";
    }
    else
    {
        pTips = "是否关闭阀1?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsBtnInWaterValve1 = !IsBtnInWaterValve1;
    if(IsBtnInWaterValve1)
    {
        WriteSingleCoil(AddressDevPLC, "160", "5", QString::number(0xff00));
        ui->label_W15->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
    else{
        WriteSingleCoil(AddressDevPLC, "160", "5", QString::number(0x0000));
        ui->label_W15->setStyleSheet("border-image: url(:/images/blackC.png);");
    }
}

void MainWindow::on_pushButton_W16_clicked()
{
    QString pTips = "";
    if(!IsBtnInWaterValve2)
    {
        pTips = "是否打开阀2?";
    }
    else
    {
        pTips = "是否关闭阀2?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsBtnInWaterValve2 = !IsBtnInWaterValve2;

    if(IsBtnInWaterValve2)
    {
        WriteSingleCoil(AddressDevPLC, "162", "5", QString::number(0xff00));
        ui->label_W16->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
    else{
        WriteSingleCoil(AddressDevPLC, "162", "5", QString::number(0x0000));
        ui->label_W16->setStyleSheet("border-image: url(:/images/blackC.png);");
    }

}

void MainWindow::on_pushButton_W17_clicked()
{
    QString pTips = "";
    if(!IsBtnInWaterValve3)
    {
        pTips = "是否打开阀3?";
    }
    else
    {
        pTips = "是否关闭阀3?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsBtnInWaterValve3 = !IsBtnInWaterValve3;

    if(IsBtnInWaterValve3)
    {
        WriteSingleCoil(AddressDevPLC, "164", "5", QString::number(0xff00));
        ui->label_W17->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
    else{
        WriteSingleCoil(AddressDevPLC, "164", "5", QString::number(0x0000));
        ui->label_W17->setStyleSheet("border-image: url(:/images/blackC.png);");
    }

}

void MainWindow::on_pushButton_W18_clicked()
{
    QString pTips = "";
    if(!IsBtnInWaterValve4)
    {
        pTips = "是否打开阀4?";
    }
    else
    {
        pTips = "是否关闭阀4?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsBtnInWaterValve4 = !IsBtnInWaterValve4;

    if(IsBtnInWaterValve4)
    {
        WriteSingleCoil(AddressDevPLC, "166", "5", QString::number(0xff00));
        ui->label_W18->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
    else{
        WriteSingleCoil(AddressDevPLC, "166", "5", QString::number(0x0000));
        ui->label_W18->setStyleSheet("border-image: url(:/images/blackC.png);");
    }

}

void MainWindow::on_pushButton_W19_clicked()
{
    QString pTips = "";
    if(!IsBtnInWaterValve5)
    {
        pTips = "是否打开阀5?";
    }
    else
    {
        pTips = "是否关闭阀5?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsBtnInWaterValve5 = !IsBtnInWaterValve5;

    if(IsBtnInWaterValve5)
    {
        WriteSingleCoil(AddressDevPLC, "168", "5", QString::number(0xff00));
        ui->label_W19->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
    else{
        WriteSingleCoil(AddressDevPLC, "168", "5", QString::number(0x0000));
        ui->label_W19->setStyleSheet("border-image: url(:/images/blackC.png);");
    }

}

void MainWindow::on_pushButton_W20_clicked()
{
    QString pTips = "";
    if(!IsBtnInWaterValve6)
    {
        pTips = "是否打开阀6?";
    }
    else
    {
        pTips = "是否关闭阀6?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsBtnInWaterValve6 = !IsBtnInWaterValve6;

    if(IsBtnInWaterValve6)
    {
        WriteSingleCoil(AddressDevPLC, "170", "5", QString::number(0xff00));
        ui->label_W20->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
    else{
        WriteSingleCoil(AddressDevPLC, "170", "5", QString::number(0x0000));
        ui->label_W20->setStyleSheet("border-image: url(:/images/blackC.png);");
    }

}

void MainWindow::on_pushButton_W21_clicked()
{
    QString pTips = "";
    if(!IsBtnInWaterValve7)
    {
        pTips = "是否打开阀7?";
    }
    else
    {
        pTips = "是否关闭阀7?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsBtnInWaterValve7 = !IsBtnInWaterValve7;

    if(IsBtnInWaterValve7)
    {
        WriteSingleCoil(AddressDevPLC, "172", "5", QString::number(0xff00));
        ui->label_W21->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
    else{
        WriteSingleCoil(AddressDevPLC, "172", "5", QString::number(0x0000));
        ui->label_W21->setStyleSheet("border-image: url(:/images/blackC.png);");
    }

}

void MainWindow::on_pushButton_W21_1_clicked()
{
    QString pTips = "";
    if(!IsBtnInWaterValve8)
    {
        pTips = "是否打开阀8?";
    }
    else
    {
        pTips = "是否关闭阀8?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsBtnInWaterValve8 = !IsBtnInWaterValve8;

    if(IsBtnInWaterValve8)
    {
        WriteSingleCoil(AddressDevPLC, "174", "5", QString::number(0xff00));
        // FIXME： 返回值 + 开关量状态值
        ui->label_W21_1->setStyleSheet("border-image: url(:/images/greenC.png);");
    }
    else{
        WriteSingleCoil(AddressDevPLC, "174", "5", QString::number(0x0000));
        ui->label_W21_1->setStyleSheet("border-image: url(:/images/blackC.png);");
    }
}

// 特殊阀
void MainWindow::on_pushButton_W22_clicked()    // 自来水阀
{
    QString pTips = "";
    if(!IsBtnInRunWaterValve)
    {
        pTips = "是否打开自来水阀?";
    }
    else
    {
        pTips = "是否关闭自来水阀?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsBtnInRunWaterValve = !IsBtnInRunWaterValve;
    if(IsBtnInRunWaterValve) WriteSingleCoil(AddressDevPLC, "184", "5", QString::number(0xff00));
    else WriteSingleCoil(AddressDevPLC, "184", "5", QString::number(0x0000));

}

void MainWindow::on_pushButton_W23_clicked()    // 吹气阀
{
    QString pTips = "";
    if(!IsIsBtnInAirValve)
    {
        pTips = "是否打开吹气阀?";
    }
    else
    {
        pTips = "是否关闭吹气阀?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsIsBtnInAirValve = !IsIsBtnInAirValve;
    if(IsIsBtnInAirValve) WriteSingleCoil(AddressDevPLC, "182", "5", QString::number(0xff00));
    else WriteSingleCoil(AddressDevPLC, "182", "5", QString::number(0x0000));
}

// 泵
void MainWindow::on_pushButton_W24_clicked()
{
    QString pTips = "";
    if(!IsBtnInInputPump1)
    {
        pTips = "是否打开泵1?";
    }
    else
    {
        pTips = "是否关闭泵1?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsBtnInInputPump1 = !IsBtnInInputPump1;
    if(IsBtnInInputPump1)WriteSingleCoil(AddressDevPLC, "178", "5", QString::number(0xff00));
    else WriteSingleCoil(AddressDevPLC, "178", "5", QString::number(0x0000));
}

void MainWindow::on_pushButton_W25_clicked()
{
    QString pTips = "";
    if(!IsBtnInInputPump2)
    {
        pTips = "是否打开泵2?";
    }
    else
    {
        pTips = "是否关闭泵2?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsBtnInInputPump2 = !IsBtnInInputPump2;
    if(IsBtnInInputPump2) WriteSingleCoil(AddressDevPLC, "179", "5", QString::number(0xff00));
    else  WriteSingleCoil(AddressDevPLC, "179", "5", QString::number(0x0000));

}

void MainWindow::on_pushButton_W26_clicked()
{
    QString pTips = "";
    if(!IsBtnInPeristalticPump)
    {
        pTips = "是否打开采样泵?";
    }
    else
    {
        pTips = "是否关闭采样泵?";
    }

    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
    if(result != QMessageBox::Yes) return;

    IsBtnInPeristalticPump = !IsBtnInPeristalticPump;
    if(IsBtnInPeristalticPump)WriteSingleCoil(AddressDevPLC, "176", "5", QString::number(0xff00));
    else WriteSingleCoil(AddressDevPLC, "176", "5", QString::number(0x0000));
}

void MainWindow::on_pushButton_171_clicked()
{
    QMessageBox::about(NULL, "提示", "<font color='black'>读取温湿度功能未投入使用！</font>");
}

void MainWindow::on_pushButton_172_clicked()
{
    QMessageBox::about(NULL, "提示", "<font color='black'>读取电压功能未投入使用！</font>");
}

void MainWindow::on_pushButton_173_clicked()
{
    QMessageBox::about(NULL, "提示", "<font color='black'>制热打开功能未投入使用！</font>");
}

void MainWindow::on_pushButton_174_clicked()
{
    QMessageBox::about(NULL, "提示", "<font color='black'>制冷打开功能未投入使用！</font>");
}

void MainWindow::on_pushButton_175_clicked()
{
    QMessageBox::about(NULL, "提示", "<font color='black'>空调关闭功能未投入使用！</font>");
}


// FIXME:
void MainWindow::on_pushButton_75_clicked()
{
    int mode = ui->comboBox_6->currentIndex();
    qDebug() << "mode==>>" << mode << "MeasureRunMode==>" << MeasureRunMode;

    if (mode == 0)
    {
        if (MeasureRunMode != _MEA_COV_RUN_MODE_)
        {
            MeasureRunMode = _MEA_COV_RUN_MODE_;
            QLOG_INFO() << "切换运行模式为 连续模式";
            QMessageBox::about(NULL, "提示", "<font color='black'>切换为连续模式！</font>");
        }
    }
    else if (mode == 1)
    {
        if (MeasureRunMode != _MEA_UNCOV_RUN_MODE_)
        {
            MeasureRunMode = _MEA_UNCOV_RUN_MODE_;
            QLOG_INFO() << "切换运行模式为 间隙模式";
            QMessageBox::about(NULL, "提示", "<font color='black'>切换为间隙模式！</font>");
        }
    }
}

void MainWindow::on_pushButton_76_clicked()
{
    int mode = ui->comboBox_8->currentIndex();

    if (mode == (PumpSelected - 1))
        return;

    switch (mode)
    {
    case 0:
        QLOG_INFO() << "选择泵工作模式为 泵A。";
        break;
    case 1:
        QLOG_INFO() << "选择泵工作模式为 泵B。";
        break;
    case 2:
        QLOG_INFO() << "选择泵工作模式为 自动切换。";
        break;
    case 3:
        QLOG_INFO() << "选择泵工作模式为 自动切换。";
        break;
    default:
        break;
    }
    PumpSelected = (uint16_t)(mode + 1);
    QString pTemp = (PumpSelected == 1) ? "泵A" :
                    (PumpSelected == 2) ? "泵B" :
                    (PumpSelected == 3) ? "交替运行" :
                    (PumpSelected == 4) ? "自动切换" : "";

    if (pTemp == "")
        QLOG_INFO() << "当前泵运行模式未初始化。";
    else
        QLOG_INFO() << "当前泵运行模式为 " << pTemp;

    WriteSingleRegister(AddressDevPLC, "41097", "6", QString::number(PumpSelected)); // 0x06 41097

}

void MainWindow::on_pushButton_77_clicked()
{
    ReadHoldingRegisters(AddressDevPLC, "41097", 7, "1");  // 寄存器数量
}

void MainWindow::on_pushButton_71_clicked()
{
    QString pStartTime = ui->lineEdit_39->text();
    StartTime = pStartTime.toUInt();

    CJsonFile jsonfile;
    if(jsonfile.update_KeyValue(pStartTime,STARTTIME))
        QMessageBox::about(NULL, "提示", "<font color='black'>保存起始时间成功！</font>");
}

void MainWindow::on_pushButton_72_clicked()
{
    QString pInternalTime = ui->lineEdit_40->text();
    IntervalTime = pInternalTime.toUInt();

    CJsonFile jsonfile;
    if(jsonfile.update_KeyValue(pInternalTime,INTERNALTIME))
        QMessageBox::about(NULL, "提示", "<font color='black'>保存间隔时间成功！</font>");
}

void MainWindow::on_pushButton_74_clicked()
{
    StartTime = 0;
    ui->lineEdit_39->setText("0");

    CJsonFile jsonfile;
    if(jsonfile.update_KeyValue("0",STARTTIME))
        QMessageBox::about(NULL, "提示", "<font color='black'>恢复默认起始时间 0 成功！</font>");
}

void MainWindow::on_pushButton_73_clicked()
{
    IntervalTime = 240;
    ui->lineEdit_40->setText("240");

    CJsonFile jsonfile;
    if(jsonfile.update_KeyValue("240",INTERNALTIME))
        QMessageBox::about(NULL, "提示", "<font color='black'>恢复默认间隔时间 240 成功！</font>");
}

// thread
void SerialWorker::ReadWaterPressure(QString reg, QString pRegs)
{
    ReadHoldingRegisters(AddressDevPLC, reg, 7, pRegs);  // 寄存器数量
}

void SerialWorker::ReadHoldingRegisters(QString pAddr,QString pRegAddr, uint16_t len,QString pRegs)
{
    if(serial->isOpen())
    {
        QString pTemp_Addr = pAddr;
        QString pRealData;

        uint16_t pReg_Bakup = pRegAddr.toUInt();

        pAddr = QString("%1").arg(pAddr.toUInt(),2,16,QChar('0'));

        QString pFunc = QString("%1").arg(0x03,2,16,QChar('0'));
        pRegAddr = QString("%1").arg(pRegAddr.toUInt(),4,16,QChar('0'));    //str = "0064"
        pRegAddr.insert(2," ");

        pRegs = QString("%1").arg(pRegs.toUInt(),4,16,QChar('0'));    //str = "0001"
        pRegs.insert(2," ");

        QString pCommText = pAddr + pFunc + pRegAddr + pRegs;

        Crc16Class crc16;
        QString pTx = crc16.crcCalculation(pCommText).toUpper();

        QLOG_DEBUG() << "ReadHoldingRegisters TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');

        serial->flush();
        serial->writeData(QString2Hex(pTx).data(),8);

        char data[100];
        int64_t pRetVal = serial->readData(data,len);
        if(pRetVal > 0)
        {
            QString strTmp;
            for(int i = 0; i<len; i++)
            {
                strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
            }
            if(crc16.crc_Checking(strTmp))
            {
                QLOG_DEBUG() << "ReadHoldingRegisters RX: "<<strTmp;
                // parse
                QByteArray buf = QString2Hex(strTmp);

                QByteArray pFunc;
                pFunc.append(buf.at(1));
                if(pFunc.toHex().toInt() == 0x03)
                {
                    QByteArray arr;
                    if(pReg_Bakup == 202) //0x03 :202-自来水压
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));
                        QLOG_DEBUG() << "自来水压==>>" << pRealData;
                        emit resultReady("自来水压:" + pRealData);
                    }
                    else if(pReg_Bakup == 200) // 0x03 : 200-采样水压
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));
                        QLOG_DEBUG() << "采样水压==>>" << pRealData;
                        emit resultReady("采样水压:" + pRealData);
                    }
                    else if(pReg_Bakup == 0)     // step
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        bool ok = false;
                        pRealData = QString::number(arr.toHex().toUShort(&ok, 16));
                        QLOG_DEBUG() << "读取PLC程序运行阶段step信息：" << pRealData;
                        if (pRealData.toUInt() != LastProcessStep)
                        {
                            LastProcessStep = pRealData.toUInt();

                            switch (pRealData.toUInt())
                            {
                            case 0:
                                QLOG_INFO() << "PLC 流程开始/结束。";
                                PlcRunState = "PLC 流程开始/结束。";
                                break;

                            case 1:
                                QLOG_INFO() << "PLC 采样前清洗，开启自来水阀。";
                                PlcRunState = "PLC 采样前清洗，开启自来水阀。";
                                break;

                            case 4:
                                QLOG_INFO() << "PLC 开启阀3/4，清洗外管路。";
                                PlcRunState = "PLC 开启阀3/4，清洗外管路。";
                                break;

                            case 5:
                                QLOG_INFO() << "PLC 开启紫外灯、阀1/2，泵A/B。";
                                PlcRunState = "PLC 开启紫外灯、阀1/2，泵A/B。";
                                break;

                            case 6:
                                QLOG_INFO() << "PLC 关闭阀6，蓄水。";
                                PlcRunState = "PLC 关闭阀6，蓄水。";
                                break;

                            case 8:
                            {
                                g_linux_ts = QDateTime::currentMSecsSinceEpoch() / 1000; // 秒级时间戳

                                QLOG_INFO() << "PLC 水样开始沉淀，沉淀5分钟后读取五参，测量杯采水。";
                                PlcRunState = "PLC 水样开始沉淀，沉淀5分钟后读取五参，测量杯采水。";
                                break;
                            }
                            case 9:
                                QLOG_INFO() << "PLC 等待测量数据。";
                                PlcRunState = "PLC 等待测量数据。";
                                break;

                            case 10:
                                QLOG_INFO() << "PLC 进行采样后清洗，开自来水阀，清洗外管路。";
                                PlcRunState = "PLC 进行采样后清洗，开自来水阀，清洗外管路。";
                                break;

                            case 11:
                                QLOG_INFO() << "PLC 沉淀池蓄水。";
                                PlcRunState = "PLC 沉淀池蓄水。";
                                break;

                            case 12:
                                QLOG_INFO() << "PLC 测量杯蓄水。";
                                PlcRunState = "PLC 测量杯蓄水。";
                                break;

                            default:
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                QLOG_DEBUG() << "ReadHoldingRegisters CRC Check ERR";
            }
        }
        else
        {
            QLOG_TRACE() << "ReadHoldingRegisters TimeOut";
        }
    }
}

void SerialWorker::ReadPLCProgramRunStep(QString reg, QString pRegs)
{
    ReadHoldingRegisters(AddressDevPLC, reg, 7, pRegs);  // 寄存器数量
}


void SerialWorker::ReadPLCProcessInfo(QString coil, QString pCoils)
{
    ReadCoils(AddressDevPLC, coil, 7, pCoils);    // addr + func + len + regs/8+if(regs%8?!=0:1,0) +crc*2
}

void SerialWorker::ReadPLCWarningInfo(QString coil, QString pCoils)
{
    ReadCoils(AddressDevPLC, coil, 7, pCoils);
}

void SerialWorker::ReadPLCValveStat(QString coil, QString pCoils)
{
    ReadCoils(AddressDevPLC, coil, 10, pCoils);  // 寄存器数量
}

void SerialWorker::ReadCoils(QString pAddr,QString coil,uint16_t len,QString pCoils)
{
    if(serial->isOpen())
    {
        QString pRealData;

        uint16_t pCoil_Bakup = coil.toUInt();

        pAddr = QString("%1").arg(pAddr.toUInt(),2,16,QChar('0'));

        QString pFunc = QString("%1").arg(0x01,2,16,QChar('0'));
        coil = QString("%1").arg(coil.toUInt(),4,16,QChar('0'));
        coil.insert(2," ");

        pCoils = QString("%1").arg(pCoils.toUInt(),4,16,QChar('0'));
        pCoils.insert(2," ");

        QString pCommText = pAddr + pFunc + coil + pCoils;

        Crc16Class crc16;
        QString pTx = crc16.crcCalculation(pCommText).toUpper();

        QLOG_DEBUG() << "ReadCoils TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');
        serial->flush();
        serial->writeData(QString2Hex(pTx).data(),8);
        char data[100];
        int64_t pRetVal = serial->readData(data,len);
        if(pRetVal > 0)
        {
            QString strTmp;
            for(int i = 0; i<len; i++)
            {
                strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
            }

            if(crc16.crc_Checking(strTmp))
            {
                QLOG_DEBUG() << "ReadCoils RX: "<<strTmp;
                // parse
                QByteArray buf = QString2Hex(strTmp);

                QByteArray pFunc;
                pFunc.append(buf.at(1));
                if(pFunc.toHex().toInt() == 0x01)
                {
                    QByteArray arr;
                    if(pCoil_Bakup == 0) //0x01 :0-水阀 浮标
                    {
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        arr[2] = buf.at(5);
                        arr[3] = buf.at(6);
                        arr[4] = buf.at(7);

                        IsValveXOpen[0] = (arr[0] & (1u << ((0 % 4) * 2))) != 0;        /// 0~7 水阀 8~9 自来水阀
                        IsValveXOpen[1] = (arr[0] & (1u << ((1 % 4) * 2))) != 0;
                        IsValveXOpen[2] = (arr[0] & (1u << ((2 % 4) * 2))) != 0;
                        IsValveXOpen[3] = (arr[0] & (1u << ((3 % 4) * 2))) != 0;

                        IsValveXOpen[4] = (arr[1] & (1u << ((1 % 4) * 2))) != 0;
                        IsValveXOpen[5] = (arr[1] & (1u << ((2 % 4) * 2))) != 0;
                        IsValveXOpen[6] = (arr[1] & (1u << ((3 % 4) * 2))) != 0;

                        IsValveXOpen[7] = (arr[2] & (1u << ((0 % 4) * 2))) != 0;
                        IsValveXOpen[8] = (arr[3] & (1u << ((3 % 4) * 2))) != 0;

                        IsBuoyTriggered[0] = ((arr[4]) & 0x01) != 0; // 0:沉淀池浮标 1:测量杯浮标
                        IsBuoyTriggered[1] = ((arr[4]) & 0x02) != 0;
                    }else if(pCoil_Bakup == 1500)
                    {
                        QLOG_DEBUG() << "读取PLC告警信息";
                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        for(int i=0;i<8;i++)
                        {
                            PLCWarningInfo[i] = (arr[0] & (0x01 << i)) != 0;
                        }
                        for(int i=0;i<3;i++)
                        {
                            PLCWarningInfo[8+i] = (arr[1] & (0x01 << i)) != 0;
                        }

                        if (PLCWarningInfo[0])
                        {
                            if (PLCWarningInfo[0])
                            {
                                QLOG_INFO() << "阀1故障";
                            }
                            if (PLCWarningInfo[1]){
                                QLOG_INFO() << "阀2故障";
                            }

                            if (PLCWarningInfo[2]){QLOG_INFO() << "阀3故障";}

                            if (PLCWarningInfo[3]){QLOG_INFO() << "阀4故障";}

                            if (PLCWarningInfo[4]){QLOG_INFO() << "阀5故障";}

                            if (PLCWarningInfo[5]){QLOG_INFO() << "阀6故障";}

                            if (PLCWarningInfo[6]){QLOG_INFO() << "阀7故障";}

                            if (PLCWarningInfo[7]){QLOG_INFO() << "阀8故障";}

                            if (PLCWarningInfo[8]){QLOG_INFO() << "自来阀故障";}

                            if (PLCWarningInfo[9]){QLOG_INFO() << "泵A故障";}

                            if (PLCWarningInfo[10]){QLOG_INFO() << "泵A故障";} // B?

                        }
                    }else if(pCoil_Bakup == 1000)
                    {

                        arr[0] = buf.at(3);
                        arr[1] = buf.at(4);
                        QLOG_DEBUG() << "读取PLC运行流程Process信息: " << arr.toHex().toUShort();
                        for(int i=0;i<8;i++)
                        {
                            PLCProcessInfo[i] = (arr[0] & (0x01 << i)) != 0;
                        }
                        for(int i=0;i<8;i++)
                        {
                            PLCProcessInfo[8+i] = (arr[1] & (0x01 << i)) != 0;
                        }
                    }
                }
            }
            else
            {
                QLOG_DEBUG() << "ReadCoils RX CRC check Err...";
            }
        }
        else
        {
            QLOG_DEBUG() << "ReadCoils RX: TimeOut";
        }
    }
}

void SerialWorker::StartStopRunMode(QString reg, bool onOff)
{
    uint16_t value;
    value = onOff ? (uint16_t)0xff00 : (uint16_t)0x0000;

    WriteSingleCoil(AddressDevPLC, reg, "5", QString::number(value));
}

void SerialWorker::WriteSingleCoil(QString pAddr,QString pRegAddr,QString pFunc,QString pSetVal)
{
    if(serial->isOpen())
    {
        QString pRealData;
        pAddr = QString("%1").arg(pAddr.toUInt(),2,16,QChar('0'));

        pFunc = QString("%1").arg(pFunc.toUInt(),2,16,QChar('0'));
        pRegAddr = QString("%1").arg(pRegAddr.toUInt(),4,16,QChar('0'));    //str = "0064"
        pRegAddr.insert(2," ");

        pSetVal = QString("%1").arg(pSetVal.toUInt(),4,16,QChar('0'));   //str = "ff00"
        pSetVal.insert(2," ");

        QString pCommText = pAddr + pFunc + pRegAddr + pSetVal;

        Crc16Class crc16;
        QString pTx = crc16.crcCalculation(pCommText).toUpper();

        QLOG_DEBUG() << "WriteSingleCoil TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');
        serial->flush();
        serial->writeData(QString2Hex(pTx).data(),8);

        char data[100];
        int pRetVal = serial->readData(data,8);
        if(pRetVal > 0)
        {
            QString strTmp;
            for(int i = 0; i<8; i++)
            {
                strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
            }
            if(crc16.crc_Checking(strTmp))
            {
                QLOG_DEBUG() << "WriteSingleCoil RX: "<<strTmp;
                // parse
                QByteArray buf = QString2Hex(strTmp);

                QByteArray pFunc;
                pFunc.append(buf.at(1));
                if(pFunc.toHex().toInt() == 0x05)
                {
                    QByteArray arrReg,arrVal;
                    bool ok;
                    arrReg[0] = buf.at(2);
                    arrReg[1] = buf.at(3);

                    arrVal[0] = buf.at(4);
                    arrVal[1] = buf.at(5);
                    // qDebug() << "-->>" << arrReg.toHex().toUShort(&ok, 16) << "===>" <<arrVal.toHex().toUShort(&ok, 16);

                    if(arrReg.toHex().toUShort(&ok, 16) == 200)
                    {
                        if(arrVal.toHex().toUShort(&ok, 16) == 0xff00)
                        {
                            QLOG_INFO() << "启动PLC自动测量。";    //FIXME ：  如果下发不成功 应对策略？？
                        }
                        if(arrVal.toHex().toUShort(&ok, 16) == 0x0000)
                        {
                            QLOG_INFO() << "关闭PLC自动测量。";
                        }
                    }
                }
            }
            else
            {
                QLOG_DEBUG() << "WriteSingleCoil RX CRC check Err...";
            }
        }else
        {
            QLOG_TRACE() << "WriteSingleCoil TimeOut ";
        }
    }
}

void SerialWorker::SendMeasuredDoneSignalToPlc(QString reg, uint16_t val)
{
    WriteSingleRegister(AddressDevPLC, reg, "6", QString::number(val));
}

bool SerialWorker::WriteMultiRegister(QString pAddr,QString pRegAddr,QString pFunc,QString pSetValList)
{
    if(serial->isOpen())
    {
        uint16_t pTempReg_Addr = pRegAddr.toUInt();

        pAddr = QString("%1").arg(pAddr.toUInt(),2,16,QChar('0'));

        pFunc = QString("%1").arg(pFunc.toUInt(),2,16,QChar('0'));
        pRegAddr = QString("%1").arg(pRegAddr.toUInt(),4,16,QChar('0'));    //str = "0064"

        QString pRegAmt,pByteAmt,pSetVal;
        int pRegSize = 0;
        if(pSetValList.contains("-"))
        {
            QStringList pItemList = pSetValList.split("-");
            pRegSize = pItemList.size();
            if(!pRegSize) return false;
            pRegAmt = QString("%1").arg(pRegSize,4,16,QChar('0'));
            pByteAmt = QString("%1").arg(pRegSize*2,2,16,QChar('0'));

            for(int i=0;i<pRegSize;i++)
            {
                pSetVal += QString("%1").arg(pItemList.at(i).toUInt(),4,16,QChar('0'));
            }
        }
        else
        {
            pRegSize = 1;
            pRegAmt = QString("%1").arg(pRegSize,4,16,QChar('0'));
            pByteAmt = QString("%1").arg(pRegSize*2,2,16,QChar('0'));

            pSetVal += QString("%1").arg(pSetValList.toUInt(),4,16,QChar('0'));
        }

        QString pCommText = pAddr + pFunc + pRegAddr + pRegAmt + pByteAmt + pSetVal;

        Crc16Class crc16;
        QString pTx = crc16.crcCalculation(pCommText).toUpper();

        QLOG_DEBUG() << "WriteMultiRegister TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');
        serial->flush();
        serial->writeData(QString2Hex(pTx).data(),9+pRegSize*2);
        char data[100];
        int64_t pRetVal = serial->readData(data,8);
        if(pRetVal > 0)
        {
            QString strTmp;
            for(int i = 0; i<8; i++)
            {
                strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
            }

            if(crc16.crc_Checking(strTmp))
            {
                QLOG_DEBUG() << "WriteMultiRegister RX: "<<strTmp;

                // parse
                QByteArray buf = QString2Hex(strTmp);

                QByteArray pFunc;
                pFunc.append(buf.at(1));
                bool ok;
                if(pFunc.toHex().toUShort(&ok,16) == 0x10)
                {
                    if(pTempReg_Addr == 0x1200)
                    {
                        return true;
                    }
                }
            }
            else
            {
                QLOG_DEBUG() << "WriteMultiRegister RX CRC check Err...";
            }
        }
        else
        {
            QLOG_DEBUG() << "WriteMultiRegister RX TimeOut...";
        }
    }
    return false;
}

bool SerialWorker::WriteSingleRegister(QString pAddr,QString pRegAddr,QString pFunc,QString pSetVal)
{
    if(serial->isOpen())
    {
        uint16_t pTempReg_Addr = pRegAddr.toUInt();

        pAddr = QString("%1").arg(pAddr.toUInt(),2,16,QChar('0'));

        pFunc = QString("%1").arg(pFunc.toUInt(),2,16,QChar('0'));
        pRegAddr = QString("%1").arg(pRegAddr.toUInt(),4,16,QChar('0'));    //str = "0064"
        pRegAddr.insert(2," ");

        pSetVal = QString("%1").arg(pSetVal.toUInt(),4,16,QChar('0'));    //str = "ff00"
        pSetVal.insert(2," ");

        QString pCommText = pAddr + pFunc + pRegAddr + pSetVal;

        Crc16Class crc16;
        QString pTx = crc16.crcCalculation(pCommText).toUpper();

        QLOG_INFO() << "==> WriteSingleRegister TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');
        serial->flush();
        serial->writeData(QString2Hex(pTx).data(),8);
        char data[100];
        int64_t pRetVal = serial->readData(data,8);
        if(pRetVal > 0)
        {
            QString strTmp;
            for(int i = 0; i<8; i++)
            {
                strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
            }

            if(crc16.crc_Checking(strTmp))
            {
                QLOG_INFO() << "==> WriteSingleRegister RX: "<<strTmp;

                // parse
                QByteArray buf = QString2Hex(strTmp);

                QByteArray pFunc;
                pFunc.append(buf.at(1));
                if(pFunc.toHex().toInt() == 0x06)
                {
                    // pTempReg_Addr 0

                    if(pTempReg_Addr == 11)
                    {
                        TransmitFlag |= BIT1; // 流程完成发送
                        QLOG_INFO() << "流程完成发送，向PLC发送测量完成信号！";
                    }
                    else if(pTempReg_Addr == 41088)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>设置成功！</font>");
                    }
                    else if(pTempReg_Addr == 41188)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>设置成功！</font>");
                    }
                    else if(pTempReg_Addr == 41097)
                    {
                        QMessageBox::about(NULL, "提示", "<font color='black'>设置泵模式成功！</font>");
                    }

                    // pTempReg_Addr 40001

                    return true;
                }
            }
            else
            {
                QLOG_DEBUG() << "WriteSingleRegister RX CRC check Err...";
            }
        }
        else
        {
            QLOG_DEBUG() << "WriteSingleRegister RX TimeOut...";
        }
    }
    return false;
}

void SerialWorker::ReadDeviceStatusInfo(CDevice* &pDevice)
{
    QMap<QString,CFactor *> mapFactorList = pDevice->get_map_DevFactors();
    QMap<QString,CFactor *>::iterator itFa;
    for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {

        CFactor *pFactor = itFa.value();
        if(pFactor)
        {
            if(pFactor->getProperty() == "1") // status info
            {
                int retry = 3;
                while (retry-- > 0)
                {
                    QString pAddr = pDevice->get_address();
                    QString pFunc = pFactor->getFactorFunc();
                    QString pRegAddr = pFactor->getRegisAddr();
                    QString pDataType = pFactor->getDataType();
                    QString pMbLen;
                    if(pDataType == QT_INT32 || pDataType == QT_UINT32 || pDataType == QT_FLOAT)
                        pMbLen = "2";
                    else
                        pMbLen = "1";
                    QString pRealData = "";
                    if(CommonReadRegister(pFactor->getByteOrder(),QT_UINT16,pAddr,pFunc,pRegAddr,pMbLen,pRealData))
                    {
                        if(pRealData != "")
                            pDevice->set_processStatus(pRealData.toUInt()); // 30019

                        break;
                    }

                    QThread::sleep(1);
                }
            }
        }
    }
}

bool MainWindow::ReadMeasureResult(CDevice* &pDevice)
{
    bool pCommRet = false;

    QMap<QString,CFactor *> mapFactorList = pDevice->get_map_DevFactors();
    QMap<QString,CFactor *>::iterator itFa;
    for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {

        CFactor *pFactor = itFa.value();
        if(pFactor)
        {
            if(pFactor->getProperty() == "0") // 测量值
            {
                QString pFunc = pFactor->getFactorFunc();
                QString pRegAddr = pFactor->getRegisAddr();
                QString pDataType = pFactor->getDataType();
                QString pMbLen;
                if(pDataType == QT_INT32 || pDataType == QT_UINT32 || pDataType == QT_FLOAT)
                {
                    pMbLen = "2";
                }
                else
                {
                    pMbLen = "1";
                }

                QString pRealData = "";
                if(CommonReadRegister(pFactor->getByteOrder(),pDataType,pDevice->get_address(),pFunc,pRegAddr,pMbLen,pRealData))
                {
                    if(pRealData != "")
                    {
                        QLOG_INFO() << "测量因子：" << pFactor->getFactorName() <<"测量值：" << pRealData;

                        pFactor->setFactorData(pRealData);
                        pFactor->setDateTime(QString::fromStdString(current_formattime()));

                        pCommRet = true;
                    }
                }
            }
        }
    }

    if(pCommRet)
    {
        return true;
    }
    return false;
}

bool SerialWorker::ReadMeasureResult(CDevice* &pDevice)
{

    QMap<QString,CFactor *> mapFactorList = pDevice->get_map_DevFactors();
    QMap<QString,CFactor *>::iterator itFa;
    for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {

        CFactor *pFactor = itFa.value();
        if(pFactor)
        {
            if(pFactor->getProperty() == "0") // 测量值
            {
                int retry = 3;
                while (retry-- > 0)
                {
                    QString pFunc = pFactor->getFactorFunc();
                    QString pRegAddr = pFactor->getRegisAddr();
                    QString pDataType = pFactor->getDataType();
                    QString pMbLen;
                    if(pDataType == QT_INT32 || pDataType == QT_UINT32 || pDataType == QT_FLOAT)
                    {
                        pMbLen = "2";
                    }
                    else
                    {
                        pMbLen = "1";
                    }

                    QString pRealData = "";
                    if(CommonReadRegister(pFactor->getByteOrder(),pDataType,pDevice->get_address(),pFunc,pRegAddr,pMbLen,pRealData))
                    {
                        if(pRealData != "")
                        {
                            QLOG_INFO() << "测量因子：" << pFactor->getFactorName() <<"测量值：" << pRealData;

                            pFactor->setFactorData(pRealData);
                            pFactor->setFlag("N");
                            emit resultReady("测量值:" + pFactor->getFactorName() + ":" + pRealData);
                            return true;
                        }
                    }
                    QThread::sleep(1);
                }
            }
        }
    }
    return false;
}

void SerialWorker::ReadDeviceWarningInfo(CDevice* &pDevice)
{
    QMap<QString,CFactor *> mapFactorList = pDevice->get_map_DevFactors();
    QMap<QString,CFactor *>::iterator itFa;
    for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {

        CFactor *pFactor = itFa.value();
        if(pFactor)
        {
            if(pFactor->getProperty() == "2") // 告警信息
            {
                int retry = 3;
                while (retry-- > 0)
                {
                    // 云璟-0x03-30020-1(uint16)

                    QString pFunc = pFactor->getFactorFunc();
                    QString pRegAddr = pFactor->getRegisAddr();
                    QString pDataType = pFactor->getDataType();
                    QString pMbLen;
                    if(pDataType == QT_INT32 || pDataType == QT_UINT32 || pDataType == QT_FLOAT)
                    {
                        pMbLen = "2";
                    }
                    else
                    {
                        pMbLen = "1";
                    }

                    QString pRealData = "";
                    if(CommonReadRegister(pFactor->getByteOrder(),pDataType,pDevice->get_address(),pFunc,pRegAddr,pMbLen,pRealData))
                    {
                        if(pRealData != "")
                        {
                            QLOG_INFO() << "采集名称：" << pFactor->getFactorName() <<"错误代码：" << pRealData;
                            break;
                        }
                    }
                    QThread::sleep(1);
                }
            }
        }
    }
}


bool MainWindow::SendMeasureCmd(QString address,QString pType)
{
    if(pType == "高指-聚光")
    {
        // 0x010  0x1200 // 0001 02 0001
        if(WriteMultiRegister(address,"4608","16","1"))
        {
            return true;
        }
    }else if(pType == "高指-鸿恺")  // 01 10   12 00    00 01   02   00 01    55 91
    {
        // 0x010  0x1200 // 0001 0200 0100
        if(WriteMultiRegister(address,"4608","16","1"))
        {
            return true;
        }

    }else if(pType == "氨氮-蓝创A")
    {
        if(WriteSingleRegister(address,"0","6","1"))
        {
            return true;
        }

    }else if(pType == "总磷-蓝创A")
    {
        if(WriteSingleRegister(address,"0","6","1"))
        {
            return true;
        }
    }else{
        if(WriteSingleRegister(address,"40001","6","1"))
        {
            return true;
        }
    }
    return false;
}

bool SerialWorker::SendMeasureCmd(QString address,QString pType)
{
    int retry = 3;

    while (retry-- > 0)
    {
        if(pType == "高指-聚光")
        {
            // 0x010  0x1200 // 0001 02 0001
            if(WriteMultiRegister(address,"4608","16","1"))
            {
                return true;
            }

        }else if(pType == "高指-鸿恺")  // 01 10   12 00    00 01   02   00 01    55 91
        {
            // 0x010  0x1200 // 0001 0200 0100
            if(WriteMultiRegister(address,"4608","16","1"))
            {
                return true;
            }

        }else if(pType == "氨氮-蓝创A")
        {
            if(WriteSingleRegister(address,"0","6","1"))
            {
                return true;
            }

        }else if(pType == "总磷-蓝创A")
        {
            if(WriteSingleRegister(address,"0","6","1"))
            {
                return true;
            }
        }else{
            if(WriteSingleRegister(address,"40001","6","1"))
            {
                return true;
            }
        }

        QThread::sleep(1);
    }
    return false;
}


SerialWorker::SerialWorker(Win_QextSerialPort *ser,QObject *parent) : QObject(parent),serial(ser)
{

}

void MainWindow::on_pushButton_52_clicked()
{
    ui->tableWidget_3->setRowCount(0);
    ui->tableWidget_3->clear();

    setPackageTableHeader();
    setPackageTableContents();
}

void MainWindow::on_pushButton_53_clicked()
{
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_2->clear();

    setDataTableHeader();
    setDataTableContents();
}

void MainWindow::setPackageTableHeader()
{
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#1B89A1}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
headerText << QStringLiteral("时间戳") << QStringLiteral("平台地址") << QStringLiteral("报文流向") << QStringLiteral("报文");
int cnt = headerText.count();
ui->tableWidget_3->setColumnCount(cnt);
ui->tableWidget_3->setHorizontalHeaderLabels(headerText);
// ui->tableWidget_3->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget_3->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget_3->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
ui->tableWidget_3->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget_3->horizontalHeader()->font();
font.setBold(true);
ui->tableWidget_3->horizontalHeader()->setFont(font);

ui->tableWidget_3->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {150,150,80,300};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget_3->setColumnWidth(i, widths[i]);
}

ui->tableWidget_3->setStyleSheet(qssTV);
ui->tableWidget_3->horizontalHeader()->setVisible(true);
ui->tableWidget_3->verticalHeader()->setDefaultSectionSize(45);
}

// 设置表格内容
void MainWindow::setPackageTableContents()
{
    ui->tableWidget_3->clearContents(); //只清除工作区，不清除表头

    QString start_ts = ui->dateTimeEdit_PS->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"));
    QString end_ts = ui->dateTimeEdit_PE->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"));

    QTableWidgetItem *pItemTime,*pItemAddr,*pItemDir,*pItemPackage;

    QSqlQuery query(db);

    QString sql;
    sql = QString("SELECT * FROM packages where timestamp>='%1' and timestamp<='%2';").arg(start_ts,end_ts);

    //qDebug() << "sql===>>" << sql;
    query.exec(sql);

    bool pRow = false;

    int i = 0;
    while(query.next())
    {
        if(!pRow) pRow = true;

        QString id = query.value(0).toString();
        QString time = query.value(1).toString();
        QString prop = query.value(2).toString();
        QString addr = query.value(3).toString();
        QString package = query.value(4).toString();

        QString pDir = "";
        if(prop.toUInt() == 0 || prop.toUInt() == 1 || prop.toUInt() == 3)    // 0-心跳；1-数据上传；2-回复；3-补发
        {
            pDir = "-->";
        }
        else if(prop.toUInt() == 2)
        {
            pDir = "<--";
        }

        ui->tableWidget_3->insertRow(i);

        pItemTime = new QTableWidgetItem(time);
        pItemTime->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_3->setItem(i, 0, pItemTime);

        pItemAddr = new QTableWidgetItem(addr);
        pItemAddr->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_3->setItem(i, 1, pItemAddr);

        pItemDir = new QTableWidgetItem(pDir);
        pItemDir->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_3->setItem(i, 2, pItemDir);

        pItemPackage = new QTableWidgetItem(package);
        pItemPackage->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_3->setItem(i, 3, pItemPackage);

        i++;
    }

    if(pRow) QMessageBox::about(NULL, "提示", "<font color='black'>查询成功！</font>");
    else QMessageBox::about(NULL, "提示", "<font color='black'>未查询到任何信息！</font>");
}


void MainWindow::setDataTableHeader()
{
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#1B89A1}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
headerText << QStringLiteral("时间戳") << QStringLiteral("因子编码") << QStringLiteral("因子名称") << QStringLiteral("数值")
           << QStringLiteral("状态标志位");
int cnt = headerText.count();
ui->tableWidget_2->setColumnCount(cnt);
ui->tableWidget_2->setHorizontalHeaderLabels(headerText);
// ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget_2->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
ui->tableWidget_2->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget_2->horizontalHeader()->font();
font.setBold(true);
ui->tableWidget_2->horizontalHeader()->setFont(font);

ui->tableWidget_2->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {180,120,120,90,150};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget_2->setColumnWidth(i, widths[i]);
}

ui->tableWidget_2->setStyleSheet(qssTV);
ui->tableWidget_2->horizontalHeader()->setVisible(true);
ui->tableWidget_2->verticalHeader()->setDefaultSectionSize(45);
}

// 设置表格内容
void MainWindow::setDataTableContents()
{
    ui->tableWidget_2->clearContents(); //只清除工作区，不清除表头

    QString start_ts = ui->dateTimeEdit_HS->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"));
    QString end_ts = ui->dateTimeEdit_HE->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"));

    QTableWidgetItem *pItemTime,*pItemKey,*pItemKeyName,*pItemValue,*pItemFlag;

    QSqlQuery query(db);

    QString sql;

    if(ui->comboBox_FC->currentText() == "--全选--")
    {
        sql = QString("SELECT * FROM historydata where timestamp>='%1' and timestamp<='%2';").arg(start_ts,end_ts);
    }
    else
    {
        if(ui->comboBox_FC->currentText().contains("  "))
        {
            QStringList pItemList = ui->comboBox_FC->currentText().split("  ");
            sql = QString("SELECT * FROM historydata where timestamp>='%1' and timestamp<='%2' and key='%3';").arg(start_ts,end_ts,pItemList[0]);
        }
    }

    //qDebug() << "sql===>>" << sql;
    query.exec(sql);

    bool pRow = false;

    int i = 0;
    while(query.next())
    {
        if(!pRow) pRow = true;

        QString id = query.value(0).toString();
        QString time = query.value(1).toString();
        QString key = query.value(2).toString();
        QString keyName = query.value(3).toString();
        QString value = query.value(4).toString();
        QString flag = query.value(5).toString();

        //()<<id<<time<<key<<value<<flag;

        ui->tableWidget_2->insertRow(i);

        pItemTime = new QTableWidgetItem(time);
        pItemTime->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setItem(i, 0, pItemTime);

        pItemKey = new QTableWidgetItem(key);
        pItemKey->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setItem(i, 1, pItemKey);

        pItemKeyName = new QTableWidgetItem(keyName);
        pItemKeyName->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setItem(i, 2, pItemKeyName);

        pItemValue = new QTableWidgetItem(value);
        pItemValue->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setItem(i, 3, pItemValue);

        pItemFlag = new QTableWidgetItem(flag);
        pItemFlag->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setItem(i, 4, pItemFlag);

        i++;
    }

    if(pRow) QMessageBox::about(NULL, "提示", "<font color='black'>查询成功！</font>");
    else QMessageBox::about(NULL, "提示", "<font color='black'>未查询到任何信息！</font>");
}

void MainWindow::on_pushButton_3_clicked()
{
    QSqlQuery query(db);

    QString start_ts = ui->dateTimeEdit_BFS->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"));
    QString end_ts = ui->dateTimeEdit_BFE->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"));

    QString sql = QString("SELECT * FROM packages where timestamp>='%1' and timestamp<='%2' and property='1';").arg(start_ts,end_ts);

    //qDebug() << "sql===>>" << sql;
    query.exec(sql);

    bool pRow = false;

    while(query.next())
    {
        if(!pRow) pRow = true;

        QString id = query.value(0).toString();
        QString time = query.value(1).toString();
        QString prop = query.value(2).toString();
        QString addr = query.value(3).toString();
        QString package = query.value(4).toString();
        // send

        QLOG_INFO() << "数据补发：" << package;
        qint8 i = 0;
        for(i = 0; i < IPlist.count(); i++)
        {
            if (SocketList[i]->state() == SocketList[i]->ConnectedState) {
                this->sendMessages(SocketList[i], package);

                QString pAddress = IPlist.at(i).toString() +":" + QString::number(Portlist.at(i));

                QString sql = QString("INSERT INTO packages (timestamp,property,address,package) "
                              "VALUES ('%1','3','%2','%3')").arg(QString::fromStdString(current_formattime()),pAddress,package);

                // property-3 : 数据补发
                // qDebug() << "sql===>>" << sql;
                QSqlQuery query(db);
                query.exec(sql);
            }
        }
    }

    if(pRow) QMessageBox::about(NULL, "提示", "<font color='black'>数据补发完成！</font>");
    else QMessageBox::about(NULL, "提示", "<font color='black'>未查询到任何信息！</font>");

}

void MainWindow::on_pushButton_32_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41088", 7, "1"); // addr + func + len + regs*2 + CRC*2
}

void MainWindow::on_pushButton_34_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }
    ReadHoldingRegisters(AddressDevPLC, "41188", 7, "1");
}

void MainWindow::on_pushButton_33_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_21->text();

    if(pSetVal.toUInt() > 0xffff)
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值有误！</font>");
        return;
    }
    WriteSingleRegister(AddressDevPLC, "41088", "6", pSetVal);
}

void MainWindow::on_pushButton_35_clicked()
{
    if(!m_pSerialCom->isOpen())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>串口未打开！</font>");
        return;
    }

    QString pSetVal = ui->lineEdit_22->text();

    if(pSetVal.toUInt() > 0xffff)
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设置值有误！</font>");
        return;
    }
    WriteSingleRegister(AddressDevPLC, "41188", "6", pSetVal);
}

void MainWindow::on_pushButton_W3_2_clicked()
{

    if(get_WorkMode_BtnStatus(_B_AUTO_MODE_))
    {

        QString pTips = "";
        if(!Lijiceliang)
        {
            pTips = "是否开始立即测量?";
        }
        else
        {
            pTips = "是否结束立即测量?";
        }

        QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), pTips);
        if(result != QMessageBox::Yes) return;

        if(!Lijiceliang)
        {
            StartRunAutoMode = true;
            Lijiceliang = true;
            ui->label_W3_2->setStyleSheet("border-image: url(:/images/greenC.png);");

            g_DateTime = current_datetime();
            g_MinTime = current_minutetime();
            QLOG_INFO() << "立即测量功能 触发！时间戳：" << QString::fromStdString(g_MinTime);

            ui->groupBox->setTitle(QString("实时数据  ")+QString::fromStdString(g_MinTime));
        }
        else
        {
            Lijiceliang = false;
            ui->label_W3_2->setStyleSheet("border-image: url(:/images/blackC.png);");
            // StartRunAutoMode = false;
        }
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>立即测量须在自动模式下进行！</font>");
        return;
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->textBrowserBF->clear();

    QSqlQuery query(db);

    QString start_ts = ui->dateTimeEdit_BFS->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"));
    QString end_ts = ui->dateTimeEdit_BFE->dateTime().toString(QLatin1String("yyyy-MM-dd hh:mm:ss"));

    QString sql = QString("SELECT * FROM packages where timestamp>='%1' and timestamp<='%2' and property='1';").arg(start_ts,end_ts);

    //qDebug() << "sql===>>" << sql;
    query.exec(sql);

    bool pRow = false;

    while(query.next())
    {
        if(!pRow) pRow = true;

        QString id = query.value(0).toString();
        QString time = query.value(1).toString();
        QString prop = query.value(2).toString();
        QString addr = query.value(3).toString();
        QString package = query.value(4).toString();

        //qDebug()<<id<<time<<prop<<package;

        ui->textBrowserBF->append(time);
        ui->textBrowserBF->append(package);
    }

    if(pRow) QMessageBox::about(NULL, "提示", "<font color='black'>查询成功！</font>");
    else QMessageBox::about(NULL, "提示", "<font color='black'>未查询到任何信息！</font>");
}

void MainWindow::on_pushButton_55_clicked()
{
    test_package_work();
}

void MainWindow::test_package_work()
{

    qint8 i = 0;
    for(i = 0; i < IPlist.count(); i++)
    {
        if (SocketList[i]->state() == SocketList[i]->ConnectedState) {
            g_StrQN = current_time();
            HJ212_PARAM_SET hj212_param;
            HJ212_DATA_PARAM hj212_data_param;
            hj212_data_param.DataTime = current_datetime() + ";";

            hj212_param = hj212_set_params(HJ212_DEF_VAL_ST, HJ212_CN_RTD_DATA, HJ212_FLAG_ACK_EN, &hj212_data_param, Passwordlist[i].toStdString(), m_MN.toStdString());
            string rep = hj212_frame_assemble(&hj212_param);
            this->sendMessages(SocketList[i], QString::fromStdString(rep));

            QString pText = "To " + IPlist.at(i).toString();
            ui->textBrowserBF->append(pText);
            ui->textBrowserBF->append(QString::fromStdString(rep));

            QString pAddress = IPlist.at(i).toString() +":" + QString::number(Portlist.at(i));

            QLOG_INFO() << "发送连接测试包至 " << pAddress;

            QString sql = QString("INSERT INTO packages (timestamp,property,address,package) "
                          "VALUES ('%1','0','%2','%3')").arg(QString::fromStdString(current_formattime()),pAddress,QString::fromStdString(rep));

            // property-0 : 测试包上传(心跳)
            // qDebug() << "sql===>>" << sql;
            QSqlQuery query(db);
            query.exec(sql);
        }
        else
        {
            QString pText = IPlist.at(i).toString() + "：非连接状态";
            ui->textBrowserBF->append("pText");
        }
    }
}

void MainWindow::on_pushButton_56_clicked()
{
    ui->textBrowserBF->clear();
}

void MainWindow::on_pushButton_57_clicked()
{
    device_widget_clear();
    device_display();
}

void MainWindow::on_pushButton_58_clicked()
{
    device_widget_clear();
}
