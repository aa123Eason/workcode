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

void facPanel::setcode(QString code)
{
    ui->fac_code->setText(code);
    ui->fac_code->setAlignment(Qt::AlignCenter);
}

void facPanel::setname(QString name)
{
    ui->fac_name->setText(name);
    ui->fac_name->setAlignment(Qt::AlignCenter);
}

void facPanel::setvalue(QString value)
{
    ui->fac_value->setText(value);
    ui->fac_value->setAlignment(Qt::AlignCenter);
}

void facPanel::setstate(QString state)
{
    ui->fac_state->setText(state);
    ui->fac_state->setAlignment(Qt::AlignCenter);
    if(state == "N")
    {
        ui->fac_statelight->setStyleSheet("background-color:#11aa00");
    }
    else
    {
        ui->fac_statelight->setStyleSheet("background-color:#aa1100");
    }


}

void facPanel::setunit(QString unit)
{
    ui->fac_unit->setText(unit);
    ui->fac_unit->setAlignment(Qt::AlignCenter);
}
