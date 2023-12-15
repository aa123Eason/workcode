#include "dialogprotocol.h"
#include "ui_dialogprotocol.h"

DialogProtocol::DialogProtocol(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProtocol)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);

    // 信号转发
    m_SignalMapperEdit = new QSignalMapper(this);
    m_SignalMapperDele = new QSignalMapper(this);
    connect(m_SignalMapperEdit,SIGNAL(mapped(QString)),this,SLOT(btnClicked_FacEdit(QString)));
    connect(m_SignalMapperDele,SIGNAL(mapped(QString)),this,SLOT(btnClicked_FacDele(QString)));

    guiInit();
}

DialogProtocol::~DialogProtocol()
{
    delete ui;
    if(m_SignalMapperEdit) delete m_SignalMapperEdit;
    if(m_SignalMapperDele) delete m_SignalMapperDele;

    qDeleteAll(m_DevBtnEditMap);
    qDeleteAll(m_DevBtnDeleMap);
}

void DialogProtocol::setTableModbusHeader()
{
    ui->tableWidget->clear();
    QString qssTV = QLatin1String("QTableWidget::item:selected{background-color:#1B89A1}"
                                  "QHeaderView::section,QTableCornerButton:section{ \
                                  padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
                                    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
                                background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
                                "QTableWidget{background-color:white;border:none;}");
    //设置表头
    QStringList headerText;
    headerText << QStringLiteral("编码") << QStringLiteral("名称") << QStringLiteral("标识")<< QStringLiteral("属性")<< QStringLiteral("功能码") << QStringLiteral("地址")
               << QStringLiteral("字节序") << QStringLiteral("数据类型") << QStringLiteral("操作");
    int cnt = headerText.count();
    ui->tableWidget->setColumnCount(cnt);
    ui->tableWidget->setHorizontalHeaderLabels(headerText);
    // ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true); //行头自适应表格

    ui->tableWidget->horizontalHeader()->setFont(QFont(QLatin1String("song"), 12));
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    QFont font =  ui->tableWidget->horizontalHeader()->font();
    font.setBold(true);
    ui->tableWidget->horizontalHeader()->setFont(font);

    ui->tableWidget->setFont(QFont(QLatin1String("song"), 10)); // 表格内容的字体为10号宋体

    int widths[] = {80, 80, 50, 50, 70, 60, 80, 80 ,100};
    for (int i = 0;i < cnt; ++ i){ //列编号从0开始
        ui->tableWidget->setColumnWidth(i, widths[i]);
    }

    ui->tableWidget->setStyleSheet(qssTV);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(45);
}


void DialogProtocol::guiInit()
{
    CJsonFile jsonfile;
    QJsonArray pJsonProtoArray = jsonfile.get_level1_array(TEMPLATE);
    int pCnt = pJsonProtoArray.size();
    QStringList protoList;
    for(int i=0;i<pCnt;i++)
    {
        QJsonObject pJsonProto = pJsonProtoArray.at(i).toObject();
        QString tempName,tempProto;

        if(pJsonProto.contains(TEMPLATE_PROTO))
        {
            tempProto = pJsonProto.value(TEMPLATE_PROTO).toString();
            if(!protoList.contains(tempProto))
            {
                ui->comboBox->addItem(tempProto);
                protoList << tempProto;
            }
        }

        if(pJsonProto.contains(TEMPLATE_NAME))
        {
            tempName = pJsonProto.value(TEMPLATE_NAME).toString();
            ui->comboBox_2->addItem(tempName);
        }
    }
    return;
}

