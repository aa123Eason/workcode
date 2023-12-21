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

//    QDateTime defaultSDT = QDateTime::currentDateTime();
    QDateTime defaultSDT = QDateTime::fromString("2023-07-11 03:00","yyyy-MM-dd HH:mm");
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

    m_axisX = new QValueAxis();
    m_axisY = new QValueAxis();
    m_axisX->setGridLineVisible(true);
    m_axisY->setGridLineVisible(true);

    QChartView *mainChartView = new QChartView(chart,ui->chartWidget);
    mainChartView->setRenderHint(QPainter::Antialiasing);
//    qDebug()<<__LINE__<<ui->chartWidget->width()<<ui->chartWidget->height()<<endl;
    mainChartView->setFixedSize(1440,960);
//    mainChartView->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    mainChartView->setRubberBand(QChartView::VerticalRubberBand);

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
    //获取需要查询的表名和日期时间格式
    if(curDTType == "分钟均值")
    {
        tableName = "T_History_Minute";
        queryFormat = "yyyy-MM-dd HH:mm";
        chart->setTitle("历史分钟数据趋势");
    }
    else if(curDTType == "小时均值")
    {
        tableName = "T_History_Hour";
        queryFormat = "yyyy-MM-dd HH";
        chart->setTitle("历史小时数据趋势");
    }
    else if(curDTType == "日均值")
    {
        tableName = "T_History_Day";
        queryFormat = "yyyy-MM-dd";
        chart->setTitle("历史日数据趋势");
    }
    else if(curDTType == "月均值")
    {
        tableName = "T_History_Month";
        queryFormat = "yyyy-MM";
        chart->setTitle("历史月数据趋势");

    }
        rangeStartDT = dt_begin.toString(queryFormat);
        rangeEndDT = dt_end.toString(queryFormat);

        //获取勾选的因子复选框
        selBoxMap.clear();
        QMap<QString,QCheckBox *>::iterator it = legendBoxMap.begin();
        while(it!=legendBoxMap.end())
        {
            if(it.value()!=nullptr)
            {
                if(it.value()->isChecked())
                {
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
    for(QList<qreal> valuelist:mapQueryResult.values())
    {
        for(qreal value:valuelist)
        {
            if(minValue>value)
                minValue = value;
            if(maxValue<value)
                maxValue = value;
        }
    }

    m_axisX->setRange(0,xMax);
    m_axisY->setRange(minValue,maxValue);



    qDebug()<<__LINE__<< minValue<<","<< maxValue<<endl;

    int xvalue = 0;

    QMap<QString,QList<qreal>>::iterator it = mapQueryResult.begin();
    QList<QLineSeries*> serieslist;
    while(it!=mapQueryResult.end())
    {
        QLineSeries *series = new QLineSeries(this);
        QString realName = facNameMap()[it.key()];
        series->setName(realName);

        QPen pen(randomColor());
        pen.setWidth(2);
        series->setPen(pen);

        QList<qreal> values = it.value();
        for(double value:values)
        {
            qDebug()<<__LINE__<<"("<<xvalue<<","<<value<<")"<<endl;
            series->append(xvalue,value);
            xvalue++;
        }




        chart->addAxis(m_axisX,Qt::AlignBottom);
        chart->addAxis(m_axisY,Qt::AlignLeft);


        series->attachAxis(m_axisX);
        series->attachAxis(m_axisY);
        serieslist.append(series);

        it++;
    }


    for(auto series:serieslist)
    {
        chart->addSeries(series);

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
