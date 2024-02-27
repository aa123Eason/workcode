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

    font.setBold(true);
    font.setPointSize(20);
//kb = new localKeyboard(this);

    connect(ui->radioButton_Com, SIGNAL(clicked()), this, SLOT(typeRadioBtnClicked()));
    connect(ui->radioButton_Net, SIGNAL(clicked()), this, SLOT(typeRadioBtnClicked()));
    connect(ui->comboBox_devProto,&QComboBox::currentTextChanged,this,&DevEdit::onComboBoxProtoCurrentChanged);
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

    ui->comboBox_devProto->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_6->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_devBaud->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_devDatabit->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_devParity->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_devProto->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_devStopBit->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);



    QScrollBar *bar = ui->comboBox_devProto->view()->verticalScrollBar();
    int h = bar->height();
    bar->resize(50,h);

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

                ui->textEdit_devParams->clear();
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
    else
    {
        QString str = "/home/rpdzkj/tmpFiles/"+dev_id+".json";
        QFile file(str);
        if(file.exists())
        {
            if(file.open(QIODevice::ReadOnly|QIODevice::Text))
            {
                QByteArray byt;
                byt.append(file.readAll());
                file.flush();
                QJsonDocument jDoc = QJsonDocument::fromJson(byt);
                byt.clear();
                QJsonObject jObj = jDoc.object();
                QJsonObject jDevice = jObj.value("device").toObject();
                QJsonObject::iterator it = jDevice.begin();
                QJsonObject::iterator end = jDevice.end();
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

                        ui->textEdit_devParams->clear();
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

    }
}

void DevEdit::loadParamtable(QString dev_params)
{
    ui->paramtable->setRowCount(0);
    qDebug()<<__LINE__<<dev_params<<endl;
    qDebug()<<__LINE__<<ui->comboBox_devProto->currentText()<<endl;

    if(ui->comboBox_devProto->currentText() == "modbus通用驱动")
    {
        if(dev_params.split(",").count()==4)
        {
            ui->paramtable->setRowCount(4);

            QTableWidgetItem *item1 = new QTableWidgetItem("起始寄存器地址");
            QTableWidgetItem *item2 = new QTableWidgetItem("寄存器数量");
            QTableWidgetItem *item3 = new QTableWidgetItem("功能码");
            QTableWidgetItem *item4 = new QTableWidgetItem("值类型");

            item1->setFont(font);
            item2->setFont(font);
            item3->setFont(font);
            item4->setFont(font);

            ui->paramtable->setItem(0,0,item1);
            ui->paramtable->setItem(1,0,item2);
            ui->paramtable->setItem(2,0,item3);
            ui->paramtable->setItem(3,0,item4);



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
                        QTableWidgetItem *item = new QTableWidgetItem(value);
                        if(item!=nullptr)
                        {
                            item->setFont(font);
                            ui->paramtable->setItem(0,1,item);
                        }
                    }
                    else if(name == "quantity")
                    {
                        QTableWidgetItem *item = new QTableWidgetItem(value);
                        if(item!=nullptr)
                        {
                            item->setFont(font);
                            ui->paramtable->setItem(1,1,item);
                        }
                    }
                    else if(name == "functionCode")
                    {

                        if(box1!=nullptr)
                            box1->setCurrentText(value);
                    }
                    else if(name == "vtype")
                    {

                        if(box2!=nullptr)
                            box2->setCurrentText(value);
                    }

                }
            }
        }
    }
    else if(ui->comboBox_devProto->currentText()=="模拟量")
    {
        QStringList paramList = dev_params.split(",");
        ui->paramtable->setRowCount(paramList.count());
        for(int i=0;i<paramList.count();++i)
        {
            QString paramItem = paramList[i];
            if(paramItem.split("=").count()==2)
            {
                QString paramName = paramItem.split("=")[0];
                QString paramValue = paramItem.split("=")[1];
                int t = i/4;
                QString index = QString::number(t+1);

                if(paramName.contains("analog_max_"))
                {
                    QTableWidgetItem *itemName = new QTableWidgetItem("模拟量"+index+"量程上限");
                    itemName->setFont(font);
                    QTableWidgetItem *itemValue = new QTableWidgetItem(paramValue);
                    itemValue->setFont(font);
                    ui->paramtable->setItem(4*t,0,itemName);
                    ui->paramtable->setItem(4*t,1,itemValue);


                }
                else if(paramName.contains("analog_min_"))
                {
                    QTableWidgetItem *itemName = new QTableWidgetItem("模拟量"+index+"量程下限");
                    itemName->setFont(font);
                    QTableWidgetItem *itemValue = new QTableWidgetItem(paramValue);
                    itemValue->setFont(font);
                    ui->paramtable->setItem(4*t+1,0,itemName);
                    ui->paramtable->setItem(4*t+1,1,itemValue);
                }
                else if(paramName.contains("upper_limit_"))
                {
                    QTableWidgetItem *itemName = new QTableWidgetItem("输入"+index+"范围上限");
                    itemName->setFont(font);

                    QString valuep = QString::number(paramValue.toDouble(),'f',0);
                    QTableWidgetItem *itemValue = new QTableWidgetItem(valuep);
                    itemValue->setFont(font);
                    ui->paramtable->setItem(4*t+2,0,itemName);
                    ui->paramtable->setItem(4*t+2,1,itemValue);
                }
                else if(paramName.contains("lower_limit_"))
                {
                    QTableWidgetItem *itemName = new QTableWidgetItem("输入"+index+"范围下限");
                    itemName->setFont(font);
                    QString valuep = QString::number(paramValue.toDouble(),'f',0);
                    QTableWidgetItem *itemValue = new QTableWidgetItem(valuep);
                    itemValue->setFont(font);
                    ui->paramtable->setItem(4*t+3,0,itemName);
                    ui->paramtable->setItem(4*t+3,1,itemValue);
                }

            }
        }
    }
}