void DialogProtocol::setTableModbusContents(QJsonArray &pJsonArray)
{
    ui->tableWidget->clearContents();

    QTableWidgetItem *pItemFaCode,*pItemFaName,*pItemTagId,*pItemProp,*pItemFaFunc,*pItemFaAddr,*pItemByteOrder,*pItemDateType;
    int pCnt = pJsonArray.size();
    if(pCnt <= 6) ui->tableWidget->setRowCount(6);
    else ui->tableWidget->setRowCount(pCnt);
    for(int i=0;i<pCnt;i++)
    {
        QJsonObject pJsonFactor = pJsonArray.at(i).toObject();
        QString pStrFactor;
        if(pJsonFactor.contains(FACTOR_CODE))
        {
            pStrFactor = pJsonFactor.value(FACTOR_CODE).toString();
            pItemFaCode = new QTableWidgetItem(pStrFactor);
            pItemFaCode->setTextAlignment(Qt::AlignCenter);
            pItemFaCode->setFlags(Qt::ItemIsEditable);
            ui->tableWidget->setItem(i, 0, pItemFaCode);
        }

        if(pJsonFactor.contains(FACTOR_NAME))
        {
            QString pStrFaName = pJsonFactor.value(FACTOR_NAME).toString();
            pItemFaName = new QTableWidgetItem(pStrFaName);
            pItemFaName->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i, 1, pItemFaName);
        }

        if(pJsonFactor.contains(FACTOR_TAG_ID))
        {
            QString pStrTagId = pJsonFactor.value(FACTOR_TAG_ID).toString();
            pItemTagId = new QTableWidgetItem(pStrTagId);
            pItemTagId->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i, 2, pItemTagId);
        }

        if(pJsonFactor.contains(FACTOR_PROPERTY))
        {
            QString pStrProp = pJsonFactor.value(FACTOR_PROPERTY).toString();
            pItemProp = new QTableWidgetItem(pStrProp);
            pItemProp->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i, 3, pItemProp);
        }

        if(pJsonFactor.contains(FACTOR_FUNC))
        {
            QString pStrFaFunc = pJsonFactor.value(FACTOR_FUNC).toString();
            pItemFaFunc = new QTableWidgetItem(pStrFaFunc);
            pItemFaFunc->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i, 4, pItemFaFunc);
        }
        if(pJsonFactor.contains(FACTOR_ADDR))
        {
            QString pStrFaAddr = pJsonFactor.value(FACTOR_ADDR).toString();
            pItemFaAddr = new QTableWidgetItem(pStrFaAddr);
            pItemFaAddr->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i, 5, pItemFaAddr);
        }
        if(pJsonFactor.contains(BYTE_ORDER))
        {
            QString pStrByteOrder = pJsonFactor.value(BYTE_ORDER).toString();
            pItemByteOrder = new QTableWidgetItem(pStrByteOrder);
            pItemByteOrder->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i, 6, pItemByteOrder);
        }
        if(pJsonFactor.contains(DATA_TYPE))
        {
            QString pStrDataType = pJsonFactor.value(DATA_TYPE).toString();
            pItemDateType = new QTableWidgetItem(pStrDataType);
            pItemDateType->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i, 7, pItemDateType);
        }

        QPushButton *pEditBtn = new QPushButton(QStringLiteral("保存"));
        pEditBtn->setFixedSize(QSize(60, 30));
        pEditBtn->setStyleSheet(QStringLiteral("QPushButton{color:white;background-color:rgb(51,204,255);font-family:黑体;font-size: 18px; border-radius: 10px;}"
                                               "QPushButton:pressed{background-color:rgb(51,129,172)}"));

        QPushButton *pDeleBtn = new QPushButton(QStringLiteral("删除"));
        pDeleBtn->setFixedSize(QSize(60, 30));
        pDeleBtn->setStyleSheet(QStringLiteral("QPushButton{color:white;background-color:red;font-family:黑体;font-size: 18px; border-radius: 10px;}"
                                               "QPushButton:pressed{background-color:rgb(51,129,172)}"));

        m_DevBtnEditMap.insert(ui->comboBox_2->currentText()+"+"+pStrFactor,pEditBtn);
        m_DevBtnDeleMap.insert(ui->comboBox_2->currentText()+"+"+pStrFactor,pDeleBtn);

        connect(pEditBtn, SIGNAL(clicked(bool)), m_SignalMapperEdit, SLOT(map()));
        // 设置signalmapper的转发规则, 转发为参数为QString类型的信号， 并把textList[i]的内容作为实参传递。
        m_SignalMapperEdit->setMapping(pEditBtn, ui->comboBox_2->currentText()+"+"+pStrFactor);

        connect(pDeleBtn, SIGNAL(clicked(bool)), m_SignalMapperDele, SLOT(map()));
        // 设置signalmapper的转发规则, 转发为参数为QString类型的信号， 并把textList[i]的内容作为实参传递。
        m_SignalMapperDele->setMapping(pDeleBtn, ui->comboBox_2->currentText()+"+"+pStrFactor);

        QWidget *btnWidget = new QWidget(this);
        QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);    // FTIXME：内存是否会随着清空tablewidget而释放
        btnLayout->addWidget(pEditBtn);
        btnLayout->addWidget(pDeleBtn);
        btnLayout->setMargin(5);
        btnLayout->setAlignment(Qt::AlignCenter);
        ui->tableWidget->setCellWidget(i, 8, btnWidget);

    }
}

