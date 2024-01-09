#ifndef DEVICECMDCTRLDLG_H
#define DEVICECMDCTRLDLG_H

#include <QDialog>
#include <QPushButton>
#include <QDebug>
#include <QLineEdit>
#include <QTextEdit>
#include <QCloseEvent>
#include <QString>
#include <QByteArray>
#include <QEvent>
#include <QProcess>
#include <QFile>
#include <QDateTime>
#include <QCheckBox>
#include <QList>
#include <QHBoxLayout>
#include <QColumnView>
#include <QTimer>

#ifdef Q_OS_LINUX
#include "util.h"
#include "uartThread/uartthread.h"
#endif
#include <unistd.h>
#include "httpclinet.h"

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
    void loadFactorAndPort();
    void connectevent();
    void timeCheckInit();

protected:
    bool eventFilter(QObject *obj = nullptr,QEvent *e = nullptr);

signals:
    void sendReback(bool);
    void sendCMD(QString cmd);
    void sendIsSendBytime(bool issendbytime);

public slots:
    #ifdef Q_OS_LINUX
    void onReceiveCMD(QString cmd);
    #endif
    void onReceivecurDT(QDateTime &);

private:
    Ui::DeviceCMDCtrlDlg *ui;
#ifdef Q_OS_LINUX
    UartThread *muartThread=nullptr;//串口线程对象
#endif
    QString str;
    QString comstr;
    Util util;
    QMap<QString,QStringList> map;
    QMap<QString,QString> namemap;
    QList<QCheckBox *> timecks;
    QStringList ontimecks;
    QDateTime curDT;
    bool isSendBytime = false;
    QTimer timer;

};

#endif // DEVICECMDCTRLDLG_H
