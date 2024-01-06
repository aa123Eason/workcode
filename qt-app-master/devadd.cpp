#include "devadd.h"
#include "ui_devadd.h"
#include <QListView>

DevAdd::DevAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DevAdd)
{
    ui->setupUi(this);

    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QStringLiteral(" "));

    ui->radioButton_Com->setChecked(true);
    connect(ui->radioButton_Com, SIGNAL(clicked()), this, SLOT(typeRadioBtnClicked()));
    connect(ui->radioButton_Net, SIGNAL(clicked()), this, SLOT(typeRadioBtnClicked()));

    connectevent();

    interGroup = new QButtonGroup(this);
    interGroup->addButton(ui->radioButton_Com, 0);
    interGroup->addButton(ui->radioButton_Net, 1);

    ui->comboBox->setEditable(true);

    ui->comboBox_2->clear();
    QJsonObject::const_iterator itor = g_Dcm_SupportDevice.constBegin();
    QJsonObject::const_iterator end_proto = g_Dcm_SupportDevice.constEnd();
    map = util.Uart_devicetype();
    namemap = util.Uart_devicetypeNameMatch();
    while(itor != end_proto)
    {
        ui->comboBox_2->addItem(namemap[itor.key()]);
        itor++;
    }

    ui->comboBox_2->setView(new QListView(this));
    ui->comboBox->setView(new QListView(this));
    ui->comboBox_baud->setView(new QListView(this));
    ui->comboBox_stopbit->setView(new QListView(this));
    ui->comboBox_databit->setView(new QListView(this));
    ui->comboBox_parity->setView(new QListView(this));
}

DevAdd::~DevAdd()
{
    if(interGroup) interGroup->deleteLater();
    delete ui;
}

