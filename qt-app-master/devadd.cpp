#include "devadd.h"
#include "ui_devadd.h"
#include <QListView>

DevAdd::DevAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DevAdd)
{
    ui->setupUi(this);

    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QStringLiteral(" "));

    ui->radioButton_Com->setChecked(true);
    connect(ui->radioButton_Com, SIGNAL(clicked()), this, SLOT(typeRadioBtnClicked()));
    connect(ui->radioButton_Net, SIGNAL(clicked()), this, SLOT(typeRadioBtnClicked()));

    interGroup = new QButtonGroup(this);
    interGroup->addButton(ui->radioButton_Com, 0);
    interGroup->addButton(ui->radioButton_Net, 1);

    ui->comboBox->setEditable(true);

    ui->comboBox_2->clear();
    QJsonObject::const_iterator itor = g_Dcm_SupportDevice.constBegin();
    QJsonObject::const_iterator end_proto = g_Dcm_SupportDevice.constEnd();
    while(itor != end_proto)
    {
        ui->comboBox_2->addItem(itor.key());
        itor++;
    }

    ui->comboBox_2->setView(new QListView(this));
    ui->comboBox->setView(new QListView(this));
    ui->comboBox_baud->setView(new QListView(this));
    ui->comboBox_stopbit->setView(new QListView(this));
    ui->comboBox_databit->setView(new QListView(this));
    ui->comboBox_parity->setView(new QListView(this));
}

DevAdd::~DevAdd()
{
    if(interGroup) interGroup->deleteLater();
    delete ui;
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

    QString pDevType = ui->comboBox_2->currentText();
    if(pDevType == "analog") g_IsAnalogDevOperated = true;
    obj.insert(QLatin1String("dev_type"), pDevType);

    obj.insert(QLatin1String("ip_addr"), ui->lineEdit_ipAddr->text());
    obj.insert(QLatin1String("parity"), ui->comboBox_parity->currentText());
    obj.insert(QLatin1String("stop_bit"), ui->comboBox_stopbit->currentText());

    // qDebug() << "obj==>>" << obj;

    httpclinet pClient;
    if(pClient.put(DCM_DEVICE,obj))
    {
        //OpenDev_Setting();
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
