#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QToolTip>

#include "msgbox.h"
#include "Crc16Class.h"

#include "chart.h"
#include <QtCharts/QChartView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

QT_CHARTS_USE_NAMESPACE

QString g_Sepuyi;
QString g_PLC;
QString g_Forward1;
QString g_Forward2;

bool g_IsChecked = false;
bool g_IsRadioBtn1Checked = false;
bool g_IsRadioBtn2Checked = false;
float g_Kv;//速度场系数 8
float g_F;//烟道面积6
float g_Ba;//大气压力4
float g_Xsw;//烟气湿度3

string g_StrQN;
QStringList g_FactorsNameList;
QMap<QString, FactorInfo*> map_Factors;
static QByteArray QString2Hex(QString hexStr);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    initLogger();   //初始化日志

    QLOG_INFO() << "app start!!";
//    datebaseinit();

    m_pDateTimer = new QTimer(this);
    m_pDateTimer->setInterval(200);
    connect(m_pDateTimer, SIGNAL(timeout()), this, SLOT(HandleDateTimeout()));
    m_pDateTimer->start();

    Widget_Init();
    Setting_Init();

    Chart *chart = new Chart();
    chart->setTitle("历史曲线");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::AllAnimations);

    QChartView *chartView = new QChartView(chart,ui->widget);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setFixedSize(500,400);

    InitComm();
    InitFactorMaps();

    setTableHeader();
    setTableContents();

    id1 = startTimer(1000);
}

MainWindow::~MainWindow()
{


    delete ui;

    qDeleteAll(map_Factors);

    /* 打断线程再退出 */
    serialWorker1->stopWork();
    serialThread_1.quit();
    /* 阻塞线程 2000ms，判断线程是否结束 */
    if (serialThread_1.wait(5000)) {
        qDebug()<<"自动监控线程 Modbus 结束";
    }

    if(m_pSerialCom1)
    {
        m_pSerialCom1->close();
        delete m_pSerialCom1;
        m_pSerialCom1 = nullptr;
    }

    /* 打断线程再退出 */
    serialWorker2->stopWork();
    serialThread_2.quit();
    /* 阻塞线程 2000ms，判断线程是否结束 */
    if (serialThread_2.wait(5000)) {
        qDebug()<<"自动监控线程 PLC 结束";
    }

    if(m_pSerialCom2)
    {
        m_pSerialCom2->close();
        delete m_pSerialCom2;
        m_pSerialCom2 = nullptr;
    }

    /* 打断线程再退出 */
    serialWorker3->stopWork();
    serialThread_3.quit();
    /* 阻塞线程 2000ms，判断线程是否结束 */
    if (serialThread_3.wait(5000)) {
        qDebug()<<"自动监控线程 HJ212 结束";
    }

    if(m_pSerialCom3)
    {
        m_pSerialCom3->close();
        delete m_pSerialCom3;
        m_pSerialCom3 = nullptr;
    }

    /* 打断线程再退出 */
    serialWorker4->stopWork();
    serialThread_4.quit();
    /* 阻塞线程 2000ms，判断线程是否结束 */
    if (serialThread_4.wait(5000)) {
        qDebug()<<"自动监控线程 HJ212-2 结束";
    }

    if(m_pSerialCom4)
    {
        m_pSerialCom4->close();
        delete m_pSerialCom4;
        m_pSerialCom4 = nullptr;
    }

    if(db.isOpen())
        db.close();
}

void MainWindow::timerEvent(QTimerEvent * ev)
{
    if(ev->timerId() == id1)
    {
        setTableContents();
    }
}


void MainWindow::handleResults(const QString & results)
{
    /* 打印线程的状态 */
    qDebug()<< endl <<"线程的状态："<<results<<endl;
}

SerialWorker::SerialWorker(Win_QextSerialPort *ser,QObject *parent) : QObject(parent),serial(ser)
{

}


void SerialWorker::doWork1() {

    /* 标志位为真 */ // Modbus
    isCanRun = true;

    /* 死循环 */
    while (1) {

        QMutexLocker locker(&lock);
        /* 如果标志位不为真 */
        if (!isCanRun) {
            /* 跳出循环 */
            break;
        }

        qDebug() << "serial thread...Modbus";
        // 获取协议名称

        if(g_Sepuyi == "天蓝")
        {
            // 01 04 00 14 00 04 B1 CD
            // 01 04 08 00 00 00 00 00 00 00 00 24 0D
            // 总烃0x14 float
            // 甲烷0x16 float
            skybluework();

        }
        else if(g_Sepuyi == "鲁南")
        {
            lunanwork();
        }
        else if(g_Sepuyi == "VOC")
        {

        }
        else if(g_Sepuyi == "VOCS")
        {
            VocsHandler();
        }


        // 校准
//        flag212_1 = "C";//校准
//        flag212_2 = "C";
//        for (int i = 0; i < 20; i++)
//        {
//            factorsStates[i] = "正在校准";
//        }

        QThread::sleep(2);
    }
}