void DevAdd::typeRadioBtnClicked() {
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

void DevAdd::on_pushButton_clicked()
{

    if(namemap.key(ui->comboBox_2->currentText()) == "lc-modbus")
    {
        QString nameStart = "start";
        QString nameQuantity = "quantity";
        QString nameFucntionCode = "functionCode";
        QString namevtype = "vtype";

        QString valueStart = ui->paramtable->item(0,1)->text();
        QString valueQuantity = ui->paramtable->item(1,1)->text();

        QComboBox *box1 = (QComboBox *)ui->paramtable->cellWidget(2,1);
        QString valueFucntionCode = box1->currentText();

        QComboBox *box2 = (QComboBox *)ui->paramtable->cellWidget(3,1);
        QString valuevtype = box2->currentText();

        QString str;
        str += nameStart + "=" + valueStart + ",";
        str += nameQuantity + "=" + valueQuantity + ",";
        str += nameFucntionCode + "=" + valueFucntionCode + ",";
        str += namevtype + "=" + valuevtype;
        ui->textEdit_devParams->setText(str);
        QFont font;
        font.setBold(true);
        font.setPointSize(20);
        ui->textEdit_devParams->setFont(font);
    }

    QJsonObject obj;
    obj.insert(QLatin1String("id"), "");
    obj.insert(QLatin1String("address"), ui->lineEdit_devAddr->text().toInt());
    obj.insert(QLatin1String("baudrate"), ui->comboBox_baud->currentText().toInt());

    QString pComTex = ui->comboBox->currentText();
    Util pUtil;
    QString pTex = pUtil.Uart_Convert(pComTex);
    if(pTex == "") obj.insert(QLatin1String("com"), pComTex);
    else obj.insert(QLatin1String("com"), pTex);

    obj.insert(QLatin1String("data_bit"), ui->comboBox_databit->currentText().toInt());
    obj.insert(QLatin1String("dev_name"), ui->lineEdit_Name->text());
    obj.insert(QLatin1String("dev_params"), ui->textEdit_devParams->toPlainText());

    QString pDevType = namemap.key(ui->comboBox_2->currentText());
    if(pDevType == "analog") g_IsAnalogDevOperated = true;
    obj.insert(QLatin1String("dev_type"), pDevType);

    obj.insert(QLatin1String("ip_addr"), ui->lineEdit_ipAddr->text());
    obj.insert(QLatin1String("parity"), ui->comboBox_parity->currentText());
    obj.insert(QLatin1String("stop_bit"), ui->comboBox_stopbit->currentText());

    // qDebug() << "obj==>>" << obj;

    httpclinet pClient;
    if(pClient.put(DCM_DEVICE,obj))
    {
        //OpenDev_Setting();
        QMessageBox::about(NULL, "提示", "<font color='black'>新增设备配置信息成功！</font>");
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>新增设备配置信息失败！</font>");
    }
    this->close();
}

void DevAdd::on_pushButton_2_clicked()
{
    this->close();
}

void DevAdd::connectevent()
{
    connect(ui->comboBox_2,&QComboBox::currentTextChanged,this,&DevAdd::onCurrentDevTypeChanged);
}

void DevAdd::onCurrentDevTypeChanged(const QString &text)
{
    QFont font;
    font.setBold(true);
    font.setPointSize(20);
    QString code = namemap.key(text);
    QStringList infoList = map[code];
    ui->paramtable->clear();
    ui->paramtable->setRowCount(0);
    if(infoList.count()>1)
    {
        qDebug()<<__LINE__<<text<<":";
        for(int i=1;i<infoList.count();++i)
        {
            qDebug()<<i<<"."<<infoList[i];
            ui->paramtable->insertRow(i-1);
            QString paramName;
            QString paramCode;
            QString paramValue;
            QStringList paramValueList;
            QString paramRes;
            if(infoList[i].split(">").count()==2)
            {
                paramCode = infoList[i].split(">")[0];
                QString tmpstr = infoList[i].split(">")[1];
                if(tmpstr.split(":").count()==2)
                {
                    paramName = tmpstr.split(":")[0];
                    QString tmpstr1 = tmpstr.split(":")[1];
                    qDebug()<<tmpstr1<<endl;
                    if(tmpstr1.split(",").count()>0)
                    {
                        for(int k=0;k<tmpstr1.split(",").count();++k)
                        {
                            if(namemap.keys().contains(tmpstr1.split(",")[k]))
                                paramValueList<<namemap[tmpstr1.split(",")[k]];
                            else
                                paramValueList<<tmpstr1.split(",")[k];
                        }
                        qDebug()<<paramValueList<<paramValueList.count()<<endl;



                    }
                }
                else
                {
                    paramName = tmpstr;
                }
            }
            else if(infoList[i].split("=").count()==2)
            {
                paramName = infoList[i].split("=")[0];
                paramValue = infoList[i].split("=")[1];
            }
            else
            {
                paramName = infoList[i];
            }
            QTableWidgetItem *itemName = new QTableWidgetItem(paramName);

            QTableWidgetItem *itemValue = new QTableWidgetItem(paramValue);



            itemName->setTextAlignment(Qt::AlignCenter);
            itemName->setFont(font);

            itemValue->setTextAlignment(Qt::AlignCenter);
            itemValue->setFont(font);

            QComboBox *combox = new QComboBox();
            if(paramValueList.count()>0)
            {
                for(auto paramvalue:paramValueList)
                {
                    combox->addItem(paramvalue);

                }

                combox->setCurrentIndex(0);
                combox->setFont(QFont("Ubuntu",20,75));
                combox->setEditable(false);

            }


            ui->paramtable->setItem(i-1,0,itemName);
            if(!paramValue.isEmpty())
            {
                ui->paramtable->setItem(i-1,1,itemValue);
            }
            else if(paramValueList.count()>0)
            {
                ui->paramtable->setCellWidget(i-1,1,combox);
            }
            else
            {
                QTableWidgetItem *item = new QTableWidgetItem("0");
                item->setTextAlignment(Qt::AlignCenter);
                item->setFont(font);
                ui->paramtable->setItem(i-1,1,item);
            }

            ui->paramtable->setRowHeight(i-1,44);
            ui->paramtable->setWordWrap(true);


        }
        qDebug()<<endl;
    }

    //show on textbroswer

    if(namemap.key(text) == "lc-modbus")
    {
        QString nameStart = "start";
        QString nameQuantity = "quantity";
        QString nameFucntionCode = "functionCode";
        QString namevtype = "vtype";

        QString valueStart = ui->paramtable->item(0,1)->text();
        QString valueQuantity = ui->paramtable->item(1,1)->text();

        QComboBox *box1 = (QComboBox *)ui->paramtable->cellWidget(2,1);
        QString valueFucntionCode = box1->currentText();

        QComboBox *box2 = (QComboBox *)ui->paramtable->cellWidget(3,1);
        QString valuevtype = box2->currentText();

        QString str;
        str += nameStart + "=" + valueStart + ",";
        str += nameQuantity + "=" + valueQuantity + ",";
        str += nameFucntionCode + "=" + valueFucntionCode + ",";
        str += namevtype + "=" + valuevtype;
        ui->textEdit_devParams->setText(str);
        ui->textEdit_devParams->setFont(font);
    }

}
