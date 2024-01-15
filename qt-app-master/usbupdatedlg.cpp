#include "usbupdatedlg.h"
#include "ui_usbupdatedlg.h"

USBUpdateDlg::USBUpdateDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::USBUpdateDlg)
{
    ui->setupUi(this);
    init();


}

USBUpdateDlg::~USBUpdateDlg()
{
    delete ui;
}

 void USBUpdateDlg::init()
 {
     connectevent();
     loadUSBUpdateDemo();

     ui->filesCB->setCurrentIndex(0);
 }


void USBUpdateDlg::loadUSBUpdateDemo()
{

    QString dirpath = "/media/rpdzkj";
    QDir dir(dirpath);
    qDebug()<<__LINE__<<__FUNCTION__<<dirpath<<endl;
    if(!dir.exists())
    {
        return;
    }

    QStringList filters;
    filters<<QString("*.zip");

    QDirIterator dir_iterator(dirpath,filters,QDir::AllEntries|QDir::NoSymLinks|QDir::NoDotAndDotDot
                              ,QDirIterator::Subdirectories);
    QStringList filelist;
    while(dir_iterator.hasNext())
    {
        dir_iterator.next();
        QFileInfo file_info = dir_iterator.fileInfo();
        QString ab_path = file_info.absoluteFilePath();
        qDebug()<<__LINE__<<ab_path<<"|"<<file_info.baseName()<<endl;
        if(file_info.baseName()=="demo");
        {
            filelist.append(ab_path);
        }


    }
    qDebug()<<__LINE__<<__FUNCTION__<<filelist<<endl;
    ui->filesCB->clear();
    for(int k = 0;k<filelist.count();++k)
    {
        if(filelist[k].contains("demo.zip"))
        {
            ui->filesCB->addItem(filelist[k]);
        }
    }






}

void USBUpdateDlg::connectevent()
{
    connect(ui->btn_close,&QPushButton::clicked,this,&USBUpdateDlg::close);
    connect(ui->btn_cancel,&QPushButton::clicked,this,&USBUpdateDlg::close);
    connect(ui->btn_reset,&QPushButton::clicked,this,&USBUpdateDlg::loadUSBUpdateDemo);
    connect(ui->filesCB,&QComboBox::currentTextChanged,this,&USBUpdateDlg::on_comboBox_currentitem_changed);
    connect(ui->btn_update,&QPushButton::clicked,this,&USBUpdateDlg::on_confirm_update);
}

void USBUpdateDlg::on_comboBox_currentitem_changed(const QString &text)
{
    QString ab_path = ui->filesCB->currentText();
    if(ab_path.isEmpty())
    {
        return ;
    }

    QFileInfo info(ab_path);
    if(!info.exists())
    {
        return ;
    }

    QString str;
    //文件名，文件路径，修改时间，文件大小
    str += "文件名:"+info.fileName()+"\r\n";
    str += "文件大小:"+QString::number(info.size())+"\r\n";
    str += "文件路径:"+info.absoluteFilePath()+"\r\n";
    str += "修改时间:"+info.lastModified().toString("yyyy-MM-dd hh:mm:ss");
    ui->infoText->setText(str);
}

void USBUpdateDlg::on_confirm_update()
{
    QString curFile = ui->filesCB->currentText();
    if(curFile.isEmpty())
    {
        return;
    }

    QProcess process;
    QString cmd1 = "rm -rf /etc/opt/lchj212/demo";
    QString cmd2 = "unzip "+curFile+" -d /etc/opt/lchj212/";
    process.start(cmd1);
    process.start(cmd2);

    QFileInfo fi("/etc/opt/lchj212/demo");
    if(fi.exists())
    {
        //提示，更新完成，是否重启
        QString str0 = "更新完成!\r\n";
        str0 += "文件名:"+fi.fileName()+"\r\n";
        str0 += "文件大小:"+QString::number(fi.size())+"\r\n";
        str0 += "文件路径:"+fi.absoluteFilePath()+"\r\n";
        str0 += "修改时间:"+fi.lastModified().toString("yyyy-MM-dd hh:mm:ss");
        str0 += "是否重启?(Y/N)";
        if(QMessageBox::Yes == QMessageBox::information(this,"提示",str0,QMessageBox::Yes))
        {
            process.start("reboot");

        }


    }

    process.close();
}
