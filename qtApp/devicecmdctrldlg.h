#ifndef DEVICECMDCTRLDLG_H
#define DEVICECMDCTRLDLG_H

#include <QDialog>
#include <QPushButton>
#include <QDebug>
#include <QLineEdit>
#include <QTextEdit>
#include <QCloseEvent>

namespace Ui {
class DeviceCMDCtrlDlg;
}

class DeviceCMDCtrlDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceCMDCtrlDlg(QWidget *parent = nullptr);
    ~DeviceCMDCtrlDlg();

    void init();
    void connectevent();

signals:
    void sendReback(bool);
    void sendCMD(QString cmd);

public slots:
    void onReceiveCMD(QString cmd);

private:
    Ui::DeviceCMDCtrlDlg *ui;
};

#endif // DEVICECMDCTRLDLG_H
