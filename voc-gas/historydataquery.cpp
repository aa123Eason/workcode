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

    QDateTime defaultSDT = QDateTime::currentDateTime();
//    QDateTime defaultSDT = QDateTime::fromString("2023-07-11 03:00","yyyy-MM-dd HH:mm");
    QDateTime defaultSDT1 = defaultSDT.addSecs(3600);
    ui->startDT->setDateTime(defaultSDT);
    ui->endDT->setDateTime(defaultSDT1);

    buildDefaultTable(12,21);

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
    connect(this,&HistoryDataQuery::sendlogmsg,this,&HistoryDataQuery::onPrintlog);
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
    ui->mainTable->setSpan(1,2,1,cols-1);
    ui->mainTable->setSpan(2,2,1,7);
    ui->mainTable->setSpan(2,9,1,2);
    ui->mainTable->setSpan(2,11,1,cols-11);
    ui->mainTable->setSpan(3,0,2,1);
    ui->mainTable->setSpan(3,1,1,2);
    ui->mainTable->setSpan(3,3,1,4);
    ui->mainTable->setSpan(3,7,1,2);
    ui->mainTable->setSpan(3,9,1,3);
    ui->mainTable->setSpan(3,12,1,2);
    ui->mainTable->setSpan(3,14,1,2);
    ui->mainTable->setSpan(3,16,2,1);
    ui->mainTable->setSpan(3,17,2,1);
    ui->mainTable->setSpan(3,18,2,1);
    ui->mainTable->setSpan(3,19,2,1);
    ui->mainTable->setSpan(3,20,2,1);

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
    //前两行
    QTableWidgetItem *item1 = new QTableWidgetItem("固定污染源名称");
    QTableWidgetItem *item2 = new QTableWidgetItem("固定污染源编号");
    QTableWidgetItem *item3 = new QTableWidgetItem("监测时间");

    ui->mainTable->setItem(1,0,item1);
    ui->mainTable->setItem(2,0,item2);
    ui->mainTable->setItem(2,9,item3);

    //数据区域
    QTableWidgetItem *itemTime = new QTableWidgetItem("时间");
    QTableWidgetItem *itemch4 = new QTableWidgetItem("甲烷");
    QTableWidgetItem *itemch4w = new QTableWidgetItem("湿值mg/m3");
    QTableWidgetItem *itemch4d = new QTableWidgetItem("干值mg/m3");
//    QTableWidgetItem *itemch4e = new QTableWidgetItem("排放量kg/h");
    QTableWidgetItem *itemnmth = new QTableWidgetItem("非甲烷总烃");
    QTableWidgetItem *itemnmthd = new QTableWidgetItem("湿值mg/m3");
    QTableWidgetItem *itemnmthw = new QTableWidgetItem("干值mg/m3");
    QTableWidgetItem *itemnmthlc = new QTableWidgetItem("折算值mg/m3");
    QTableWidgetItem *itemnmthe = new QTableWidgetItem("排放量kg/h");
    QTableWidgetItem *itemth = new QTableWidgetItem("总烃");
    QTableWidgetItem *itemthw = new QTableWidgetItem("湿值mg/m3");
    QTableWidgetItem *itemthd = new QTableWidgetItem("干值mg/m3");
//    QTableWidgetItem *itemthe = new QTableWidgetItem("排放量kg/h");
    QTableWidgetItem *itemthflu = new QTableWidgetItem("烟尘");
    QTableWidgetItem *itemthfluw = new QTableWidgetItem("湿值mg/m3");
    QTableWidgetItem *itemthflud = new QTableWidgetItem("干值mg/m3");
    QTableWidgetItem *itemthflue = new QTableWidgetItem("排放量kg/h");
    QTableWidgetItem *itemll = new QTableWidgetItem("流量");
    QTableWidgetItem *itemllbk = new QTableWidgetItem("标况流量m3/h");
    QTableWidgetItem *itemllgk = new QTableWidgetItem("工况流量m3/h");
    QTableWidgetItem *itemo2 = new QTableWidgetItem("氧气含量");
    QTableWidgetItem *itemow = new QTableWidgetItem("湿值%");
    QTableWidgetItem *itemod = new QTableWidgetItem("干值%");
    QTableWidgetItem *itemft = new QTableWidgetItem("烟气温度℃");
    QTableWidgetItem *itemfh = new QTableWidgetItem("烟气湿度%");
    QTableWidgetItem *itemfp = new QTableWidgetItem("烟气压力KPa");
    QTableWidgetItem *itemff = new QTableWidgetItem("烟气流速m3/s");
    QTableWidgetItem *itemelse = new QTableWidgetItem("备注");

    ui->mainTable->setItem(3,0,itemTime);
    ui->mainTable->setItem(3,1,itemch4);
    ui->mainTable->setItem(4,1,itemch4w);
    ui->mainTable->setItem(4,2,itemch4d);
