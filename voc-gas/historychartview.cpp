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
}

HistoryChartView::~HistoryChartView()
{
    delete ui;

    if(curdb.isOpen())
        curdb.close();
}

void HistoryChartView::widgetInit()
{
    QDateTime defaultSDT = QDateTime::currentDateTime();
    QDateTime defaultSDT1 = defaultSDT.addDays(1);

    ui->startDT->setDateTime(defaultSDT);
    ui->endDT->setDateTime(defaultSDT1);

    qDebug()<<__LINE__<<gFactorsNameList<<endl;

    for(QString fac:gFactorsNameList)
    {

        if(!legendBoxMap.contains(fac))
        {
            QCheckBox *box = new QCheckBox(fac);
            box->setCheckable(true);
            box->setChecked(Qt::Checked);
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

void HistoryChartView::chartInit()
{
    QChart *chart = new QChart();
    chart->setTitle("历史数据趋势");
    chart->legend()->hide();


    QChartView *mainChartView = new QChartView(chart,ui->chartWidget);
    mainChartView->setRenderHint(QPainter::Antialiasing);
    mainChartView->setFixedSize(500,400);
    mainChartView->setRubberBand(QChartView::VerticalRubberBand);
}

void HistoryChartView::getLocalDB(QSqlDatabase &db)
{
    if(!db.isOpen())
    {
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(QApplication::applicationDirPath()+"/VocGas.db");
    }
    curdb = db;
    qDebug()<<__LINE__<<curdb.databaseName()<<curdb.isOpen()<<endl;

}

void HistoryChartView::connectevent()
{
    connect(ui->queryDTType,&QComboBox::currentTextChanged,this,&HistoryChartView::on_comboboxDTType_currentChanged);
    connect(ui->queryBtn,&QPushButton::clicked,this,&HistoryChartView::on_pushbutton_query);
}

//slot function
void HistoryChartView::on_comboboxDTType_currentChanged(const QString &text)
{
    qDebug()<<__LINE__<<"当前选择项："<<text<<endl;
}

void HistoryChartView::on_pushbutton_query()
{
    qDebug()<<__LINE__<<"开始查询"<<endl;
}

void HistoryChartView::onReceiveGlobalMapAndList(QStringList &g_FactorsNameList,QMap<QString, FactorInfo*> &map_Factors)
{
    gFactorsNameList = g_FactorsNameList;
    mapFactors = map_Factors;
}
