#include "facedit.h"
#include "ui_facedit.h"

FacEdit::FacEdit(QString id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FacEdit)
{
    ui->setupUi(this) ;

    qDebug() << "id=--***********-->>" <<id;

    m_FcodeID = id;
    FacEdit_Init(id);
}

FacEdit::~FacEdit()
{
    delete ui;
}

void FacEdit::FacEdit_Init(QString id)
{
    ui->comboBox_fst->clear();
    ui->comboBox_fcode->clear();

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

    QJsonObject jsonObjectFa;
    httpclinet pClient;

    if(pClient.get(DCM_DEVICE_FACTOR,jsonObjectFa))
    {
        //qDebug() << "jsonObjectFa===>>" << jsonObjectFa;

        QJsonObject::const_iterator it = jsonObjectFa.constBegin();
        QJsonObject::const_iterator end = jsonObjectFa.constEnd();
        while(it != end)
        {
            QJsonObject pJsonFac = it.value().toObject();
            //qDebug() << "**********" << it.key();
            if(pJsonFac.value("id").toString() == id)
            {
                //qDebug() << "&&&&&&&&&&&" <<pJsonFac.value("factor_code").toString();

                bool calc_type = pJsonFac.value("calc_type").toInt() == 0;
                if(!calc_type)
                {
                    ui->radioButton_decN->setChecked(true);
                    ui->lineEdit_dec->setText(QString::number(pJsonFac.value("decimals").toInt()));
                }
                else
                {
                    ui->radioButton_decY->setChecked(true);
                }

                bool is_change_send_msg = pJsonFac.value("is_change_send_msg").toInt() == 0;
                if(is_change_send_msg) ui->radioButton_FBJC->setChecked(false);
                else ui->radioButton_FBJC->setChecked(true);

                bool is_change_send_msg_store = pJsonFac.value("is_change_send_msg_store").toInt() == 0;
                if(is_change_send_msg_store) ui->radioButton_FBJCC->setChecked(false);
                else ui->radioButton_FBJCC->setChecked(true);

                bool is_continuous_alarm_over_standard = pJsonFac.value("is_continuous_alarm_over_standard").toInt() == 0;
                if(is_continuous_alarm_over_standard) ui->radioButton_CXBJ->setChecked(false);
                else ui->radioButton_CXBJ->setChecked(true);

                bool is_manual_flag = pJsonFac.value("is_manual_flag").toInt() == 0;
                if(is_manual_flag)
                {
                    ui->radioButton_SDSZF->setChecked(false);
                }
                else
                {
                    ui->radioButton_SDSZF->setChecked(true);
                    ui->lineEdit_mf->setText(pJsonFac.value("manual_flag").toString());
                }

                ui->comboBox_fst->setCurrentText(pJsonFac.value("st").toString());
                ui->lineEdit_tagId->setText(pJsonFac.value("tag_id").toString());

                QString pFcode = pJsonFac.value("factor_code").toString();

                int index = -1;
                index = ui->comboBox_fcode->findData(pFcode);
                //qDebug() << "index==>" << index;
                if(index>=0) ui->comboBox_fcode->setCurrentIndex(index);
                else{
                    ui->comboBox_fcode->setEditable(true);
                    ui->comboBox_fcode->setCurrentText(pFcode);
                }

                ui->comboBox_falias->setCurrentText(pJsonFac.value("factor_alias").toString());
                ui->lineEdit_alarmUpper->setText(QString::number(pJsonFac.value("alarm_upper").toDouble()));
                ui->lineEdit_alarmLower->setText(QString::number(pJsonFac.value("alarm_lower").toDouble()));
                ui->lineEdit_coeff->setText(QString::number(pJsonFac.value("coefficient").toInt()));

                // read from CONF

                QString pItem = g_Device_ID+"-"+pFcode;

                qDebug() << "pItem===>>" << pItem;
                ConfFactor_Filled(pItem);

                break;
            }
            it++;
        }
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>获取因子信息失败!</font>");
    }
}

