#include "dialogdevprop.h"
#include "ui_dialogdevprop.h"


DialogDevProp::DialogDevProp(QString pFactorKey,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDevProp)
{
    ui->setupUi(this);
    setModal(true);
    setWindowTitle(QStringLiteral(" "));
    setAttribute(Qt::WA_DeleteOnClose); // 关闭时释放内存
    // qDebug() << "pFactorKey==>" << pFactorKey;
    // qDebug() << "g_ConfObjDevParam==>" << g_ConfObjDevParam;
    showTableContent(pFactorKey);
}

DialogDevProp::~DialogDevProp()
{
    delete ui;
}

void DialogDevProp::showTableContent(QString pFactorKey)
{
    // header
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#1B89A1}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
headerText << QStringLiteral("属性") << QStringLiteral("数值") << QStringLiteral("Flag") << QStringLiteral("采集时间");
int cnt = headerText.count();
ui->tableWidget->setColumnCount(cnt);
ui->tableWidget->setHorizontalHeaderLabels(headerText);
ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
//ui->tableWidget->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
//QFont font =  ui->tableWidget->horizontalHeader()->font();
//font.setBold(true);
//ui->tableWidget->horizontalHeader()->setFont(font);

ui->tableWidget->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {150, 150, 100, 170};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget->setColumnWidth(i, widths[i]);
}


ui->tableWidget->setStyleSheet(qssTV);
ui->tableWidget->horizontalHeader()->setVisible(true);
ui->tableWidget->verticalHeader()->setDefaultSectionSize(45);

// content
// ui->tableWidget->clearContents(); //只清除工作区，不清除表头


// 该设备下 的仪器参数

QJsonObject::const_iterator it = g_ConfObjDevParam.constBegin();
QJsonObject::const_iterator end = g_ConfObjDevParam.constEnd();
QJsonObject pjsonObjList;
int pCnt = 0;
while(it != end)
{
    if(pFactorKey.contains("-"))
    {
        if(it.key().contains(pFactorKey.split("-").at(0)))
        {
            pjsonObjList.insert(it.key(),it.value());
            pCnt++;
        }
    }

    it++;
}

//qDebug() << "pFactorKey:" << pFactorKey;
//qDebug() << "pCnt:" << pCnt;
//qDebug() << "pjsonObjList:" << pjsonObjList;

ui->tableWidget->setRowCount(pCnt);

QTableWidgetItem *item;
QJsonObject::const_iterator itor = pjsonObjList.constBegin();
QJsonObject::const_iterator endList = pjsonObjList.constEnd();
int index = 0;
while(itor != endList)
{
    QJsonObject pJsonObj = itor.value().toObject();
    // code
    item = new QTableWidgetItem(pJsonObj[QLatin1String("FactorCode")].toString());
    item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(index, 0, item);

    // data
    item = new QTableWidgetItem(pJsonObj[QLatin1String("Data")].toString());
    item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(index, 1, item);

    // flag
    item = new QTableWidgetItem(pJsonObj[QLatin1String("Flag")].toString());
    item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(index, 2, item);

    // time
    item = new QTableWidgetItem(pJsonObj[QLatin1String("CollectAt")].toString());
    item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(index, 3, item);
    index++;
    itor++;
}

}