void SerialWorker::skybluework()
{
    if(serial->isOpen())
    {
        QString pAddr = "1"; //01
        QString pRegAddr = "20"; // 00 14
        QString pRegs = "4";

        pAddr = QString("%1").arg(pAddr.toUInt(),2,16,QChar('0'));

        QString pFunc = QString("%1").arg(0x04,2,16,QChar('0'));
        pRegAddr = QString("%1").arg(pRegAddr.toUInt(),4,16,QChar('0'));    //str = "0020"
        pRegAddr.insert(2," ");

        pRegs = QString("%1").arg(pRegs.toUInt(),4,16,QChar('0'));    //str = "0003"
        pRegs.insert(2," ");

        QString pCommText = pAddr + pFunc + pRegAddr + pRegs;

        Crc16Class crc16;
        QString pTx = crc16.crcCalculation(pCommText).toUpper();

        qDebug() << "ReadInputRegisters TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');
        serial->flush();
        serial->writeData(QString2Hex(pTx).data(),8);

        char data[100];
        int pRetVal = serial->readData(data,13);
        if(pRetVal > 0)
        {
            QString strTmp;
            for(int i = 0; i<13; i++)
            {
                strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
            }

            if(crc16.crc_Checking(strTmp))
            {
                qDebug() << "ReadHoldingRegisters RX: "<<strTmp;
                // parse
                QByteArray buf = QString2Hex(strTmp);

                QByteArray pFunc;
                pFunc.append(buf.at(1));
                if(pFunc.toHex().toInt() == 0x04)
                {
                    QByteArray arrTH,arrCH4,arrNMTH; //默认 ABCD
                    arrTH[0] = buf.at(4);
                    arrTH[1] = buf.at(3);
                    arrTH[2] = buf.at(6);
                    arrTH[3] = buf.at(5);

                    arrCH4[0] = buf.at(8);
                    arrCH4[1] = buf.at(7);
                    arrCH4[2] = buf.at(10);
                    arrCH4[3] = buf.at(9);

                    arrNMTH[0] = buf.at(12);
                    arrNMTH[1] = buf.at(11);
                    arrNMTH[2] = buf.at(14);
                    arrNMTH[3] = buf.at(13);

                    float accTH;
                    memcpy(&accTH, arrTH.data(), 4);
                    QString pRealDataTH = QString::number(accTH,'f',2);

                    qDebug() << "总烃：" << pRealDataTH;

                    float accCH4;
                    memcpy(&accCH4, arrCH4.data(), 4);
                    QString pRealDataCH4 = QString::number(accCH4,'f',2);

                    qDebug() << "甲烷：" << pRealDataCH4;

                    float accNMTH;
                    memcpy(&accNMTH, arrNMTH.data(), 4);
                    QString pRealDataNMTH = QString::number(accNMTH,'f',2);

                    qDebug() << "非甲烷总烃：" << pRealDataNMTH;

                    if(map_Factors.contains("总烃"))
                    {
                        map_Factors["总烃"]->m_value = pRealDataTH;
                        if(map_Factors.contains("总烃干值")&&map_Factors.contains("烟气湿度"))
                        {
                            map_Factors["总烃干值"]->m_value = map_Factors["总烃"]->m_value.toDouble()/(1-map_Factors["烟气湿度"]->m_value.toDouble());
                        }
                    }

                    if(map_Factors.contains("甲烷"))
                    {
                        map_Factors["甲烷"]->m_value = pRealDataCH4;
                        if(map_Factors.contains("甲烷干值")&&map_Factors.contains("烟气湿度"))
                        {
                            map_Factors["甲烷干值"]->m_value = map_Factors["甲烷"]->m_value.toDouble()/(1-map_Factors["烟气湿度"]->m_value.toDouble());
                        }
                    }

                    if(map_Factors.contains("非甲烷总烃"))
                    {
                        map_Factors["非甲烷总烃"]->m_value = pRealDataNMTH;
                        if(map_Factors.contains("非甲烷总烃干值")&&map_Factors.contains("烟气湿度"))
                        {
                            map_Factors["非甲烷总烃干值"]->m_value = map_Factors["非甲烷总烃"]->m_value.toDouble()/(1-map_Factors["烟气湿度"]->m_value.toDouble());
                        }
                    }

                }
            }
        }
    }
}

void SerialWorker::lunanwork()
{

}

