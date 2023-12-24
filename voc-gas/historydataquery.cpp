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

    buildDefaultTable(12,16);

}

void HistoryDataQuery::connectevent()
{
    connect(ui->queryBtn,&QPushButton::clicked,this,&HistoryDataQuery::onQuery);
    connect(ui->exportBtn,&QPushButton::clicked,this,&HistoryDataQuery::onExport);
    connect(ui->queryDTType,&QComboBox::currentTextChanged,this,&HistoryDataQuery::on_comboBox_currentChanged);
    connect(ui->htop,&QPushButton::clicked,this,[=]()
    {
        ui->mainTable->horizontalScrollBar()->setValue(ui->mainTable->horizontalScrollBar()->minimum());
    });
    connect(ui->hbottom,&QPushButton::clicked,this,[=]()
    {
        ui->mainTable->horizontalScrollBar()->setValue(ui->mainTable->horizontalScrollBar()->maximum());
    });
    connect(ui->vtop,&QPushButton::clicked,this,[=]()
    {
        ui->mainTable->verticalScrollBar()->setValue(ui->mainTable->verticalScrollBar()->minimum());
    });
    connect(ui->vbottom,&QPushButton::clicked,this,[=]()
    {
        ui->mainTable->verticalScrollBar()->setValue(ui->mainTable->verticalScrollBar()->maximum());
    });
}

