#ifndef DEVICECMDCTRLDLG_H
#define DEVICECMDCTRLDLG_H

#include <QDialog>
#include <QPushButton>
#include <QDebug>
#include <QLineEdit>
#include <QTextEdit>
#include <QCloseEvent>
#include <QSerialPort>
#include <QString>
#include <QByteArray>
#include <QEvent>
#include <QProcess>

#include "usbutility.h"

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

protected:
    bool eventFilter(QObject *obj = nullptr,QEvent *e = nullptr);

signals:
    void sendReback(bool);
    void sendCMD(QString cmd);

public slots:
    void onReceiveCMD(QString cmd);


private:
    Ui::DeviceCMDCtrlDlg *ui;
    QSerialPort *serial = nullptr;
    QString str;
};

#endif // DEVICECMDCTRLDLG_H