void SerialWorker::VocsHandler() {

    bool pCommStatus = false;

    //Tx: -15 04 00 10 00 10 F3 17
    //Rx: -15 04 20 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 66 BB

    if(serial->isOpen())
    {
        float Kv = 0;//速度场系数
        float Vp = 0;//CMS湿排气流速
        float Vs = 0;//断面湿排气流速
        float F = 0;//断面面积
        float Ba = 0;//大气压力
        float Ps = 0;//烟气压力
        float ts = 0;//烟气温度
        float Xsw = 0;//湿度

        QString pAddr = "21"; //15
        QString pRegAddr = "16"; // 00 10
        QString pRegs = "16"; // 00 10

        pAddr = QString("%1").arg(pAddr.toUInt(),2,16,QChar('0'));

        QString pFunc = QString("%1").arg(0x04,2,16,QChar('0'));
        pRegAddr = QString("%1").arg(pRegAddr.toUInt(),4,16,QChar('0'));    //str = "0020"
        pRegAddr.insert(2," ");

        pRegs = QString("%1").arg(pRegs.toUInt(),4,16,QChar('0'));    //str = "0003"
        pRegs.insert(2," ");

        QString pCommText = pAddr + pFunc + pRegAddr + pRegs;

        Crc16Class crc16;
        QString pTx = crc16.crcCalculation(pCommText).toUpper();

        qDebug() << "==>>> ReadHoldingRegisters TX: " << QByteArray::fromHex(pTx.toLatin1()).toHex(' ');
        serial->flush();
        serial->writeData(QString2Hex(pTx).data(),8);

        char data[37];
        int pRetVal = serial->readData(data,37);
        if(pRetVal > 0)
        {
            QString strTmp;
            for(int i = 0; i<37; i++)
            {
                strTmp +=  QString().sprintf("%02x", (unsigned char)data[i]);
            }

            if(crc16.crc_Checking(strTmp))
            {
                qDebug() << "ReadHoldingRegisters RX: "<<strTmp;
                // parse
                QByteArray buf = QString2Hex(strTmp);

                QByteArray pFunc;
                pFunc.append(buf.at(1));
                if(pFunc.toHex().toInt() == 0x04)
                {
                    int16_t parMa[7] = {0};
                    qDebug() << "000001";

                    if(map_Factors.contains("烟气温度"))
                        parMa[0] = buf[map_Factors["烟气温度"]->m_Chan * 2 + 1] << 8 | buf[map_Factors["烟气温度"]->m_Chan * 2 + 2];//烟气温度初始值
                    if(map_Factors.contains("烟气压力"))
                        parMa[1] = buf[map_Factors["烟气压力"]->m_Chan * 2 + 1] << 8 | buf[map_Factors["烟气压力"]->m_Chan * 2 + 2];//烟气压力初始值
                    if(map_Factors.contains("烟气流速"))
                        parMa[2] = buf[map_Factors["烟气流速"]->m_Chan * 2 + 1] << 8 | buf[map_Factors["烟气流速"]->m_Chan * 2 + 2];//烟气流速初始值
                    if(map_Factors.contains("烟尘湿值"))
                        parMa[3] = buf[map_Factors["烟尘湿值"]->m_Chan * 2 + 1] << 8 | buf[map_Factors["烟尘湿值"]->m_Chan * 2 + 2];//烟尘湿值初始值
                    if(map_Factors.contains("氧气含量"))
                        parMa[4] = buf[map_Factors["氧气含量"]->m_Chan * 2 + 1] << 8 | buf[map_Factors["氧气含量"]->m_Chan * 2 + 2];//氧气含量初始值
                    if(map_Factors.contains("烟气湿度"))
                        parMa[5] = buf[map_Factors["烟气湿度"]->m_Chan * 2 + 1] << 8 | buf[map_Factors["烟气湿度"]->m_Chan * 2 + 2];//烟气湿度度初始值
                    if(map_Factors.contains("硫化氢"))
                        parMa[6] = buf[map_Factors["硫化氢"]->m_Chan * 2 + 1] << 8 | buf[map_Factors["硫化氢"]->m_Chan * 2 + 2];//硫化氢初始值

                    for (int i = 0; i < 6; i++)
                    {
                        if (parMa[i] > 27648) parMa[i] = 27648;
                        if (parMa[i] < 5530) parMa[i] = 5530;
                    }

                    if(map_Factors.contains("烟气湿度"))
                        map_Factors["烟气温度"]->m_value = QString::number(((float)(parMa[0] - 5530) / 22118 * map_Factors["烟气温度"]->m_LC) + map_Factors["烟气温度"]->m_RangeLower,'f',2);//烟气温度最终值
                    if(map_Factors.contains("烟气压力"))
                        map_Factors["烟气压力"]->m_value = QString::number(((float)(parMa[1] - 5530) / 22118 * map_Factors["烟气压力"]->m_LC) + map_Factors["烟气压力"]->m_RangeLower,'f',2);//烟气压力最终值
                    if(map_Factors.contains("烟气流速"))
                        map_Factors["烟气流速"]->m_value = QString::number(((float)(parMa[2] - 5530) / 22118 * map_Factors["烟气流速"]->m_LC) + map_Factors["烟气流速"]->m_RangeLower,'f',2);//烟气流速最终值
                    if(map_Factors.contains("烟尘湿值"))
                        map_Factors["烟尘湿值"]->m_value = QString::number(((float)(parMa[3] - 5530) / 22118 * map_Factors["烟尘湿值"]->m_LC) + map_Factors["烟尘湿值"]->m_RangeLower,'f',2);//烟尘湿值最终值
                    if(map_Factors.contains("氧气含量"))
                        map_Factors["氧气含量"]->m_value = QString::number(((float)(parMa[4] - 5530) / 22118 * map_Factors["氧气含量"]->m_LC) + map_Factors["氧气含量"]->m_RangeLower,'f',2);//氧气含量最终值

                    qDebug() << "000002";
                    // 是否选中！！
                    if(map_Factors.contains("烟气湿度"))
                    {
                        if(g_IsChecked) map_Factors["烟气湿度"]->m_value = g_Xsw;
                        else map_Factors["烟气湿度"]->m_value = QString::number(((float)(parMa[5] - 5530) / 22118 * map_Factors["烟气湿度"]->m_LC) + map_Factors["烟气湿度"]->m_RangeLower,'f',2);//烟气湿度最终值;
                    }
                    if(map_Factors.contains("硫化氢"))
                        map_Factors["硫化氢"]->m_value = QString::number(((float)(parMa[6] - 5530) / 22118 * map_Factors["硫化氢"]->m_LC) + map_Factors["硫化氢"]->m_RangeLower,'f',2);//氧气含量最终值

                    Kv = g_Kv;
                    if(map_Factors.contains("烟气流速"))
                        Vp = map_Factors["烟气流速"]->m_value.toDouble();
                    Vs = Kv * Vp;
                    F = g_F;
                    Ba = g_Ba;
                    if(map_Factors.contains("烟气压力"))
                        Ps = map_Factors["烟气压力"]->m_value.toDouble();
                    if(map_Factors.contains("烟气温度"))
                        ts = map_Factors["烟气温度"]->m_value.toDouble();
                    Xsw = g_Xsw;

                    qDebug() << "000003";



                    float Qs = 3600 * F * Kv * Vp;
                    if(map_Factors.contains("工况流量"))
                        map_Factors["工况流量"]->m_value = QString::number(Qs,'f',2);
                    if(map_Factors.contains("标况流量"))
                        map_Factors["标况流量"]->m_value = QString::number(Qs*(273/(273+ts))*((Ba+Ps)/101325)*(1-Xsw/100),'f',2);
                    if(map_Factors.contains("烟尘干值")&&map_Factors.contains("烟尘湿值"))
                        map_Factors["烟尘干值"]->m_value = QString::number(map_Factors["烟尘湿值"]->m_value.toDouble() / (1-Xsw),'f',2);
                    if(map_Factors.contains("烟尘排放量")&&map_Factors.contains("烟尘湿值")&&map_Factors.contains("标况流量"))
                        map_Factors["烟尘排放量"]->m_value = QString::number( map_Factors["烟尘湿值"]->m_value.toDouble() * map_Factors["标况流量"]->m_value.toDouble() / 1000000,'f',2);
                    if(map_Factors.contains("氧气含量干值")&&map_Factors.contains("氧气含量"))
                        map_Factors["氧气含量干值"]->m_value = QString::number(map_Factors["氧气含量"]->m_value.toDouble() / (1-Xsw),'f',2);
                    if(map_Factors.contains("硫化氢干值")&&map_Factors.contains("硫化氢湿值"))
                        map_Factors["硫化氢干值"]->m_value = QString::number(map_Factors["硫化氢"]->m_value.toDouble() / (1-Xsw),'f',2);

                    //
                    qDebug() << "000004";


                }
            }
        }
    }
}

