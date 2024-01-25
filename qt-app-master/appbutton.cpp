#include "appbutton.h"
#include "ui_appbutton.h"

AppButton::AppButton(QString name,QString imgUrl,QWidget *parent) :
    QFrame(parent),
    ui(new Ui::AppButton)
{
    ui->setupUi(this);

    ui->btn_name->setWordWrap(true);

    if(!name.isEmpty())
    {
        appName = name;
        ui->btn_name->setText(appName);

    }
    else
    {
        ui->btn_name->setText("未知名称");
    }

    if(!imgUrl.isEmpty())
    {
        appPixmap = QPixmap(imgUrl);

        ui->btn_icon->setPixmap(appPixmap);
    }
    else
    {
        QPixmap pixmap = QPixmap(":/new/images/image/appdft.png");

        ui->btn_icon->setPixmap(pixmap);
    }

}

AppButton::~AppButton()
{
    delete ui;
}

void AppButton::setName(QString name)
{
    if(!name.isEmpty())
    {
        appName = name;
        ui->btn_name->setText(appName);

    }
    else
    {
        ui->btn_name->setText("未知名称");
    }
}

void AppButton::setPixmap(QString picUrl)
{
    if(!picUrl.isEmpty())
    {
        appPixmap = QPixmap(picUrl);


        ui->btn_icon->setPixmap(appPixmap);
    }
    else
    {
        QPixmap pixmap = QPixmap(":/new/images/image/appdft.png");

        ui->btn_icon->setPixmap(pixmap);
    }
}

void AppButton::setSize(int w,int h)
{
    this->resize(w,h);
}
