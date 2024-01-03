#include "dialogproto.h"
#include "ui_dialogproto.h"

DialogProto::DialogProto(QString proto,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProto)
{
    ui->setupUi(this);
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QStringLiteral(" "));

    m_DevProto = proto;
    ui->lineEdit->setText(proto);

    textEdit_filled();
}

DialogProto::~DialogProto()
{
    delete ui;
}

void DialogProto::textEdit_filled()
{
    QJsonObject pDevProto;
    httpclinet pClient;
    if(pClient.get(DCM_SUPPORT_DEVICE,pDevProto))
    {
        QJsonObject::const_iterator itor = pDevProto.constBegin();
        QJsonObject::const_iterator end_proto = pDevProto.constEnd();
        while(itor != end_proto)
        {
            if(itor.key() == m_DevProto)
            {
                ui->plainTextEdit->setPlainText(itor.value().toString());
                break;
            }
            itor++;
        }
    }
}
