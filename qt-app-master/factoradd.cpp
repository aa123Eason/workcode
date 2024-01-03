#include "factoradd.h"
#include "ui_factoradd.h"

FactorAdd::FactorAdd(QString pDevID,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FactorAdd)
{
    ui->setupUi(this);
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QStringLiteral(" "));

    // qDebug() << "pDevID===>>" << pDevID;

    m_DevId = pDevID;

    ui->comboBox_fcode->clear();
    //g_Dcm_Factor
    {
        QJsonObject::const_iterator itFactor = g_Dcm_Factor.constBegin();
        QJsonObject::const_iterator endF = g_Dcm_Factor.constEnd();

        while(itFactor != endF)
        {
            QJsonObject pFactorObj;
            pFactorObj = itFactor.value().toObject();
            QString pStrCode = pFactorObj.value("code").toString();
            QString pStrFactorName = pFactorObj.value("name").toString();
            ui->comboBox_fcode->addItem(pStrCode+ "-" + pStrFactorName, pStrCode);
            itFactor++;
        }

        for(int i=0;i<g_Dcm_SystemCode.size();i++)
        {
            QString pStrST = QString::number(g_Dcm_SystemCode.at(i).toObject().value("id").toInt());
            QString pStrName = g_Dcm_SystemCode.at(i).toObject().value("name").toString();

            ui->comboBox_fst->addItem(pStrST+ "-" + pStrName, pStrST);
        }
    }

    ui->radioButton_decY->setChecked(true);
    ui->radioButton_SDSZF->setChecked(false);
    ui->radioButton_FBJC->setChecked(false);
    ui->radioButton_FBJCC->setChecked(false);
    ui->radioButton_CXBJ->setChecked(false);

    GroupF1 = new QButtonGroup(this);
    GroupF1->addButton(ui->radioButton_decY, 0);
    GroupF1->addButton(ui->radioButton_decN, 1);

    connect(ui->radioButton_decY, SIGNAL(clicked()), this, SLOT(decRadioBtnClicked()));
    connect(ui->radioButton_decN, SIGNAL(clicked()), this, SLOT(decRadioBtnClicked()));
    connect(ui->radioButton_MNL, SIGNAL(clicked()), this, SLOT(analogRadioBtnClicked()));

    ui->label_dec->setVisible(false);
    ui->lineEdit_dec->setVisible(false);

    ui->label_mf->setVisible(false);
    ui->lineEdit_mf->setVisible(false);

    ui->radioButton_MNL->setChecked(false);
    analogRadioBtnClicked();
}

FactorAdd::~FactorAdd()
{
    if(GroupF1) GroupF1->deleteLater();

    delete ui;
}

void FactorAdd::analogRadioBtnClicked() {
    if(ui->radioButton_MNL->isChecked())
    {
        ui->lineEdit_AU1->setVisible(true);
        ui->lineEdit_AD1->setVisible(true);
        ui->lineEdit_AU2->setVisible(true);
        ui->lineEdit_AD2->setVisible(true);
        ui->label_au1->setVisible(true);
        ui->label_au2->setVisible(true);
        ui->label_ad1->setVisible(true);
        ui->label_ad2->setVisible(true);
    }
    else
    {
        ui->lineEdit_AU1->setVisible(false);
        ui->lineEdit_AD1->setVisible(false);
        ui->lineEdit_AU2->setVisible(false);
        ui->lineEdit_AD2->setVisible(false);
        ui->label_au1->setVisible(false);
        ui->label_au2->setVisible(false);
        ui->label_ad1->setVisible(false);
        ui->label_ad2->setVisible(false);
    }
}

void FactorAdd::decRadioBtnClicked() {
    if(GroupF1->checkedId() != 0)
    {
        ui->label_dec->setVisible(true);
        ui->lineEdit_dec->setVisible(true);
    }
    else
    {
        ui->label_dec->setVisible(false);
        ui->lineEdit_dec->setVisible(false);
    }
}

void FactorAdd::mfRadioBtnClicked() {
    if(ui->radioButton_SDSZF->isChecked())
    {
        ui->label_mf->setVisible(true);
        ui->lineEdit_mf->setVisible(true);
    }
    else
    {
        ui->label_mf->setVisible(false);
        ui->lineEdit_mf->setVisible(false);
    }
}


