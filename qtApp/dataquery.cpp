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
    QDateTime dt_end = QDateTime::currentDateTime();
    QDateTime dt_start = dt_end.addSecs(-60);
    ui->startdt->setDateTime(dt_start);
    ui->enddt->setDateTime(dt_end);



    getFactors();

    if(ui->factorname->count())
    {
        ui->factorname->setCurrentIndex(0);
    }

    getUpsetAddr();

    if(ui->deviceaddr->count())
    {
        ui->deviceaddr->setCurrentIndex(0);
    }

}

void DataQuery::connectevent()
{
    connect(ui->ck_rtk,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            ui->tableStack->setCurrentWidget(ui->pagertk);
            ui->deviceaddr->hide();
            ui->factorname->show();
            ui->tablertk->clearContents();
        }
    });
    connect(ui->ck_min,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            ui->tableStack->setCurrentWidget(ui->pagemin);
            ui->deviceaddr->hide();
            ui->factorname->show();
            ui->tablemin->clearContents();
        }
    });
    connect(ui->ck_hour,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            ui->tableStack->setCurrentWidget(ui->pagehour);
            ui->deviceaddr->hide();
            ui->factorname->show();
            ui->tablehour->clearContents();
        }
    });
    connect(ui->ck_day,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            ui->tableStack->setCurrentWidget(ui->pageday);
            ui->deviceaddr->hide();
            ui->factorname->show();
            ui->tableday->clearContents();
        }
    });
    connect(ui->ck_month,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            ui->tableStack->setCurrentWidget(ui->pagemonth);
            ui->deviceaddr->hide();
            ui->factorname->show();
            ui->tablemonth->clearContents();
        }
    });
    connect(ui->ck_trans,&QCheckBox::stateChanged,this,[=](int state)
    {
        if(state)
        {
            ui->tableStack->setCurrentWidget(ui->pagetrans);
            ui->deviceaddr->show();
            ui->factorname->hide();
            ui->tabletrans->clearContents();
        }
    });

    connect(ui->tableStack,&QStackedWidget::currentChanged,this,[=](int index)
    {
        qDebug()<<__LINE__<<ui->tableStack->widget(index)->objectName()<<endl;
    });


    connect(ui->query,&QPushButton::clicked,this,&DataQuery::onQuery);
//    connect(ui->btn_export,&QPushButton::clicked,this,&DataQuery::onExport);
    connect(ui->factorname,&QComboBox::currentTextChanged,this,[=](const QString &text)
    {
        qDebug()<<__LINE__<<text<<endl;
    });
}

void DataQuery::onQuery()
{
    qDebug()<<__LINE__<<"查询:"<<ui->factorname->currentText();
    QString curFac = ui->factorname->currentText();
    if(ui->startdt->dateTime()>=ui->enddt->dateTime())
    {
        QMessageBox::warning(this,"提示","查询失败，起始日期必须早于终止日期！");
        return;
    }

    if(ui->ck_rtk->isChecked())
    {
        qDebug()<<"实时数据"<<endl;
        query_rtk();
    }
    else if(ui->ck_min->isChecked())
    {
        qDebug()<<"分钟数据"<<endl;
        query_min();
    }
    else if(ui->ck_hour->isChecked())
    {
        qDebug()<<"小时数据"<<endl;
        query_hour();
    }
    else if(ui->ck_day->isChecked())
    {
        qDebug()<<"日数据"<<endl;
        query_day();
    }
    else if(ui->ck_month->isChecked())
    {
        qDebug()<<"月数据"<<endl;
        query_month();
    }
    else if(ui->ck_trans->isChecked())
    {
        qDebug()<<"传输报文"<<endl;
        query_trans();
    }
}

void DataQuery::onExport()
{

}

void DataQuery::query_rtk()
{
    //查询
    qDebug()<<__LINE__<<__FUNCTION__<<endl;
    QString curFac = ui->factorname->currentText();
    QString fac_code;
    QString dtstr1 = ui->startdt->dateTime().toString("yyyy-MM-dd HH:mm");
    QString dtstr2 = ui->enddt->dateTime().toString("yyyy-MM-dd HH:mm");

    if(curFac.split("-").count()==2)
    {
        fac_code = curFac.split("-")[0];
    }

    QString querystr = "/dcm/history/realtime_data?factor_code=";
    HttpClient httpClient;
    QJsonObject resObj;
    if(!fac_code.isEmpty())
    {
        QJsonObject dataObj;
        dataObj.insert("page_size",20);
        dataObj.insert("pages_index",0);
        dataObj.insert("since",dtstr1);
        dataObj.insert("until",dtstr2);
        //dcm/history/realtime_data?factor_code=a01012
        querystr += fac_code;
        qDebug()<<__LINE__<<querystr<<endl;
        if(httpClient.asynpostdata(querystr,dataObj,resObj))
        {
            QMessageBox::about(this,"提示","查询成功！");
            qDebug()<<__LINE__<<resObj<<endl;

            //填表
            fillinTable(resObj,ui->tablertk);

        }
        else
        {
            QMessageBox::warning(this,"提示","查询无效，无法从远端post数据！");
        }

    }





    qDebug()<<__LINE__<<querystr<<endl;
}

