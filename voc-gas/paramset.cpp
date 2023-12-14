#include "paramset.h"
#include "ui_paramset.h"
#include <QCheckBox>
#include <QComboBox>
#include "common.h"
#include <QFileDialog>

ParamSet::ParamSet(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ParamSet)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->tabWidget->setStyleSheet("QTabWidget#tabWidget{background-color:rgb(255,0,0);}\
                                 QTabBar::tab{background-color:rgb(220,200,180);width:100;height:40;color:rgb(0,0,0);font:10pt '新宋体'}\
                                 QTabBar::tab::selected{background-color:rgb(0,100,200);width:100;height:40;color:rgb(255,0,0);font:12pt '新宋体'}");

    setUserTableHeader();
    setUserTableContents();

    setTableHeader();
    setTableContents();

    setTableFactorHeader();
    setTableFactorContents();

    setTableUpHeader();
    setTableUpContents();

    System_Display();
    Fanchui_Display();

    ui->dateEdit->setDate(QDate::currentDate());
}

ParamSet::~ParamSet()
{

    delete ui;
}

void ParamSet::System_Display()
{

    QString dir_file = SYSTEM_SETTING_FILE;
    QFile file(dir_file);

    if(file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "配置文件格式错误！";
        }
        QJsonObject jsonObject= document.object();
        if(jsonObject.contains(SYS_SET1) && jsonObject.contains(SYS_SET2) && jsonObject.contains(SYS_SET3) && jsonObject.contains(SYS_SET4) && jsonObject.contains(SYS_SET5) && jsonObject.contains(SYS_SET6) && jsonObject.contains(SYS_SET7) && jsonObject.contains(SYS_SET8))
        {
            ui->lineEdit->setText(jsonObject.value(SYS_SET1).toString());
            ui->lineEdit_2->setText(jsonObject.value(SYS_SET2).toString());
            ui->lineEdit_3->setText(jsonObject.value(SYS_SET3).toString());
            ui->lineEdit_4->setText(jsonObject.value(SYS_SET4).toString());
            ui->lineEdit_5->setText(jsonObject.value(SYS_SET5).toString());
            ui->lineEdit_6->setText(jsonObject.value(SYS_SET6).toString());
            ui->lineEdit_7->setText(jsonObject.value(SYS_SET7).toString());
            ui->lineEdit_8->setText(jsonObject.value(SYS_SET8).toString());
            ui->checkBox->setChecked(jsonObject.value(SYS_CHECK).toBool());
            if(jsonObject.value(SYS_RADIOBUTTON1).toBool())
            {
                ui->radioButton->setChecked(true);
                ui->radioButton_2->setChecked(false);
            }
            else
            {
                ui->radioButton->setChecked(false);
                ui->radioButton_2->setChecked(true);
            }
        }
    }
}

void ParamSet::Fanchui_Display()
{

    QString dir_file = FAN_SETTING_FILE;
    QFile file(dir_file);

    if(file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "配置文件格式错误！";
        }
        QJsonObject jsonObject= document.object();
        if(jsonObject.contains(FAN_SET) && jsonObject.contains(FAN_LAST))
        {
            ui->lineEdit_9->setText(jsonObject.value(FAN_SET).toString());
            ui->lineEdit_10->setText(jsonObject.value(FAN_LAST).toString());
        }
    }
}

void ParamSet::loadfactorsInfo()
{
    QString filepath = QApplication::applicationDirPath()+"/voc-factors.json";
    QFile file(filepath);
    QJsonDocument jDoc;
    QJsonObject jObj;
    if(file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QByteArray bytArr;
        bytArr.append(file.readAll());
        jDoc = QJsonDocument::fromJson(bytArr);
        jObj = jDoc.object();
        bytArr.clear();
        file.close();
    }
    QJsonObject jFactor = jObj.value("factors").toObject();

    QJsonObject::iterator it = jFactor.begin();

    while(it!=jObj.end())
    {
        QString name = it.key();
        QJsonObject jValue = it.value().toObject();
        float alarmUpper = jValue["AlarmUpper"].toDouble();
        QString chan = jValue["Chan"].toString();
        bool isDisplay = jValue["Display"].toBool();
        float rangeLower = jValue["RangeLower"].toDouble();
        float rangeUpper = jValue["RangeUpper"].toDouble();
        QString unit = jValue["Unit"].toString();
        bool isUpload = jValue["Upload"].toBool();
        bool isUsed = jValue["Used"].toBool();
        QString index = jValue["Index"].toString();

        FactorInfo *fac = new FactorInfo();
        fac->m_name = name;
        fac->m_AlarmUpper = alarmUpper;
        fac->m_Chan = chan.toInt();
        fac->m_display = isDisplay;
        fac->m_Alias = index;
        fac->m_RangeLower = rangeLower;
        fac->m_RangeUpper = rangeUpper;
        fac->m_unit = unit;
        fac->m_upload = isUpload;
        fac->m_used = isUsed;

        fac->printFactor();
        curMap.insert(name,fac);

        it++;
    }

}

void ParamSet::setUserTableHeader()
{
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#1B89A1}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
headerText << QStringLiteral("用户类型") << QStringLiteral("用户密码") << QStringLiteral("清空数据")
           << QStringLiteral("参数设置") << QStringLiteral("反吹设置") << QStringLiteral("时间设置") << QStringLiteral("查看历史")
              << QStringLiteral("查看曲线") << QStringLiteral("FID-A") << QStringLiteral("FID-B")
                 << QStringLiteral("色谱柱实时温度") << QStringLiteral("已完成次数及仪器状态");
int cnt = headerText.count();
ui->tableWidget_2->setColumnCount(cnt);
ui->tableWidget_2->setHorizontalHeaderLabels(headerText);
// ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget_2->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
ui->tableWidget_2->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget_2->horizontalHeader()->font();
font.setBold(true);
ui->tableWidget_2->horizontalHeader()->setFont(font);

ui->tableWidget_2->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {100, 100, 100, 100, 100, 100, 100, 100, 100,100, 150, 180};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget_2->setColumnWidth(i, widths[i]);
}

ui->tableWidget_2->setStyleSheet(qssTV);
ui->tableWidget_2->horizontalHeader()->setVisible(true);
ui->tableWidget_2->verticalHeader()->setDefaultSectionSize(45);

ui->tableWidget_2->setFrameShape(QFrame::NoFrame);
ui->tableWidget_2->verticalHeader()->hide();
}


