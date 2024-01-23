#include "widget.h"
#include "ui_widget.h"

QStringList availblePorts;
QJsonObject portsStateObj;
QMap<QString,SerialPort *> commMap;

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
    m_httpWorker->stopWork();
    m_thread.quit();
    /* 阻塞线程 2000ms，判断线程是否结束 */
    if (m_thread.wait(2000)) {
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

}

void Widget::page_deviceAddInit()
{

}

void Widget::page_deviceLogInit()
{

}


void Widget::device_init()
{
    facnameMap = util.Uart_facnameMatch();

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

    m_httpWorker->sendResultReady();

    /* 判断线程是否在运行 */
    if(!m_thread.isRunning()) {
        /* 开启线程 */
        m_thread.start();
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
headerText << QStringLiteral("连接状态")  << QStringLiteral("因子名称") << QStringLiteral("端口名")<< QStringLiteral("操作");
int cnt = headerText.count();
ui->device_table->setColumnCount(cnt);
ui->device_table->setHorizontalHeaderLabels(headerText);
// ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->device_table->horizontalHeader()->setStretchLastSection(true); //行头自适应表格
int widths[] = {120, 200, 200,230};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->device_table->setColumnWidth(i, widths[i]);
}

ui->device_table->horizontalHeader()->setFont(headerfont);
ui->device_table->setStyleSheet(qssTV);
ui->device_table->horizontalHeader()->setVisible(true);
ui->device_table->verticalHeader()->setVisible(false);
ui->device_table->setFrameShape(QFrame::NoFrame);
ui->device_table->setTextElideMode(Qt::ElideNone);
ui->device_table->resizeRowsToContents();

}

void Widget::setTableContent(const QJsonObject &pDevice,const QJsonObject &pFactor)
{
    qDebug()<<__LINE__<<"Device:"<<pDevice<<endl;
    qDebug()<<__LINE__<<"Factor:"<<pFactor<<endl;
    ui->device_table->setRowCount(0);

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
//                QString modbus_index = QString::number(fac_info.value("modbus_index").toInt());
//                qDebug()<<__LINE__<<fac_FullName<<modbus_index<<endl;
                ui->device_table->insertRow(row);

                //state
                QPixmap pixmap(STATE_OFF);
                QLabel *itemState = new QLabel();
                itemState->setPixmap(pixmap.scaled(20,20));
                itemState->setAlignment(Qt::AlignCenter);
                ui->device_table->setCellWidget(row,0,itemState);


                //name
                QTableWidgetItem *itemName = new QTableWidgetItem();
                itemName->setFont(itemfont);
                itemName->setText(fac_FullName);
                itemName->setTextAlignment(Qt::AlignCenter);
                ui->device_table->setItem(row,1,itemName);

                //port
                QTableWidgetItem *itemPort = new QTableWidgetItem();
                itemPort->setFont(itemfont);
                itemPort->setText(device_com);
                itemPort->setTextAlignment(Qt::AlignCenter);
                ui->device_table->setItem(row,2,itemPort);

//                //modbus_index
//                QTableWidgetItem *itemIndex = new QTableWidgetItem();
//                itemIndex->setFont(itemfont);
//                itemIndex->setText(modbus_index);
//                itemIndex->setTextAlignment(Qt::AlignCenter);
//                ui->device_table->setItem(row,3,itemIndex);

                //operate





            }

            row++;
            itFactor++;
        }

        itDevice++;
    }
}




//slot function
void Widget::handleResults(const QJsonObject &pDevice,const QJsonObject &pFactor)
{

    setTableContent(pDevice,pFactor);

}

void Widget::handleDateTimeout()
{

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

        portsStateObj = checkPortState(availblePorts);


        emit resultReady(pDevice,pFactor);


        /* 使用QThread里的延时函数，当作一个普通延时 */
        QThread::sleep(REALTIME_FLUSH_PERIOD);



    }
}

