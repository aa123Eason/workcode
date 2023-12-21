#include "historydataquery.h"
#include "ui_historydataquery.h"

HistoryDataQuery::HistoryDataQuery(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistoryDataQuery)
{
    ui->setupUi(this);
    widgetinit();
    databaseinit();
    connectevent();
    modifyTable();


}

HistoryDataQuery::~HistoryDataQuery()
{
    delete ui;
    if(curdb.isOpen())
        curdb.close();
}

void HistoryDataQuery::widgetinit()
{
    this->setWindowState(Qt::WindowMaximized);

//    QDateTime defaultSDT = QDateTime::currentDateTime();
    QDateTime defaultSDT = QDateTime::fromString("2023-07-11 03:00","yyyy-MM-dd HH:mm");
    QDateTime defaultSDT1 = defaultSDT.addSecs(3600);
    ui->startDT->setDateTime(defaultSDT);
    ui->endDT->setDateTime(defaultSDT1);

    buildDefaultTable();

}

void HistoryDataQuery::connectevent()
{
    connect(ui->queryBtn,&QPushButton::clicked,this,&HistoryDataQuery::onQuery);
    connect(ui->exportBtn,&QPushButton::clicked,this,&HistoryDataQuery::onExport);
    connect(ui->queryDTType,&QComboBox::currentTextChanged,this,&HistoryDataQuery::on_comboBox_currentChanged);
}

void HistoryDataQuery::databaseinit()
{
    if(!curdb.isOpen())
    {
        curdb = QSqlDatabase::addDatabase("QSQLITE");
        curdb.setDatabaseName(QApplication::applicationDirPath()+"/VocGas.db");
        curdb.open();
    }
    qDebug()<<__LINE__<<curdb.databaseName()<<curdb.isOpen()<<endl;
}

void HistoryDataQuery::modifyTable()
{
    //文本全部居中,加粗
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setFamily("微软雅黑");
    int rows = ui->mainTable->rowCount();
    int cols = ui->mainTable->columnCount();
    for(int i=0;i<rows;++i)
    {
        for(int j=0;j<cols;++j)
        {
            QTableWidgetItem *item = ui->mainTable->item(i,j);
            if(item!=nullptr)
            {
                item->setTextAlignment(Qt::AlignCenter);
                item->setFont(font);
            }
        }
    }
}

void HistoryDataQuery::buildDefaultTable()
{
    //确定缺省表格的行列数
    int cols = 16;
    int rows = 21;
    ui->mainTable->setRowCount(rows);
    ui->mainTable->setColumnCount(cols);
//    ui->mainTable->resizeRowsToContents();
//    ui->mainTable->resizeColumnsToContents();

    mergeCell(rows,cols);
    fillindfttxt(rows,cols);

}

void HistoryDataQuery::mergeCell(int rows,int cols)
{
    //制作表头
    ui->mainTable->setSpan(0,0,1,cols);
    ui->mainTable->setSpan(1,0,1,2);
    ui->mainTable->setSpan(2,0,1,2);
    ui->mainTable->setSpan(1,2,1,cols-2);
    ui->mainTable->setSpan(2,2,1,7);
    ui->mainTable->setSpan(2,9,1,2);
    ui->mainTable->setSpan(2,11,1,5);
    ui->mainTable->setSpan(3,0,2,1);
    ui->mainTable->setSpan(3,1,1,3);
    ui->mainTable->setSpan(3,4,1,3);
    ui->mainTable->setSpan(3,7,1,3);
    ui->mainTable->setSpan(3,10,2,1);
    ui->mainTable->setSpan(3,11,2,1);
    ui->mainTable->setSpan(3,12,2,1);
    ui->mainTable->setSpan(3,13,2,1);
    ui->mainTable->setSpan(3,14,2,1);
    ui->mainTable->setSpan(3,15,2,1);

    //最后一行
    ui->mainTable->setSpan(rows-1,0,2,2);
    ui->mainTable->setSpan(rows-1,2,2,cols-2);

    //表格框线加粗

    ui->mainTable->setGridStyle(Qt::SolidLine);
    ui->mainTable->setWordWrap(true);

    for(int i = 0;i<rows;++i)
    {
        ui->mainTable->setRowHeight(i,54);
        for(int j =0;j<cols;++j)
        {
            if(j==0)
            {
                ui->mainTable->setColumnWidth(j,120);
            }
        }
    }
}