//    ui->mainTable->setItem(4,3,itemch4e);
    ui->mainTable->setItem(3,3,itemnmth);
    ui->mainTable->setItem(4,3,itemnmthw);
    ui->mainTable->setItem(4,4,itemnmthd);
    ui->mainTable->setItem(4,5,itemnmthlc);
    ui->mainTable->setItem(4,6,itemnmthe);
    ui->mainTable->setItem(3,7,itemth);
    ui->mainTable->setItem(4,7,itemthw);
    ui->mainTable->setItem(4,8,itemthd);
//    ui->mainTable->setItem(4,10,itemthe);
    ui->mainTable->setItem(3,9,itemthflu);
    ui->mainTable->setItem(4,9,itemthfluw);
    ui->mainTable->setItem(4,10,itemthflud);
    ui->mainTable->setItem(4,11,itemthflue);

    ui->mainTable->setItem(3,12,itemll);
    ui->mainTable->setItem(4,12,itemllbk);
    ui->mainTable->setItem(4,13,itemllgk);
    ui->mainTable->setItem(3,14,itemo2);
    ui->mainTable->setItem(4,14,itemow);
    ui->mainTable->setItem(4,15,itemod);
    ui->mainTable->setItem(3,16,itemft);
    ui->mainTable->setItem(3,17,itemfh);
    ui->mainTable->setItem(3,18,itemfp);
    ui->mainTable->setItem(3,19,itemff);
    ui->mainTable->setItem(3,20,itemelse);


    //统计区域
    QTableWidgetItem *itemav = new QTableWidgetItem("平均值");
    QTableWidgetItem *itemmax = new QTableWidgetItem("最大值");
    QTableWidgetItem *itemmin = new QTableWidgetItem("最小值");
    QTableWidgetItem *itemnum = new QTableWidgetItem("样本数");
    QTableWidgetItem *itempfl = new QTableWidgetItem();
    QString formatStr = ui->queryDTType->currentText();
    if(formatStr == "分钟查询")
    {
        itempfl->setText("小时排放量t");
    }
    else if(formatStr == "小时查询")
    {
        itempfl->setText("日排放量t");
    }
    else if(formatStr == "日查询")
    {
        itempfl->setText("月排放量t");
    }
    else if(formatStr == "月查询")
    {
        itempfl->setText("年排放量t");
    }
    else
    {
        itempfl->setText("小时排放量t");
    }

    ui->mainTable->setItem(rows-6,0,itemav);
    ui->mainTable->setItem(rows-5,0,itemmax);
    ui->mainTable->setItem(rows-4,0,itemmin);
    ui->mainTable->setItem(rows-3,0,itemnum);
    ui->mainTable->setItem(rows-2,0,itempfl);


    //表尾
    QTableWidgetItem *itemthr1 = new QTableWidgetItem("废气排放总量单位");
    QTableWidgetItem *itemthr2 = new QTableWidgetItem("X10000m3/d");
    ui->mainTable->setItem(rows-1,0,itemthr1);
    ui->mainTable->setItem(rows-1,2,itemthr2);



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
    else
    {
        emit sendlogmsg("历史数据查询窗口——查询方式："+ui->queryDTType->currentText());
    }

    QString dtStr1 = dt1.toString(queryFormat);
    QString dtStr2 = dt2.toString(queryFormat);

    qDebug()<<__LINE__<<dtStr1<<dtStr2<<endl;
    emit sendlogmsg("历史数据查询窗口——时间范围："+dtStr1+"~"+dtStr2);

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
    queFactors<<"甲烷"<<"甲烷干值"<<"非甲烷总烃"<<"非甲烷总烃干值"<<"折算非甲烷总烃"<<"非甲烷总烃排放量"
             <<"总烃"<<"总烃干值"<<"烟尘湿值"<<"烟尘干值"<<"烟尘排放量"<<"标况流量"<<"工况流量"<<"氧气含量"<<"氧气含量干值"<<"烟气温度"<<"烟气湿度"<<"烟气压力"<<"烟气流速";
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

    emit sendlogmsg("历史数据查询窗口——查询表名："+tableName);

    qDebug()<<__LINE__<<queStr<<endl;
    emit sendlogmsg("历史数据查询窗口——查询sql命令："+queStr);

    int delDT = dt1.secsTo(dt2);
    int refDT = delDT;

    if(ui->queryDTType->currentText() == "分钟查询")
    {
        refDT = delDT/60;
    }
    else if(ui->queryDTType->currentText() == "小时查询")
    {
        refDT = delDT / 3600;
    }
    else if(ui->queryDTType->currentText() == "日查询")
    {
        refDT = dt1.daysTo(dt2);
    }
    else if(ui->queryDTType->currentText() == "月查询")
    {
        refDT = dt1.daysTo(dt2) / 30;
    }
    else
    {
        refDT = delDT/60;
    }

    if(refDT>1440)
    {
        QMessageBox::warning(this,"提示","查询量过大，会拖慢运行速度，建议缩短时间范围");
        return;
    }

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
    emit sendlogmsg("历史数据查询窗口——导出文件："+filePath);

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
    xlsx.mergeCells(QXlsx::CellRange(2,3,2,cols+1));
    xlsx.mergeCells(QXlsx::CellRange(3,3,3,9));
    xlsx.mergeCells(QXlsx::CellRange(3,10,3,11));
    xlsx.mergeCells(QXlsx::CellRange(3,12,3,cols));
    xlsx.mergeCells(QXlsx::CellRange(4,1,5,1));
    xlsx.mergeCells(QXlsx::CellRange(4,2,4,3));
    xlsx.mergeCells(QXlsx::CellRange(4,4,4,7));
    xlsx.mergeCells(QXlsx::CellRange(4,8,4,9));
    xlsx.mergeCells(QXlsx::CellRange(4,10,4,12));
    xlsx.mergeCells(QXlsx::CellRange(4,13,4,14));
    xlsx.mergeCells(QXlsx::CellRange(4,15,4,16));
    xlsx.mergeCells(QXlsx::CellRange(4,17,5,17));
    xlsx.mergeCells(QXlsx::CellRange(4,18,5,18));
    xlsx.mergeCells(QXlsx::CellRange(4,19,5,19));
    xlsx.mergeCells(QXlsx::CellRange(4,20,5,20));
    xlsx.mergeCells(QXlsx::CellRange(4,21,5,21));

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

