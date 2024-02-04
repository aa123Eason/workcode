#include "historychartview.h"
#include "ui_historychartview.h"
#include "mainwindow.h"

HistoryChartView::HistoryChartView(QSqlDatabase &db,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistoryChartView)
{
    ui->setupUi(this);

    widgetInit();
    getLocalDB(db);
    chartInit();
    connectevent();
}

HistoryChartView::~HistoryChartView()
{
    delete ui;

    if(curdb.isOpen())
        curdb.close();
}

void HistoryChartView::widgetInit()
{
    this->setWindowState(Qt::WindowMaximized);

    QDateTime defaultSDT = QDateTime::currentDateTime();
//    QDateTime defaultSDT = QDateTime::fromString("2023-07-11 03:00","yyyy-MM-dd HH:mm");
    QDateTime defaultSDT1 = defaultSDT.addSecs(3600);
    ui->startDT->setDateTime(defaultSDT);
    ui->endDT->setDateTime(defaultSDT1);



}

void HistoryChartView::chartInit()
{
    chart = new QChart();
    chart->setTitle("历史数据趋势");
//    chart->legend()->hide();
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->legend()->setMarkerShape(QLegend::MarkerShapeRectangle);

    QFont font;
    font.setPointSize(10);
    font.setBold(true);
    font.setFamily("微软雅黑");
    chart->legend()->setFont(font);
    chart->legend()->resize(50,400);

    m_axisX = new QValueAxis(chart);
    m_axisY = new QValueAxis(chart);
    m_axisX->setGridLineVisible(true);
    m_axisY->setGridLineVisible(true);

    QChartView *mainChartView = new QChartView(chart,ui->chartWidget);
    mainChartView->setRenderHint(QPainter::Antialiasing);
    qDebug()<<__LINE__<<this->width()<<this->height()<<endl;
    mainChartView->setFixedSize(800,600);
    mainChartView->setAlignment(Qt::AlignCenter);
    mainChartView->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    mainChartView->setRubberBand(QChartView::VerticalRubberBand);
//    chart->resize(mainChartView->width(),mainChartView->height());
    mainChartView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QHBoxLayout layout;
    layout.addWidget(mainChartView);
    layout.setMargin(2);
    ui->chartWidget->setLayout(&layout);

}

void HistoryChartView::getLocalDB(QSqlDatabase &db)
{
    if(!db.isOpen())
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(QApplication::applicationDirPath()+"/VocGas.db");
        db.open();
    }
    curdb = db;
    qDebug()<<__LINE__<<curdb.databaseName()<<curdb.isOpen()<<endl;

}

void HistoryChartView::connectevent()
{
    connect(ui->queryDTType,&QComboBox::currentTextChanged,this,&HistoryChartView::on_comboboxDTType_currentChanged);
    connect(ui->queryBtn,&QPushButton::clicked,this,&HistoryChartView::on_pushbutton_query);

    MainWindow *w = (MainWindow *)qobject_cast<MainWindow *>(sender());
//    Q_ASSERT(w);
    if(w!=nullptr)
    {
        connect(w,&MainWindow::sendGlobalMapAndList,this,&HistoryChartView::onReceiveGlobalMapAndList);
    }

    connect(this,&HistoryChartView::sendlogmsg,this,&HistoryChartView::onPrintlog);


}

QString HistoryChartView::loadStyleSheet(QString qsspath)
{
    QString qssStr;
    QFile file(qsspath);
    if(file.open((QIODevice::ReadOnly|QIODevice::Text)))
    {
        QByteArray bytArr;
        bytArr.append(file.readAll());
        qssStr = bytArr;
        bytArr.clear();
    }
    file.close();
    return qssStr;
}