void ParamSet::setUserTableContents()
{
    QStringList pUserList;

    pUserList<< "超级管理员" << "管理员" << "普通用户";

    ui->tableWidget_2->clearContents();
    QTableWidgetItem *pItem1,*pItemPW;
    QCheckBox *pItemCheck1,*pItemCheck2,*pItemCheck3,*pItemCheck4,*pItemCheck5,*pItemCheck6,*pItemCheck7,*pItemCheck8,*pItemCheck9,*pItemCheck10;
    ui->tableWidget_2->setRowCount(3);

    QString dir_file = USERS_SETTING_FILE;
    QFile file(dir_file);

    if(file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "配置文件格式错误！";
        }
        QJsonObject jsonObject= document.object();
        if(jsonObject.contains(USERS))
        {
            // display tranverse
            QJsonObject pUsersJsonObj = jsonObject.value(USERS).toObject();

            QJsonObject::const_iterator it = pUsersJsonObj.constBegin();
            QJsonObject::const_iterator end = pUsersJsonObj.constEnd();

            ui->tableWidget_2->setRowCount(3);

            int row=0;
            while (it != end) {

                QJsonObject pJsonUser = it.value().toObject();

                pItem1 = new QTableWidgetItem(it.key());
                pItem1->setTextAlignment(Qt::AlignCenter);
                pItem1->setFlags(Qt::ItemIsEditable);
                ui->tableWidget_2->setItem(row, 0, pItem1);

                pItemPW = new QTableWidgetItem(pJsonUser.value(USERPASSWD).toString());
                pItemPW->setTextAlignment(Qt::AlignCenter);
                pItemPW->setFlags(Qt::ItemIsEditable);
                ui->tableWidget_2->setItem(row, 1, pItemPW);

                pItemCheck1 = new QCheckBox();
                if(pJsonUser.value(USESET_QINGKONG).toBool()) pItemCheck1->setCheckState(Qt::Checked);
                else pItemCheck1->setCheckState(Qt::Unchecked);
                pItemCheck1->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(row, 2, pItemCheck1);

                pItemCheck2 = new QCheckBox();
                if(pJsonUser.value(USESET_CANSHU).toBool()) pItemCheck2->setCheckState(Qt::Checked);
                else pItemCheck2->setCheckState(Qt::Unchecked);
                pItemCheck2->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(row, 3, pItemCheck2);

                pItemCheck3 = new QCheckBox();
                if(pJsonUser.value(USESET_FANCHUI).toBool()) pItemCheck3->setCheckState(Qt::Checked);
                else pItemCheck3->setCheckState(Qt::Unchecked);
                pItemCheck3->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(row, 4, pItemCheck3);

                pItemCheck4 = new QCheckBox();
                if(pJsonUser.value(USESET_SHIJIAN).toBool()) pItemCheck4->setCheckState(Qt::Checked);
                else pItemCheck4->setCheckState(Qt::Unchecked);
                pItemCheck4->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(row, 5, pItemCheck4);

                pItemCheck5 = new QCheckBox();
                if(pJsonUser.value(USESET_LISHI).toBool()) pItemCheck5->setCheckState(Qt::Checked);
                else pItemCheck5->setCheckState(Qt::Unchecked);
                pItemCheck5->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(row, 6, pItemCheck5);

                pItemCheck6 = new QCheckBox();
                if(pJsonUser.value(USESET_QUXIAN).toBool()) pItemCheck6->setCheckState(Qt::Checked);
                else pItemCheck6->setCheckState(Qt::Unchecked);
                pItemCheck6->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(row, 7, pItemCheck6);

                pItemCheck7 = new QCheckBox();
                if(pJsonUser.value(USESET_FIDA).toBool()) pItemCheck7->setCheckState(Qt::Checked);
                else pItemCheck7->setCheckState(Qt::Unchecked);
                pItemCheck7->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(row, 8, pItemCheck7);

                pItemCheck8 = new QCheckBox();
                if(pJsonUser.value(USESET_FIDB).toBool()) pItemCheck8->setCheckState(Qt::Checked);
                else pItemCheck8->setCheckState(Qt::Unchecked);
                pItemCheck8->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(row, 9, pItemCheck8);

                pItemCheck9 = new QCheckBox();
                if(pJsonUser.value(USESET_QINGKONG).toBool()) pItemCheck9->setCheckState(Qt::Checked);
                else pItemCheck9->setCheckState(Qt::Unchecked);
                pItemCheck9->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(row, 10, pItemCheck9);

                pItemCheck10 = new QCheckBox();
                if(pJsonUser.value(USESET_ZHUANGTAI).toBool()) pItemCheck10->setCheckState(Qt::Checked);
                else pItemCheck10->setCheckState(Qt::Unchecked);
                pItemCheck10->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(row, 11, pItemCheck10);

                it++;
                row++;
            }
        }
        else
        {
            int pCnt = pUserList.size();
            for(int index=0;index<pCnt;index++)
            {
                pItem1 = new QTableWidgetItem(pUserList.at(index));
                pItem1->setTextAlignment(Qt::AlignCenter);
                pItem1->setFlags(Qt::ItemIsEditable);
                ui->tableWidget_2->setItem(index, 0, pItem1);

                pItemPW = new QTableWidgetItem("-");
                pItemPW->setTextAlignment(Qt::AlignCenter);
                pItemPW->setFlags(Qt::ItemIsEditable);
                ui->tableWidget_2->setItem(index, 1, pItemPW);


                pItemCheck1 = new QCheckBox();
                pItemCheck1->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(index, 2, pItemCheck1);

                pItemCheck2 = new QCheckBox();
                pItemCheck2->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(index, 3, pItemCheck2);

                pItemCheck3 = new QCheckBox();
                pItemCheck3->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(index, 4, pItemCheck3);

                pItemCheck4 = new QCheckBox();
                pItemCheck4->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(index, 5, pItemCheck4);

                pItemCheck5 = new QCheckBox();
                pItemCheck5->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(index, 6, pItemCheck5);

                pItemCheck6 = new QCheckBox();
                pItemCheck6->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(index, 7, pItemCheck6);

                pItemCheck7 = new QCheckBox();
                pItemCheck7->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(index, 8, pItemCheck7);

                pItemCheck8 = new QCheckBox();
                pItemCheck8->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(index, 9, pItemCheck8);

                pItemCheck9 = new QCheckBox();
                pItemCheck9->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget_2->setCellWidget(index, 10, pItemCheck9);

                pItemCheck10 = new QCheckBox();
                pItemCheck10->setStyleSheet("QCheckBox{spacing: 8px;} QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");

                ui->tableWidget_2->setCellWidget(index, 11, pItemCheck10);
            }
        }
    }
}

