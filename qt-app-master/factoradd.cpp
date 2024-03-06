#include "factoradd.h"
#include "ui_factoradd.h"

FactorAdd::FactorAdd(QString pDevID,QString devtype,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FactorAdd)
{
    ui->setupUi(this);
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QStringLiteral("新增因子"));

    // qDebug() << "pDevID===>>" << pDevID;

    m_DevId = pDevID;
    m_DevType = devtype;

    ui->comboBox_fcode->clear();
    ui->comboBox_falias->clear();

    for(int i=0;i<100;++i)
    {
        ui->comboBox_falias->addItem(QString::number(i));
    }

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

    ui->comboBox_falias->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_fcode->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_fst->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//    kb = new localKeyboard(this);
    connect(ui->keyboard,&QPushButton::clicked,this,[=]()
    {

//        if(!kb->isVisible())
//             kb->show();
        QProcess pro;
        pro.startDetached("pkill florence");
        pro.close();
        QProcess pro1;
        pro1.startDetached("florence");
        pro1.close();
    });

    ui->label_dec->setVisible(false);
    ui->lineEdit_dec->setVisible(false);

    ui->label_mf->setVisible(false);
    ui->lineEdit_mf->setVisible(false);

    ui->radioButton_MNL->setChecked(false);
    ui->modbus_add->setText("0");
    analogRadioBtnClicked();
}

FactorAdd::~FactorAdd()
{
    if(GroupF1) GroupF1->deleteLater();



    delete ui;
}

void FactorAdd::analogRadioBtnClicked() {

    qDebug()<<__LINE__<<m_DevType<<endl;
//    ui->radioButton_MNL->setCheckable(m_DevType.contains("analog"));

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

    QString pKey;
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

    if(findSameALias(g_Device_ID,ui->comboBox_falias->currentText()))
    {
        QMessageBox::about(NULL, "提示", "<font color='red'>序号与其他因子重复</font>");
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
    obj.insert(QLatin1String("coefficient"), ui->lineEdit_coeff->text().toDouble());
    obj.insert(QLatin1String("modbus_index"), ui->modbus_add->text().toInt());

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
    pKey = g_Device_ID + "-" + pFcode;


    if(ui->radioButton_YQCS->isChecked()) obj.insert(CONF_IS_DEVICE_PROPERTY,true);
    else obj.insert(CONF_IS_DEVICE_PROPERTY,false);

    if(ui->radioButton_MNL->isChecked()) obj.insert(CONF_IS_ANALOG_PARAM,true);
    else obj.insert(CONF_IS_ANALOG_PARAM,false);

    obj.insert(CONF_ANALOG_PARAM_AU1,ui->lineEdit_AU1->text().toDouble());
    obj.insert(CONF_ANALOG_PARAM_AD1,ui->lineEdit_AD1->text().toDouble());
    obj.insert(CONF_ANALOG_PARAM_AU2,ui->lineEdit_AU2->text().toDouble());
    obj.insert(CONF_ANALOG_PARAM_AD2,ui->lineEdit_AD2->text().toDouble());
    obj.insert(CONF_FACTOR_ALIAS,ui->comboBox_falias->currentText());

    QJsonObject jFacObj;
    jFacObj.insert(pKey,obj);
    QString fileName = "/home/rpdzkj/tmpFiles/"+g_Device_ID+".json";
    writeinLocalJson(fileName,jFacObj,pKey);

    httpclinet pClient;
    if(pClient.put(DCM_DEVICE_FACTOR,obj))
    {

        QJsonObject jFac;
        if(Conf_FactorUpdate(pKey,jFac))
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

void FactorAdd::writeinLocalJson(QString filename,QJsonObject &obj,QString pKey)
{
        QFile file(filename);
        if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            return;
        }
        QByteArray byt;
        byt.append(file.readAll());
        file.flush();

        QJsonDocument jDoc = QJsonDocument::fromJson(byt);
        QJsonObject jObj = jDoc.object();
        if(!jObj.contains("factors"))
        {
            jObj.insert("factors",obj);
        }
        else
        {
            QJsonObject jFacs = jObj.value("factors").toObject();
            if(jFacs.contains(pKey))
            {
                jFacs.remove(pKey);
            }
            jFacs.insert(pKey,obj.value(pKey).toObject());
            jObj.remove("factors");
            jObj.insert("factors",jFacs);
        }

        qDebug()<<__LINE__<<jObj<<endl;

        writeDevParams(jObj);



        QJsonDocument jDoc1;
        jDoc1.setObject(jObj);
        QFile file1(filename);
        file1.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate);
        //提示，因子数据已同步到本地JSON,文件路径
        if(file1.write(jDoc1.toJson()))
        {
            QMessageBox::information(this,"提示","因子数据已同步到本地JSON,文件路径:\n"+filename);
        }
        else
        {
            QMessageBox::warning(this,"提示","因子数据未能同步到本地JSON,文件路径:\n"+filename);
        }

        file1.close();
        byt.clear();
        file.close();

}

void FactorAdd::writeDevParams(QJsonObject &obj)
{
    QString devparams;
    QString filename = "/home/rpdzkj/tmpFiles/"+m_DevId+".json";
    qDebug()<<__LINE__<<filename<<endl;
//    QFile file(filename);
//    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
//    {
//        return ;
//    }

//    QByteArray byt;
//    byt.append(file.readAll());
//    file.flush();

//    QJsonDocument jDoc = QJsonDocument::fromJson(byt);
//    byt.clear();
//    file.close();

    QJsonObject jObj = obj;
    QJsonObject jDev = jObj.value("device").toObject();
    QJsonObject jFacs = jObj.value("factors").toObject();

    qDebug()<<__LINE__<<jFacs.keys()<<endl;



    QJsonObject::iterator itFac = jFacs.begin();
    int num=0;
    int numFacs = jFacs.count();
    while(itFac != jFacs.end())
    {
        QString alias = QString::number(num+1);
        QString pKey = itFac.key();
        qDebug()<<__LINE__<<pKey<<endl;
        QJsonObject jValue = itFac.value().toObject();
        if(jValue.value(CONF_IS_ANALOG_PARAM).toBool())
        {
            devparams += "analog_max_"+alias+"="+QString::number(jValue.value(CONF_ANALOG_PARAM_AU1).toDouble(),'f',2)+",";
            devparams += "analog_min_"+alias+"="+QString::number(jValue.value(CONF_ANALOG_PARAM_AD1).toDouble(),'f',2)+",";
            devparams += "upper_limit_"+alias+"="+QString::number(jValue.value(CONF_ANALOG_PARAM_AU2).toDouble(),'f',2)+",";
            devparams += "lower_limit_"+alias+"="+QString::number(jValue.value(CONF_ANALOG_PARAM_AD2).toDouble(),'f',2);
            if(num<numFacs-1)
            {
                devparams += ",";
            }
        }


        num++;
        itFac++;
    }

    qDebug()<<__LINE__<<"dev_params"<<devparams<<endl;

    if(jDev.value("dev_type").toString()=="analog")
    {
        if(jDev.contains("dev_params"))
        {
            jDev.remove("dev_params");
        }
        jDev.insert("dev_params",devparams);
    }
    qDebug()<<__LINE__<<"dev:"<<jDev<<endl;



    jObj.remove("device");
    jObj.insert("device",jDev);

    qDebug()<<__LINE__<<"new:"<<jObj<<endl;

    QJsonDocument jDocW;
    jDocW.setObject(jObj);


    QFile file1(filename);
    if(file1.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate))
    {
        if(file1.write(jDocW.toJson()))
        {
            qDebug()<<__LINE__<<"YES"<<endl;
        }
        else
        {
            qDebug()<<__LINE__<<"NO"<<endl;
        }
        file1.flush();
        file1.close();
    }




}

