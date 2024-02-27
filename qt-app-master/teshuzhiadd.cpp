#include "teshuzhiadd.h"
#include "ui_teshuzhiadd.h"

TeshuzhiAdd::TeshuzhiAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TeshuzhiAdd)
{
    ui->setupUi(this);

    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QStringLiteral("新增特殊值"));
    connectevent();
    loadAvailbleFacs();
}

TeshuzhiAdd::~TeshuzhiAdd()
{

    delete ui;
}

void TeshuzhiAdd::loadAvailbleFacs()
{
    nameMap = util.Uart_facnameMatch();
    QJsonObject jDevFacs;
    if(h.get(DCM_DEVICE_FACTOR,jDevFacs))
    {
        for(int i=0;i<jDevFacs.count();++i)
        {
            QString key = jDevFacs.keys()[i];
            QString deviceid,code,name;
            if(key.split("-").count()==2)
            {
                deviceid = key.split("-")[0];
                code = key.split("-")[1];
                name = nameMap[code];
                avaFacMap.insert(code,name);
            }

        }
    }

    ui->lineEdit_Code->clear();
    QMap<QString,QString>::iterator it = avaFacMap.begin();
    while(it != avaFacMap.end())
    {
        ui->lineEdit_Code->addItem(it.key(),it.value());
        it++;
    }

    ui->lineEdit_Code->setCurrentIndex(0);

    QStringList valueList;
    valueList<<"最低"<<"低"<<"正常"<<"高"<<"最高";
    ui->lineEdit_Val->clear();
    ui->lineEdit_Val->addItems(valueList);
    ui->lineEdit_Val->setCurrentIndex(0);
}

void TeshuzhiAdd::connectevent()
{
    connect(ui->lineEdit_Code,&QComboBox::currentTextChanged,this,[=](const QString &text)
    {
        if(!text.isEmpty())
        {
            QString dataStr = ui->lineEdit_Code->currentData().toString();
            ui->lineEdit_Name->setText(dataStr);
        }
    });

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

void TeshuzhiAdd::on_buttonBox_accepted()
{

    QJsonObject obj,obj2;
    obj.insert(QLatin1String("key"), ui->lineEdit_Key->text());
    obj.insert(QLatin1String("name"), ui->lineEdit_Name->text());
    obj.insert(QLatin1String("value"), ui->lineEdit_Val->currentText());

    // qDebug() << "obj==>>" << obj;
    obj2 = obj;
    obj2.insert("code",ui->lineEdit_Code->currentText());

    QJsonObject devsJson =  facsMatchDevid(ui->lineEdit_Code->currentText());


    obj2.insert("device",devsJson);

    httpclinet pClient;
    if(pClient.post(DCM_SET_TESHUZHI+ui->lineEdit_Code->currentText(),obj))
    {
        //OpenDev_Setting();
//        Conf_TeshuzhiUpdate();
        writeinLocalJson(obj2);
        emit addSuccess();
        QMessageBox::about(NULL, "提示", "<font color='black'>新增特殊值配置成功！</font>");
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>新增特殊值配置失败！</font>");
    }

    this->close();
}

QJsonObject TeshuzhiAdd::facsMatchDevid(QString code)
{
    QJsonObject jRef,jRes;
    if(h.get(DCM_DEVICE_FACTOR,jRef))
    {
        for(int i=0;i<jRef.count();++i)
        {
            QString name = jRef.keys()[i];
            if(name.split("-").count()==2)
            {
                QString devid = name.split("-")[0];
                QString refcode = name.split("-")[1];
                if(code == refcode)
                {
                    jRes.insert(QString::number(i),devid);
                }
            }
        }
    }

    return jRes;
}

void TeshuzhiAdd::writeinLocalJson(QJsonObject &obj)
{
    QString code = obj.value("code").toString();
    QString name = obj.value("name").toString();
    QString key = obj.value("key").toString();
    QString value = obj.value("value").toString();

    QJsonObject devObj = obj.value("device").toObject();

    QJsonObject::iterator it = devObj.begin();
    while(it != devObj.end())
    {
        QString devid = it.value().toString();
        QString filename = "/home/rpdzkj/tmpFiles/"+devid+".json";
        QString fackey = devid+"-"+code;

#pragma region File_Read_Write {
        QFile fileR(filename);
        if(!fileR.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            return;
        }
        QByteArray byt;
        byt.append(fileR.readAll());
        fileR.flush();
        fileR.close();

        QJsonDocument jDoc = QJsonDocument::fromJson(byt);
        byt.clear();

        QJsonObject jObj = jDoc.object();
        QJsonObject jDev,jFacs,jSpe;
        jDev = jObj.value("device").toObject();
        jFacs = jObj.value("factors").toObject();
        if(jObj.contains("specials"))
        {
            jSpe = jObj.value("specials").toObject();
        }

        if(jFacs.contains(fackey))
        {
            jSpe.insert(code+"|"+key,value);

        }
        qDebug()<<__LINE__<<"specials:"<<jSpe<<endl;

        if(jObj.contains("specials"))
        {
            jObj.remove("specials");
        }
        jObj.insert("specials",jSpe);
        qDebug()<<__LINE__<<"new:"<<jObj<<endl;


        QFile fileW(filename);
        QJsonDocument jDoc1;
        jDoc1.setObject(jObj);
        fileW.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Truncate);
        fileW.write(jDoc1.toJson());
        fileW.flush();
        fileW.close();

#pragma endregion}

        it++;
    }

}

bool TeshuzhiAdd::Conf_TeshuzhiUpdate()
{
    QFile file(CONF);
    qDebug()<<__LINE__<<file.fileName()<<endl;
    if (file.exists()){
        // judge if json format is correct!!
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString value = file.readAll();
        file.close();
        QJsonParseError parseJsonErr;
        QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(),&parseJsonErr);
        if(!(parseJsonErr.error == QJsonParseError::NoError))
        {
            QMessageBox::about(NULL, "提示", "<font color='black'>修改因子自定义配置失败！</font>");
            return false;
        }

        QJsonObject jsonObject = document.object();

        QJsonValueRef RefVersionTlist = jsonObject.find(TESHUZHI).value();
        QJsonObject jsonObjTelist = RefVersionTlist.toObject();

        // key:  device_id-code

        QJsonObject pTeshuzhi;

        pTeshuzhi.insert(CONF_TE_NAME,ui->lineEdit_Name->text());
        pTeshuzhi.insert(CONF_TE_KEY,ui->lineEdit_Key->text());
        pTeshuzhi.insert(CONF_TE_VALUE,ui->lineEdit_Val->currentText());

        jsonObjTelist.insert(ui->lineEdit_Code->currentText(),pTeshuzhi);
        RefVersionTlist = jsonObjTelist;

        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QJsonDocument jsonDoc;
        jsonDoc.setObject(jsonObject);
        file.write(jsonDoc.toJson());
        file.close();

    }
    return true;
}