void ParamSet::setTableFactorContents()
{
    // 检查voc 配置文件是否存在
    QStringList pFactorList,pFactorAlias,pFactorInput,pFactorUnit;
    pFactorList << "总烃"<<"总烃干值" << "非甲烷总烃"<<"非甲烷总烃干值"
                <<"折算非甲烷总烃"<<"折算非甲烷总烃干值"<<"折算非甲烷总烃排放量"
                << "甲烷"<<"甲烷干值" << "烟气温度" << "烟气压力"
                << "烟气流速" << "工况流量" << "标况流量" << "苯系物含量"
                << "烟尘湿度" << "氧气含量"  << "烟气湿度" << "苯含量"
                << "甲苯" << "间二甲苯" << "零二甲苯" << "烟尘干值";
    pFactorAlias << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10" << "11" << "12" << "13" << "14" << "15" << "16" << "17" << "18" << "19" << "20" << "21" << "22" << "23" << "24" << "25" << "26" << "27" << "28" << "29" << "30";
    pFactorInput << "0" << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10" << "11" << "12" << "13" << "14";
    pFactorUnit << "mg/m^3" << "mg/Nm^3" << "m^3/h" << "m^3/s" << "kg/h" << "%" << "℃" << "Pa" << "KPa"<< "m/s"<< "PPM";

    ui->tableWidget->clearContents();
    QTableWidgetItem *pItem1 = nullptr,*pRangeUp = nullptr,*pRangeDown = nullptr,*pAlarmUp = nullptr;
    QComboBox *pUnit = nullptr,*pChan = nullptr,*pIndex = nullptr;
    QCheckBox *pDisplay = nullptr,*pUpload = nullptr,*pUsed = nullptr;

    QString dir_file = QApplication::applicationDirPath()+"/voc-factors.json";
    QFile file(dir_file);

    if(file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "配置文件格式错误！";
        }
        QJsonObject jsonObject= document.object();
        if(jsonObject.contains(FACTORS))
        {
            // display tranverse
            QJsonObject pFactorsJsonObj = jsonObject.value(FACTORS).toObject();

            QJsonObject::const_iterator it = pFactorsJsonObj.constBegin();
            QJsonObject::const_iterator end = pFactorsJsonObj.constEnd();

            ui->tableWidget->setRowCount(pFactorList.size());

            int row=0;
            while (it != end) {

                QJsonObject pJsonFactor = it.value().toObject();

                pItem1 = new QTableWidgetItem(it.key());
                pItem1->setTextAlignment(Qt::AlignCenter);
                pItem1->setFlags(Qt::ItemIsEditable);
                ui->tableWidget->setItem(row, 0, pItem1);

                pUnit = new QComboBox();
                for(int i=0;i<11;i++)
                {
                   pUnit->addItem(pFactorUnit.at(i));
                }
                pUnit->setCurrentText(pJsonFactor.value(UNIT).toString());
                ui->tableWidget->setCellWidget(row, 1, pUnit);

                pChan = new QComboBox();
                for(int i=0;i<15;i++)
                {
                   pChan->addItem(pFactorInput.at(i));
                }
                pChan->setCurrentText(pJsonFactor.value(CHAN).toString());
                ui->tableWidget->setCellWidget(row, 2, pChan);

                pRangeUp = new QTableWidgetItem(pJsonFactor.value(RANGEUPPER).toString());
                pRangeUp->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(row, 3, pRangeUp);

                pRangeDown = new QTableWidgetItem(pJsonFactor.value(RANGELOWER).toString());
                pRangeDown->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(row, 4, pRangeDown);

                pAlarmUp = new QTableWidgetItem(pJsonFactor.value(ALARMUPPER).toString());
                pAlarmUp->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(row, 5, pAlarmUp);

                pDisplay = new QCheckBox();
                if(pJsonFactor.value(DISPLAY).toBool()) pDisplay->setCheckState(Qt::Checked);
                else pDisplay->setCheckState(Qt::Unchecked);
                pDisplay->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget->setCellWidget(row, 6, pDisplay);

                pUpload = new QCheckBox();
                if(pJsonFactor.value(UPLOAD).toBool()) pUpload->setCheckState(Qt::Checked);
                else pUpload->setCheckState(Qt::Unchecked);
                pUpload->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget->setCellWidget(row, 7, pUpload);

                pUsed = new QCheckBox();
                if(pJsonFactor.value(USED).toBool()) pUsed->setCheckState(Qt::Checked);
                else pUsed->setCheckState(Qt::Unchecked);
                pUsed->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget->setCellWidget(row, 8, pUsed);

                pIndex = new QComboBox();
                for(int i=0;i<30;i++)
                {
                   pIndex->addItem(pFactorAlias.at(i));
                }
                pIndex->setCurrentText(pJsonFactor.value(INDEX).toString());
                ui->tableWidget->setCellWidget(row, 9, pIndex);

                it++;
                row++;
            }
        }
        else
        {
            int pFactorSize = pFactorList.size();
            ui->tableWidget->setRowCount(pFactorSize);

            for(int i=0;i<pFactorSize;i++)
            {
                pItem1 = new QTableWidgetItem(pFactorList.at(i));
                pItem1->setTextAlignment(Qt::AlignCenter);
                pItem1->setFlags(Qt::ItemIsEditable);
                ui->tableWidget->setItem(i, 0, pItem1);

                pUnit = new QComboBox();
                for(int i=0;i<11;i++)
                {
                   pUnit->addItem(pFactorUnit.at(i));
                }
                ui->tableWidget->setCellWidget(i, 1, pUnit);

                pChan = new QComboBox();
                for(int i=0;i<15;i++)
                {
                   pChan->addItem(pFactorInput.at(i));
                }
                ui->tableWidget->setCellWidget(i, 2, pChan);

                pRangeUp = new QTableWidgetItem("0");
                pRangeUp->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(i, 3, pRangeUp);

                pRangeDown = new QTableWidgetItem("0");
                pRangeDown->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(i, 4, pRangeDown);

                pAlarmUp = new QTableWidgetItem("0");
                pAlarmUp->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget->setItem(i, 5, pAlarmUp);

                pDisplay = new QCheckBox();
                pDisplay->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget->setCellWidget(i, 6, pDisplay);

                pUpload = new QCheckBox();
                pUpload->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget->setCellWidget(i, 7, pUpload);

                pUsed = new QCheckBox();
                pUsed->setStyleSheet("QCheckBox{spacing: 8px;}QCheckBox::indicator{width: 60px;height: 30px;} QCheckBox::indicator:unchecked {image: url(:/images/Off.png);} QCheckBox::indicator:checked {image: url(:/images/On.png);}");
                ui->tableWidget->setCellWidget(i, 8, pUsed);

                pIndex = new QComboBox();
                for(int i=0;i<30;i++)
                {
                   pIndex->addItem(pFactorAlias.at(i));
                }
                ui->tableWidget->setCellWidget(i, 9, pIndex);
            }
        }
    }
}


void ParamSet::setTableFactorHeader()
{
    QString qssTV = QLatin1String("QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
headerText << QStringLiteral("因子名称") << QStringLiteral("因子单位") << QStringLiteral("输入通道")
           << QStringLiteral("量程上限") << QStringLiteral("量程下限") << QStringLiteral("报警上限") << QStringLiteral("是否显示")
           << QStringLiteral("是否上传") << QStringLiteral("折算启用") << QStringLiteral("排列序号");
int cnt = headerText.count();
ui->tableWidget->setColumnCount(cnt);
ui->tableWidget->setHorizontalHeaderLabels(headerText);
// ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget->horizontalHeader()->setFont(QFont(QLatin1String("song"), 10));
ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget->horizontalHeader()->font();
font.setBold(true);
ui->tableWidget->horizontalHeader()->setFont(font);

ui->tableWidget->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {100, 70, 70, 70, 70, 70, 70, 70, 70, 70};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget->setColumnWidth(i, widths[i]);
}

ui->tableWidget->setStyleSheet(qssTV);
ui->tableWidget->horizontalHeader()->setVisible(true);
ui->tableWidget->verticalHeader()->setDefaultSectionSize(45);
ui->tableWidget->setFrameShape(QFrame::NoFrame);
ui->tableWidget->verticalHeader()->hide();
}

void ParamSet::setTableContents()
{
    QStringList pComList,pObjList,pBaudList,parityList,protocolList,pDataBitList,pStopBitList;
    pDataBitList << "5" << "6" <<"7" <<"8";
    pStopBitList << "1" << "1.5" <<"2";
    pObjList << "Modbus" << "PLC";
    protocolList << "天蓝" << "鲁南" << "SPRT" << "VOCS";
    parityList << "None" << "Odd" << "Even" << "Mark" <<"Space";
    pBaudList << "600" << "1200" << "2400" << "4800" << "9600" << "19200" << "38400" << "43000" << "56000" << "57900" << "115200";
    pComList << "COM1" << "COM2" << "COM3" << "COM4" << "COM5" << "COM6" << "COM7" << "COM8" << "COM9" << "COM10" << "COM11" << "COM12" << "COM13" << "COM14" << "COM15" << "COM16" << "COM17" << "COM18" << "COM19" << "COM20";

    ui->tableWidget_Down->clearContents();
    QTableWidgetItem *pItem1;
    QComboBox *pCom,*pBaud,*pParity,*pDatabit,*pStopbit,*pType;

    QString dir_file = COMM_SETTING_FILE;
    QFile file(dir_file);

    if(file.exists())
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "配置文件格式错误！";
        }
        QJsonObject jsonObject= document.object();
        if(jsonObject.contains(PORTSDOWN))
        {
            if(jsonObject.contains(PORTSDOWN))
            {
                // display tranverse
                QJsonObject pFactorsJsonObj = jsonObject.value(PORTSDOWN).toObject();

                QJsonObject::const_iterator it = pFactorsJsonObj.constBegin();
                QJsonObject::const_iterator end = pFactorsJsonObj.constEnd();

                ui->tableWidget_Down->setRowCount(2);

                int row=0;
                while (it != end) {

                    QJsonObject pJsonCom = it.value().toObject();

                    pItem1 = new QTableWidgetItem(it.key());
                    pItem1->setTextAlignment(Qt::AlignCenter);
                    pItem1->setFlags(Qt::ItemIsEditable);
                    ui->tableWidget_Down->setItem(row, 0, pItem1);

                    pCom = new QComboBox();
                    for(int i=0;i<pComList.size();i++)
                    {
                       pCom->addItem(pComList.at(i));
                    }
                    pCom->setCurrentText(pJsonCom.value(PORTNAME).toString());
                    ui->tableWidget_Down->setCellWidget(row, 1, pCom);

                    pBaud = new QComboBox();
                    for(int i=0;i<pBaudList.size();i++)
                    {
                       pBaud->addItem(pBaudList.at(i));
                    }
                    pBaud->setCurrentText(pJsonCom.value(BAUD).toString());
                    ui->tableWidget_Down->setCellWidget(row, 2, pBaud);

                    pParity = new QComboBox();
                    for(int i=0;i<5;i++)
                    {
                       pParity->addItem(parityList.at(i));
                    }
                    pParity->setCurrentText(pJsonCom.value(PARITY).toString());
                    ui->tableWidget_Down->setCellWidget(row, 3, pParity);

                    pDatabit = new QComboBox();
                    for(int i=0;i<4;i++)
                    {
                       pDatabit->addItem(pDataBitList.at(i));
                    }
                    pDatabit->setCurrentText(pJsonCom.value(DATABIT).toString());
                    ui->tableWidget_Down->setCellWidget(row, 4, pDatabit);

                    pStopbit = new QComboBox();
                    for(int i=0;i<3;i++)
                    {
                       pStopbit->addItem(pStopBitList.at(i));
                    }
                    pStopbit->setCurrentText(pJsonCom.value(STOPBIT).toString());
                    ui->tableWidget_Down->setCellWidget(row, 5, pStopbit);


                    pType = new QComboBox();
                    for(int i=0;i<protocolList.size();i++)
                    {
                       pType->addItem(protocolList.at(i));
                    }
                    pType->setCurrentText(pJsonCom.value(DEVICETYPE).toString());
                    ui->tableWidget_Down->setCellWidget(row, 6, pType);

                    it++;
                    row++;
                }
            }
        }
        else
        {
            ui->tableWidget_Down->setRowCount(2);

            int pCnt = pObjList.size();
            for(int index=0;index<pCnt;index++)
            {
                pItem1 = new QTableWidgetItem(pObjList.at(index));
                pItem1->setTextAlignment(Qt::AlignCenter);
                pItem1->setFlags(Qt::ItemIsEditable);
                ui->tableWidget_Down->setItem(index, 0, pItem1);

                pCom = new QComboBox();
                int pComCnt = pComList.size();
                for(int i=0;i<pComCnt;i++)
                {
                   pCom->addItem(pComList.at(i));
                }
                ui->tableWidget_Down->setCellWidget(index, 1, pCom);

                pBaud = new QComboBox();
                int pBaudCnt = pBaudList.size();
                for(int i=0;i<pBaudCnt;i++)
                {
                   pBaud->addItem(pBaudList.at(i));
                }
                ui->tableWidget_Down->setCellWidget(index, 2, pBaud);

                pParity = new QComboBox();
                int pairtyCnt = parityList.size();
                for(int i=0;i<pairtyCnt;i++)
                {
                   pParity->addItem(parityList.at(i));
                }
                ui->tableWidget_Down->setCellWidget(index, 3, pParity);

                pDatabit = new QComboBox();
                pDatabit->addItem("8");
                pDatabit->addItem("7");
                pDatabit->addItem("6");
                pDatabit->addItem("5");
                ui->tableWidget_Down->setCellWidget(index, 4, pDatabit);

                pStopbit = new QComboBox();
                pStopbit->addItem("1");
                pStopbit->addItem("1.5");
                pStopbit->addItem("2");
                ui->tableWidget_Down->setCellWidget(index, 5, pStopbit);

                pType = new QComboBox();
                int protoCnt = protocolList.size();
                for(int i=0;i<protoCnt;i++)
                {
                   pType->addItem(protocolList.at(i));
                }
                ui->tableWidget_Down->setCellWidget(index, 6, pType);
            }

        }
    }

}