void DevEdit::onComboBoxProtoCurrentChanged(const QString &text)
{
    ui->paramtable->setRowCount(0);

    loadParamtable(ui->textEdit_devParams->toPlainText());

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

    obj.remove(QLatin1String("dev_params"));
    obj.insert(QLatin1String("dev_params"), builddevparams());

    obj.insert(QLatin1String("dev_type"), namemap.key(ui->comboBox_devProto->currentText()));
    obj.insert(QLatin1String("ip_addr"), ui->lineEdit_ipAddr->text());
    obj.insert(QLatin1String("parity"), ui->comboBox_devParity->currentText());
    obj.insert(QLatin1String("stop_bit"), ui->comboBox_devStopBit->currentText());

     qDebug() << "obj==>>" << obj;
    QString path = "/home/rpdzkj/tmpFiles/"+ui->lineEdit_id->text()+".json";
    writeinfile(path,obj);

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

void DevEdit::writeloglocal(QString text)
{
    QString path = "/home/rpdzkj/log.txt";
    QFile file(path);
    file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append);

    QString str = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm")+text+"\r\n";
    file.write(str.toUtf8());
    file.flush();
    file.close();
}

void DevEdit::writeinfile(QString filepath,QJsonObject &obj)
{

    qDebug() << "0:jObjx==>>" << obj;
    qDebug() << "path==>>" << filepath;


    QFile fileR(filepath),fileW(filepath);
    if(fileR.exists())
    {
        if(fileR.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            QByteArray byt;
            byt.append(fileR.readAll());
            fileR.flush();
            fileR.close();
            QJsonDocument jDocx = QJsonDocument::fromJson(byt);
            QJsonObject jObjx = jDocx.object();
            //device
            qDebug() << "1:jObjx==>>"<<jObjx<<endl;
            if(jObjx.contains("device"))
            {
                qDebug() << "device==>>" << jObjx.value("device").toObject()<<endl;
                jObjx.remove("device");
            }
            jObjx.insert("device",obj);
            qDebug() << "2:jObjx==>>"<<jObjx<<endl;

//            QMessageBox::about(NULL, "提示", strx);

            //factors
            QJsonObject jNewDev = jObjx.value("device").toObject();
            QJsonObject jOldFacs = jObjx.value("factors").toObject();
            QJsonObject jNewFacs = jOldFacs;
            qDebug() << "jOldFacs==>>"<<jOldFacs<<endl;
            QString devparams;
//            strx = __LINE__+"\r\n";
//            QMessageBox::about(NULL, "提示", strx);
            if(jNewDev.value("dev_type").toString() == "analog")
            {
                devparams = jNewDev.value("dev_params").toString();
//                strx = devparams+"\r\n";
//                QMessageBox::about(NULL, "提示", strx);
                qDebug() <<__LINE__<<"dev_params:"<<devparams<<endl;
                qDebug() <<__LINE__<<"dev_params count"<<devparams.split(",").count()<<endl;
                if(devparams.split(",").count()>0)
                {
                    for(int i=0;i<devparams.split(",").count();++i)
                    {
//                        QMessageBox::about(NULL, "提示", i);
                        QString facitem = devparams.split(",")[i];
                        qDebug() <<__LINE__<<"["<<i<<"]:"<<facitem<<endl;
                        writeloglocal("["+QString::number(i)+"]:"+facitem);
                        if(facitem.split("=").count()==2)
                        {
                            QString key = facitem.split("=")[0];
                            QString value = facitem.split("=")[1];
                            qDebug() <<__LINE__<<key<<":"<<value<<endl;
                            writeloglocal(key+":"+value);


                            if(key.split("_").count()==3)
                            {
                                qDebug() <<__LINE__<<key.split("_")[0]<<" "<<key.split("_")[1]<<" "<<key.split("_")[2]<<endl;
                                writeloglocal(key.split("_")[0]+" "+key.split("_")[1]+" "+key.split("_")[2]);

                                int alias = key.split("_")[2].toInt();
                                writeloglocal(key.split("_")[2]+" "+QString::number(alias));
                                QString oriname = key.split("_")[0]+"_"+key.split("_")[1];
                                QString curFac = jNewFacs.keys()[alias-1];
                                QJsonObject jValue = jNewFacs.value(curFac).toObject();
                                qDebug() <<"**********************"<<endl;
                                qDebug() <<__LINE__<<oriname<<" "<<curFac<<endl;
                                writeloglocal(oriname+" "+curFac);



                                if(oriname == "analog_max")
                                {
                                    jValue.remove(CONF_ANALOG_PARAM_AU1);
                                    jValue.insert(CONF_ANALOG_PARAM_AU1,value.toDouble());
                                }
                                else if(oriname == "analog_min")
                                {
                                    jValue.remove(CONF_ANALOG_PARAM_AD1);
                                    jValue.insert(CONF_ANALOG_PARAM_AD1,value.toDouble());
                                }
                                else if(oriname == "upper_limit")
                                {
                                    jValue.remove(CONF_ANALOG_PARAM_AU2);
                                    jValue.insert(CONF_ANALOG_PARAM_AU2,value.toInt());
                                }
                                else if(oriname == "lower_limit")
                                {
                                    jValue.remove(CONF_ANALOG_PARAM_AD2);
                                    jValue.insert(CONF_ANALOG_PARAM_AD2,value.toInt());
                                }

                                writeloglocal("jNewFacs");

                                if(jNewFacs.contains(curFac))
                                    jNewFacs.remove(curFac);
                                jNewFacs.insert(curFac,jValue);

                            }
                        }
                    }
                    writeloglocal("jNewFacs:1");
//                    QMessageBox::about(NULL, "提示", strx);

                }
                writeloglocal("jNewFacs:2");

                 qDebug() << "jNewFacs==>>"<<jNewFacs<<endl;

                 if(jObjx.contains("factors"))
                     jObjx.remove("factors");
                jObjx.insert("factors",jNewFacs);
            }



            QJsonDocument jNewDoc;
            jNewDoc.setObject(jObjx);
            fileW.open(QIODevice::WriteOnly|QIODevice::Text);
            fileW.write(jNewDoc.toJson());
            fileW.close();


        }
    }
    else
    {
        QJsonDocument jNewDoc;
        QJsonObject jNewObj;
        jNewObj.insert("device",obj);
        fileW.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate);
        fileW.write(jNewDoc.toJson());
        fileW.close();
    }

}