// 15 02 00 00 00 14 7B 11
// recebuf[3] == 0x00
/*
 *
*       if (recebuf[3] == 0x00)
        {
            //维护状态
            flag212_1 = flag212_2 = "M";
            factorsStates[3] = "正在维护";
        }
        else
        {
            factorsStates[3] = factorsStates[4] = factorsStates[5] = factorsStates[6] = factorsStates[7] = factorsStates[11] = factorsStates[20] = factorsStates[12] = factorsStates[13] = factorsStates[14] = factorsStates[15] = "测量正常";
        }
 */

void SerialWorker::doWork2() {

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

        // 温压流模块PLC
        qDebug() << "serial thread...PLC" << g_PLC;

        if(g_PLC == "VOCS")
        {
            qDebug() << "plc debug.....";
            VocsHandler();
        }
        else if(g_PLC == "鲁南")
        {
            lunanwork();
        }
        else if(g_PLC == "天蓝")
        {
            skybluework();
        }
        else if(g_PLC == "VOC")
        {

        }

        QThread::sleep(2);
    }
}

void SerialWorker::UploadHandler1()
{

//            HJ212_PARAM_SET hj212_param;
//            HJ212_DATA_PARAM hj212_data_param;

//            hj212_data_param.DataTime = g_DateTime;
//            QString pRtnData;

//                QMap<QString,CFactor *>::iterator itFa;

//                int n=0;
//                for (itFa = mapFactorList.begin(); itFa != mapFactorList.end(); itFa++) {
//                    n++;
//                    CFactor *pFactor = itFa.value();

//                    if(pFactor)
//                    {
//                        QString prop = pFactor->getProperty();
//                        if(prop.toUInt() == 0)
//                        {
//                            QString pFactorCode =  pFactor->getFactorCode();
//                            // qDebug() << "pFactorCode===>>" << pFactorCode;

//                            pRtnData += pFactorCode+HJ212_FIELD_NAME_DATA_RTD + pFactor->getFactorData()+"," + pFactorCode+HJ212_FIELD_NAME_DATA_FLAG + HJ212_DATA_FLAG_RUN;

                            // pRtnData += ";"; *****************
//                        }
//                    }
//                }

//            hj212_data_param.Rtd = pRtnData.toStdString();

//            hj212_param = hj212_set_params(HJ212_DEF_VAL_ST, HJ212_CN_RTD_DATA, HJ212_FLAG_ACK_EN, &hj212_data_param, Passwordlist[i].toStdString(), m_MN.toStdString());
//            string rep = hj212_frame_assemble(&hj212_param);
//            this->sendMessages(SocketList[i], QString::fromStdString(rep));

}

void SerialWorker::doWork3() {

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

        qDebug() << "serial thread...HJ212 2017";

        UploadHandler1();

        QThread::sleep(2);
    }
}

void SerialWorker::doWork4() {

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

        qDebug() << "serial thread...HJ212_2";

        //UploadHandler2();

        QThread::sleep(2);
    }
}

// doWork5 -- modbus slave 转发表 点表

void MainWindow::InitFactorMaps()
{
    // tranverse
    QJsonObject pJsonFactors;
    QString dir_file = QApplication::applicationDirPath()+"/voc-factors.json";
    QFile file(dir_file);

    if(file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "配置文件格式错误！";
        }
        QJsonObject jsonObject= document.object();
        if(jsonObject.contains(FACTORS))
        {
            pJsonFactors = jsonObject.value(FACTORS).toObject();
        }
    }

    //map_Factors 20
    // --------------------1-----------2----------3-----------4------------5---------6-------------7------------8-------------9---------------10---------------11---------------12-----------13------------14----------15----------16---------17---------18---------19----------20---
    g_FactorsNameList << "总烃" <<"总烃干值"<<"甲烷"<<"甲烷干值"<<"非甲烷总烃"<<"非甲烷总烃干值"
                      <<"烟气温度"<<"烟气压力"<<"烟气流速"<<"烟气湿度"<<"氧气含量"<<"氧气含量干值"
                      <<"折算非甲烷总烃"<<"折算非甲烷总烃干值"<<"标况流量"<<"工况流量"<<"非甲烷总烃排放量"
                      <<"苯系物含量"<< "烟尘湿值"<<"烟尘干值" << "烟尘排放量"
                      << "硫化氢"<<"硫化氢干值" << "苯含量" << "甲苯"
                      << "间二甲苯" << "邻二甲苯";
    int pCnt = g_FactorsNameList.size();
    for(int i=0;i<pCnt;i++)
    {
        QString pItemName = g_FactorsNameList.at(i);

        // 是否上传 因子编码
        // 量程上限 量程下限 差值 输入通道

        QJsonObject pJsonfactor = pJsonFactors.value(pItemName).toObject();

        bool pDisplay = pJsonfactor.value(DISPLAY).toBool();
        bool pUpload = pJsonfactor.value(UPLOAD).toBool();
        uint16_t pChan = pJsonfactor.value(CHAN).toString().toInt();
        float pRangeUpper = pJsonfactor.value(RANGEUPPER).toString().toDouble();
        float pRangeLower = pJsonfactor.value(RANGELOWER).toString().toDouble();
        QString pUnit = pJsonfactor.value(UNIT).toString();
        float pAlarmUpper = pJsonfactor.value("AlarmUpper").toString().toDouble();
        bool pUsed = pJsonfactor.value("Used").toBool();

        FactorInfo *pItemInfo = new FactorInfo();
        pItemInfo->m_name = pItemName;
        pItemInfo->m_value = "0.00";
        pItemInfo->m_state = "D";
        pItemInfo->m_unit = pUnit;
        pItemInfo->m_used = pUsed;
        pItemInfo->m_display = pDisplay;
        pItemInfo->m_upload = pUpload;
        pItemInfo->m_Chan = pChan;
        pItemInfo->m_RangeUpper = pRangeUpper;
        pItemInfo->m_RangeLower = pRangeLower;
        pItemInfo->m_LC = pRangeUpper - pRangeLower;
        pItemInfo->m_AlarmUpper = pAlarmUpper;
        pItemInfo->m_Alias = QString::number(i);

        if(pItemInfo->m_display)
        {
            map_Factors.insert(pItemName,pItemInfo);
            seqlist.append(pItemInfo->m_Alias);
            nameseqlist.append(pItemName);
            facseqlist.append(pItemInfo);
        }
    }

     qDebug() << "map_Factors==>" << map_Factors;
     qDebug() << "seqlist==>" << seqlist;
     qDebug() << "nameseqlist==>" << nameseqlist;
     qDebug() << "facseqlist==>" << facseqlist;
}