QMap<QString,QString> HistoryChartView::facNameMap()
{
    QMap<QString,QString> nameMap;
    //一般值
    nameMap["THC"] = "总烃";
    nameMap["NMHC"] = "非甲烷总烃";
    nameMap["Methane"] = "甲烷";
    nameMap["FlueGasTemp"] = "烟气温度";
    nameMap["FlueGasPress"] = "烟气压力";
    nameMap["FlueGasVelocity"] = "烟气流速";
    nameMap["WorkingFlow"] = "工况流量";
    nameMap["StandardFlow"] = "标况流量";
    nameMap["BenSeriesContent"] = "苯系物含量";
    nameMap["NMHCConverted"] = "折算非甲烷总烃";
    nameMap["NMHCEmiss"] = "非甲烷总烃排放量";
    nameMap["DustConcentration"] = "烟尘湿值";
    nameMap["DustConcentrationEmiss"] = "烟尘排放量";
    nameMap["OxygenContent"] = "氧气含量";
    nameMap["FlueGasHumidity"] = "烟气湿度";
    nameMap["HydrogenSulfide"] = "硫化氢";
    nameMap["BenContent"] = "苯含量";
    nameMap["Toluene"] = "甲苯";
    nameMap["Mxylene"] = "间二甲苯";
    nameMap["Oxylene"] = "邻二甲苯";
    nameMap["yiben"] = "乙苯";
    nameMap["benyixi"] = "苯乙烯";


    //干值
    nameMap["THC1"] = "总烃干值";
    nameMap["NMHC1"] = "非甲烷总烃干值";
    nameMap["Methane1"] = "甲烷干值";
    nameMap["NMHCConverted1"] = "折算非甲烷总烃干值";
    nameMap["HydrogenSulfide1"] = "硫化氢干值";
    nameMap["OxygenContent1"] = "氧气含量干值";
    nameMap["DustDryValue"] = "烟尘干值";

    return nameMap;
}

//slot function
void HistoryChartView::on_comboboxDTType_currentChanged(const QString &text)
{
    qDebug()<<__LINE__<<"当前选择项："<<text<<endl;
}

