#include "dialognewfactor.h"
#include "ui_dialognewfactor.h"

DialogNewFactor::DialogNewFactor(QString protoName,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewFactor)
{
    ui->setupUi(this);
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);

    m_ProtoName = protoName;
}

DialogNewFactor::~DialogNewFactor()
{
    delete ui;
}

void DialogNewFactor::on_buttonBox_accepted()
{
    if(ui->lineEditAddr->text() == "" || ui->lineEditCode->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>因子编码和地址不能为空！</font>");
        return;
    }

    CFactor pFactor;
    pFactor.setByteOrder(ui->comboBoxByteOrder->currentText());
    pFactor.setDataType(ui->comboBoxDataType->currentText());
    pFactor.setFactorCode(ui->lineEditCode->text());
    pFactor.setFactorName(ui->lineEditName->text());
    pFactor.setFactorFunc(ui->comboBoxFunc->currentText());
    pFactor.setTagId(ui->comboBoxTagID->currentText());
    pFactor.setProperty(ui->comboBoxProp->currentText());
    pFactor.setRegisAddr(ui->lineEditAddr->text());

    CJsonFile jsonfile;
    jsonfile.insert_factornode(m_ProtoName,pFactor);
}