void ParamSet::setTableHeader()
{

    QString qssTV = QLatin1String("QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
headerText << QStringLiteral("通讯目标") << QStringLiteral("串口号") << QStringLiteral("波特率")
           << QStringLiteral("校验位") << QStringLiteral("数据位") << QStringLiteral("停止位") << QStringLiteral("型号");
int cnt = headerText.count();
ui->tableWidget_Down->setColumnCount(cnt);
ui->tableWidget_Down->setHorizontalHeaderLabels(headerText);
// ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget_Down->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget_Down->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
ui->tableWidget_Down->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget_Down->horizontalHeader()->font();
font.setBold(true);
ui->tableWidget_Down->horizontalHeader()->setFont(font);

ui->tableWidget_Down->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {130, 80, 80, 80, 80, 80, 150};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget_Down->setColumnWidth(i, widths[i]);
}

ui->tableWidget_Down->setStyleSheet(qssTV);
ui->tableWidget_Down->horizontalHeader()->setVisible(true);
ui->tableWidget_Down->verticalHeader()->setDefaultSectionSize(45);

ui->tableWidget_Down->setFrameShape(QFrame::NoFrame);
ui->tableWidget_Down->verticalHeader()->hide();
}

void ParamSet::setTableUpContents()
{
    QStringList pComList,pObjList,pBaudList,parityList,protocolList,pDataBitList,pStopBitList;
    pDataBitList << "5" << "6" <<"7" <<"8";
    pStopBitList << "1" << "1.5" <<"2";
    pObjList << "HJ212" << "HJ212-2";
    protocolList << "2017" << "2005";
    parityList << "None" << "Odd" << "Even" << "Mark" <<"Space";
    pBaudList << "600" << "1200" << "2400" << "4800" << "9600" << "19200" << "38400" << "43000" << "56000" << "57900" << "115200";
    pComList << "COM1" << "COM2" << "COM3" << "COM4" << "COM5" << "COM6" << "COM7" << "COM8" << "COM9" << "COM10" << "COM11" << "COM12" << "COM13" << "COM14" << "COM15" << "COM16" << "COM17" << "COM18" << "COM19" << "COM20";

    ui->tableWidget_Up->clearContents();
    QTableWidgetItem *pItem1;
    QComboBox *pCom,*pBaud,*pParity,*pDatabit,*pStopbit,*pType;

    QString dir_file = COMM_SETTING_FILE;
    QFile file(dir_file);

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString value = file.readAll();
    file.close();
    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
    if(!(parseJsonErr.error == QJsonParseError::NoError))
    {
        QLOG_ERROR() << "配置文件格式错误！";
    }
    QJsonObject jsonObject= document.object();
    if(jsonObject.contains(PORTSUP))
    {
        if(jsonObject.contains(PORTSUP))
        {
            // display tranverse
            QJsonObject pFactorsJsonObj = jsonObject.value(PORTSUP).toObject();

            QJsonObject::const_iterator it = pFactorsJsonObj.constBegin();
            QJsonObject::const_iterator end = pFactorsJsonObj.constEnd();

            ui->tableWidget_Up->setRowCount(2);

            int row=0;
            while (it != end) {

                QJsonObject pJsonCom = it.value().toObject();

                pItem1 = new QTableWidgetItem(it.key());
                pItem1->setTextAlignment(Qt::AlignCenter);
                pItem1->setFlags(Qt::ItemIsEditable);
                ui->tableWidget_Up->setItem(row, 0, pItem1);

                pCom = new QComboBox();
                for(int i=0;i<pComList.size();i++)
                {
                   pCom->addItem(pComList.at(i));
                }
                pCom->setCurrentText(pJsonCom.value(PORTNAME).toString());
                ui->tableWidget_Up->setCellWidget(row, 1, pCom);

                pBaud = new QComboBox();
                for(int i=0;i<pBaudList.size();i++)
                {
                   pBaud->addItem(pBaudList.at(i));
                }
                pBaud->setCurrentText(pJsonCom.value(BAUD).toString());
                ui->tableWidget_Up->setCellWidget(row, 2, pBaud);

                pParity = new QComboBox();
                for(int i=0;i<5;i++)
                {
                   pParity->addItem(parityList.at(i));
                }
                pParity->setCurrentText(pJsonCom.value(PARITY).toString());
                ui->tableWidget_Up->setCellWidget(row, 3, pParity);

                pDatabit = new QComboBox();
                for(int i=0;i<4;i++)
                {
                   pDatabit->addItem(pDataBitList.at(i));
                }
                pDatabit->setCurrentText(pJsonCom.value(DATABIT).toString());
                ui->tableWidget_Up->setCellWidget(row, 4, pDatabit);

                pStopbit = new QComboBox();
                for(int i=0;i<3;i++)
                {
                   pStopbit->addItem(pStopBitList.at(i));
                }
                pStopbit->setCurrentText(pJsonCom.value(STOPBIT).toString());
                ui->tableWidget_Up->setCellWidget(row, 5, pStopbit);


                pType = new QComboBox();
                for(int i=0;i<protocolList.size();i++)
                {
                   pType->addItem(protocolList.at(i));
                }
                pType->setCurrentText(pJsonCom.value(DEVICETYPE).toString());
                ui->tableWidget_Up->setCellWidget(row, 6, pType);

                it++;
                row++;
            }
        }
    }
    else
    {
        ui->tableWidget_Up->setRowCount(2);

        int pCnt = pObjList.size();
        for(int index=0;index<pCnt;index++)
        {
            pItem1 = new QTableWidgetItem(pObjList.at(index));
            pItem1->setTextAlignment(Qt::AlignCenter);
            pItem1->setFlags(Qt::ItemIsEditable);
            ui->tableWidget_Up->setItem(index, 0, pItem1);

            pCom = new QComboBox();
            int pComCnt = pComList.size();
            for(int i=0;i<pComCnt;i++)
            {
               pCom->addItem(pComList.at(i));
            }
            ui->tableWidget_Up->setCellWidget(index, 1, pCom);

            pBaud = new QComboBox();
            int pBaudCnt = pBaudList.size();
            for(int i=0;i<pBaudCnt;i++)
            {
               pBaud->addItem(pBaudList.at(i));
            }
            ui->tableWidget_Up->setCellWidget(index, 2, pBaud);

            pParity = new QComboBox();
            int pairtyCnt = parityList.size();
            for(int i=0;i<pairtyCnt;i++)
            {
               pParity->addItem(parityList.at(i));
            }
            ui->tableWidget_Up->setCellWidget(index, 3, pParity);

            pDatabit = new QComboBox();
            pDatabit->addItem("8");
            pDatabit->addItem("7");
            pDatabit->addItem("6");
            pDatabit->addItem("5");
            ui->tableWidget_Up->setCellWidget(index, 4, pDatabit);

            pStopbit = new QComboBox();
            pStopbit->addItem("1");
            pStopbit->addItem("1.5");
            pStopbit->addItem("2");
            ui->tableWidget_Up->setCellWidget(index, 5, pStopbit);

            pType = new QComboBox();
            int protoCnt = protocolList.size();
            for(int i=0;i<protoCnt;i++)
            {
               pType->addItem(protocolList.at(i));
            }
            ui->tableWidget_Up->setCellWidget(index, 6, pType);
        }
    }
}

