#include "comboboxselectdlg.h"
#include "ui_comboboxselectdlg.h"

ComBoBoxSelectDlg::ComBoBoxSelectDlg(QString title,int rows,int cols,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComBoBoxSelectDlg)
{
    ui->setupUi(this);
    ui->title->setText(title);

    this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    this->setFocusPolicy(Qt::StrongFocus);
    ui->itemLayout->setRowCount(rows);
    ui->itemLayout->setColumnCount(cols);

    font.setBold(true);
    font.setPointSize(20);

    connect(ui->cancel,&QPushButton::clicked,this,&ComBoBoxSelectDlg::close);
    connect(this,&ComBoBoxSelectDlg::sendSelectedButton,this,[=](QString name)
    {
        qDebug()<<__LINE__<<name<<" is selected!"<<endl;
        this->close();
    });

}

ComBoBoxSelectDlg::~ComBoBoxSelectDlg()
{
    delete ui;
}

void ComBoBoxSelectDlg::addButton(int row,int col,QString name)
{
    qDebug()<<__LINE__<<__FUNCTION__<<name<<endl;
    QPushButton *btn = new QPushButton(name);
    if(btn!=nullptr)
    {

        btn->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
        btn->resize(100,80);
        btn->setStyleSheet("QPushButton\n{\n	\n	background-color: rgb(52, 101, 164);\n	border-radius:2px;\n\n}\n\nQPushButton::hover\n{\n	\n	\n	background-color: rgb(7, 73, 133);/"
                           "\n\ncolor: rgb(252, 233, 79);\n\n}");
        btn->setFlat(true);
        connect(btn,&QPushButton::clicked,this,[=]()
        {
            emit sendSelectedButton(btn->text());
        });

        ui->itemLayout->setCellWidget(row,col,btn);
        ui->itemLayout->setRowHeight(row,100);
        ui->itemLayout->setColumnWidth(col,80);
    }
}
