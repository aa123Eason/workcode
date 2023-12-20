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
    QDateTime defaultSDT1 = defaultSDT.addDays(1);

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

    QMap<QString,QCheckBox *>::iterator it = legendBoxMap.begin();
    int index = 0;
    while(it!=legendBoxMap.end())
    {
        if(it.value()!=nullptr)
        {
            connect(it.value(),&QCheckBox::stateChanged,this,[=](int state)
            {
                const auto markers = chart->legend()->markers();
                markers.at(index)->series()->setVisible(state);
            });
        }
        index++;
        it++;
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


    if(!curdb.isOpen())
    {
        QMessageBox::warning(this,"查询错误提示","查询失败，本地数据库异常");
        return;
    }
    //拼接查询语句
    QString queStr = "select HistoryTime,";
    for(QString fac:gFactorsNameList)
    {
        QString realName = HistoryChartView::facNameMap().key(fac);
        qDebug()<<__LINE__<<fac<<"--"<<realName<<endl;
        queStr += realName;
        if(fac != gFactorsNameList.last())
            queStr += ",";

    }
    queStr += " from "+tableName+" where HistoryTime between \'";
    queStr += rangeStartDT + "\' and \'"+ rangeEndDT + "\';";

    qDebug()<<__LINE__<<queStr<<endl;

    QSqlQuery query(queStr);
    query.exec();
    QSqlRecord records = query.record();

    qDebug()<<__LINE__<<"result size:"<<records.count()<<endl;

    int num = 0;
    while(query.next())
    {
        for(QString fac:gFactorsNameList)
        {
            QString realName = HistoryChartView::facNameMap().key(fac);
            QString facRecord = query.value(realName).toString();
//            if(facRecord.isEmpty())
//                facRecord = QString::number(0.00,'f');

            mapQueryResult[realName]<< facRecord.toDouble();

        }

        if(num < records.count())
        {
            dateTimeList<<query.value("HistoryTime").toString();
        }
        num++;
    }

    qDebug()<<__LINE__<<mapQueryResult<<endl;
    qDebug()<<__LINE__<<dateTimeList<<endl;
    paintCharts();


}

void HistoryChartView::paintCharts()
{
    qreal minValue =-20,maxValue = 100;
    int xMax = mapQueryResult.count();
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

    m_axisX->setTickCount(10);
    m_axisX->setRange(0,xMax);


    m_axisY->setRange(minValue,maxValue);

    int xvalue = 0;
    int index = 0;

    for(QList<qreal> valuelist:mapQueryResult.values())
    {

        QSplineSeries *series = new QSplineSeries(this);
        QString realName = facNameMap()[mapQueryResult.keys()[index]];
        series->setName(realName);
        QPen pen;
        QColor color = randomColor();
        pen.setColor(color);
        QTime time= QTime::currentTime();
        qsrand(time.msec()+time.second()*1000);
        pen.setWidthF(qrand()%5);
        series->setPen(pen);

        for(qreal value:valuelist)
        {
            series->append(xvalue++,value);

        }

        series->attachAxis(m_axisX);
        series->attachAxis(m_axisY);

        chart->addSeries(series);
        chart->addAxis(m_axisX,Qt::AlignBottom);
        chart->addAxis(m_axisY,Qt::AlignLeft);
        index++;
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
            box->setChecked(Qt::Checked);
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