//FactorInfo::FactorInfo(QString name,QString value,QString state,QString unit,bool display,bool upload)
//{
//    m_name = name;
//    m_value = value;
//    m_state = state;
//    m_unit = unit;
//    m_display = display;
//    m_upload = upload;
//}

//FactorInfo::~FactorInfo()
//{
//}

void MainWindow::InitSysSetting()
{
    QFile file(SYSTEM_SETTING_FILE);    // FIXME: 只读一次 检测文件修改更新
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "系统配置文件格式错误！";
        }
        QJsonObject jsonObject = document.object();
        g_Kv = jsonObject.value(SYS_SET8).toString().toDouble();//速度场系数 8
        g_F = jsonObject.value(SYS_SET6).toString().toDouble();//烟道面积6
        g_Ba = jsonObject.value(SYS_SET4).toString().toDouble();//大气压力4
        g_Xsw = jsonObject.value(SYS_SET3).toString().toDouble();//烟气湿度3
        g_IsChecked = jsonObject.value(SYS_CHECK).toBool();
        g_IsRadioBtn1Checked = jsonObject.value(SYS_RADIOBUTTON1).toBool();
        g_IsRadioBtn2Checked = jsonObject.value(SYS_RADIOBUTTON2).toBool();

    }
}

void MainWindow::InitComm()
{

    // tranverse
    QString dir_file = COMM_SETTING_FILE;
    QFile file(dir_file);

    if(file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "配置文件格式错误！";
        }
        QJsonObject jsonObject= document.object();
        if(jsonObject.contains(PORTSDOWN))
        {
            QJsonObject pJsonDev = jsonObject.value(PORTSDOWN).toObject();
            if(pJsonDev.contains(MODBUS))
            {
                QJsonObject pJsonModbus = pJsonDev.value(MODBUS).toObject();

                QString pItemName = pJsonModbus.value(PORTNAME).toString();
                QString pItemBaud = pJsonModbus.value(BAUD).toString();
                QString pItemDataBit = pJsonModbus.value(DATABIT).toString();
                QString pItemStopBit = pJsonModbus.value(STOPBIT).toString();
                QString pItemParity = pJsonModbus.value(PARITY).toString();

                g_Sepuyi = pJsonModbus.value(DEVICETYPE).toString();

                if(!m_pSerialCom1)
                {
                    m_pSerialCom1 = new Win_QextSerialPort(pItemName,QextSerialBase::Polling);
                }

                if(OpenCom(m_pSerialCom1,pItemName,pItemBaud,pItemDataBit,pItemStopBit,pItemParity))
                {

                    // 1.新建串口处理子线程
                    serialWorker1 = new SerialWorker(m_pSerialCom1);

                    // 将串口和子类一同移入子线程
                    serialWorker1->moveToThread(&serialThread_1);

                    // 2.连接信号和槽
                    connect(&serialThread_1, &QThread::finished,
                            serialWorker1, &QObject::deleteLater);           // 线程结束，自动删除对象
                    connect(&serialThread_1, SIGNAL(finished()),
                            &serialThread_1, SLOT(deleteLater()));
                    connect(this, &MainWindow::startWork1,
                            serialWorker1, &SerialWorker::doWork1);   // 主线程串口数据发送的信号

                    /* 接收到 worker 发送过来的信号 */
                    connect(serialWorker1, SIGNAL(resultReady(QString)),
                            this, SLOT(handleResults(QString)));
                }

                /* 判断线程是否在运行 */
                if(!serialThread_1.isRunning()) {
                    /* 开启线程 */
                    serialThread_1.start();
                }

                /* 发送正在运行的信号，线程收到信号后执行后返回线程耗时函数 + 此字符串 */
                emit this->startWork1();
            }

            if(pJsonDev.contains(PLC))
            {
                QJsonObject pJsonPLC = pJsonDev.value(PLC).toObject();

                QString pItemName = pJsonPLC.value(PORTNAME).toString();
                QString pItemBaud = pJsonPLC.value(BAUD).toString();
                QString pItemDataBit = pJsonPLC.value(DATABIT).toString();
                QString pItemStopBit = pJsonPLC.value(STOPBIT).toString();
                QString pItemParity = pJsonPLC.value(PARITY).toString();

                g_PLC = pJsonPLC.value(DEVICETYPE).toString();

                if(!m_pSerialCom2)
                {
                    m_pSerialCom2 = new Win_QextSerialPort(pItemName,QextSerialBase::Polling);
                }

                if(OpenCom(m_pSerialCom2,pItemName,pItemBaud,pItemDataBit,pItemStopBit,pItemParity))
                {
                    // 1.新建串口处理子线程
                    serialWorker2 = new SerialWorker(m_pSerialCom2);

                    // 将串口和子类一同移入子线程
                    serialWorker2->moveToThread(&serialThread_2);

                    // 2.连接信号和槽
                    connect(&serialThread_2, &QThread::finished,
                            serialWorker2, &QObject::deleteLater);           // 线程结束，自动删除对象
                    connect(&serialThread_2, SIGNAL(finished()),
                            &serialThread_2, SLOT(deleteLater()));
                    connect(this, &MainWindow::startWork2,
                            serialWorker2, &SerialWorker::doWork2);   // 主线程串口数据发送的信号

                    /* 接收到 worker 发送过来的信号 */
                    connect(serialWorker2, SIGNAL(resultReady(QString)),
                            this, SLOT(handleResults(QString)));
                }

                /* 判断线程是否在运行 */
                if(!serialThread_2.isRunning()) {
                    /* 开启线程 */
                    serialThread_2.start();
                }

                /* 发送正在运行的信号，线程收到信号后执行后返回线程耗时函数 + 此字符串 */
                emit this->startWork2();
            }
        }

        if(jsonObject.contains(PORTSUP))
        {
            QJsonObject pJsonDev = jsonObject.value(PORTSUP).toObject();

            if(pJsonDev.contains(HJ212))
            {
                QJsonObject pJsonForward1 = pJsonDev.value(HJ212).toObject();

                QString pItemName = pJsonForward1.value(PORTNAME).toString();
                QString pItemBaud = pJsonForward1.value(BAUD).toString();
                QString pItemDataBit = pJsonForward1.value(DATABIT).toString();
                QString pItemStopBit = pJsonForward1.value(STOPBIT).toString();
                QString pItemParity = pJsonForward1.value(PARITY).toString();

                g_Forward1 = pJsonForward1.value(DEVICETYPE).toString();

                if(!m_pSerialCom3)
                {
                    m_pSerialCom3 = new Win_QextSerialPort(pItemName,QextSerialBase::Polling);
                }

                if(OpenCom(m_pSerialCom3,pItemName,pItemBaud,pItemDataBit,pItemStopBit,pItemParity))
                {
                    // 1.新建串口处理子线程
                    serialWorker3 = new SerialWorker(m_pSerialCom3);

                    // 将串口和子类一同移入子线程
                    serialWorker3->moveToThread(&serialThread_3);

                    // 2.连接信号和槽
                    connect(&serialThread_3, &QThread::finished,
                            serialWorker3, &QObject::deleteLater);           // 线程结束，自动删除对象
                    connect(&serialThread_3, SIGNAL(finished()),
                            &serialThread_3, SLOT(deleteLater()));
                    connect(this, &MainWindow::startWork3,
                            serialWorker3, &SerialWorker::doWork3);   // 主线程串口数据发送的信号

                    /* 接收到 worker 发送过来的信号 */
                    connect(serialWorker3, SIGNAL(resultReady(QString)),
                            this, SLOT(handleResults(QString)));
                }

                /* 判断线程是否在运行 */
                if(!serialThread_3.isRunning()) {
                    /* 开启线程 */
                    serialThread_3.start();
                }

                /* 发送正在运行的信号，线程收到信号后执行后返回线程耗时函数 + 此字符串 */
                emit this->startWork3();
            }

            if(pJsonDev.contains(HJ212_2))
            {
                QJsonObject pJsonForward2 = pJsonDev.value(HJ212_2).toObject();

                QString pItemName = pJsonForward2.value(PORTNAME).toString();
                QString pItemBaud = pJsonForward2.value(BAUD).toString();
                QString pItemDataBit = pJsonForward2.value(DATABIT).toString();
                QString pItemStopBit = pJsonForward2.value(STOPBIT).toString();
                QString pItemParity = pJsonForward2.value(PARITY).toString();

                g_Forward2 = pJsonForward2.value(DEVICETYPE).toString();

                if(!m_pSerialCom4)
                {
                    m_pSerialCom4 = new Win_QextSerialPort(pItemName,QextSerialBase::Polling);
                }

                if(OpenCom(m_pSerialCom4,pItemName,pItemBaud,pItemDataBit,pItemStopBit,pItemParity))
                {
                    // 1.新建串口处理子线程
                    serialWorker4 = new SerialWorker(m_pSerialCom4);

                    // 将串口和子类一同移入子线程
                    serialWorker4->moveToThread(&serialThread_4);

                    // 2.连接信号和槽
                    connect(&serialThread_4, &QThread::finished,
                            serialWorker4, &QObject::deleteLater);           // 线程结束，自动删除对象
                    connect(&serialThread_4, SIGNAL(finished()),
                            &serialThread_4, SLOT(deleteLater()));
                    connect(this, &MainWindow::startWork4,
                            serialWorker4, &SerialWorker::doWork4);   // 主线程串口数据发送的信号

                    /* 接收到 worker 发送过来的信号 */
                    connect(serialWorker4, SIGNAL(resultReady(QString)),
                            this, SLOT(handleResults(QString)));
                }

                /* 判断线程是否在运行 */
                if(!serialThread_4.isRunning()) {
                    /* 开启线程 */
                    serialThread_4.start();
                }

                /* 发送正在运行的信号，线程收到信号后执行后返回线程耗时函数 + 此字符串 */
                emit this->startWork4();
            }
        }
    }
}