void FactorAdd::on_pushButton_clicked()
{
    if(ui->comboBox_fcode->currentText() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>因子编码不能为空！</font>");
        return;
    }

    if(ui->comboBox_falias->currentText() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>因子序号不能为空！</font>");
        return;
    }

    if(ui->comboBox_fst->currentText() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>ST不能为空！</font>");
        return;
    }

    QJsonObject obj;
    obj.insert(QLatin1String("id"), "");
    obj.insert(QLatin1String("st"), ui->comboBox_fst->currentData().toString());
    obj.insert(QLatin1String("device_id"), m_DevId);
    QString pFcode = ui->comboBox_fcode->currentData().toString();
    obj.insert(QLatin1String("factor_code"), pFcode);
    obj.insert(QLatin1String("factor_alias"), ui->comboBox_falias->currentText());
    obj.insert(QLatin1String("alarm_lower"), ui->lineEdit_alarmLower->text().toInt());
    obj.insert(QLatin1String("alarm_upper"), ui->lineEdit_alarmUpper->text().toInt());
    obj.insert(QLatin1String("tag_id"), ui->lineEdit_tagId->text());
    obj.insert(QLatin1String("coefficient"), ui->lineEdit_coeff->text().toInt());

    if(GroupF1->checkedId() == 0)
    {
        obj.insert(QLatin1String("decimals"), 0);
        obj.insert(QLatin1String("calc_type"), 0);
    }
    else
    {
        obj.insert(QLatin1String("decimals"), ui->lineEdit_dec->text().toInt());
        obj.insert(QLatin1String("calc_type"), 1);
    }

    if(ui->radioButton_SDSZF->isChecked())
    {
        obj.insert(QLatin1String("is_manual_flag"), 1);
        obj.insert(QLatin1String("manual_flag"), ui->lineEdit_mf->text());
    }
    else
    {
        obj.insert(QLatin1String("is_manual_flag"), 1);
        obj.insert(QLatin1String("manual_flag"), "");
    }

    if(ui->radioButton_FBJC->isChecked()) obj.insert(QLatin1String("is_change_send_msg"), 1);
    else obj.insert(QLatin1String("is_change_send_msg"), 0);

    if(ui->radioButton_FBJCC->isChecked()) obj.insert(QLatin1String("is_change_send_msg_store"), 1);
    else obj.insert(QLatin1String("is_change_send_msg_store"), 0);

    if(ui->radioButton_CXBJ->isChecked()) obj.insert(QLatin1String("is_continuous_alarm_over_standard"), 1);
    else obj.insert(QLatin1String("is_continuous_alarm_over_standard"), 0);

    // qDebug() << "obj==>>" << obj;

    httpclinet pClient;
    if(pClient.put(DCM_DEVICE_FACTOR,obj))
    {
        QString pKey = g_Device_ID + "-" + pFcode;
        if(Conf_FactorUpdate(pKey))
        {
            emit addSuccess();
            QMessageBox::about(NULL, "提示", "<font color='black'>新增因子配置信息成功！</font>");
        }
        else
        {
            QMessageBox::about(NULL, "提示", "<font color='black'>新增因子配置信息失败！</font>");
        }
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>新增因子配置信息失败！</font>");
    }
    this->close();
}

void FactorAdd::on_pushButton_2_clicked()
{
    this->close();
}

bool FactorAdd::Conf_FactorUpdate(QString pKey)
{

    QJsonObject pFactor;
    if(ui->radioButton_YQCS->isChecked()) pFactor.insert(CONF_IS_DEVICE_PROPERTY,true);
    else pFactor.insert(CONF_IS_DEVICE_PROPERTY,false);

    if(ui->radioButton_MNL->isChecked()) pFactor.insert(CONF_IS_ANALOG_PARAM,true);
    else pFactor.insert(CONF_IS_ANALOG_PARAM,false);

    pFactor.insert(CONF_ANALOG_PARAM_AU1,ui->lineEdit_AU1->text().toDouble());
    pFactor.insert(CONF_ANALOG_PARAM_AD1,ui->lineEdit_AD1->text().toDouble());
    pFactor.insert(CONF_ANALOG_PARAM_AU2,ui->lineEdit_AU2->text().toDouble());
    pFactor.insert(CONF_ANALOG_PARAM_AD2,ui->lineEdit_AD2->text().toDouble());
    pFactor.insert(CONF_FACTOR_ALIAS,ui->comboBox_falias->currentText());

    httpclinet pClient;
    if(pClient.post(DCM_CONF_FACTOR_EDIT+pKey,pFactor))
    {
        return true;
    }
    return  false;
}