void DataQuery::fillinTable(QJsonObject &resObj,,QTableWidget * table = nullptr)
{
//    ui->pages->setText(QString::number(resObj.value("pages").toInt()));
//    ui->curPage->setText(QString::number(1));
    if(table==nullptr)
        return;
    //表头
    table->horizontalHeader()->hide();
    table->verticalHeader()->hide();
    table->setColumnCount(4);
    table->insertRow(0);
    table->insertRow(1);
    table->setSpan(0,0,2,1);
    table->setSpan(0,1,1,3);
    table->setItem(0,0,new QTableWidgetItem("时间"));
    table->setItem(0,1,new QTableWidgetItem(ui->factorname->currentText()));
    table->setItem(1,1,new QTableWidgetItem("实时值"));
    table->setItem(1,2,new QTableWidgetItem("状态值"));
    table->setItem(1,3,new QTableWidgetItem("标签值"));
    //数据

    QJsonArray dataArray = resObj.value("data").toArray();

    QJsonArray::iterator it = dataArray.begin();
    int index = 0;
    while(it != dataArray.end())
    {
        QJsonObject subObj = it->toObject();

        QString dtstr = subObj.value("CollectAt").toString();
        QString value = subObj.value("Data").toString();
        QString note = subObj.value("Flag").toString();
        QString tagId = subObj.value("TagID").toString();


        table->insertRow(2+index);
        table->setItem(2+index,0,new QTableWidgetItem(dtstr));
        table->setItem(2+index,1,new QTableWidgetItem(value));
        table->setItem(2+index,2,new QTableWidgetItem(note));
        table->setItem(2+index,3,new QTableWidgetItem(tagId));

        index++;
        it++;
    }




    QFont font;
    font.setFamily("微软雅黑");
    font.setBold(true);
    font.setPointSize(14);

    for(int i=0;i<ui->tablertk->rowCount();++i)
    {
        for(int j=0;j<ui->tablertk->columnCount();++j)
        {
            QTableWidgetItem *item = ui->tablertk->item(i,j);
            if(item!=nullptr)
            {

                item->setTextAlignment(Qt::AlignCenter);
                item->setFont(font);
                if(i<=1)
                {
                    item->setBackgroundColor(QColor(Qt::gray));
                }
            }
        }
    }

    ui->tablertk->resizeColumnsToContents();





}

void DataQuery::query_min()
{
    //查询
    qDebug()<<__LINE__<<__FUNCTION__<<endl;

}

void DataQuery::query_hour()
{
    //查询
    qDebug()<<__LINE__<<__FUNCTION__<<endl;
}

void DataQuery::query_day()
{
    //查询
    qDebug()<<__LINE__<<__FUNCTION__<<endl;

}

void DataQuery::query_month()
{
    //查询
    qDebug()<<__LINE__<<__FUNCTION__<<endl;

}

void DataQuery::query_trans()
{
    //查询
    qDebug()<<__LINE__<<__FUNCTION__<<endl;
}


void DataQuery::onReceiveFac(QString facname)
{
    for(int i=0;i<ui->factorname->count();++i)
    {
        if(ui->factorname->itemText(i).contains(facname))
        {
            ui->factorname->setCurrentIndex(i);
            ui->ck_rtk->setChecked(true);
            onQuery();
            break;
        }
    }
}

void DataQuery::getFactors()
{
    getCommonFactors();
    QJsonObject jObj;
    HttpClient hClient;
    ui->factorname->clear();
    hClient.asyngetdata("/dcm/realtime_data",jObj);
    qDebug()<<__LINE__<<jObj<<endl;
    QJsonObject::iterator it = jObj.begin();
    while(it != jObj.end())
    {
        QString code;
        QString full_name;
        if(it.key().split("-").count()>1)
        {
            code = it.key().split("-")[1];

            QJsonObject subObj = fullFactorsInfo.value(code).toObject();
            QString realName = subObj.value("name").toString();
            full_name = code+"-"+realName;
            qDebug()<<__LINE__<<full_name<<endl;

            ui->factorname->addItem(full_name);

        }
        else
        {
            code = it.key();
            QJsonObject subObj = fullFactorsInfo.value(code).toObject();
            QString realName = subObj.value("name").toString();
            full_name = code+"-"+realName;
            qDebug()<<__LINE__<<full_name<<endl;

            ui->factorname->addItem(full_name);
        }
        it++;
    }
}

void DataQuery::getCommonFactors()
{    
    HttpClient httpClient;
    httpClient.asyngetdata("/dcm/factor",fullFactorsInfo);
    qDebug()<<__LINE__<<fullFactorsInfo.count()<<endl;


}

void DataQuery::getUpsetAddr()
{
    ui->deviceaddr->clear();
    HttpClient httpClient;
    if(!httpClient.asyngetdata("/dcm/connect_stat",fullDevicesInfo))
    {
        QMessageBox::warning(this,"提示","获取设备地址失败");
        return;
    }
    qDebug()<<__LINE__<<fullDevicesInfo<<endl;
    QJsonObject::Iterator it = fullDevicesInfo.begin();
    while(it != fullDevicesInfo.end())
    {
        if(it.value().toBool())
        {
            QString ip_addr_port = it.key();
            ui->deviceaddr->addItem(ip_addr_port);
        }

        it++;
    }

}