bool MainWindow::OpenCom(Win_QextSerialPort *m_pSerialCom,QString pItemName,QString pItemBaud,QString pItemDataBit,QString pItemStopBit,QString pItemParity)
{

    if (m_pSerialCom->open(QIODevice::ReadWrite))
    {
        qDebug()<<"打开串口成功...";

        m_pSerialCom->flush();

        if(pItemBaud == "600") m_pSerialCom->setBaudRate(BAUD600);
        else if(pItemBaud == "1200") m_pSerialCom->setBaudRate(BAUD1200);
        else if(pItemBaud == "2400") m_pSerialCom->setBaudRate(BAUD2400);
        else if(pItemBaud == "4800") m_pSerialCom->setBaudRate(BAUD4800);
        else if(pItemBaud == "1200") m_pSerialCom->setBaudRate(BAUD9600);
        else if(pItemBaud == "2400") m_pSerialCom->setBaudRate(BAUD19200);
        else if(pItemBaud == "4800") m_pSerialCom->setBaudRate(BAUD38400);
        else if(pItemBaud == "1200") m_pSerialCom->setBaudRate(BAUD57600);
        else if(pItemBaud == "2400") m_pSerialCom->setBaudRate(BAUD2400);
        else if(pItemBaud == "4800") m_pSerialCom->setBaudRate(BAUD115200);

        if(pItemDataBit == "5") m_pSerialCom->setDataBits(DATA_5);
        else if(pItemDataBit == "6") m_pSerialCom->setDataBits(DATA_6);
        else if(pItemDataBit == "7") m_pSerialCom->setDataBits(DATA_7);
        else if(pItemDataBit == "8") m_pSerialCom->setDataBits(DATA_8);

        if(pItemParity == "None") m_pSerialCom->setParity(PAR_NONE);
        else if(pItemParity == "Odd") m_pSerialCom->setParity(PAR_ODD);
        else if(pItemParity == "Even") m_pSerialCom->setParity(PAR_EVEN);
        else if(pItemParity == "Mark") m_pSerialCom->setParity(PAR_MARK);
        else if(pItemParity == "Space") m_pSerialCom->setParity(PAR_SPACE);

        if(pItemStopBit == "1") m_pSerialCom->setStopBits(STOP_1);
        else if(pItemStopBit == "1.5") m_pSerialCom->setStopBits(STOP_1_5);
        else if(pItemStopBit == "2") m_pSerialCom->setStopBits(STOP_2);

        m_pSerialCom->setFlowControl(FLOW_OFF); //设置数据流控制，我们使用无数据流控制的默认设置
        m_pSerialCom->setTimeout(500);  // 串口 超时 毫秒
    }

    return true;
}