void DialogProtocol::btnClicked_FacEdit(QString protoName)
{
    // qDebug() << "protoName edit==++==>>" << protoName;
    if(protoName.contains("+"))
    {
        QStringList protoStrList = protoName.split("+");
        if(protoStrList.size() == 2)
        {
            QString pFactorCode = protoStrList[1];
            for(int row=0; row<ui->tableWidget->rowCount(); row++)
            {
                QTableWidgetItem* item = ui->tableWidget->item(row,0);
                if(item != nullptr)
                {
                    if(item->text() == pFactorCode)
                    {
                        // qDebug() << "item==66=>>" << item->text();

                        QTableWidgetItem* pItemName = ui->tableWidget->item(row,1);
                        QTableWidgetItem* pItemTagId = ui->tableWidget->item(row,2);
                        QTableWidgetItem* pItemProp = ui->tableWidget->item(row,3);
                        QTableWidgetItem* pItemFunc = ui->tableWidget->item(row,4);
                        QTableWidgetItem* pItemRegisAddr = ui->tableWidget->item(row,5);
                        QTableWidgetItem* pItemByteOrder = ui->tableWidget->item(row,6);
                        QTableWidgetItem* pItemDataType = ui->tableWidget->item(row,7);

                        if(pItemName != nullptr && pItemProp != nullptr && pItemTagId != nullptr && pItemFunc != nullptr && pItemRegisAddr != nullptr && pItemByteOrder != nullptr && pItemDataType != nullptr)
                        {

                            CFactor pFactor;
                            pFactor.setFactorName(pItemName->text());
                            pFactor.setTagId(pItemTagId->text());
                            pFactor.setProperty(pItemProp->text());
                            pFactor.setFactorCode(protoStrList[1]);
                            pFactor.setFactorFunc(pItemFunc->text());
                            pFactor.setRegisAddr(pItemRegisAddr->text());
                            pFactor.setByteOrder(pItemByteOrder->text());
                            pFactor.setDataType(pItemDataType->text());
                            pFactor.setDataType(pItemDataType->text());

                            CJsonFile jsonfile;
                            jsonfile.edit_factornode(protoStrList[0],pFactor);
                            QMessageBox::about(NULL, "提示", "<font color='black'>保存因子配置成功！</font>");
                            return;
                        }
                    }
                }
            }
        }
    }
}


void DialogProtocol::btnClicked_FacDele(QString protoName)
{
    qDebug() << "pDevName dele====>>" << protoName;

    CJsonFile jsonfile;
    jsonfile.remove_factornode(protoName);

}

