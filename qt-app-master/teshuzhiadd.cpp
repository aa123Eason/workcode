#include "teshuzhiadd.h"
#include "ui_teshuzhiadd.h"

TeshuzhiAdd::TeshuzhiAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TeshuzhiAdd)
{
    ui->setupUi(this);

    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QStringLiteral(" "));
}

TeshuzhiAdd::~TeshuzhiAdd()
{
    delete ui;
}

void TeshuzhiAdd::on_buttonBox_accepted()
{

    QJsonObject obj;
    obj.insert(QLatin1String("key"), ui->lineEdit_Key->text());
    obj.insert(QLatin1String("name"), ui->lineEdit_Name->text());
    obj.insert(QLatin1String("value"), ui->lineEdit_Val->text());

    // qDebug() << "obj==>>" << obj;

    httpclinet pClient;
    if(pClient.post(DCM_SET_TESHUZHI+ui->lineEdit_Code->text(),obj))
    {
        //OpenDev_Setting();
        QMessageBox::about(NULL, "提示", "<font color='black'>新增特殊值配置成功！</font>");
    }
    else
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>新增特殊值配置失败！</font>");
    }

    this->close();
}

bool TeshuzhiAdd::Conf_TeshuzhiUpdate()
{
    QFile file(CONF);
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
        pTeshuzhi.insert(CONF_TE_VALUE,ui->lineEdit_Val->text());

        jsonObjTelist.insert(ui->lineEdit_Code->text(),pTeshuzhi);
        RefVersionTlist = jsonObjTelist;

        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QJsonDocument jsonDoc;
        jsonDoc.setObject(jsonObject);
        file.write(jsonDoc.toJson());
        file.close();

    }
    return true;
}