//    if(ui->queryDTType->currentText() == "分钟查询")
//    {
//        if(resMap["HistoryTime"].count()>1440)
//        {
//            QMessageBox::warning(this,"提示","查询量过大，会拖慢运行速度，建议缩短时间范围");
//            return;
//        }
//    }

    for(int i=0;i<resMap["HistoryTime"].count();++i)
    {
        QString historyTime = resMap["HistoryTime"][i];
        for(int j=5;j<ui->mainTable->rowCount()-6;++j)
        {
            QString dt = ui->mainTable->item(j,0)->text();

            if(historyTime == dt)
            {
                QString ch4 = resMap["甲烷"][i];
                QString ch4_dry = resMap["甲烷干值"][i];
                QString nmch4 = resMap["非甲烷总烃"][i];
                QString nmch4_dry = resMap["非甲烷总烃干值"][i];
                QString nmch4_lc = resMap["折算非甲烷总烃"][i];
                QString nmch4_emiss = resMap["非甲烷总烃排放量"][i];
                QString thc = resMap["总烃"][i];
                QString thc_dry = resMap["总烃干值"][i];
                QString flu_wet = resMap["烟尘湿值"][i];
                QString flu_dry = resMap["烟尘干值"][i];
                QString flu_emiss = resMap["烟尘排放量"][i];
                QString flow = resMap["标况流量"][i];
                QString workflow = resMap["工况流量"][i];
                QString oxygen = resMap["氧气含量"][i];
                QString oxygen_dry = resMap["氧气含量干值"][i];
                QString fluTmp = resMap["烟气温度"][i];
                QString fluHum = resMap["烟气湿度"][i];
                QString flupress = resMap["烟气压力"][i];
                QString fluspeed = resMap["烟气流速"][i];

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

                QTableWidgetItem *item_nmch4_lc = new QTableWidgetItem(nmch4_lc);
                item_nmch4_lc->setFont(font);
                item_nmch4_lc->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_nmch4_emiss = new QTableWidgetItem(nmch4_emiss);
                item_nmch4_emiss->setFont(font);
                item_nmch4_emiss->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_thc = new QTableWidgetItem(thc);
                item_thc->setFont(font);
                item_thc->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_thc_dry = new QTableWidgetItem(thc_dry);
                item_thc_dry->setFont(font);
                item_thc_dry->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_flu_wet = new QTableWidgetItem(flu_wet);
                item_flu_wet->setFont(font);
                item_flu_wet->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_flu_dry = new QTableWidgetItem(flu_dry);
                item_flu_dry->setFont(font);
                item_flu_dry->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_flu_emiss = new QTableWidgetItem(flu_emiss);
                item_flu_emiss->setFont(font);
                item_flu_emiss->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_flow = new QTableWidgetItem(flow);
                item_flow->setFont(font);
                item_flow->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_workflow = new QTableWidgetItem(workflow);
                item_workflow->setFont(font);
                item_workflow->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_oxygen = new QTableWidgetItem(oxygen);
                item_oxygen->setFont(font);
                item_oxygen->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_oxygen_dry = new QTableWidgetItem(oxygen_dry);
                item_oxygen_dry->setFont(font);
                item_oxygen_dry->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_fluTmp = new QTableWidgetItem(fluTmp);
                item_fluTmp->setFont(font);
                item_fluTmp->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_fluHum = new QTableWidgetItem(fluHum);
                item_fluHum->setFont(font);
                item_fluHum->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_flupress = new QTableWidgetItem(flupress);
                item_flupress->setFont(font);
                item_flupress->setTextAlignment(Qt::AlignCenter);

                QTableWidgetItem *item_fluspeed = new QTableWidgetItem(fluspeed);
                item_fluspeed->setFont(font);
                item_fluspeed->setTextAlignment(Qt::AlignCenter);

                ui->mainTable->setItem(j,1,item_ch4);
                ui->mainTable->setItem(j,2,item_ch4_dry);
                ui->mainTable->setItem(j,3,item_nmch4);
                ui->mainTable->setItem(j,4,item_nmch4_dry);
                ui->mainTable->setItem(j,5,item_nmch4_lc);
                ui->mainTable->setItem(j,6,item_nmch4_emiss);
                ui->mainTable->setItem(j,7,item_thc);
                ui->mainTable->setItem(j,8,item_thc_dry);
                ui->mainTable->setItem(j,9,item_flu_wet);
                ui->mainTable->setItem(j,10,item_flu_dry);
                ui->mainTable->setItem(j,11,item_flu_emiss);
                ui->mainTable->setItem(j,12,item_flow);
                ui->mainTable->setItem(j,13,item_workflow);
                ui->mainTable->setItem(j,14,item_oxygen);
                ui->mainTable->setItem(j,15,item_oxygen_dry);
                ui->mainTable->setItem(j,16,item_fluTmp);
                ui->mainTable->setItem(j,17,item_fluHum);
                ui->mainTable->setItem(j,18,item_flupress);
                ui->mainTable->setItem(j,19,item_fluspeed);

            }
        }
    }
}

void HistoryDataQuery::calStastics()
{
//    if(resMap.count()==0)
//    {
//        QMessageBox::warning(this,"提示","查无数据");
//        return;
//    }

    qDebug()<<__LINE__<<"resMap"<<resMap<<endl;

    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setFamily("微软雅黑");
    qDebug()<<__LINE__<<resMap.count()<<resMap["HistoryTime"].count()<<endl;
    qDebug()<<__LINE__<<resMap["HistoryTime"]<<endl;

    QList<int> facIndexList;
    facIndexList<<1<<2<<3<<4<<5<<6<<7<<8<<9<<10<<11<<12<<13<<14<<15<<16<<17<<18<<19;
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
                else
                {
                    sum += 0;
                    if(max<=0)
                        max = 0;

                    if(min>=0)
                        min = 0;
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
        buildDefaultTable(11+dataRows,21);
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
            dt_k = dt_start.addSecs(k*60).toString(dtformat1);
        }
        else if(curText == "小时查询")
        {
            dt_k = dt_start.addSecs(k*3600).toString(dtformat1);
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

void HistoryDataQuery::onPrintlog(QString msg)
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