void ParamSet::setTableUpHeader()
{
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#1B89A1}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
"QTableWidget{background-color:white;border:none;}");
//设置表头
QStringList headerText;
headerText << QStringLiteral("通讯目标") << QStringLiteral("串口号") << QStringLiteral("波特率")
           << QStringLiteral("校验位") << QStringLiteral("数据位") << QStringLiteral("停止位") << QStringLiteral("型号");
int cnt = headerText.count();
ui->tableWidget_Up->setColumnCount(cnt);
ui->tableWidget_Up->setHorizontalHeaderLabels(headerText);
// ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
ui->tableWidget_Up->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

ui->tableWidget_Up->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
ui->tableWidget_Up->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
QFont font =  ui->tableWidget_Up->horizontalHeader()->font();
font.setBold(true);
ui->tableWidget_Up->horizontalHeader()->setFont(font);

ui->tableWidget_Up->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

int widths[] = {130, 80, 80, 80, 80, 80, 150};
for (int i = 0;i < cnt; ++ i){ //列编号从0开始
    ui->tableWidget_Up->setColumnWidth(i, widths[i]);
}

ui->tableWidget_Up->setStyleSheet(qssTV);
ui->tableWidget_Up->horizontalHeader()->setVisible(true);
ui->tableWidget_Up->verticalHeader()->setDefaultSectionSize(45);
ui->tableWidget_Up->setFrameShape(QFrame::NoFrame);
ui->tableWidget_Up->verticalHeader()->hide();
}

void ParamSet::on_pushButton_clicked()
{
    ui->textBrowser->clear();

    QString dir_root = LOG_PATH;

    // 声明目录对象
    QString path_root = ui->dateEdit->date().toString(QLatin1String("yyyy-MM"));
    QString file_name = ui->dateEdit->date().toString(QLatin1String("dd")) + ".txt";

    QString dir_str = dir_root + path_root;
    QString pDir_FileName = dir_str + "/" + file_name;
    QFile file(pDir_FileName);

    //打开文件
    bool isOK = file.open(QIODevice::ReadOnly);
    if(isOK == true){

        QByteArray array;
        while (file.atEnd() == false) {
            //读一行
            array = file.readLine();
            ui->textBrowser->append(array.trimmed());
        }

        QMessageBox::about(NULL, "提示", "<font color='black'>获取日志信息成功！</font>");
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>未查询到相关日志信息！</font>");
    }
    file.close();
}

