#include "devicecmdctrldlg.h"
#include "ui_devicecmdctrldlg.h"

DeviceCMDCtrlDlg::DeviceCMDCtrlDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceCMDCtrlDlg)
{
    ui->setupUi(this);
}

DeviceCMDCtrlDlg::~DeviceCMDCtrlDlg()
{
    delete ui;
}