void HistoryChartView::on_pushbutton_query()
{
    qDebug()<<__LINE__<<"开始查询"<<endl;
    QString curDTType = ui->queryDTType->currentText();
    QString tableName;
    QDateTime dt_begin = ui->startDT->dateTime();
    QDateTime dt_end = ui->endDT->dateTime();


    if(dt_begin>=dt_end)
    {
        QMessageBox::warning(this,"查询错误提示","查询时间有误，起始时间必须早于终止时间");
        return;
    }

    QString rangeStartDT,rangeEndDT;
    int lendt = dt_begin.secsTo(dt_end);
    int reflen;
    //获取需要查询的表名和日期时间格式
    if(curDTType == "分钟均值")
    {
        reflen = lendt/60;
        tableName = "T_History_Minute";
        queryFormat = "yyyy-MM-dd HH:mm";
        chart->setTitle("历史分钟数据趋势");
    }
    else if(curDTType == "小时均值")
    {
        reflen = lendt/3600;
        tableName = "T_History_Hour";
        queryFormat = "yyyy-MM-dd HH";
        chart->setTitle("历史小时数据趋势");
    }
    else if(curDTType == "日均值")
    {
        reflen = dt_begin.daysTo(dt_end);
        tableName = "T_History_Day";
        queryFormat = "yyyy-MM-dd";
        chart->setTitle("历史日数据趋势");
    }
    else if(curDTType == "月均值")
    {
        reflen = dt_begin.daysTo(dt_end)/30;
        tableName = "T_History_Month";
        queryFormat = "yyyy-MM";
        chart->setTitle("历史月数据趋势");

    }
        rangeStartDT = dt_begin.toString(queryFormat);
        rangeEndDT = dt_end.toString(queryFormat);
        emit sendlogmsg("历史曲线窗口——查询起始时间："+rangeStartDT);
        emit sendlogmsg("历史曲线窗口——查询结束时间："+rangeEndDT);
        emit sendlogmsg("历史曲线窗口——查询方式："+curDTType);
        emit sendlogmsg("历史曲线窗口——查询表名："+tableName);

        //获取勾选的因子复选框
        selBoxMap.clear();
        QMap<QString,QCheckBox *>::iterator it = legendBoxMap.begin();
        while(it!=legendBoxMap.end())
        {
            if(it.value()!=nullptr)
            {
                if(it.value()->isChecked())
                {
                    emit sendlogmsg("历史曲线窗口——勾选查询因子:"+it.key());
                    selBoxMap[it.key()]=it.value();
                }

            }
            it++;
        }

        qDebug()<<__LINE__<<selBoxMap.keys()<<endl;



        if(!curdb.isOpen())
        {
            QMessageBox::warning(this,"查询错误提示","查询失败，本地数据库异常");
            return;
        }

//        if(curDTType == "分钟均值")
//        {
        qDebug()<<__LINE__<<reflen<<endl;
        if(reflen>1440)
        {
            QMessageBox::warning(this,"提示","查询量过大，会拖慢运行速度，建议缩短时间范围");
            return;
        }

//        }

        //拼接查询语句
        QString queStr = "select ";
        qDebug()<<"==================x"<<endl;
        for(QString fac:selBoxMap.keys())
        {
            QString realName = HistoryChartView::facNameMap().key(fac);
            qDebug()<<__LINE__<<fac<<"--"<<realName<<endl;
            queStr += realName;
            if(fac != selBoxMap.keys().last())
                queStr += ",";

        }
        qDebug()<<"==================xx"<<endl;
        queStr += " from "+tableName+" where HistoryTime between \'";
        queStr += rangeStartDT + "\' and \'"+ rangeEndDT + "\';";

        qDebug()<<__LINE__<<queStr<<endl;
        emit sendlogmsg("历史曲线窗口——查询sql命令:"+queStr);

        QSqlQuery query(queStr);
        query.exec();
        QSqlRecord records = query.record();

        qDebug()<<__LINE__<<"result size:"<<records.count()<<endl;


        int num0 = 0;
        mapQueryResult.clear();
        while(query.next())
        {
            qDebug()<<__LINE__<<query.record().count()<<endl;
            for(QString fac:selBoxMap.keys())
            {
                QString realName = HistoryChartView::facNameMap().key(fac);
                qDebug()<<__LINE__<<fac<<"["<<num0<<"]"<<query.value(realName).toString()<<endl;
                QString realValue = query.value(realName).toString();
                qDebug()<<__LINE__<<realValue<<endl;

                mapQueryResult[realName].append(realValue.toDouble());
            }
            num0++;
        }

        qDebug()<<__LINE__<<mapQueryResult<<endl;


        paintCharts();

        connectMarkers();

}

void HistoryChartView::paintCharts()
{
    qDebug()<<__LINE__<<chart->series().count()<<endl;


    chart->removeAllSeries();
    chart->legend()->markers().clear();

    double minValue =-50,maxValue = 1000;
    if(mapQueryResult.values().count()==0)
        return;
    int xMax = mapQueryResult.values()[0].count();
    if(xMax==0)
        return;
//    for(QList<qreal> valuelist:mapQueryResult.values())
//    {
//        for(qreal value:valuelist)
//        {
//            if(minValue<value)
//                minValue = value;
//            if(maxValue<value)
//                maxValue = value;
//        }
//    }

    m_axisX->setRange(0,xMax);

    QMap<QString,QList<qreal>>::iterator it = mapQueryResult.begin();

    while(it!=mapQueryResult.end())
    {
        QLineSeries *series = new QLineSeries(this);
        QString realName = facNameMap()[it.key()];
        series->setName(realName);
        chart->addSeries(series);

        chart->addAxis(m_axisX,Qt::AlignBottom);
        chart->addAxis(m_axisY,Qt::AlignLeft);

        QPen pen(randomColor());
        pen.setWidth(2);
        series->setPen(pen);
        int xvalue = 0;
        QList<qreal> values = it.value();
        for(double value:values)
        {
            if(minValue>value)
                minValue = value;

            if(maxValue<value)
                maxValue = value;

            qDebug()<<__LINE__<<realName<<"("<<xvalue<<","<<value<<")"<<endl;

            series->append(xvalue,value);
            xvalue++;
        }

        qDebug()<<__LINE__<< minValue<<","<< maxValue<<endl;
        m_axisY->setRange(minValue, maxValue);


        series->attachAxis(m_axisX);
        series->attachAxis(m_axisY);


        it++;
    }










}