void DevEdit::on_pushButton_Cancel_clicked()
{
    this->close();
}

QString DevEdit::builddevparams()
{
    QString resStr;
//    if(ui->tabWidget_params->currentWidget() == ui->tab_text)
//    {
//        QString str = ui->textEdit_devParams->toPlainText();

//        resStr = str;
//    }
//    else
//    {

    if(namemap.key(ui->comboBox_devProto->currentText()) == "lc-modbus")
    {
        QString tmpStr1;
        tmpStr1 += "start="+ui->paramtable->item(0,1)->text()+",";
        tmpStr1 += "quantity="+ui->paramtable->item(1,1)->text()+",";
        QComboBox *box1 = (QComboBox *)ui->paramtable->cellWidget(2,1);
        QComboBox *box2 = (QComboBox *)ui->paramtable->cellWidget(3,1);
        tmpStr1 += "functionCode="+box1->currentText()+",";
        tmpStr1 += "vtype="+box2->currentText();

        resStr = tmpStr1;
    }
    else if(namemap.key(ui->comboBox_devProto->currentText()) == "analog")
    {
        QString tmpStr2;
        httpclinet h;
        QJsonObject jDevObj;
        int num=0;
        if(h.get(DCM_DEVICE_FACTOR,jDevObj))
        {
            QJsonObject::iterator it = jDevObj.begin();
            while(it != jDevObj.end())
            {
                if(it.key().contains(ui->lineEdit_id->text()))
                {
                    num++;
                }
                it++;
            }
        }

        qDebug()<<__LINE__<<num<<endl;
        for(int i = 0;i<num;++i)
        {

            tmpStr2 += "analog_max_"+QString::number(i+1)+"="+ui->paramtable->item(4*i,1)->text()+",";
            tmpStr2 += "analog_min_"+QString::number(i+1)+"="+ui->paramtable->item(4*i+1,1)->text()+",";
            tmpStr2 += "upper_limit_"+QString::number(i+1)+"="+ui->paramtable->item(4*i+2,1)->text()+",";
            tmpStr2 += "lower_limit_"+QString::number(i+1)+"="+ui->paramtable->item(4*i+3,1)->text();
            if(i!=num-1)
            {
                tmpStr2 += ",";
            }

        }

        resStr = tmpStr2;
    }
    qDebug()<<__LINE__<<resStr<<endl;

    //write in device_factor
    if(namemap.key(ui->comboBox_devProto->currentText()) == "analog")
    {
        QStringList paramlist = resStr.split(",");
        httpclinet h;
        QJsonObject jObj;
        if(h.get(DCM_DEVICE_FACTOR,jObj))
        {
            for(int i =0;i<jObj.count();++i)
            {
                if(ui->lineEdit_id->text()==jObj.keys()[i].split("-")[0])
                {
                    QString keyName = jObj.keys()[i];
                    QJsonObject jSubObj = jObj.value(jObj.keys()[i]).toObject();
                    QString seq = jSubObj.value("factor_alias").toString();
                    for(int j=0;j<paramlist.count();++j)
                    {
                        QString paramitem = paramlist[j];
                        if(paramitem.split("=").count()==2)
                        {
                            QString paraName = paramitem.split("=")[0];
                            QString paramValue = paramitem.split("=")[1];
                            if(paraName.split("_").count()==3)
                            {
                                qDebug()<<__LINE__<<paraName<<endl;
                                QString refseq = paraName.split("_")[2];
                                qDebug()<<__LINE__<<seq<<"vs"<<refseq<<endl;
                                if(seq == refseq)
                                {

                                    if(!jSubObj.contains(CONF_IS_ANALOG_PARAM))
                                    {
                                        jSubObj.insert(CONF_IS_ANALOG_PARAM,true);

                                        if(paraName == "analog_max_"+seq)
                                        {
                                            if(jSubObj.contains(CONF_ANALOG_PARAM_AU1))
                                                jSubObj.remove(CONF_ANALOG_PARAM_AU1);
                                            jSubObj.insert(CONF_ANALOG_PARAM_AU1,paramValue.toDouble());
                                        }
                                        else if(paraName == "analog_min_"+seq)
                                        {
                                            if(jSubObj.contains(CONF_ANALOG_PARAM_AD1))
                                                jSubObj.remove(CONF_ANALOG_PARAM_AD1);
                                            jSubObj.insert(CONF_ANALOG_PARAM_AD1,paramValue.toDouble());
                                        }
                                        else if(paraName == "upper_limit_"+seq)
                                        {
                                            if(jSubObj.contains(CONF_ANALOG_PARAM_AU2))
                                                jSubObj.remove(CONF_ANALOG_PARAM_AU2);
                                            jSubObj.insert(CONF_ANALOG_PARAM_AU2,paramValue.toDouble());
                                        }
                                        else if(paraName == "lower_limit_"+seq)
                                        {
                                            if(jSubObj.contains(CONF_ANALOG_PARAM_AD2))
                                                jSubObj.remove(CONF_ANALOG_PARAM_AD2);
                                            jSubObj.insert(CONF_ANALOG_PARAM_AD2,paramValue.toDouble());
                                        }
                                    }
                                    else
                                    {

                                        if(jSubObj.value(CONF_IS_ANALOG_PARAM).toBool())
                                        {
                                            if(paraName == "analog_max_"+seq)
                                            {
                                                if(jSubObj.contains(CONF_ANALOG_PARAM_AU1))
                                                    jSubObj.remove(CONF_ANALOG_PARAM_AU1);
                                                jSubObj.insert(CONF_ANALOG_PARAM_AU1,paramValue.toDouble());
                                            }
                                            else if(paraName == "analog_min_"+seq)
                                            {
                                                if(jSubObj.contains(CONF_ANALOG_PARAM_AD1))
                                                    jSubObj.remove(CONF_ANALOG_PARAM_AD1);
                                                jSubObj.insert(CONF_ANALOG_PARAM_AD1,paramValue.toDouble());
                                            }
                                            else if(paraName == "upper_limit_"+seq)
                                            {
                                                if(jSubObj.contains(CONF_ANALOG_PARAM_AU2))
                                                    jSubObj.remove(CONF_ANALOG_PARAM_AU2);
                                                jSubObj.insert(CONF_ANALOG_PARAM_AU2,paramValue.toDouble());
                                            }
                                            else if(paraName == "lower_limit_"+seq)
                                            {
                                                if(jSubObj.contains(CONF_ANALOG_PARAM_AD2))
                                                    jSubObj.remove(CONF_ANALOG_PARAM_AD2);
                                                jSubObj.insert(CONF_ANALOG_PARAM_AD2,paramValue.toDouble());
                                            }

                                        }
                                        else
                                        {
                                            if(paraName == "analog_max_"+seq)
                                            {
                                                if(jSubObj.contains(CONF_ANALOG_PARAM_AU1))
                                                    jSubObj.remove(CONF_ANALOG_PARAM_AU1);
                                                jSubObj.insert(CONF_ANALOG_PARAM_AU1,QString::number(0.00,'f',2));
                                            }
                                            else if(paraName == "analog_min_"+seq)
                                            {
                                                if(jSubObj.contains(CONF_ANALOG_PARAM_AD1))
                                                    jSubObj.remove(CONF_ANALOG_PARAM_AD1);
                                                jSubObj.insert(CONF_ANALOG_PARAM_AD1,QString::number(0.00,'f',2));
                                            }
                                            else if(paraName == "upper_limit_"+seq)
                                            {
                                                if(jSubObj.contains(CONF_ANALOG_PARAM_AU2))
                                                    jSubObj.remove(CONF_ANALOG_PARAM_AU2);
                                                jSubObj.insert(CONF_ANALOG_PARAM_AU2,QString::number(5));
                                            }
                                            else if(paraName == "lower_limit_"+seq)
                                            {
                                                if(jSubObj.contains(CONF_ANALOG_PARAM_AD2))
                                                    jSubObj.remove(CONF_ANALOG_PARAM_AD2);
                                                jSubObj.insert(CONF_ANALOG_PARAM_AD2,QString::number(1));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    jObj.remove(keyName);
                    jObj.insert(keyName,jSubObj);
                    qDebug()<<__LINE__<<jSubObj<<endl;



                }
            }

            qDebug()<<__LINE__<<jObj<<endl;

            h.put(DCM_DEVICE_FACTOR,jObj);

        }


    }

    return resStr;
//    }
}
