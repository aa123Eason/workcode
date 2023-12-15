#include "dialognewdev.h"
#include "ui_dialognewdev.h"



DialogNewDev::DialogNewDev(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewDev)
{
    ui->setupUi(this);
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);

    guiInit();
}

DialogNewDev::~DialogNewDev()
{
    delete ui;
}

void DialogNewDev::guiInit()
{
    CJsonFile jsonfile;
    QJsonArray pJsonProtoArray = jsonfile.get_level1_array(TEMPLATE);
    int pCnt = pJsonProtoArray.size();
    for(int i=0;i<pCnt;i++)
    {
        QJsonObject pJsonProto = pJsonProtoArray.at(i).toObject();
        QString tempName;

        if(pJsonProto.contains(TEMPLATE_NAME))
        {
            tempName = pJsonProto.value(TEMPLATE_NAME).toString();
            ui->comboBox->addItem(tempName);
        }
    }
    return;
}

void DialogNewDev::on_buttonBox_accepted()
{
    // qjsonfile
    CJsonFile jsonfile;
    jsonfile.insert_devnode(ui->lineEdit->text(), ui->lineEdit_2->text(), ui->comboBox->currentText());
    emit exitWin();
}