bool ParamSet::Save_FactorSet()
{
    QFile file(QApplication::applicationDirPath()+"/voc-factors.json");
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "配置文件格式错误！";
            return false;
        }

        bool pCheckState = false;

        QJsonObject jsonObject = document.object();
        if(jsonObject.contains(FACTORS))
        {
            QJsonValueRef RefVersion = jsonObject.find(FACTORS).value();
            QJsonObject jsonObjectNode = RefVersion.toObject();

            for(int row=0; row<ui->tableWidget->rowCount(); row++)
            {
                QString pFactorName = ui->tableWidget->item(row,0)->text();

                QJsonValueRef RefVersionF = jsonObjectNode.find(pFactorName).value();
                QJsonObject jsonObjectNodeF = RefVersionF.toObject();

                QComboBox *pBoxUnit = (QComboBox *)ui->tableWidget->cellWidget(row,1);
                QJsonValueRef ElementOneValueRefUnit = jsonObjectNodeF.find(UNIT).value();
                ElementOneValueRefUnit = QJsonValue(pBoxUnit->currentText());

                QComboBox *pBoxChan = (QComboBox *)ui->tableWidget->cellWidget(row,2);
                QJsonValueRef ElementOneValueRefChan = jsonObjectNodeF.find(CHAN).value();
                ElementOneValueRefChan = QJsonValue(pBoxChan->currentText());

                QJsonValueRef ElementOneValueRefRU = jsonObjectNodeF.find(RANGEUPPER).value();
                ElementOneValueRefRU = QJsonValue(ui->tableWidget->item(row,3)->text());

                QJsonValueRef ElementOneValueRefRL = jsonObjectNodeF.find(RANGELOWER).value();
                ElementOneValueRefRL = QJsonValue(ui->tableWidget->item(row,4)->text());

                QJsonValueRef ElementOneValueRefAU = jsonObjectNodeF.find(ALARMUPPER).value();
                ElementOneValueRefAU = QJsonValue(ui->tableWidget->item(row,5)->text());

                QCheckBox *pCheckDisplay = (QCheckBox *)ui->tableWidget->cellWidget(row,6);
                if(pCheckDisplay->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefDisplay = jsonObjectNodeF.find(DISPLAY).value();
                ElementOneValueRefDisplay = QJsonValue(pCheckState);

                QCheckBox *pCheckUpload = (QCheckBox *)ui->tableWidget->cellWidget(row,7);
                if(pCheckUpload->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefUpload = jsonObjectNodeF.find(UPLOAD).value();
                ElementOneValueRefUpload = QJsonValue(pCheckState);

                QCheckBox *pCheckUsed = (QCheckBox *)ui->tableWidget->cellWidget(row,8);
                if(pCheckUsed->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefUsed = jsonObjectNodeF.find(USED).value();
                ElementOneValueRefUsed = QJsonValue(pCheckState);

                QComboBox *pBoxIndex = (QComboBox *)ui->tableWidget->cellWidget(row,9);
                QJsonValueRef ElementOneValueRefIndex = jsonObjectNodeF.find(INDEX).value();
                ElementOneValueRefIndex = QJsonValue(pBoxIndex->currentText());

                RefVersionF = jsonObjectNodeF;
            }

            RefVersion = jsonObjectNode;
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();

            return true;
        }
        else {

            QJsonObject pRootJsonObj;
            QJsonObject pFactorsJsonObj;

            for(int row=0; row<ui->tableWidget->rowCount(); row++)
            {
                QJsonObject pFactorJsonObj;

                QComboBox *pBoxUnit = (QComboBox *)ui->tableWidget->cellWidget(row,1);
                pFactorJsonObj.insert(UNIT,pBoxUnit->currentText());

                QComboBox *pBoxChan = (QComboBox *)ui->tableWidget->cellWidget(row,2);
                pFactorJsonObj.insert(CHAN,pBoxChan->currentText());

                pFactorJsonObj.insert(RANGEUPPER,ui->tableWidget->item(row,3)->text());
                pFactorJsonObj.insert(RANGELOWER,ui->tableWidget->item(row,4)->text());
                pFactorJsonObj.insert(ALARMUPPER,ui->tableWidget->item(row,5)->text());

                QCheckBox *pCheckDisplay = (QCheckBox *)ui->tableWidget->cellWidget(row,6);
                if(pCheckDisplay->checkState() == Qt::Checked) pFactorJsonObj.insert(DISPLAY,true);
                else pFactorJsonObj.insert(DISPLAY,false);

                QCheckBox *pCheckUpload = (QCheckBox *)ui->tableWidget->cellWidget(row,7);
                if(pCheckUpload->checkState() == Qt::Checked) pFactorJsonObj.insert(UPLOAD,true);
                else pFactorJsonObj.insert(UPLOAD,false);

                QCheckBox *pCheckUsed = (QCheckBox *)ui->tableWidget->cellWidget(row,8);
                if(pCheckUsed->checkState() == Qt::Checked) pFactorJsonObj.insert(USED,true);
                else pFactorJsonObj.insert(USED,false);

                QComboBox *pBoxIndex = (QComboBox *)ui->tableWidget->cellWidget(row,9);
                pFactorJsonObj.insert(INDEX,pBoxIndex->currentText());

                pFactorsJsonObj.insert(ui->tableWidget->item(row,0)->text(),pFactorJsonObj);

            }

            pRootJsonObj.insert(FACTORS,pFactorsJsonObj);

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(pRootJsonObj);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
    }
    return false;
}


void ParamSet::on_pushButton_10_clicked()
{
    // 因子

    if(Save_FactorSet())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>保存因子信息成功！</font>");
    }
}

bool ParamSet::Save_CommSet()
{
    QFile file(COMM_SETTING_FILE);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "配置文件格式错误！";
            return false;
        }

        QJsonObject jsonObject = document.object();
        if(jsonObject.contains(PORTSDOWN) && jsonObject.contains(PORTSUP))
        {
            QJsonValueRef RefVersionDown = jsonObject.find(PORTSDOWN).value();
            QJsonObject jsonObjectNodeDown = RefVersionDown.toObject();

            QJsonValueRef RefVersionUp = jsonObject.find(PORTSUP).value();
            QJsonObject jsonObjectNodeUp = RefVersionUp.toObject();

            for(int row=0; row<2; row++)
            {
                QString pDevice = ui->tableWidget_Down->item(row,0)->text();

                QJsonValueRef RefVersionComm = jsonObjectNodeDown.find(pDevice).value();
                QJsonObject jsonObjectNodeC = RefVersionComm.toObject();

                QComboBox *pBoxPortName = (QComboBox *)ui->tableWidget_Down->cellWidget(row,1);
                QJsonValueRef ElementOneValueRefName = jsonObjectNodeC.find(PORTNAME).value();
                ElementOneValueRefName = QJsonValue(pBoxPortName->currentText());

                QComboBox *pBoxPortBaud = (QComboBox *)ui->tableWidget_Down->cellWidget(row,2);
                QJsonValueRef ElementOneValueRefBaud = jsonObjectNodeC.find(BAUD).value();
                ElementOneValueRefBaud = QJsonValue(pBoxPortBaud->currentText());

                QComboBox *pBoxPortParity = (QComboBox *)ui->tableWidget_Down->cellWidget(row,3);
                QJsonValueRef ElementOneValueRefParity = jsonObjectNodeC.find(PARITY).value();
                ElementOneValueRefParity = QJsonValue(pBoxPortParity->currentText());

                QComboBox *pBoxPortDataBit = (QComboBox *)ui->tableWidget_Down->cellWidget(row,4);
                QJsonValueRef ElementOneValueRefDatabit = jsonObjectNodeC.find(DATABIT).value();
                ElementOneValueRefDatabit = QJsonValue(pBoxPortDataBit->currentText());

                QComboBox *pBoxPortStopBit = (QComboBox *)ui->tableWidget_Down->cellWidget(row,5);
                QJsonValueRef ElementOneValueRefStopBit = jsonObjectNodeC.find(STOPBIT).value();
                ElementOneValueRefStopBit = QJsonValue(pBoxPortStopBit->currentText());

                QComboBox *pBoxPortDevType = (QComboBox *)ui->tableWidget_Down->cellWidget(row,6);
                QJsonValueRef ElementOneValueRefDevType = jsonObjectNodeC.find(DEVICETYPE).value();
                ElementOneValueRefDevType = QJsonValue(pBoxPortDevType->currentText());

                RefVersionComm = jsonObjectNodeC;
            }

            for(int row=0; row<2; row++)
            {
                QString pDevice = ui->tableWidget_Up->item(row,0)->text();

                QJsonValueRef RefVersionComm = jsonObjectNodeUp.find(pDevice).value();
                QJsonObject jsonObjectNodeC = RefVersionComm.toObject();

                QComboBox *pBoxPortName = (QComboBox *)ui->tableWidget_Up->cellWidget(row,1);
                QJsonValueRef ElementOneValueRefName = jsonObjectNodeC.find(PORTNAME).value();
                ElementOneValueRefName = QJsonValue(pBoxPortName->currentText());

                QComboBox *pBoxPortBaud = (QComboBox *)ui->tableWidget_Up->cellWidget(row,2);
                QJsonValueRef ElementOneValueRefBaud = jsonObjectNodeC.find(BAUD).value();
                ElementOneValueRefBaud = QJsonValue(pBoxPortBaud->currentText());

                QComboBox *pBoxPortParity = (QComboBox *)ui->tableWidget_Up->cellWidget(row,3);
                QJsonValueRef ElementOneValueRefParity = jsonObjectNodeC.find(PARITY).value();
                ElementOneValueRefParity = QJsonValue(pBoxPortParity->currentText());

                QComboBox *pBoxPortDataBit = (QComboBox *)ui->tableWidget_Up->cellWidget(row,4);
                QJsonValueRef ElementOneValueRefDatabit = jsonObjectNodeC.find(DATABIT).value();
                ElementOneValueRefDatabit = QJsonValue(pBoxPortDataBit->currentText());

                QComboBox *pBoxPortStopBit = (QComboBox *)ui->tableWidget_Up->cellWidget(row,5);
                QJsonValueRef ElementOneValueRefStopBit = jsonObjectNodeC.find(STOPBIT).value();
                ElementOneValueRefStopBit = QJsonValue(pBoxPortStopBit->currentText());

                QComboBox *pBoxPortDevType = (QComboBox *)ui->tableWidget_Up->cellWidget(row,6);
                QJsonValueRef ElementOneValueRefDevType = jsonObjectNodeC.find(DEVICETYPE).value();
                ElementOneValueRefDevType = QJsonValue(pBoxPortDevType->currentText());

                RefVersionComm = jsonObjectNodeC;
            }

            RefVersionDown = jsonObjectNodeDown;
            RefVersionUp = jsonObjectNodeUp;
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();

            return true;
        }
        else {

            QJsonObject pRootJsonObj;
            QJsonObject pCommsJsonObjUp;
            QJsonObject pCommsJsonObjDown;

            for(int row=0; row<2; row++)    // 下行
            {
                QJsonObject pComJsonObj;

                QComboBox *pBoxPortName = (QComboBox *)ui->tableWidget_Down->cellWidget(row,1);
                pComJsonObj.insert(PORTNAME,pBoxPortName->currentText());

                QComboBox *pBoxPortBaud = (QComboBox *)ui->tableWidget_Down->cellWidget(row,2);
                pComJsonObj.insert(BAUD,pBoxPortBaud->currentText());

                QComboBox *pBoxPortParity = (QComboBox *)ui->tableWidget_Down->cellWidget(row,3);
                pComJsonObj.insert(PARITY,pBoxPortParity->currentText());

                QComboBox *pBoxPortDataBit = (QComboBox *)ui->tableWidget_Down->cellWidget(row,4);
                pComJsonObj.insert(DATABIT,pBoxPortDataBit->currentText());

                QComboBox *pBoxPortStopBit = (QComboBox *)ui->tableWidget_Down->cellWidget(row,5);
                pComJsonObj.insert(STOPBIT,pBoxPortStopBit->currentText());

                QComboBox *pBoxPortDeviceType = (QComboBox *)ui->tableWidget_Down->cellWidget(row,6);
                pComJsonObj.insert(DEVICETYPE,pBoxPortDeviceType->currentText());

                pCommsJsonObjDown.insert(ui->tableWidget_Down->item(row,0)->text(),pComJsonObj);
            }

            for(int row=0; row<2; row++)    // 上行
            {
                QJsonObject pComJsonObj;

                QComboBox *pBoxPortName = (QComboBox *)ui->tableWidget_Up->cellWidget(row,1);
                pComJsonObj.insert(PORTNAME,pBoxPortName->currentText());

                QComboBox *pBoxPortBaud = (QComboBox *)ui->tableWidget_Up->cellWidget(row,2);
                pComJsonObj.insert(BAUD,pBoxPortBaud->currentText());

                QComboBox *pBoxPortParity = (QComboBox *)ui->tableWidget_Up->cellWidget(row,3);
                pComJsonObj.insert(PARITY,pBoxPortParity->currentText());

                QComboBox *pBoxPortDataBit = (QComboBox *)ui->tableWidget_Up->cellWidget(row,4);
                pComJsonObj.insert(DATABIT,pBoxPortDataBit->currentText());

                QComboBox *pBoxPortStopBit = (QComboBox *)ui->tableWidget_Up->cellWidget(row,5);
                pComJsonObj.insert(STOPBIT,pBoxPortStopBit->currentText());

                QComboBox *pBoxPortDeviceType = (QComboBox *)ui->tableWidget_Up->cellWidget(row,6);
                pComJsonObj.insert(DEVICETYPE,pBoxPortDeviceType->currentText());

                pCommsJsonObjUp.insert(ui->tableWidget_Up->item(row,0)->text(),pComJsonObj);
            }

            pRootJsonObj.insert(PORTSDOWN,pCommsJsonObjDown);
            pRootJsonObj.insert(PORTSUP,pCommsJsonObjUp);

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(pRootJsonObj);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
    }
    return false;
}