void HistoryDataQuery::fillindfttxt(int rows,int cols)
{
    QTableWidgetItem *item1 = new QTableWidgetItem("固定污染源名称");
    QTableWidgetItem *item2 = new QTableWidgetItem("固定污染源编号");
    QTableWidgetItem *item3 = new QTableWidgetItem("时间");
    QTableWidgetItem *item4 = new QTableWidgetItem("甲烷");
    QTableWidgetItem *item5 = new QTableWidgetItem("总烃");
    QTableWidgetItem *item6 = new QTableWidgetItem("非甲烷总烃");
    QTableWidgetItem *item7 = new QTableWidgetItem("监测时间");

    QTableWidgetItem *item8a = new QTableWidgetItem("mg/m3");
    QTableWidgetItem *item9a = new QTableWidgetItem("折算mg/m3");
    QTableWidgetItem *item10a = new QTableWidgetItem("kg/h");

    QTableWidgetItem *item8b = new QTableWidgetItem("mg/m3");
    QTableWidgetItem *item9b = new QTableWidgetItem("折算mg/m3");
    QTableWidgetItem *item10b = new QTableWidgetItem("kg/h");

    QTableWidgetItem *item8c = new QTableWidgetItem("mg/m3");
    QTableWidgetItem *item9c = new QTableWidgetItem("折算mg/m3");
    QTableWidgetItem *item10c = new QTableWidgetItem("kg/h");

    QTableWidgetItem *item11 = new QTableWidgetItem("流量m3/h");
    QTableWidgetItem *item12 = new QTableWidgetItem("O2%");
    QTableWidgetItem *item13 = new QTableWidgetItem("温度℃");
    QTableWidgetItem *item14 = new QTableWidgetItem("湿度%");
    QTableWidgetItem *item15 = new QTableWidgetItem("负荷");
    QTableWidgetItem *item16 = new QTableWidgetItem("备注");
    QTableWidgetItem *item17 = new QTableWidgetItem("平均值");
    QTableWidgetItem *item18 = new QTableWidgetItem("最大值");
    QTableWidgetItem *item19 = new QTableWidgetItem("最小值");
    QTableWidgetItem *item20 = new QTableWidgetItem("样本数");
//    QTableWidgetItem *item21 = new QTableWidgetItem("排放量(t)");
//    QTableWidgetItem *item22 = new QTableWidgetItem("废气排放总量单位");
    QTableWidgetItem *item23 = new QTableWidgetItem("×10000m³/d");



    ui->mainTable->setItem(1,0,item1);
    ui->mainTable->setItem(2,0,item2);
    ui->mainTable->setItem(2,9,item7);
    ui->mainTable->setItem(3,0,item3);
    ui->mainTable->setItem(3,1,item4);
    ui->mainTable->setItem(3,4,item5);
    ui->mainTable->setItem(3,7,item6);

    ui->mainTable->setItem(4,1,item8a);
    ui->mainTable->setItem(4,2,item9a);
    ui->mainTable->setItem(4,3,item10a);

    ui->mainTable->setItem(4,4,item8b);
    ui->mainTable->setItem(4,5,item9b);
    ui->mainTable->setItem(4,6,item10b);

    ui->mainTable->setItem(4,7,item8c);
    ui->mainTable->setItem(4,8,item9c);
    ui->mainTable->setItem(4,9,item10c);

    ui->mainTable->setItem(3,10,item11);
    ui->mainTable->setItem(3,11,item12);
    ui->mainTable->setItem(3,12,item13);
    ui->mainTable->setItem(3,13,item14);
    ui->mainTable->setItem(3,14,item15);
    ui->mainTable->setItem(3,15,item16);

    ui->mainTable->setItem(rows-6,0,item17);
    ui->mainTable->setItem(rows-5,0,item18);
    ui->mainTable->setItem(rows-4,0,item19);
    ui->mainTable->setItem(rows-3,0,item20);
//    ui->mainTable->setItem(rows-2,0,item21);
//    ui->mainTable->setItem(rows-1,0,item22);
    ui->mainTable->setItem(rows-1,2,item23);



}