void FactorAdd::on_pushButton_2_clicked()
{
    this->close();
}

bool FactorAdd::Conf_FactorUpdate(QString pKey,QJsonObject &jFac)
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
    pFactor.insert("factoralias",ui->comboBox_falias->currentText().toInt());
    jFac = pFactor;
    qDebug()<<__LINE__<<jFac<<endl;

    httpclinet pClient;
    QJsonObject jRes;
    if(pClient.post(DCM_CONF_FACTOR_EDIT+pKey,pFactor,jRes))
    {
        qDebug()<<__LINE__<<jRes<<endl;
        return true;
    }
    return  false;
}

bool FactorAdd::findSameALias(QString devid,QString alias)
{
    QJsonObject jObj;
    httpclinet h;
    if(h.get(DCM_DEVICE_FACTOR,jObj))
    {
        QJsonObject::iterator it = jObj.begin();
        while(it != jObj.end())
        {
            QJsonObject valueObj = it.value().toObject();
            if(devid == valueObj.value("device_id").toString())
            {
                if(alias == valueObj.value("factor_alias").toString())
                {
                    return true;
                }
            }
            it++;
        }
        return false;
    }
    else
    {
//        QString dirpath = "/home/rpdzkj/tmpFiles/";
//        QDir dir(dirpath);
//        if(!dir.exists())
//        {
//            return false;
//        }
//        QStringList filter;
//        filter<<".json";
//        dir.setNameFilters(filter);
//        QFileInfoList *fileInfoList = new QFileInfoList(dir.entryInfoList(filter));
//        for(int i=0;i<fileInfoList->count();++i)
//        {
//            qDebug()<<__LINE__<<fileInfoList->at(i).filePath()<<endl;
//            qDebug()<<__LINE__<<fileInfoList->at(i).fileName()<<endl;

//        }

        return false;
    }
}
