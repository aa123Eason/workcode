#include "selectdtdlg.h"
#include "ui_selectdtdlg.h"

SelectDTDlg::SelectDTDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectDTDlg)
{
    ui->setupUi(this);
    init();
    connectevent();
}

SelectDTDlg::~SelectDTDlg()
{
    delete ui;
}


void SelectDTDlg::init()
{
    this->setWindowFlags(Qt::WindowStaysOnTopHint);
//    this->setWindowModality(Qt::WindowModal);
    ui->timeedit->installEventFilter(this);
}

void SelectDTDlg::connectevent()
{
    connect(ui->btncancel,&QPushButton::clicked,this,&SelectDTDlg::close);

    connect(ui->btnok,&QPushButton::clicked,this,[=]()
    {
        dt.setDate(ui->datesel->selectedDate());
        dt.setTime(ui->timeedit->time());
        emit senddt(dt);
    });
}

bool SelectDTDlg::eventFilter(QObject *o, QEvent *e)
{

    if(o == ui->timeedit)
    {
        if(e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *me = (QMouseEvent *)e;
            if(me->button() == Qt::LeftButton)
            {
                QProcess pro;
                pro.startDetached("pkill florence");
                pro.close();
                QProcess pro1;
                pro1.startDetached("florence");
                pro1.close();
            }

        }
    }

    QDialog::eventFilter(o,e);
}
