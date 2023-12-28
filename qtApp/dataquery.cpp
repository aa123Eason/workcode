#include "dataquery.h"
#include "ui_dataquery.h"

DataQuery::DataQuery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataQuery)
{
    ui->setupUi(this);
    connectevent();
    init();
}

DataQuery::~DataQuery()
{
    delete ui;
}

void DataQuery::init()
{
    ui->ck_rtk->setChecked(true);
}

void DataQuery::connectevent()
{
    connect(ui->ck_rtk,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            ui->tableStack->setCurrentWidget(ui->pagertk);
        }
    });
    connect(ui->ck_min,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            ui->tableStack->setCurrentWidget(ui->pagemin);
        }
    });
    connect(ui->ck_hour,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            ui->tableStack->setCurrentWidget(ui->pagehour);
        }
    });
    connect(ui->ck_day,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            ui->tableStack->setCurrentWidget(ui->pageday);
        }
    });
    connect(ui->ck_month,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            ui->tableStack->setCurrentWidget(ui->pagemonth);
        }
    });
    connect(ui->ck_trans,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            ui->tableStack->setCurrentWidget(ui->pagetrans);
        }
    });

    connect(ui->tableStack,&QStackedWidget::currentChanged,this,[=](int index)
    {
        qDebug()<<__LINE__<<ui->tableStack->widget(index)->objectName()<<endl;
    });



}


