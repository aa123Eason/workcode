#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWIndow::MainWIndow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWIndow)
{
    ui->setupUi(this);
    widgetInit();
    connectevent();
}

MainWIndow::~MainWIndow()
{
    delete ui;
}

void MainWIndow::widgetInit()//mainwindow init
{
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnBottomHint);
    ui->realtime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));

}

void MainWIndow::connectevent()
{
    connect(ui->close_btn,&QPushButton::clicked,this,&MainWIndow::close);
}
