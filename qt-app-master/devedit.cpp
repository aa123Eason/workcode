#include "devedit.h"
#include "ui_devedit.h"

DevEdit::DevEdit(QString dev_id,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DevEdit)
{
    ui->setupUi(this);

    interGroup = new QButtonGroup(this);
    interGroup->addButton(ui->radioButton_Com, 0);
    interGroup->addButton(ui->radioButton_Net, 1);
    ui->label_59->hide();
    ui->lineEdit_id->hide();
    map = util.Uart_devicetype();
    namemap = util.Uart_devicetypeNameMatch();

    connect(ui->radioButton_Com, SIGNAL(clicked()), this, SLOT(typeRadioBtnClicked()));
    connect(ui->radioButton_Net, SIGNAL(clicked()), this, SLOT(typeRadioBtnClicked()));
    connect(ui->comboBox_devProto,&QComboBox::currentTextChanged,this,&DevEdit::onComboBoxProtoCurrentChanged);
    DevEdit_Init(dev_id);
}

DevEdit::~DevEdit()
{
    delete ui;
    if(interGroup) interGroup->deleteLater();
}

void DevEdit::typeRadioBtnClicked() {
    if(interGroup->checkedId() == 0)
    {
        // com
        ui->tabWidget->setCurrentIndex(0);
    }
    else
    {
        // net
        ui->tabWidget->setCurrentIndex(1);
    }
}


void DevEdit::DevEdit_Init(QString dev_id)
{
    httpclinet pClient;
    QJsonObject pJsonProto;
    if(pClient.get(DCM_SUPPORT_DEVICE,pJsonProto))
    {
        QJsonObject::const_iterator itor = g_Dcm_SupportDevice.constBegin();
        QJsonObject::const_iterator end_proto = g_Dcm_SupportDevice.constEnd();
        while(itor != end_proto)
        {
            ui->comboBox_devProto->addItem(namemap[itor.key()]);
            itor++;
        }
    }

    QJsonObject pJsonDev;
    if(pClient.get(DCM_DEVICE,pJsonDev))
    {
        QJsonObject::const_iterator it = pJsonDev.constBegin();
        QJsonObject::const_iterator end = pJsonDev.constEnd();

        while(it != end)
        {
            if(it.key() == dev_id)
            {
                QJsonObject pJsondev = it.value().toObject();
                if(pJsondev.value("ip_addr").toString() == "")
                {
                    ui->radioButton_Com->setChecked(true);
                    ui->tabWidget->setCurrentIndex(0);
                }else
                {
                    ui->radioButton_Net->setChecked(true);
                    ui->tabWidget->setCurrentIndex(1);
                }

                ui->lineEdit_id->setText(pJsondev.value("id").toString());
                ui->lineEdit_ipAddr->setText(pJsondev.value("ip_addr").toString());
                ui->lineEdit_devAddr->setText(QString::number(pJsondev.value("address").toInt()));

                QString pTex = pJsondev.value("com").toString();
                Util pUtil;
                QString pComTex = pUtil.Uart_Revert(pTex);
                if(pComTex == "") ui->comboBox_6->setCurrentText(pTex);
                else ui->comboBox_6->setCurrentText(pComTex);


                ui->textEdit_devParams->setText(pJsondev.value("dev_params").toString());

                QString pDevType = pJsondev.value("dev_type").toString();
                if(pDevType == namemap["analog"]) g_IsAnalogDevOperated = true;

                ui->comboBox_devProto->setCurrentText(namemap[pDevType]);
                loadParamtable(pJsondev.value("dev_params").toString());
                ui->lineEdit_Name->setText(pJsondev.value("dev_name").toString());

                ui->comboBox_devBaud->setCurrentText(QString::number(pJsondev.value("baudrate").toInt()));
                ui->comboBox_devDatabit->setCurrentText(QString::number(pJsondev.value("data_bit").toInt()));
                ui->comboBox_devParity->setCurrentText(pJsondev.value("parity").toString());
                ui->comboBox_devStopBit->setCurrentText(pJsondev.value("stop_bit").toString());

                break;
            }

            it++;
        }
    }
}