void HistoryDataQuery::databaseinit()
{
    qDebug()<<__LINE__<<QApplication::applicationDirPath()<<endl;
    if(!curdb.isOpen())
    {
        curdb = QSqlDatabase::addDatabase("QSQLITE");
        curdb.setDatabaseName(QApplication::applicationDirPath()+"/VocGas.db");
        curdb.open();
    }

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

void HistoryDataQuery::buildDefaultTable(int rows,int cols)
{
    //确定缺省表格的行列数

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
    resMap.clear();

    QDateTime dt1 = ui->startDT->dateTime();
    QDateTime dt2 = ui->endDT->dateTime();

    if(dt1>=dt2)
    {
        QMessageBox::warning(this,"提示","起始时间必须早于终止时间，查询失败！");
        return;
    }

    if(ui->queryDTType->currentText()=="请选择查询方式")
    {
        QMessageBox::warning(this,"提示","请选择查询方式");
        return;
    }

    QString dtStr1 = dt1.toString(queryFormat);
    QString dtStr2 = dt2.toString(queryFormat);

    qDebug()<<__LINE__<<dtStr1<<dtStr2<<endl;

    //补充数据区域的行
    editDataRow(dt1,dt2);
    //填入日期时间点
    filldateTime(dt1,dt2);

    if(!curdb.isOpen())
    {
        QMessageBox::warning(this,"提示","无法正常打开本地数据库，查询失败！");
        return;
    }

    QString queStr = "select HistoryTime,";
    QStringList queFactors;
    queFactors<<"甲烷"<<"甲烷干值"<<"总烃"<<"总烃干值"<<"非甲烷总烃"<<"非甲烷总烃干值"<<"非甲烷总烃排放量"
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

    qDebug()<<__LINE__<<queStr<<endl;

    QSqlQuery q(queStr);
    q.exec();

    while(q.next())
    {
        resMap["HistoryTime"]<<q.value("HistoryTime").toString();
        for(QString fac:queFactors)
        {
            QString code = HistoryChartView::facNameMap().key(fac);
            //qDebug()<<__LINE__<<code<<":"<<q.value(code).toString()<<endl;

            if(q.value(code).toString() != "ú")
            {
                resMap[fac]<< q.value(code).toString();
            }
            else
            {
                resMap[fac]<< QString::number(0,'f',2);
            }
        }
    }


    qDebug()<<__LINE__<<tableName<<endl;
    qDebug()<<__LINE__<<queryFormat<<endl;

    fillinDatas();

    calStastics();

}

void HistoryDataQuery::onExport()
{
    int rows = ui->mainTable->rowCount();
    int cols = ui->mainTable->columnCount();
    QString cutText = ui->queryDTType->currentText();
    if(rows==0||cols==0||cutText=="请选择查询方式")
    {
        QMessageBox::warning(this,"提示","导出无效");
        return;
    }

    QString tableTitle = ui->mainTable->item(0,0)->text();
    QString queryDT = ui->mainTable->item(2,11)->text();
    if(queryDT.contains("~"))
        queryDT = queryDT.replace("~","_");
    if(queryDT.contains(" "))
        queryDT = queryDT.replace(" ","");
    if(tableTitle.isEmpty()||queryDT.isEmpty())
    {
        QMessageBox::warning(this,"提示","导出无效");
        return;
    }
    QString fileName = tableTitle+"_"+queryDT+".xlsx";
    QString filePath = QApplication::applicationDirPath()+"/export/"+fileName;

    QXlsx::Document xlsx;
    xlsx.addSheet(tableTitle);

    qDebug()<<__LINE__<<endl;

    for(int i=1;i<=rows;++i)
    {
        for(int j=1;j<=cols;++j)
        {
            if(ui->mainTable->item(i-1,j-1)!=nullptr)
                xlsx.write(i,j,ui->mainTable->item(i-1,j-1)->text());

        }
    }

    qDebug()<<__LINE__<<endl;

    //制作表头
    xlsx.mergeCells(QXlsx::CellRange(1,1,1,cols));
    xlsx.mergeCells(QXlsx::CellRange(2,1,2,2));
    xlsx.mergeCells(QXlsx::CellRange(3,1,3,2));
    xlsx.mergeCells(QXlsx::CellRange(2,1,2,cols-2));
    xlsx.mergeCells(QXlsx::CellRange(3,3,3,9));
    xlsx.mergeCells(QXlsx::CellRange(3,10,3,11));
    xlsx.mergeCells(QXlsx::CellRange(3,12,3,16));
    xlsx.mergeCells(QXlsx::CellRange(4,1,5,1));
    xlsx.mergeCells(QXlsx::CellRange(4,2,4,4));
    xlsx.mergeCells(QXlsx::CellRange(4,5,4,7));
    xlsx.mergeCells(QXlsx::CellRange(4,8,4,10));
    xlsx.mergeCells(QXlsx::CellRange(4,11,5,11));
    xlsx.mergeCells(QXlsx::CellRange(4,12,5,12));
    xlsx.mergeCells(QXlsx::CellRange(4,13,5,13));
    xlsx.mergeCells(QXlsx::CellRange(4,14,5,14));
    xlsx.mergeCells(QXlsx::CellRange(4,15,5,15));
    xlsx.mergeCells(QXlsx::CellRange(4,16,5,16));

    //最后一行
    xlsx.mergeCells(QXlsx::CellRange(rows,1,rows+1,2));
    xlsx.mergeCells(QXlsx::CellRange(rows,3,rows+1,cols));

    qDebug()<<__LINE__<<filePath<<endl;

    if(xlsx.saveAs(filePath))
    {
        QMessageBox::about(this,"提示","导出成功:\r\n"+filePath);
    }
    else
    {
        QMessageBox::warning(this,"提示","导出失败:\r\n"+filePath);
    }


}



void HistoryDataQuery::fillinDatas()
{
    if(resMap.count()==0)
    {
        QMessageBox::warning(this,"提示","查无数据");
        return;
    }

    qDebug()<<__LINE__<<"resMap"<<resMap<<endl;

    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setFamily("微软雅黑");
    qDebug()<<__LINE__<<resMap.count()<<resMap["HistoryTime"].count()<<endl;
    qDebug()<<__LINE__<<resMap["HistoryTime"]<<endl;

    if(ui->queryDTType->currentText() == "分钟查询")
    {
        if(resMap["HistoryTime"].count()>1440)
        {
            QMessageBox::warning(this,"提示","查询量过大，会拖慢运行速度，建议缩短时间范围");
            return;
        }
    }

    for(int i=0;i<resMap["HistoryTime"].count();++i)
    {
        QString historyTime = resMap["HistoryTime"][i];
        for(int j=5;j<ui->mainTable->rowCount()-6;++j)
        {
            QString dt = ui->mainTable->item(j,0)->text();
            if(dt.contains("~"))
            {
                if(dt.split("~").count()>1)
                {

                    if(historyTime == dt.split("~")[0])
                    {
                        //                    qDebug()<<__LINE__<<"YES:"<<i<<j<<historyTime<<dt<<endl;
                        QString ch4 = resMap["甲烷"][i];
                        QString ch4_dry = resMap["甲烷干值"][i];
                        QString nmch4 = resMap["非甲烷总烃"][i];
                        QString nmch4_dry = resMap["非甲烷总烃干值"][i];
                        //                    QString nmch4_emiss = resMap["非甲烷总烃排放量"][i];
                        QString thc = resMap["总烃"][i];
                        QString thc_dry = resMap["总烃干值"][i];
                        QString flow = resMap["标况流量"][i];
                        QString oxygen = resMap["氧气含量"][i];
                        QString fluTmp = resMap["烟气温度"][i];
                        QString fluHum = resMap["烟气湿度"][i];
                        QString flupress = resMap["烟气压力"][i];

                        QTableWidgetItem *item_ch4 = new QTableWidgetItem(ch4);
                        item_ch4->setFont(font);
                        item_ch4->setTextAlignment(Qt::AlignCenter);

                        QTableWidgetItem *item_ch4_dry = new QTableWidgetItem(ch4_dry);
                        item_ch4_dry->setFont(font);
                        item_ch4_dry->setTextAlignment(Qt::AlignCenter);

                        QTableWidgetItem *item_nmch4 = new QTableWidgetItem(nmch4);
                        item_nmch4->setFont(font);
                        item_nmch4->setTextAlignment(Qt::AlignCenter);

                        QTableWidgetItem *item_nmch4_dry = new QTableWidgetItem(nmch4_dry);
                        item_nmch4_dry->setFont(font);
                        item_nmch4_dry->setTextAlignment(Qt::AlignCenter);

                        //                    QTableWidgetItem *item_nmch4_emiss = new QTableWidgetItem(nmch4_emiss);
                        //                    item_nmch4_emiss->setFont(font);
                        //                    item_nmch4_emiss->setTextAlignment(Qt::AlignCenter);

                        QTableWidgetItem *item_thc = new QTableWidgetItem(thc);
                        item_thc->setFont(font);
                        item_thc->setTextAlignment(Qt::AlignCenter);

                        QTableWidgetItem *item_thc_dry = new QTableWidgetItem(thc_dry);
                        item_thc_dry->setFont(font);
                        item_thc_dry->setTextAlignment(Qt::AlignCenter);

                        QTableWidgetItem *item_flow = new QTableWidgetItem(flow);
                        item_flow->setFont(font);
                        item_flow->setTextAlignment(Qt::AlignCenter);

                        QTableWidgetItem *item_oxygen = new QTableWidgetItem(oxygen);
                        item_oxygen->setFont(font);
                        item_oxygen->setTextAlignment(Qt::AlignCenter);

                        QTableWidgetItem *item_fluTmp = new QTableWidgetItem(fluTmp);
                        item_fluTmp->setFont(font);
                        item_fluTmp->setTextAlignment(Qt::AlignCenter);

                        QTableWidgetItem *item_fluHum = new QTableWidgetItem(fluHum);
                        item_fluHum->setFont(font);
                        item_fluHum->setTextAlignment(Qt::AlignCenter);

                        QTableWidgetItem *item_flupress = new QTableWidgetItem(flupress);
                        item_flupress->setFont(font);
                        item_flupress->setTextAlignment(Qt::AlignCenter);

                        ui->mainTable->setItem(j,1,item_ch4);
                        ui->mainTable->setItem(j,2,item_ch4_dry);
                        ui->mainTable->setItem(j,4,item_nmch4);
                        ui->mainTable->setItem(j,5,item_nmch4_dry);
                        //                    ui->mainTable->setItem(j,6,item_nmch4_emiss);
                        ui->mainTable->setItem(j,7,item_thc);
                        ui->mainTable->setItem(j,8,item_thc_dry);
                        ui->mainTable->setItem(j,10,item_flow);
                        ui->mainTable->setItem(j,11,item_oxygen);
                        ui->mainTable->setItem(j,12,item_fluTmp);
                        ui->mainTable->setItem(j,13,item_fluHum);
                        ui->mainTable->setItem(j,14,item_flupress);



                    }

                }
            }
            else
            {
                if(historyTime == dt)
                {
                    //                    qDebug()<<__LINE__<<"YES:"<<i<<j<<historyTime<<dt<<endl;
                    QString ch4 = resMap["甲烷"][i];
                    QString ch4_dry = resMap["甲烷干值"][i];
                    QString nmch4 = resMap["非甲烷总烃"][i];
                    QString nmch4_dry = resMap["非甲烷总烃干值"][i];
                    //                    QString nmch4_emiss = resMap["非甲烷总烃排放量"][i];
                    QString thc = resMap["总烃"][i];
                    QString thc_dry = resMap["总烃干值"][i];
                    QString flow = resMap["标况流量"][i];
                    QString oxygen = resMap["氧气含量"][i];
                    QString fluTmp = resMap["烟气温度"][i];
                    QString fluHum = resMap["烟气湿度"][i];
                    QString flupress = resMap["烟气压力"][i];

                    QTableWidgetItem *item_ch4 = new QTableWidgetItem(ch4);
                    item_ch4->setFont(font);
                    item_ch4->setTextAlignment(Qt::AlignCenter);

                    QTableWidgetItem *item_ch4_dry = new QTableWidgetItem(ch4_dry);
                    item_ch4_dry->setFont(font);
                    item_ch4_dry->setTextAlignment(Qt::AlignCenter);

                    QTableWidgetItem *item_nmch4 = new QTableWidgetItem(nmch4);
                    item_nmch4->setFont(font);
                    item_nmch4->setTextAlignment(Qt::AlignCenter);

                    QTableWidgetItem *item_nmch4_dry = new QTableWidgetItem(nmch4_dry);
                    item_nmch4_dry->setFont(font);
                    item_nmch4_dry->setTextAlignment(Qt::AlignCenter);

                    //                    QTableWidgetItem *item_nmch4_emiss = new QTableWidgetItem(nmch4_emiss);
                    //                    item_nmch4_emiss->setFont(font);
                    //                    item_nmch4_emiss->setTextAlignment(Qt::AlignCenter);

                    QTableWidgetItem *item_thc = new QTableWidgetItem(thc);
                    item_thc->setFont(font);
                    item_thc->setTextAlignment(Qt::AlignCenter);

                    QTableWidgetItem *item_thc_dry = new QTableWidgetItem(thc_dry);
                    item_thc_dry->setFont(font);
                    item_thc_dry->setTextAlignment(Qt::AlignCenter);

                    QTableWidgetItem *item_flow = new QTableWidgetItem(flow);
                    item_flow->setFont(font);
                    item_flow->setTextAlignment(Qt::AlignCenter);

                    QTableWidgetItem *item_oxygen = new QTableWidgetItem(oxygen);
                    item_oxygen->setFont(font);
                    item_oxygen->setTextAlignment(Qt::AlignCenter);

                    QTableWidgetItem *item_fluTmp = new QTableWidgetItem(fluTmp);
                    item_fluTmp->setFont(font);
                    item_fluTmp->setTextAlignment(Qt::AlignCenter);

                    QTableWidgetItem *item_fluHum = new QTableWidgetItem(fluHum);
                    item_fluHum->setFont(font);
                    item_fluHum->setTextAlignment(Qt::AlignCenter);

                    QTableWidgetItem *item_flupress = new QTableWidgetItem(flupress);
                    item_flupress->setFont(font);
                    item_flupress->setTextAlignment(Qt::AlignCenter);

                    ui->mainTable->setItem(j,1,item_ch4);
                    ui->mainTable->setItem(j,2,item_ch4_dry);
                    ui->mainTable->setItem(j,4,item_nmch4);
                    ui->mainTable->setItem(j,5,item_nmch4_dry);
                    //                    ui->mainTable->setItem(j,6,item_nmch4_emiss);
                    ui->mainTable->setItem(j,7,item_thc);
                    ui->mainTable->setItem(j,8,item_thc_dry);
                    ui->mainTable->setItem(j,10,item_flow);
                    ui->mainTable->setItem(j,11,item_oxygen);
                    ui->mainTable->setItem(j,12,item_fluTmp);
                    ui->mainTable->setItem(j,13,item_fluHum);
                    ui->mainTable->setItem(j,14,item_flupress);



                }
            }
        }
    }
}

void HistoryDataQuery::calStastics()
{
    if(resMap.count()==0)
    {
        QMessageBox::warning(this,"提示","查无数据");
        return;
    }

    qDebug()<<__LINE__<<"resMap"<<resMap<<endl;

    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setFamily("微软雅黑");
    qDebug()<<__LINE__<<resMap.count()<<resMap["HistoryTime"].count()<<endl;
    qDebug()<<__LINE__<<resMap["HistoryTime"]<<endl;

    QList<int> facIndexList;
    facIndexList<<1<<2<<4<<5<<7<<8<<10<<11<<12<<13<<14;
    qDebug()<<__LINE__<<facIndexList<<endl;
    for(int i=0;i<facIndexList.count();++i)
    {
        QList<qreal> values={0,0,0,0,0};
        //0:avg,1:max,2:min,3:num,4:sum
        qreal avg = 0.0,max = 0.0,min = 0.0,num = 0,sum = 0.0;
        for(int j=5;j<=ui->mainTable->rowCount()-6;++j)
        {
            if(ui->mainTable->item(j,facIndexList[i])!=nullptr)
            {
                QString value = ui->mainTable->item(j,facIndexList[i])->text();

                if(!value.isEmpty())
                {
                    qDebug()<<__LINE__<<facIndexList[i]<<j<<value<<endl;
                    num++;

                    sum += value.toDouble();

                    if(max<value.toDouble())
                    {
                        max = value.toDouble();
                    }

                    if(min>value.toDouble())
                    {
                        min = value.toDouble();
                    }
                }
            }


        }
        if(num+0)
            avg = sum/(double)num;
        else
            avg = 0;



        values[0] = avg;
        values[1] = max;
        values[2] = min;
        values[3] = num;
        values[4] = sum;
        qDebug()<<__LINE__<<values<<endl;
        QTableWidgetItem *itemAVG = new QTableWidgetItem(QString::number(values[0],'f',3));
        itemAVG->setFont(font);
        itemAVG->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *itemMAX = new QTableWidgetItem(QString::number(values[1],'f',3));
        itemMAX->setFont(font);
        itemMAX->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *itemMIN = new QTableWidgetItem(QString::number(values[2],'f',3));
        itemMIN->setFont(font);
        itemMIN->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *itemNUM = new QTableWidgetItem(QString::number(values[3]));
        itemNUM->setFont(font);
        itemNUM->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem *itemSUM = new QTableWidgetItem(QString::number(values[4],'f',3));
        itemSUM->setFont(font);
        itemSUM->setTextAlignment(Qt::AlignCenter);

        int rowindex = ui->mainTable->rowCount()-6;
        ui->mainTable->setItem(rowindex,facIndexList[i],itemAVG);
        ui->mainTable->setItem(rowindex+1,facIndexList[i],itemMAX);
        ui->mainTable->setItem(rowindex+2,facIndexList[i],itemMIN);
        ui->mainTable->setItem(rowindex+3,facIndexList[i],itemNUM);
        ui->mainTable->setItem(rowindex+4,facIndexList[i],itemSUM);
        qDebug()<<__LINE__<<facIndexList<<endl;
    }
}

void HistoryDataQuery::editDataRow(QDateTime dt_start,QDateTime dt_end)
{
    qint64 delta_dt = dt_start.secsTo(dt_end);
    qDebug()<<__LINE__<<dt_start<<dt_end<<delta_dt<<endl;
    QString curText = ui->queryDTType->currentText();
    qint64 dataRows;
    if(curText == "分钟查询")
    {
        dataRows = delta_dt/60;
    }
    else if(curText == "小时查询")
    {
        dataRows = delta_dt/3600;
    }
    else if(curText == "日查询")
    {
        QList<int> big_month={1,3,5,7,8,10,12};
        QList<int> small_mouth = {4,6,9,11};
        int year = dt_start.date().year();
        int month = dt_start.date().month();
        bool isSimYear = (year%4==0)&&(year%100!=0)||(year%400==0);

        if(big_month.contains(month))
        {
            dataRows = 31;
        }
        else if(small_mouth.contains(month))
        {
            dataRows = 30;
        }
        else if(month == 2)
        {
            if(isSimYear)
            {
                dataRows = 29;
            }
            else
            {
                dataRows = 28;
            }
        }
    }
    else if(curText == "月查询")
    {
        dataRows = 12;
    }
    else
    {
        dataRows = 0;
    }

    ui->mainTable->setRowCount(0);
    ui->mainTable->setColumnCount(0);

    if(dataRows>0)
    {
        buildDefaultTable(11+dataRows,16);
        on_comboBox_currentChanged(curText);
        modifyTable();

    }

}

void HistoryDataQuery::filldateTime(QDateTime dt_start,QDateTime dt_end)
{

    QString curText = ui->queryDTType->currentText();
    QString dtformat,dtformat1;
    if(curText == "分钟查询")
    {
        dtformat = "yyyy年MM月dd日 HH时";
        dtformat1 = "yyyy-MM-dd HH:mm";
    }
    else if(curText == "小时查询")
    {
        dtformat = "yyyy年MM月dd日";
        dtformat1 = "yyyy-MM-dd HH";
    }
    else if(curText == "日查询")
    {
        dtformat = "yyyy年MM月";
        dtformat1 = "yyyy-MM-dd";
    }
    else if(curText == "月查询")
    {
        dtformat = "yyyy年";
        dtformat1 = "yyyy-MM";
    }
    else
    {
        dtformat = "yyyy年MM月dd日 HH时";
        dtformat1 = "yyyy-MM-dd HH:mm";
    }

    QString str_start = dt_start.toString(dtformat);
    QString str_end = dt_end.toString(dtformat);

    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setFamily("微软雅黑");


    QString checkTimeRange;
    if(curText == "分钟查询"||curText == "小时查询")
        checkTimeRange = str_start+"~"+str_end;
    else
        checkTimeRange = str_start;
    QTableWidgetItem *itemCheckTimeRange = new QTableWidgetItem(checkTimeRange);
    itemCheckTimeRange->setTextAlignment(Qt::AlignCenter);
    itemCheckTimeRange->setFont(font);
    ui->mainTable->setItem(2,11,itemCheckTimeRange);

    int datarows = ui->mainTable->rowCount()-11;
    int baserow = 5;

    for(int k=0;k<datarows;++k)
    {
        QString dt_k;
        if(curText == "分钟查询")
        {
            dt_k = dt_start.addSecs(k*60).toString(dtformat1)+"~"+dt_start.addSecs((k+1)*60).toString(dtformat1);
        }
        else if(curText == "小时查询")
        {
            dt_k = dt_start.addSecs(k*3600).toString(dtformat1)+"~"+dt_start.addSecs((k+1)*3600).toString(dtformat1);
        }
        else if(curText == "日查询")
        {
            dt_k = QString::number(k+1)+"日";
        }
        else if(curText == "月查询")
        {
            dt_k = QString::number(k+1)+"月";
        }

        QTableWidgetItem *itemDT = new QTableWidgetItem(dt_k);
        itemDT->setTextAlignment(Qt::AlignCenter);
        itemDT->setFont(font);
        ui->mainTable->setColumnWidth(0,220);
        ui->mainTable->setRowHeight(baserow+k,84);
        ui->mainTable->setItem(baserow+k,0,itemDT);


    }



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
