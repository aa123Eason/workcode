#include "mainwindow.h"
#include "ui_mainwindow.h"

QString curAppName;

MainWIndow::MainWIndow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWIndow)
{
    ui->setupUi(this);
    widgetInit();
    connectevent();
    addLocalApp();

    id1 = startTimer(1000);
}

MainWIndow::~MainWIndow()
{
    delete ui;
}

void MainWIndow::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == id1)
    {

        ui->realtime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    }
}

void MainWIndow::widgetInit()//mainwindow init
{
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnBottomHint);
    this->setWindowState(Qt::WindowMaximized);
    ui->mainpanel->setCurrentIndex(0);
    ui->realtime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
    QJsonObject jObj = get_rtk_data();
    QString str;
    if(map.count()>0)
    {
        str = "已检测到"+QString::number(map.count())+"个因子";
        ui->state_rtkfacs->setText(str);

    }
    else
    {
        str = "未检测到因子";
        ui->state_rtkfacs->setText(str);
    }

    ui->realtime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    ui->userNameEdit->installEventFilter(this);
    ui->pwdEdit->installEventFilter(this);

}

QJsonObject MainWIndow::get_rtk_data()
{
    QJsonObject jObj;
    HttpClient httpClient;
    httpClient.getdata("/dcm/realtime_data",jObj);
    qDebug()<<__LINE__<<jObj<<endl;
    QJsonObject::iterator it = jObj.begin();
    while(it!=jObj.end())
    {
        QJsonObject subObj = it.value().toObject();
        QString code = subObj.value("FactorCode").toString();
        QString value = subObj.value("value").toString();
        QString stateNote = subObj.value("Flag").toString();
        facPanel *facs = new facPanel();
        facs->setcode(code);
        facs->setname("");
        facs->setvalue(value);
        facs->setstate(stateNote);
        facs->setcode("");
        map.insert(code,facs);
        it++;
    }

    QMap<QString,facPanel*>::iterator it0 = map.begin();
    int row = 0,col = 0;
    while(it0!=map.end())
    {
        ui->rtklayout->addWidget(it0.value(),row,col++);
        if(col >5)
        {
            col = 0;
        }
        row++;
        it0++;
    }


    return jObj;
}

void MainWIndow::connectevent()
{
    connect(ui->close_btn,&QPushButton::clicked,this,&MainWIndow::close);
    connect(ui->rtk_btn,&QPushButton::clicked,this,[=]()
    {
        ui->mainpanel->setCurrentIndex(0);
    });
    connect(ui->main_btn,&QPushButton::clicked,this,[=]()
    {
        if(isLogin||ui->login->text() == "退出登录")
        {
            ui->mainpanel->setCurrentIndex(1);
        }
        else
        {
            QMessageBox::warning(this,"提示","未登录!");
            ui->mainpanel->setCurrentIndex(2);
        }
    });
    connect(ui->login,&QPushButton::clicked,this,[=]()
    {
        if(ui->login->text() == "登录")
            ui->mainpanel->setCurrentIndex(2);
        else if(ui->login->text() == "退出登录")
        {
            if(QMessageBox::Yes == QMessageBox::question(this,"提示","确定退出登录?"))
            {
                ui->mainpanel->setCurrentIndex(0);
                ui->login->setText("登录");
            }
        }
    });

    connect(ui->mainpanel,&QStackedWidget::currentChanged,this,[=](int index)
    {
        switch(index)
        {
        case 0:
        {
            qDebug()<<__LINE__<<"实时数据"<<endl;
            break;
        }
        case 1:
        {
            qDebug()<<__LINE__<<"主界面"<<endl;

            break;
        }
        case 2:
        {
            qDebug()<<__LINE__<<"登录界面"<<endl;

            break;
        }
        case 3:
        {
            if(!curAppName.isEmpty())
            {
                qDebug()<<__LINE__<<curAppName+"界面"<<endl;
            }
            break;
        }
        default:
            break;
        }
    });

    connect(this,&MainWIndow::appClicked,this,&MainWIndow::startApp);

    connect(ui->defaultinfo,&QPushButton::clicked,this,[=]()
    {
        ui->userNameEdit->setText("admin");
        ui->pwdEdit->setText("lcdcm");
    });

    connect(ui->btn_Login,&QPushButton::clicked,this,[=]()
    {

        if(ui->userNameEdit->text() == "admin" && ui->pwdEdit->text() == "lcdcm")
        {
             isLogin = true;
        }
        else
        {
            isLogin = false;
        }

        if(isLogin)
        {
            QMessageBox::about(this,"提示","登录成功!");
            ui->mainpanel->setCurrentIndex(1);
            ui->login->setText("退出登录");

        }
        else
        {
            QMessageBox::warning(this,"提示","用户名或密码不正确，登录失败!");
        }
    });

    connect(ui->btn_Cancel,&QPushButton::clicked,this,[=]()
    {
        ui->userNameEdit->clear();
        ui->pwdEdit->clear();
    });

    connect(ui->openKeyboard,&QPushButton::clicked,this,[=]()
    {
        qDebug()<<"open keyboard"<<endl;
        QProcess::execute("florence");
    });
}