void DevEdit::loadParamtable(QString dev_params)
{
    qDebug()<<__LINE__<<dev_params<<endl;
    qDebug()<<__LINE__<<ui->comboBox_devProto->currentText()<<endl;
    if(ui->comboBox_devProto->currentText() == "modbus通用驱动")
    {
        if(dev_params.split(",").count()==4)
        {
            QStringList infos = dev_params.split(",");
            qDebug()<<__LINE__<<infos<<endl;
            for(auto str:infos)
            {
                 qDebug()<<__LINE__<<str<<endl;
                if(str.split("=").count()==2)
                {
                    QString name = str.split("=")[0];
                    QString value = str.split("=")[1];
                    qDebug()<<__LINE__<<name<<":"<<value<<endl;

                    if(name == "start")
                    {
                        ui->paramtable->setItem(0,1,new QTableWidgetItem(value));
                    }
                    else if(name == "quantity")
                    {
                        ui->paramtable->setItem(1,1,new QTableWidgetItem(value));
                    }
                    else if(name == "functionCode")
                    {
                        QComboBox *box = (QComboBox *)ui->paramtable->cellWidget(2,1);
                        box->setCurrentText(value);
                    }
                    else if(name == "vtype")
                    {
                        QComboBox *box = (QComboBox *)ui->paramtable->cellWidget(3,1);
                        box->setCurrentText(value);
                    }

                }
            }
        }
    }
}

void DevEdit::onComboBoxProtoCurrentChanged(const QString &text)
{
    //起始寄存器地址，寄存器数量，功能码，值类型
    if(namemap.key(text) == "lc-modbus")
    {
        ui->paramtable->setRowCount(4);
        ui->paramtable->setItem(0,0,new QTableWidgetItem("起始寄存器地址"));
        ui->paramtable->setItem(1,0,new QTableWidgetItem("寄存器数量"));
        ui->paramtable->setItem(2,0,new QTableWidgetItem("功能码"));
        ui->paramtable->setItem(3,0,new QTableWidgetItem("值类型"));

        QFont font;
        font.setBold(true);
        font.setPointSize(20);

        //16位整型，长整型，长整型反向，32位浮点型，浮点型4321,32位浮点型反向，64位浮点型，64位浮点型反向

        QComboBox *box1 = new QComboBox;
        box1->addItem("1");
        box1->addItem("2");
        box1->addItem("3");
        box1->addItem("4");

        box1->setFont(font);
        box1->setCurrentIndex(0);

        QComboBox *box2 = new QComboBox;
        box2->addItem("16位整型");
        box2->addItem("长整型");
        box2->addItem("长整型反向");
        box2->addItem("32位浮点型");
        box2->addItem("浮点型4321");
        box2->addItem("32位浮点型反向");
        box2->addItem("64位浮点型");
        box2->addItem("64位浮点型反向");
        box2->addItem("coils线圈");

        box2->setFont(font);
        box2->setCurrentIndex(0);

        ui->paramtable->setCellWidget(2,1,box1);
        ui->paramtable->setCellWidget(3,1,box2);


    }
}

void DevEdit::on_pushButton_Detail_clicked()
{
    DialogProto *pDialogProto = new DialogProto(namemap.key(ui->comboBox_devProto->currentText()));
    pDialogProto->show();
}

void DevEdit::on_pushButton_UpdateDev_clicked()
{
    QJsonObject obj;
    obj.insert(QLatin1String("id"), ui->lineEdit_id->text());
    obj.insert(QLatin1String("address"), ui->lineEdit_devAddr->text().toInt());
    obj.insert(QLatin1String("baudrate"), ui->comboBox_devBaud->currentText().toInt());

    QString pComTex = ui->comboBox_6->currentText();
    Util pUtil;
    QString pTex = pUtil.Uart_Convert(pComTex);
    if(pTex == "") obj.insert(QLatin1String("com"), pComTex);
    else obj.insert(QLatin1String("com"), pTex);

    obj.insert(QLatin1String("data_bit"), ui->comboBox_devDatabit->currentText().toInt());
    obj.insert(QLatin1String("dev_name"), ui->lineEdit_Name->text());
    obj.insert(QLatin1String("dev_params"), ui->textEdit_devParams->toPlainText());
    obj.insert(QLatin1String("dev_type"), namemap.key(ui->comboBox_devProto->currentText()));
    obj.insert(QLatin1String("ip_addr"), ui->lineEdit_ipAddr->text());
    obj.insert(QLatin1String("parity"), ui->comboBox_devParity->currentText());
    obj.insert(QLatin1String("stop_bit"), ui->comboBox_devStopBit->currentText());

    // qDebug() << "obj==>>" << obj;

    httpclinet pClient;
    if(pClient.put(DCM_DEVICE,obj))
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>更新设备配置信息成功！</font>");
        this->close();
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>更新设备配置信息失败！</font>");
    }
}

void DevEdit::on_pushButton_Cancel_clicked()
{
    this->close();
}
