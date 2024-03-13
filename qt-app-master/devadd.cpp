#include "devadd.h"
#include "ui_devadd.h"
#include <QListView>

DevAdd::DevAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DevAdd)
{
    ui->setupUi(this);

//    setModal(true);
//    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QStringLiteral(" "));

    ui->radioButton_Com->setChecked(true);
    connect(ui->radioButton_Com, SIGNAL(clicked()), this, SLOT(typeRadioBtnClicked()));
    connect(ui->radioButton_Net, SIGNAL(clicked()), this, SLOT(typeRadioBtnClicked()));

    connectevent();

    interGroup = new QButtonGroup(this);
    interGroup->addButton(ui->radioButton_Com, 0);
    interGroup->addButton(ui->radioButton_Net, 1);

//    ui->comboBox->setEditable(true);

    ui->comboBox_2->clear();
    QJsonObject::const_iterator itor = g_Dcm_SupportDevice.constBegin();
    QJsonObject::const_iterator end_proto = g_Dcm_SupportDevice.constEnd();
    map = util.Uart_devicetype();
    namemap = util.Uart_devicetypeNameMatch();
//    ui->comboBox_2->addItem("");
    while(itor != end_proto)
    {
        if(!namemap[itor.key()].isEmpty())
            ui->comboBox_2->addItem(namemap[itor.key()]);
        itor++;
    }
    ui->comboBox_2->setCurrentIndex(0);

    ui->comboBox_2->setView(new QListView(this));
    ui->comboBox->setView(new QListView(this));
    ui->comboBox_baud->setView(new QListView(this));
    ui->comboBox_stopbit->setView(new QListView(this));
    ui->comboBox_databit->setView(new QListView(this));
    ui->comboBox_parity->setView(new QListView(this));
    installEvents();


    ui->comboBox->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_2->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_baud->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_stopbit->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_databit->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->comboBox_parity->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

//kb = new localKeyboard(this);

}

DevAdd::~DevAdd()
{
    if(interGroup) interGroup->deleteLater();

    delete ui;
}

void DevAdd::installEvents()
{
    ui->comboBox->installEventFilter(this);
    ui->comboBox_2->installEventFilter(this);
    ui->comboBox_baud->installEventFilter(this);
    ui->comboBox_parity->installEventFilter(this);
    ui->comboBox_stopbit->installEventFilter(this);
    ui->comboBox_databit->installEventFilter(this);
}

bool DevAdd::eventFilter(QObject *o, QEvent *e)
{
    if(e->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *me =(QMouseEvent *)e;
        if(me->button()==Qt::LeftButton)
        {
            if(QString(o->metaObject()->className())==QString("QComboBox"))
            {
                QComboBox *box = (QComboBox*)o;
                box->setEditable(false);
                qDebug()<<__LINE__<<__FUNCTION__<<box->objectName()<<endl;
                int count_item = box->count();
                int row = 0,col = 0;
                if(dlgbox != nullptr)
                    dlgbox->close();
                dlgbox = new ComBoBoxSelectDlg("选择下拉项",count_item,1);

//                // 获取屏幕宽度

//                int screenWidth = QApplication::desktop()->width();

//                // 获取屏幕高度

//                int screenHeight = QApplication::desktop()->height();

                //move(QWindow::pos() + (screenWidth - width()) / 2 - (screenHeight - height()) / 2);
//               dlgbox->move(QDialog::pos().x()+(screenWidth - width()) / 2,QDialog::pos().y() - (screenHeight - height()) / 2);


                for(int i=0;i<count_item;++i)
                {
                    qDebug()<<__LINE__<<__FUNCTION__<<"name==>"<<box->itemText(i);

                    if(!box->itemText(i).isEmpty())
                        dlgbox->addButton(box->itemText(i));

                    dlgbox->setWindowFlags(Qt::WindowStaysOnTopHint);

                    connect(dlgbox,&ComBoBoxSelectDlg::sendSelectedButton,this,[=](QString name)
                    {
                        box->setCurrentText(name);
                    });


                    if(col<1)
                    {
                        col++;
                    }
                    else
                    {
                        col=0;
                        row++;
                    }

                }

                dlgbox->show();
            }
        }
    }

    return QDialog::eventFilter(o,e);
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
    QJsonObject jRes;
    if(pClient.put(DCM_DEVICE,obj,jRes))
    {
        qDebug()<<"Res==>"<<jRes<<endl;
        //OpenDev_Setting();
        buildLocalJson(obj);
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


}

void DevAdd::buildLocalJson(QJsonObject &obj)
{
    httpclinet h;
    QJsonObject jObj,jDevice;
    if(h.get(DCM_DEVICE,jObj))
    {
        qDebug() << "inputobj==>>" << obj<<endl;
        qDebug() << "outputobj==>>" << jObj<<endl;
        QString currentid;

        if(compare2devices(obj,jObj,currentid))
        {
            if(!currentid.isEmpty())
            {
                jDevice.insert("device",jObj.value(currentid).toObject());
                QString filestr = "/home/rpdzkj/tmpFiles/"+currentid+".json";
                qDebug()<<__LINE__<<filestr<<endl;
                //             QDir dir;
                //             if(!dir.exists(filestr))
                //             {
                //                 qDebug()<<__LINE__<<filestr<<" is not exist!"<<endl;
                //                 dir.mkdir(filestr);

                //             }

                QFile file(filestr);
                QJsonDocument jDoc;
                jDoc.setObject(jDevice);
                file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate);
                file.write(jDoc.toJson());
                file.close();
            }

        }
    }
}

bool DevAdd::compare2devices(QJsonObject &nowobj,QJsonObject &refobj,QString &idcode)
{
    QJsonObject::iterator it = refobj.begin();
    while(it!=refobj.end())
    {
        QJsonObject subObj = it.value().toObject();
        qDebug()<<__LINE__<<subObj<<endl;
        bool isSame = true;
        isSame = isSame && (nowobj.value("address").toString()==subObj.value("address").toString());
        isSame = isSame && (nowobj.value("baudrate").toInt()==subObj.value("baudrate").toInt());
        isSame = isSame && (nowobj.value("com").toString()==subObj.value("com").toString());
        isSame = isSame && (nowobj.value("data_bit").toInt()==subObj.value("data_bit").toInt());
        isSame = isSame && (nowobj.value("parity").toString()==subObj.value("parity").toString());
        isSame = isSame && (nowobj.value("stop_bit").toString()==subObj.value("stop_bit").toString());
        isSame = isSame && (nowobj.value("dev_name").toString()==subObj.value("dev_name").toString());
        isSame = isSame && (nowobj.value("dev_type").toString()==subObj.value("dev_type").toString());
        isSame = isSame && (nowobj.value("ip_addr").toString()==subObj.value("ip_addr").toString());
        isSame = isSame && (nowobj.value("dev_params").toString()==subObj.value("dev_params").toString());
        if(isSame)
        {
            idcode = subObj.value("id").toString();
            qDebug()<<__LINE__<<"curID:"<<idcode<<endl;
            return true;
        }

        it++;
    }

    return false;
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
    ui->textEdit_devParams->clear();
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
