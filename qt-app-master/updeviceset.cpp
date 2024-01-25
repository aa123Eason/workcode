#include "updeviceset.h"
#include "ui_updeviceset.h"

UpDeviceSet::UpDeviceSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpDeviceSet)
{
    ui->setupUi(this);
}

UpDeviceSet::~UpDeviceSet()
{
    delete ui;
}