void DialogProtocol::on_comboBox_2_currentTextChanged(const QString &arg1)
{
    // qDebug() << "arg1===>>" << arg1;

    QMap<QString, QPushButton *>::const_iterator itEdit = m_DevBtnEditMap.constBegin();
    while (itEdit != m_DevBtnEditMap.constEnd())
    {
        m_SignalMapperEdit->removeMappings(itEdit.value());
        ++itEdit;
    }

    QMap<QString, QPushButton *>::const_iterator itDele = m_DevBtnDeleMap.constBegin();
    while (itDele != m_DevBtnDeleMap.constEnd())
    {
        m_SignalMapperDele->removeMappings(itDele.value());
        ++itDele;
    }

    qDeleteAll(m_DevBtnEditMap);
    qDeleteAll(m_DevBtnDeleMap);
    m_DevBtnEditMap.clear();
    m_DevBtnDeleMap.clear();

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(0);
    ui->tableWidget->setRowCount(0);
    if(arg1 == "") return;

    CJsonFile jsonfile;
    QJsonArray pJsonProtoArray = jsonfile.get_level1_array(TEMPLATE);
    int pCnt = pJsonProtoArray.size();
    QStringList protoList;
    for(int i=0;i<pCnt;i++)
    {
        QJsonObject pJsonProto = pJsonProtoArray.at(i).toObject();
        if(pJsonProto.contains(TEMPLATE_NAME))
        {
            QString pStrName = pJsonProto.value(TEMPLATE_NAME).toString();
            if(pStrName == ui->comboBox_2->currentText())
            {
                QJsonArray pJsonFactorList = pJsonProto.value(FACTORS).toArray();
                // table
                if(ui->comboBox->currentText() == MODBUS_RTU)
                {
                    setTableModbusHeader();
                    setTableModbusContents(pJsonFactorList);
                    return;
                }
            }
        }
    }
}

void DialogProtocol::on_pushButtonProtoAdd_clicked()
{
    m_DialogProtoAdd = new DialogProtoAdd();
    m_DialogProtoAdd->show();
}

void DialogProtocol::on_pushButtonProtoDele_clicked()
{
    CJsonFile jsonfile;
    jsonfile.remove_protonode(ui->comboBox_2->currentText());

    ui->comboBox_2->clear();
    ui->comboBox->clear();
    this->guiInit();

    QMessageBox::about(NULL, "提示", "<font color='black'>保存设备配置成功！</font>");
}

void DialogProtocol::on_pushButtonProtoFresh_clicked()
{
    ui->comboBox_2->clear();
    ui->comboBox->clear();
    this->guiInit();

    QMessageBox::about(NULL, "提示", "<font color='black'>获取最新设备类型配置成功！</font>");
}

void DialogProtocol::on_pushButtonFaAdd_clicked()
{
    m_DialogNewFactor = new DialogNewFactor(ui->comboBox_2->currentText());
    m_DialogNewFactor->show();
}

void DialogProtocol::on_pushButtonFaFresh_clicked()
{
    qDeleteAll(m_DevBtnEditMap);
    qDeleteAll(m_DevBtnDeleMap);
    m_DevBtnEditMap.clear();
    m_DevBtnDeleMap.clear();

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(0);
    ui->tableWidget->setRowCount(0);

    CJsonFile jsonfile;
    QJsonArray pJsonProtoArray = jsonfile.get_level1_array(TEMPLATE);
    int pCnt = pJsonProtoArray.size();
    QStringList protoList;
    for(int i=0;i<pCnt;i++)
    {
        QJsonObject pJsonProto = pJsonProtoArray.at(i).toObject();
        if(pJsonProto.contains(TEMPLATE_NAME))
        {
            QString pStrName = pJsonProto.value(TEMPLATE_NAME).toString();
            if(pStrName == ui->comboBox_2->currentText())
            {
                QJsonArray pJsonFactorList = pJsonProto.value(FACTORS).toArray();
                // table
                if(ui->comboBox->currentText() == MODBUS_RTU)
                {
                    setTableModbusHeader();
                    setTableModbusContents(pJsonFactorList);
                    QMessageBox::about(NULL, "提示", "<font color='black'>刷新成功！</font>");
                    return;
                }
            }
        }
    }
}