void MainWIndow::addApp(int row,int col,QString name,QString iconpath)
{
//    QDesktopWidget* pDesktopWidget = QApplication::desktop();
//    //获取可用桌面大小
//    QRect deskRect = QApplication::desktop()->availableGeometry();

    AppButton *btn = new AppButton(name,iconpath,this);
    qreal w = ui->page_main->layout()->geometry().width()/5.0-6;
    qreal h = w * 0.618;
    w= qRound64(w);
    h = qRound64(h);
    btn->setSize(w,h);
    layout.addWidget(btn,row,col,Qt::AlignCenter);
    btn->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    layout.setRowStretch(row,2);
    layout.setColumnStretch(col,2);
    layout.setRowMinimumHeight(row, 200);
    layout.setColumnMinimumWidth(col, 130);

    btn->installEventFilter(this);


}

bool MainWIndow::eventFilter(QObject *obj,QEvent *e)
{
    if(obj == nullptr || e == nullptr)
        return false;
    if(obj->objectName() == "AppButton")
    {
        AppButton *btn = (AppButton *)obj;
        if(e->type() == QEvent::HoverEnter)
        {
            btn->setToolTip(btn->appName);
        }
        else if(e->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *me = (QMouseEvent *)e;
            if(me->button() == Qt::LeftButton)
            {
                curAppName = btn->appName;
                qDebug()<<__LINE__<<"打开"<<btn->appName<<endl;
                emit appClicked(curAppName);
            }
        }
        else if(obj == ui->userNameEdit||obj == ui->pwdEdit)
        {
            if(e->type() == QEvent::MouseButtonPress)
            {
                QMouseEvent *me = (QMouseEvent *)e;
                if(me->button() == Qt::LeftButton)
                {
                    qDebug()<<"open keyboard"<<endl;
                    QProcess::execute("florence");
                }
            }
        }
    }

    return QWidget::eventFilter(obj,e);
}

void MainWIndow::startApp(QString name)
{

    if(name == "设备指令控制")
    {
        ui->mainpanel->setCurrentIndex(3);
        deviceCMDCtrlDlg = new DeviceCMDCtrlDlg();
        loadAppDlg(deviceCMDCtrlDlg);

        connect(deviceCMDCtrlDlg,&DeviceCMDCtrlDlg::sendReback,this,[=](bool flag)
        {
            appLayout.removeWidget(deviceCMDCtrlDlg);
            deviceCMDCtrlDlg->deleteLater();
            ui->mainpanel->setCurrentIndex(1);
        });
    }
    else if(name == "采集设备设置")
    {
        ui->mainpanel->setCurrentIndex(3);
        DeviceSet *deviceSet = new DeviceSet();
        loadAppDlg(deviceSet);
    }
    else if(name == "上位机设置")
    {
        ui->mainpanel->setCurrentIndex(3);
        UpDeviceSet *upDeviceSet = new UpDeviceSet();
        loadAppDlg(upDeviceSet);
    }
    else if(name == "数据查询")
    {
        ui->mainpanel->setCurrentIndex(3);
        DataQuery *dataQuery = new DataQuery();
        loadAppDlg(dataQuery);
    }
    else
    {
        //提示，应用尚未开发，敬请期待……
        deleteApp();
        QMessageBox::warning(this,"提示",name+"尚未开发，敬请期待……");
    }
}

void MainWIndow::deleteApp()
{
    //删除其他窗口
    if(QLayoutItem *it = appLayout.takeAt(0))
    {
        delete it->widget();
        delete it;
    }

}

void MainWIndow::addLocalApp()
{
    layout.setSizeConstraint(QLayout::SetMaximumSize);
    ui->appFrame->setLayout(&layout);

    addApp(0,0,"系统信息");
    addApp(0,1,"网络设置");
    addApp(0,2,"采集设备设置",":/new/images/image/device.png");
    addApp(0,3,"上位机设置",":/new/images/image/swj.png");
    addApp(0,4,"数据查询",":/new/images/image/query.png");
    addApp(1,0,"本地串口调试");
    addApp(1,1,"系统时间设置");
    addApp(1,2,"补发数据");
    addApp(1,3,"特殊值配置");
    addApp(1,4,"串口输出配置");
    addApp(2,0,"系统更新",":/new/images/image/restart.png");
    addApp(2,1,"设备指令控制",":/new/images/image/xtsj.png");


}

//void MainWIndow::loadAppDlg(QDialog *dlg)
//{
//    if(dlg!=nullptr)
//    {
//        //删除其他窗口
//        deleteApp();
//        //添加目标窗口
//        appLayout.addWidget(dlg,1,Qt::AlignCenter);
//        appLayout.setSizeConstraint(QLayout::SetMaximumSize);
//        ui->page_appshow->setLayout(&appLayout);
//    }

//}

void MainWIndow::loadAppDlg(QWidget *w)
{
    if(w!=nullptr)
    {
        //删除其他窗口
        deleteApp();
        //添加目标窗口
        appLayout.addWidget(w,1,Qt::AlignCenter);
//        appLayout.setColumnMinimumWidth(0,1000);
//        appLayout.SetSpacing(1);
        appLayout.setSizeConstraint(QLayout::SetDefaultConstraint);
        ui->page_appshow->setLayout(&appLayout);
    }

}