//slot function
void HistoryDataQuery::onQuery()
{
    qDebug()<<__LINE__<<"开始查询"<<endl;


    QDateTime dt1 = ui->startDT->dateTime();
    QDateTime dt2 = ui->endDT->dateTime();

    if(dt1>=dt2)
    {
        QMessageBox::warning(this,"提示","起始时间必须早于终止时间，查询失败！");
        return;
    }

    QString dtStr1 = dt1.toString(queryFormat);
    QString dtStr2 = dt1.toString(queryFormat);

    if(!curdb.isOpen())
    {
        QMessageBox::warning(this,"提示","无法正常打开本地数据库，查询失败！");
        return;
    }

    QString queStr = "select HistoryTime,";
    QStringList queFactors;
    queFactors<<"甲烷"<<"甲烷干值"<<"总烃"<<"总烃干值"<<"非甲烷总烃"<<"折算非甲烷总烃"
             <<"标况流量"<<"氧气含量"<<"烟气温度"<<"烟气湿度"<<"烟气压力";
    for(QString fac:queFactors)
    {
        QString code = HistoryChartView::facNameMap().key(fac);
        queStr += code;
        if(fac != queFactors.last())
        {
            queStr += ",";
        }
        else
        {
            queStr += " ";
        }
    }

    queStr += "from " + tableName + " where HistoryTime between ";
    queStr += "\'" + dtStr1 + "\' and \'" + dtStr2 + "\';";

    QSqlQuery q(queStr);
    q.exec();

    while(q.next())
    {
        resMap["HistoryTime"]<<q.value("HistoryTime").toString();
        for(QString fac:queFactors)
        {
            QString code = HistoryChartView::facNameMap().key(fac);
            resMap[fac]<< q.value(code).toString();
        }
    }

    qDebug()<<__LINE__<<resMap<<endl;
    qDebug()<<__LINE__<<tableName<<endl;
    qDebug()<<__LINE__<<queryFormat<<endl;
}

void HistoryDataQuery::onExport()
{

}

void HistoryDataQuery::on_comboBox_currentChanged(const QString &curText)
{
    //根据不同的选项填入不同的内容
    QTableWidgetItem *itemTitle = nullptr;
    QTableWidgetItem *item22 = nullptr;
    QTableWidgetItem *item23 = nullptr;


    if(curText == "分钟查询")
    {
        itemTitle = new QTableWidgetItem("废气排放连续监测分钟平均值日报表");
        item22 = new QTableWidgetItem("小时排放总量(t)");
        item23 = new QTableWidgetItem("废气分钟排放总量单位");

        tableName = "T_History_Minute";
        queryFormat = "yyyy-MM-dd HH:mm";
    }
    else if(curText == "小时查询")
    {
        itemTitle = new QTableWidgetItem("废气排放连续监测小时平均值日报表");
        item22 = new QTableWidgetItem("日排放量(t)");
        item23 = new QTableWidgetItem("废气小时排放总量单位");

        tableName = "T_History_Hour";
        queryFormat = "yyyy-MM-dd HH";
    }
    else if(curText == "日查询")
    {
        itemTitle = new QTableWidgetItem("废气排放连续监测日平均值日报表");
        item22 = new QTableWidgetItem("月排放总量(t)");
        item23 = new QTableWidgetItem("废气日排放总量单位");

        tableName = "T_History_Day";
        queryFormat = "yyyy-MM-dd";
    }
    else if(curText == "月查询")
    {
        itemTitle = new QTableWidgetItem("废气排放连续监测月平均值日报表");
        item22 = new QTableWidgetItem("年排放总量(t)");
        item23 = new QTableWidgetItem("废气月排放总量单位");

        tableName = "T_History_Month";
        queryFormat = "yyyy-MM";
    }
    else
    {
        itemTitle = new QTableWidgetItem("废气排放连续监测分钟平均值日报表");
        item22 = new QTableWidgetItem("小时排放总量(t)");
        item23 = new QTableWidgetItem("废气分钟排放总量单位");

        tableName = "T_History_Minute";
        queryFormat = "yyyy-MM-dd HH:mm";

    }

    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setFamily("微软雅黑");

    if(itemTitle!=nullptr)
    {
        itemTitle->setTextAlignment(Qt::AlignCenter);
        itemTitle->setFont(font);
    }

    if(item22!=nullptr)
    {
        item22->setTextAlignment(Qt::AlignCenter);
        item22->setFont(font);
    }

    if(item23!=nullptr)
    {
        item23->setTextAlignment(Qt::AlignCenter);
        item23->setFont(font);
    }


    int rows = ui->mainTable->rowCount();
    int cols = ui->mainTable->columnCount();

    ui->mainTable->setItem(0,0,itemTitle);
    ui->mainTable->setItem(rows-2,0,item22);
    ui->mainTable->setItem(rows-1,0,item23);

}
