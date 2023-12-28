#include "deviceset.h"
#include "ui_deviceset.h"

DeviceSet::DeviceSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeviceSet)
{
    ui->setupUi(this);
}

DeviceSet::~DeviceSet()
{
    delete ui;
}