void MainWindow::printFactors(QMap<QString,FactorInfo *> map)
{
    QMap<QString,FactorInfo *>::iterator it = map.begin();
    while(it!=map.end())
    {
        qDebug()<<__LINE__<<it.key()<<endl;
        it++;
    }

}


void MainWindow::Setting_Init()
{
    Setting_Check(FACTORS_SETTING_FILE);
    Setting_Check(COMM_SETTING_FILE);
    Setting_Check(SYSTEM_SETTING_FILE);
    Setting_Check(FAN_SETTING_FILE);
    Setting_Check(USERS_SETTING_FILE);
}

void MainWindow::Setting_Check(QString filename)
{
    QFile file(filename);
    if(!file.exists())
    {
        if(!file.open(QIODevice::ReadWrite)) {
            QLOG_INFO() << "创建配置文件失败！";
        }
        else
        {
            QLOG_INFO() << "创建配置文件成功！";
            QJsonObject jsonObject;
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();
        }
    }
}

void MainWindow::Widget_Init()
{

    ui->tableWidget->setFrameShape(QFrame::NoFrame);
    ui->tableWidget->verticalHeader()->hide();

    ui->pushButton_5->setAttribute(Qt::WA_Hover,true);
    ui->pushButton_5->installEventFilter(this);

    ui->pushButton_6->setAttribute(Qt::WA_Hover,true);
    ui->pushButton_6->installEventFilter(this);

    ui->pushButton_7->setAttribute(Qt::WA_Hover,true);
    ui->pushButton_7->installEventFilter(this);

    ui->pushButton_4->setAttribute(Qt::WA_Hover,true);
    ui->pushButton_4->installEventFilter(this);

    ui->pushButton_9->setAttribute(Qt::WA_Hover,true);
    ui->pushButton_9->installEventFilter(this);

    ui->pushButton_8->setAttribute(Qt::WA_Hover,true);
    ui->pushButton_8->installEventFilter(this);

    ui->pushButton_Set->setAttribute(Qt::WA_Hover,true);
    ui->pushButton_Set->installEventFilter(this);

    ui->pushButton_User->setAttribute(Qt::WA_Hover,true);
    ui->pushButton_User->installEventFilter(this);

    ui->label_User->setAttribute(Qt::WA_Hover,true);
    ui->label_User->installEventFilter(this);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->label_User)
    {
        if(event->type() == QEvent::HoverEnter)
        {
            QToolTip::showText(QCursor::pos(), "超级管理员"); // FIXME:
            return true;
        }
    }

    if(obj == ui->pushButton_4)
    {
        if(event->type() == QEvent::HoverEnter)
        {
            QToolTip::showText(QCursor::pos(), "更多操作");
            return true;
        }
    }

    if(obj == ui->pushButton_5)
    {
        if(event->type() == QEvent::HoverEnter)
        {
            QToolTip::showText(QCursor::pos(), "时间设定");
            return true;
        }
    }

    if(obj == ui->pushButton_6)
    {
        if(event->type() == QEvent::HoverEnter)
        {
            QToolTip::showText(QCursor::pos(), "历史数据");
            return true;
        }
    }

    if(obj == ui->pushButton_7)
    {
        if(event->type() == QEvent::HoverEnter)
        {
            QToolTip::showText(QCursor::pos(), "数据曲线");
            return true;
        }
    }

    if(obj == ui->pushButton_9)
    {
        if(event->type() == QEvent::HoverEnter)
        {
            QToolTip::showText(QCursor::pos(), "数据清空");
            return true;
        }
    }

    if(obj == ui->pushButton_8)
    {
        if(event->type() == QEvent::HoverEnter)
        {
            QToolTip::showText(QCursor::pos(), "校准模式");
            return true;
        }
    }

    if(obj == ui->pushButton_Set)
    {
        if(event->type() == QEvent::HoverEnter)
        {
            QToolTip::showText(QCursor::pos(), "参数设置");
            return true;
        }
    }

    if(obj == ui->pushButton_User)
    {
        if(event->type() == QEvent::HoverEnter)
        {
            QToolTip::showText(QCursor::pos(), "用户管理");
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
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
headerText << QStringLiteral("名称") << QStringLiteral("数值") << QStringLiteral("单位")
           << QStringLiteral("状态");
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

int widths[] = {85, 85, 85, 85};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget->setColumnWidth(i, widths[i]);
}

ui->tableWidget->setStyleSheet(qssTV);
ui->tableWidget->horizontalHeader()->setVisible(true);
ui->tableWidget->verticalHeader()->setDefaultSectionSize(45);
}