bool FacEdit::ConfFactor_Filled(QString pItem)
{
    QJsonObject jsonObject;
    httpclinet pClient;

    if(pClient.get(DCM_CONF,jsonObject))
    {
        QJsonObject pJsonFactors = jsonObject.value(FACTORS).toObject();
         qDebug() << "pJsonFactors=+++++==>>" << pJsonFactors;

        QJsonObject::const_iterator it = pJsonFactors.constBegin();
        QJsonObject::const_iterator end = pJsonFactors.constEnd();
        while(it != end)
        {

            qDebug() << "it.key()----->>" << it.key();

            if(pItem.toLower() == it.key())
            {
                QJsonObject pJsonFac = it.value().toObject();

                qDebug() << "pJsonFac****>>>" << pJsonFac;

                if(pJsonFac.contains(CONF_IS_DEVICE_PROPERTY))
                {
                    if(pJsonFac.value(CONF_IS_DEVICE_PROPERTY).toBool())
                        ui->radioButton_YQCS->setChecked(true);
                    else
                        ui->radioButton_YQCS->setChecked(false);
                }

                if(pJsonFac.contains(CONF_IS_ANALOG_PARAM))
                {
                    if(pJsonFac.value(CONF_IS_ANALOG_PARAM).toBool())
                    {
                        ui->radioButton_MNL->setChecked(true);
                    }
                    else
                    {
                        ui->radioButton_MNL->setChecked(false);
                    }
                }

                if(pJsonFac.contains(CONF_ANALOG_PARAM_AU1))
                {
                    ui->lineEdit_AU1->setText(QString::number(pJsonFac.value(CONF_ANALOG_PARAM_AU1).toDouble()));
                }

                if(pJsonFac.contains(CONF_ANALOG_PARAM_AD1))
                {
                    ui->lineEdit_AD1->setText(QString::number(pJsonFac.value(CONF_ANALOG_PARAM_AD1).toDouble()));
                }

                if(pJsonFac.contains(CONF_ANALOG_PARAM_AU2))
                {
                    ui->lineEdit_AU2->setText(QString::number(pJsonFac.value(CONF_ANALOG_PARAM_AU2).toDouble()));
                }

                if(pJsonFac.contains(CONF_ANALOG_PARAM_AD2))
                {
                    ui->lineEdit_AD2->setText(QString::number(pJsonFac.value(CONF_ANALOG_PARAM_AD2).toDouble()));
                }

                return true;
            }
            it++;
        }
    }
    return false;
}

void FacEdit::on_pushButton_cancel_clicked()
{
    this->close();
}

void FacEdit::on_pushButton_Saved_clicked()
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

    if(ui->comboBox_fst->currentData().toString() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>ST不能为空！</font>");
        return;
    }

    QString pFCode = ui->comboBox_fcode->currentData().toString();
    QString pKey = g_Device_ID + "-" + pFCode;

    QJsonObject obj;
    obj.insert(QLatin1String("id"), m_FcodeID);
    obj.insert(QLatin1String("st"), ui->comboBox_fst->currentData().toString());
    obj.insert(QLatin1String("device_id"), g_Device_ID);
    obj.insert(QLatin1String("factor_code"), pFCode);
    obj.insert(QLatin1String("factor_alias"), ui->comboBox_falias->currentText());
    obj.insert(QLatin1String("alarm_lower"), ui->lineEdit_alarmLower->text().toDouble());
    obj.insert(QLatin1String("alarm_upper"), ui->lineEdit_alarmUpper->text().toDouble());
    obj.insert(QLatin1String("tag_id"), ui->lineEdit_tagId->text());
    obj.insert(QLatin1String("coefficient"), ui->lineEdit_coeff->text().toInt());

    if(ui->radioButton_decN->isChecked())
    {
        obj.insert(QLatin1String("decimals"), ui->lineEdit_dec->text().toInt());
        obj.insert(QLatin1String("calc_type"), 1);
    }
    else
    {
        obj.insert(QLatin1String("decimals"), 0);
        obj.insert(QLatin1String("calc_type"), 0);
    }

    if(ui->radioButton_SDSZF->isChecked())
    {
        obj.insert(QLatin1String("is_manual_flag"), 1);
        obj.insert(QLatin1String("manual_flag"), ui->lineEdit_mf->text());
    }
    else
    {
        obj.insert(QLatin1String("is_manual_flag"), 0);
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
        // update conf
        if(Conf_FactorUpdate(pKey))
        {
            emit editSuccess();
            QMessageBox::about(NULL, "提示", "<font color='black'>修改因子配置成功！</font>");
        }
        else
        {
            QMessageBox::about(NULL, "提示", "<font color='black'>修改因子配置失败！</font>");
        }
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>修改因子配置失败！</font>");
    }
    this->close();
}

bool FacEdit::Conf_FactorUpdate(QString pKey)
{
    QJsonObject obj;
    obj.insert(QLatin1String(CONF_ANALOG_PARAM_AU1), ui->lineEdit_AU1->text().toDouble());
    obj.insert(QLatin1String(CONF_ANALOG_PARAM_AD1), ui->lineEdit_AD1->text().toDouble());
    obj.insert(QLatin1String(CONF_ANALOG_PARAM_AU2), ui->lineEdit_AU2->text().toDouble());
    obj.insert(QLatin1String(CONF_ANALOG_PARAM_AD2), ui->lineEdit_AD2->text().toDouble());
    obj.insert(QLatin1String(CONF_FACTOR_ALIAS), ui->comboBox_falias->currentText().toInt());

    if(ui->radioButton_YQCS->isChecked()) obj.insert(CONF_IS_DEVICE_PROPERTY,true);
    else obj.insert(CONF_IS_DEVICE_PROPERTY,false);

    if(ui->radioButton_MNL->isChecked()) obj.insert(CONF_IS_ANALOG_PARAM,true);
    else obj.insert(CONF_IS_ANALOG_PARAM,false);

    httpclinet pClient;
    if(pClient.post(DCM_CONF_FACTOR_EDIT+pKey,obj))
    {
        return true;
    }

    return false;
}
