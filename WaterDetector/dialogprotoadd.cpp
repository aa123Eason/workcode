#include "dialogprotoadd.h"
#include "ui_dialogprotoadd.h"

DialogProtoAdd::DialogProtoAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProtoAdd)
{
    ui->setupUi(this);
    setModal(true);
}

DialogProtoAdd::~DialogProtoAdd()
{
    delete ui;
}

void DialogProtoAdd::on_buttonBox_accepted()
{
    if(ui->lineEdit->text() == "")
    {
        QMessageBox::about(NULL, "提示", "<font color='black'>设备类型名称不能为空！</font>");
        return;
    }

    CJsonFile jsonfile;
    jsonfile.insert_protonode(ui->lineEdit->text(),ui->comboBox->currentText());
    QMessageBox::about(NULL, "提示", "<font color='black'>新增设备类型成功！</font>");
}