void MainWindow::setTableContents()
{


    ui->tableWidget->clearContents();
    QTableWidgetItem *pItem1=nullptr,*pItem2=nullptr,*pItem3=nullptr,*pItem4=nullptr;
    int numDisplayed = map_Factors.count();
    ui->tableWidget->setRowCount(numDisplayed);    // FIXME:

    int index = 0;
//    printFactors(map_Factors);

    while (index<seqlist.count()) {

        qDebug()<<__LINE__<<index<<endl;
        // qDebug() << iterator.key() << ":::::::" << iterator.value();

        FactorInfo* pItemInfo = facseqlist.at(index);

        pItem1 = new QTableWidgetItem(pItemInfo->m_name);
        pItem1->setTextAlignment(Qt::AlignCenter);
        pItem1->setFlags(Qt::ItemIsEditable);
        ui->tableWidget->setItem(index, 0, pItem1);

        pItem2 = new QTableWidgetItem(pItemInfo->m_value);
        //        pItem2 = new QTableWidgetItem(QString::number(num++));
        pItem2->setFlags(Qt::ItemIsEditable);
        pItem2->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(index, 1, pItem2);

        pItem3 = new QTableWidgetItem(pItemInfo->m_unit);
        pItem3->setFlags(Qt::ItemIsEditable);
        pItem3->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(index, 2, pItem3);

        pItem4 = new QTableWidgetItem(pItemInfo->m_state);
        pItem4->setFlags(Qt::ItemIsEditable);
        pItem4->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(index, 3, pItem4);

        index++;


    }
}

int MainWindow::countFactordisplay(QMap<QString,FactorInfo *> map)
{
    int numdisplay = 0;
    QMap<QString,FactorInfo *>::iterator it = map.begin();
    while(it!=map.end())
    {
        if(it.value()->m_display)
            numdisplay++;
        it++;
    }

    return numdisplay;
}

void MainWindow::HandleDateTimeout()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss");
    ui->label_2->setText(str);
}

void MainWindow::on_pushButton_3_clicked()
{
    this->close();
}

void MainWindow::on_pushButton_clicked()
{
    this->showMinimized();
}

void MainWindow::on_pushButton_4_clicked()
{
    QMessageBox::StandardButton result= msgBox::question(QStringLiteral("提示"), "是否退出登录");
    if(result != QMessageBox::Yes) return;

    //my_Process.startDetached("D:/QtWork/build-VocLogin-Desktop_Qt_5_12_12_MinGW_32_bit-Release/release/VocLogin.exe");
    this->close();
}

void MainWindow::on_pushButton_Set_clicked()
{

    ParamSet *paramSet = new ParamSet();
    paramSet->setWindowModality(Qt::ApplicationModal);
    paramSet->show();
}

bool MainWindow::datebaseinit()
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

void MainWindow::initLogger()
{
    // 1. 启动日志记录机制
    logger = &Logger::instance();
    editLogger();
}

void MainWindow::editLogger()
{
    QString dir_root = "D:/voc-log/";

    QDateTime date = QDateTime::currentDateTime();

    // 声明目录对象
    QString path = date.toString("yyyy-MM");
    QString file_name = date.toString("dd") + ".txt";

    QString dir_str = dir_root + path;

    QDir dir;
    if(!dir.exists(dir_str))
    {
        dir.mkpath(dir_str);
    }

    QString pDir_FileName = dir_str + "/" + file_name;

    QFile file(pDir_FileName);
    if(file.exists() == false)
    {
        // create file
        // 如果没有此文件，就创建
        file.open(QFile::WriteOnly|QFile::Text|QIODevice::Append);
    }

    logger->setLoggingLevel(QsLogging::InfoLevel);
    //设置log位置为exe所在目录
    const QString sLogPath(QDir(QCoreApplication::applicationDirPath()).filePath(pDir_FileName));

    // 2. 添加两个destination
    DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
      sLogPath, EnableLogRotation, MaxSizeBytes(10485760), MaxOldLogCount(0))); // 10Mb

    logger->addDestination(fileDestination);
}

//析构
void MainWindow::destroyLogger()
{
    QsLogging::Logger::destroyInstance();
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