QColor HistoryChartView::randomColor()
{
    QTime time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    int r = 255-qrand() % 256;
    int g = qrand() % 256;
    int b = qrand() % 256;

    return QColor(r,g,b);
}

void HistoryChartView::connectMarkers()
{
    const auto markers = chart->legend()->markers();
    for(QLegendMarker *marker:markers)
    {
        QObject::disconnect(marker,&QLegendMarker::clicked,this,&HistoryChartView::handleMarkersClicked);
        QObject::connect(marker,&QLegendMarker::clicked,this,&HistoryChartView::handleMarkersClicked);

    }
}

void HistoryChartView::disconnectMarkers()
{
    const auto markers = chart->legend()->markers();
    for(QLegendMarker *marker:markers)
    {
        QObject::disconnect(marker,&QLegendMarker::clicked,this,&HistoryChartView::handleMarkersClicked);
    }
}

void HistoryChartView::onReceiveGlobalMapAndList(QStringList &g_FactorsNameList,QMap<QString, FactorInfo*> &map_Factors)
{
    gFactorsNameList = g_FactorsNameList;
    mapFactors = map_Factors;

    qDebug()<<__LINE__<<gFactorsNameList<<endl;

    for(QString fac:gFactorsNameList)
    {

        if(!legendBoxMap.contains(fac))
        {
            QCheckBox *box = new QCheckBox(fac);
            box->setCheckable(true);
            box->setChecked(Qt::Unchecked);
            QString checkStyleSheet = loadStyleSheet(":/images/checkboxstyle.qss");
            box->setStyleSheet(checkStyleSheet);
            QFont font;
            font.setPointSize(14);
            font.setFamily("微软雅黑");
            font.setBold(true);
            box->setFont(font);

            legendBoxMap.insert(fac,box);
        }

    }

    QMap<QString,QCheckBox *>::iterator itbegin = legendBoxMap.begin();
    while(itbegin != legendBoxMap.end())
    {
        if(itbegin.value()!=nullptr)
        {
             layout->addWidget(itbegin.value(),1);
        }

        itbegin++;
    }

    ui->groupBox->setLayout(layout);
}

void HistoryChartView::handleMarkersClicked()
{
    QLegendMarker *marker = qobject_cast<QLegendMarker *>(sender());
    Q_ASSERT(marker);

    switch(marker->type())
    {
    case QLegendMarker::LegendMarkerTypeXY:
    {
        marker->series()->setVisible(!marker->series()->isVisible());
        marker->setVisible(true);

        qreal alpha = 1.0;
        if(!marker->series()->isVisible())
        {
            alpha = 0.5;
        }
            QColor color;
            QBrush brush = marker->labelBrush();
            color = brush.color();
            color.setAlphaF(alpha);
            brush.setColor(color);
            marker->setLabelBrush(brush);
            brush = marker->brush();
            color = brush.color();
            color.setAlphaF(alpha);
            brush.setColor(color);
            marker->setBrush(brush);
            QPen pen = marker->pen();
            color = pen.color();
            color.setAlphaF(alpha);
            pen.setColor(color);
            marker->setPen(pen);

            break;


    }
    default:
        break;
    }
}

void HistoryChartView::onPrintlog(QString msg)
{

    QString str = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz")+ msg;
    QString txt = "\r\n"+str+"";
    QLOG_INFO() << txt;
    QString dir_root = QApplication::applicationDirPath()+"/"+LOG_PATH;

    // 声明目录对象
    QString path_root = QDateTime::currentDateTime().date().toString(QLatin1String("yyyy-MM"));
    QString file_name = QDateTime::currentDateTime().date().toString(QLatin1String("dd")) + ".txt";

    QString dir_str = dir_root + path_root;
    QString pDir_FileName = dir_str + "/" + file_name;
    QFile file(pDir_FileName);
    QByteArray array;
    array.append(txt);
    file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);
    if(file.waitForBytesWritten(3000))
        file.write(array,array.length());
    else
        file.flush();
    file.close();
}
