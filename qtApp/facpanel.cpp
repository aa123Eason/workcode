#include "facpanel.h"
#include "ui_facpanel.h"

facPanel::facPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::facPanel)
{
    ui->setupUi(this);
}

facPanel::~facPanel()
{
    delete ui;
}