void ParamSet::on_pushButton_7_clicked()
{
    // 通讯配置

    if(Save_CommSet())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>保存通讯配置信息成功！</font>");
        QMessageBox::about(NULL, "提示", "<font color='black'>重启后串口通讯生效！</font>");
    }
}

bool ParamSet::Save_SysSet()
{
    QFile file(SYSTEM_SETTING_FILE);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "配置文件格式错误！";
            return false;
        }

        QJsonObject jsonObject = document.object();
        if(jsonObject.contains(SYS_SET1) && jsonObject.contains(SYS_SET2) && jsonObject.contains(SYS_SET3) && jsonObject.contains(SYS_SET4) && jsonObject.contains(SYS_SET5) && jsonObject.contains(SYS_SET6) && jsonObject.contains(SYS_SET7) && jsonObject.contains(SYS_SET8))
        {
            QJsonValueRef ElementOneValueRefSet1 = jsonObject.find(SYS_SET1).value();
            ElementOneValueRefSet1 = QJsonValue(ui->lineEdit->text());

            QJsonValueRef ElementOneValueRefSet2 = jsonObject.find(SYS_SET2).value();
            ElementOneValueRefSet2 = QJsonValue(ui->lineEdit_2->text());

            QJsonValueRef ElementOneValueRefSet3 = jsonObject.find(SYS_SET3).value();
            ElementOneValueRefSet3 = QJsonValue(ui->lineEdit_3->text());

            QJsonValueRef ElementOneValueRefSet4 = jsonObject.find(SYS_SET4).value();
            ElementOneValueRefSet4 = QJsonValue(ui->lineEdit_4->text());

            QJsonValueRef ElementOneValueRefSet5 = jsonObject.find(SYS_SET5).value();
            ElementOneValueRefSet5 = QJsonValue(ui->lineEdit_5->text());

            QJsonValueRef ElementOneValueRefSet6 = jsonObject.find(SYS_SET6).value();
            ElementOneValueRefSet6 = QJsonValue(ui->lineEdit_6->text());

            QJsonValueRef ElementOneValueRefSet7 = jsonObject.find(SYS_SET7).value();
            ElementOneValueRefSet7 = QJsonValue(ui->lineEdit_7->text());

            QJsonValueRef ElementOneValueRefSet8 = jsonObject.find(SYS_SET8).value();
            ElementOneValueRefSet8 = QJsonValue(ui->lineEdit_8->text());

            if(jsonObject.contains(SYS_CHECK) && jsonObject.contains(SYS_RADIOBUTTON1) && jsonObject.contains(SYS_RADIOBUTTON2))
            {
                QJsonValueRef ElementOneValueRefChcek = jsonObject.find(SYS_CHECK).value();
                if(ui->checkBox->checkState() == Qt::Checked)
                    ElementOneValueRefChcek = QJsonValue(true);
                else
                    ElementOneValueRefChcek = QJsonValue(false);

                QJsonValueRef ElementOneValueRefRadio = jsonObject.find(SYS_RADIOBUTTON1).value();
                if(ui->radioButton->isChecked())
                    ElementOneValueRefRadio = QJsonValue(true);
                else
                    ElementOneValueRefRadio = QJsonValue(false);

                QJsonValueRef ElementOneValueRefRadio2 = jsonObject.find(SYS_RADIOBUTTON2).value();
                if(ui->radioButton_2->isChecked())
                    ElementOneValueRefRadio2 = QJsonValue(true);
                else
                    ElementOneValueRefRadio2 = QJsonValue(false);
            }

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();

            return true;
        }
        else {

            QJsonObject pRootJsonObj;

            pRootJsonObj.insert(SYS_SET1,ui->lineEdit->text());
            pRootJsonObj.insert(SYS_SET2,ui->lineEdit_2->text());
            pRootJsonObj.insert(SYS_SET3,ui->lineEdit_3->text());
            pRootJsonObj.insert(SYS_SET4,ui->lineEdit_4->text());
            pRootJsonObj.insert(SYS_SET5,ui->lineEdit_5->text());
            pRootJsonObj.insert(SYS_SET6,ui->lineEdit_6->text());
            pRootJsonObj.insert(SYS_SET7,ui->lineEdit_7->text());
            pRootJsonObj.insert(SYS_SET8,ui->lineEdit_8->text());

            if(ui->checkBox->checkState() == Qt::Checked)
                pRootJsonObj.insert(SYS_CHECK,true);
            else
                pRootJsonObj.insert(SYS_CHECK,false);

            if(ui->radioButton->isChecked())
            {
                pRootJsonObj.insert(SYS_RADIOBUTTON1,true);
                pRootJsonObj.insert(SYS_RADIOBUTTON2,false);
            }
            else
            {
                pRootJsonObj.insert(SYS_RADIOBUTTON1,false);
                pRootJsonObj.insert(SYS_RADIOBUTTON2,true);
            }

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(pRootJsonObj);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
    }
    return false;
}


void ParamSet::on_pushButton_8_clicked()
{
    // 系统配置

    if(Save_SysSet())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>保存系统配置信息成功！</font>");
    }
}

bool ParamSet::Save_FanSet()
{
    QFile file(FAN_SETTING_FILE);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "配置文件格式错误！";
            return false;
        }

        QJsonObject jsonObject = document.object();
        if(jsonObject.contains(FAN_SET) && jsonObject.contains(FAN_LAST))
        {
            QJsonValueRef ElementOneValueRefSet = jsonObject.find(FAN_SET).value();
            ElementOneValueRefSet = QJsonValue(ui->lineEdit_9->text());

            QJsonValueRef ElementOneValueRefLast = jsonObject.find(FAN_LAST).value();
            ElementOneValueRefLast = QJsonValue(ui->lineEdit_10->text());

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();

            return true;
        }
        else {

            QJsonObject pRootJsonObj;

            pRootJsonObj.insert(FAN_SET,ui->lineEdit_9->text());
            pRootJsonObj.insert(FAN_LAST,ui->lineEdit_10->text());

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(pRootJsonObj);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
    }
    return false;
}

void ParamSet::on_pushButton_11_clicked()
{
    // 反吹配置

    if(Save_FanSet())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>保存反吹配置信息成功！</font>");
    }
}

