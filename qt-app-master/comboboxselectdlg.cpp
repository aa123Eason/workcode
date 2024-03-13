#include "comboboxselectdlg.h"
#include "ui_comboboxselectdlg.h"

ComBoBoxSelectDlg::ComBoBoxSelectDlg(QString title,int rows,int cols,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComBoBoxSelectDlg)
{
    ui->setupUi(this);
    ui->title->setText(title);

    this->setWindowModality(Qt::ApplicationModal);

//    this->setFocusPolicy(Qt::StrongFocus);
//    ui->itemLayout->setRowCount(rows);
//    ui->itemLayout->setColumnCount(cols);

    font.setBold(true);
    font.setPointSize(20);



    connect(ui->cancel,&QPushButton::clicked,this,&ComBoBoxSelectDlg::close);
    connect(this,&ComBoBoxSelectDlg::sendSelectedButton,this,[=](QString name)
    {
        qDebug()<<__LINE__<<name<<" is selected!"<<endl;
        this->close();
    });

    connect(ui->listitem,&QListWidget::currentTextChanged,this,[=](const QString &text)
    {
        emit sendSelectedButton(text);
        this->close();
    });

}

ComBoBoxSelectDlg::~ComBoBoxSelectDlg()
{
    delete ui;
}

void ComBoBoxSelectDlg::addButton(QString name)
{
    qDebug()<<__LINE__<<__FUNCTION__<<name<<endl;
    QPushButton *btn = new QPushButton(name);
    if(btn!=nullptr)
    {




        btn->setStyleSheet("QPushButton\n{\n	\n	background-color: rgb(135, 206, 235);\nheight:20px;\nborder-radius:2px;\n\n}\n\nQPushButton::hover\n{\n	\n	\n	background-color: rgb(7, 73, 133);/"
                           "\n\ncolor: rgb(252, 233, 79);\n\n}");





        QScrollBar *hs = new QScrollBar(Qt::Horizontal);
        QScrollBar *vs = new QScrollBar(Qt::Vertical);
        vs->resize(20,ui->listitem->height());
        hs->resize(20,ui->listitem->height());

        QListWidgetItem *item = new QListWidgetItem(name);
        ui->listitem->addItem(name);
        ui->listitem->setItemWidget(item,btn);
        ui->listitem->setVerticalScrollBar(vs);
        ui->listitem->setHorizontalScrollBar(hs);



    }
}
