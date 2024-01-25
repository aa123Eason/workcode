#include "teshuzhidele.h"
#include "ui_teshuzhidele.h"
#include <QListView>

TeshuzhiDele::TeshuzhiDele(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TeshuzhiDele)
{
    ui->setupUi(this);

    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QStringLiteral(" "));

    ui->comboBoxTeshuzhi->setView(new QListView(this));
    TeshuzhiGui_Init();
}

TeshuzhiDele::~TeshuzhiDele()
{
    delete ui;
}

void TeshuzhiDele::TeshuzhiGui_Init()
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
            return;
        }

        QJsonObject jsonObject = document.object();
        if(jsonObject.contains(TESHUZHI))
        {
            QJsonObject pTeshuzhi = jsonObject.value(TESHUZHI).toObject();
            QJsonObject::const_iterator it = pTeshuzhi.constBegin();
            QJsonObject::const_iterator end = pTeshuzhi.constEnd();
            while(it != end)
            {
                QJsonObject pJsonTe = it.value().toObject();
                if(pJsonTe.contains(CONF_TE_NAME))
                {
                    QString pTeName = pJsonTe.value(CONF_TE_NAME).toString();

                    ui->comboBoxTeshuzhi->addItem(it.key()+"-"+pTeName,it.key());
                }
                it++;
            }
        }
    }
}