bool ParamSet::Save_UserSet()
{
    QFile file(USERS_SETTING_FILE);
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QLOG_ERROR() << "配置文件格式错误！";
            return false;
        }

        bool pCheckState = false;

        QJsonObject jsonObject = document.object();
        if(jsonObject.contains(USERS))
        {
            QJsonValueRef RefVersion = jsonObject.find(USERS).value();
            QJsonObject jsonObjectNode = RefVersion.toObject();

            for(int row=0; row<ui->tableWidget_2->rowCount(); row++)
            {
                QString pUserType = ui->tableWidget_2->item(row,0)->text();

                QJsonValueRef RefVersionU = jsonObjectNode.find(pUserType).value();
                QJsonObject jsonObjectNodeU = RefVersionU.toObject();

                QJsonValueRef ElementOneValueRef = jsonObjectNodeU.find(USERPASSWD).value();
                ElementOneValueRef = QJsonValue(ui->tableWidget_2->item(row,1)->text());

                QCheckBox *pCheckClear = (QCheckBox *)ui->tableWidget_2->cellWidget(row,2);
                if(pCheckClear->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefClear = jsonObjectNodeU.find(USESET_QINGKONG).value();
                ElementOneValueRefClear = QJsonValue(pCheckState);

                QCheckBox *pCheckParam = (QCheckBox *)ui->tableWidget_2->cellWidget(row,3);
                if(pCheckParam->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefParam = jsonObjectNodeU.find(USESET_CANSHU).value();
                ElementOneValueRefParam = QJsonValue(pCheckState);

                QCheckBox *pCheckFan = (QCheckBox *)ui->tableWidget_2->cellWidget(row,4);
                if(pCheckFan->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefFan = jsonObjectNodeU.find(USESET_FANCHUI).value();
                ElementOneValueRefFan = QJsonValue(pCheckState);

                QCheckBox *pCheckTime = (QCheckBox *)ui->tableWidget_2->cellWidget(row,5);
                if(pCheckTime->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefTime = jsonObjectNodeU.find(USESET_SHIJIAN).value();
                ElementOneValueRefTime = QJsonValue(pCheckState);

                QCheckBox *pCheckHis = (QCheckBox *)ui->tableWidget_2->cellWidget(row,6);
                if(pCheckHis->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefHis = jsonObjectNodeU.find(USESET_LISHI).value();
                ElementOneValueRefHis = QJsonValue(pCheckState);

                QCheckBox *pCheckCurve = (QCheckBox *)ui->tableWidget_2->cellWidget(row,7);
                if(pCheckCurve->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefCurve = jsonObjectNodeU.find(USESET_QUXIAN).value();
                ElementOneValueRefCurve = QJsonValue(pCheckState);

                QCheckBox *pCheckFida = (QCheckBox *)ui->tableWidget_2->cellWidget(row,8);
                if(pCheckFida->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefFida = jsonObjectNodeU.find(USESET_FIDA).value();
                ElementOneValueRefFida = QJsonValue(pCheckState);

                QCheckBox *pCheckFidb = (QCheckBox *)ui->tableWidget_2->cellWidget(row,9);
                if(pCheckFidb->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefFidb = jsonObjectNodeU.find(USESET_FIDB).value();
                ElementOneValueRefFidb = QJsonValue(pCheckState);

                QCheckBox *pCheckTemp = (QCheckBox *)ui->tableWidget_2->cellWidget(row,10);
                if(pCheckTemp->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefTemp = jsonObjectNodeU.find(USESET_WENDU).value();
                ElementOneValueRefTemp = QJsonValue(pCheckState);

                QCheckBox *pCheckZ = (QCheckBox *)ui->tableWidget_2->cellWidget(row,11);
                if(pCheckZ->checkState() == Qt::Checked) pCheckState = true;
                else pCheckState = false;
                QJsonValueRef ElementOneValueRefZ = jsonObjectNodeU.find(USESET_ZHUANGTAI).value();
                ElementOneValueRefZ = QJsonValue(pCheckState);

                RefVersionU = jsonObjectNodeU;
            }

            RefVersion = jsonObjectNode;
            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(jsonObject);
            file.write(jsonDoc.toJson());
            file.close();

            return true;
        }
        else {

            QJsonObject pRootJsonObj;
            QJsonObject pUsersJsonObj;

            for(int row=0; row<ui->tableWidget_2->rowCount(); row++)
            {
                QJsonObject pUserJsonObj;

                pUserJsonObj.insert(USERPASSWD,ui->tableWidget_2->item(row,1)->text());

                QCheckBox *pCheck1 = (QCheckBox *)ui->tableWidget_2->cellWidget(row,2);
                if(pCheck1->checkState() == Qt::Checked) pUserJsonObj.insert(USESET_QINGKONG,true);
                else pUserJsonObj.insert(USESET_QINGKONG,false);

                QCheckBox *pCheck2 = (QCheckBox *)ui->tableWidget_2->cellWidget(row,3);
                if(pCheck2->checkState() == Qt::Checked) pUserJsonObj.insert(USESET_CANSHU,true);
                else pUserJsonObj.insert(USESET_CANSHU,false);

                QCheckBox *pCheck3 = (QCheckBox *)ui->tableWidget_2->cellWidget(row,4);
                if(pCheck3->checkState() == Qt::Checked) pUserJsonObj.insert(USESET_FANCHUI,true);
                else pUserJsonObj.insert(USESET_FANCHUI,false);

                QCheckBox *pCheck4 = (QCheckBox *)ui->tableWidget_2->cellWidget(row,5);
                if(pCheck4->checkState() == Qt::Checked) pUserJsonObj.insert(USESET_SHIJIAN,true);
                else pUserJsonObj.insert(USESET_SHIJIAN,false);

                QCheckBox *pCheck5 = (QCheckBox *)ui->tableWidget_2->cellWidget(row,6);
                if(pCheck5->checkState() == Qt::Checked) pUserJsonObj.insert(USESET_LISHI,true);
                else pUserJsonObj.insert(USESET_LISHI,false);

                QCheckBox *pCheck6 = (QCheckBox *)ui->tableWidget_2->cellWidget(row,7);
                if(pCheck6->checkState() == Qt::Checked) pUserJsonObj.insert(USESET_QUXIAN,true);
                else pUserJsonObj.insert(USESET_QUXIAN,false);

                QCheckBox *pCheck7 = (QCheckBox *)ui->tableWidget_2->cellWidget(row,8);
                if(pCheck7->checkState() == Qt::Checked) pUserJsonObj.insert(USESET_FIDA,true);
                else pUserJsonObj.insert(USESET_FIDA,false);

                QCheckBox *pCheck8 = (QCheckBox *)ui->tableWidget_2->cellWidget(row,9);
                if(pCheck8->checkState() == Qt::Checked) pUserJsonObj.insert(USESET_FIDB,true);
                else pUserJsonObj.insert(USESET_FIDB,false);

                QCheckBox *pCheck9 = (QCheckBox *)ui->tableWidget_2->cellWidget(row,10);
                if(pCheck9->checkState() == Qt::Checked) pUserJsonObj.insert(USESET_WENDU,true);
                else pUserJsonObj.insert(USESET_WENDU,false);

                QCheckBox *pCheck10 = (QCheckBox *)ui->tableWidget_2->cellWidget(row,11);
                if(pCheck10->checkState() == Qt::Checked) pUserJsonObj.insert(USESET_ZHUANGTAI,true);
                else pUserJsonObj.insert(USESET_ZHUANGTAI,false);

                pUsersJsonObj.insert(ui->tableWidget_2->item(row,0)->text(),pUserJsonObj);

            }

            pRootJsonObj.insert(USERS,pUsersJsonObj);

            file.open(QIODevice::WriteOnly | QIODevice::Text);
            QJsonDocument jsonDoc;
            jsonDoc.setObject(pRootJsonObj);
            file.write(jsonDoc.toJson());
            file.close();
            return true;
        }
    }
    return false;
}

void ParamSet::on_pushButton_12_clicked()
{
    // 用户管理配置

    if(Save_UserSet())
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>保存用户管理信息成功！</font>");
    }
}

