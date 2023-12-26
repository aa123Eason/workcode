#ifndef DEVICECMDCTRLDLG_H
#define DEVICECMDCTRLDLG_H

#include <QDialog>

namespace Ui {
class DeviceCMDCtrlDlg;
}

class DeviceCMDCtrlDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceCMDCtrlDlg(QWidget *parent = nullptr);
    ~DeviceCMDCtrlDlg();

private:
    Ui::DeviceCMDCtrlDlg *ui;
};

#endif // DEVICECMDCTRLDLG_H
