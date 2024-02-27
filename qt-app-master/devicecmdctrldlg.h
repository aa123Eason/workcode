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
#include <QMessageBox>
#include <QEvent>
#include <QGridLayout>

//#ifdef Q_OS_LINUX
#include "util.h"
//#include "uartThread/uartthread.h"
//#endif
#include <unistd.h>
#include "httpclinet.h"
#include "serialport.h"
#include <QStackedWidget>
#include "common.h"
#include <QFile>
#include <QDir>



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
    void timeCheckInit(QGridLayout &timeset,QWidget *w = nullptr);
    void writeloglocal(QString);
    void loadtmcksState();

protected:
    bool eventFilter(QObject *obj = nullptr,QEvent *e = nullptr);

signals:
    void sendReback(bool);
    void sendCMD(QString cmd);
    void sendIsSendBytime(bool issendbytime);
    void sendloopread(bool);

public slots:
//    #ifdef Q_OS_LINUX
    void onReceiveCMD(QString cmd);
//    #endif
    void onReceivecurDT(QDateTime &);
    void onTimeout();
    void onStackedCurrentChanged(int index);
    void onSaveTimeset();
    void onResetTimeset();
    void onSendByTime();

private:
    Ui::DeviceCMDCtrlDlg *ui;
//#ifdef Q_OS_LINUX
//    UartThread *muartThread=nullptr;//串口线程对象
//#endif
    QString str;
    QString comstr;
    Util util;
    QMap<QString,QStringList> map;
    QMap<QString,QString> namemap;
    QList<QCheckBox *> timecks;
    QList<QCheckBox *> typecks;
    QStringList ontimecks;
    QDateTime curDT;
    bool isSendBytime = false;
    QTimer timer;
    bool isLoopOn = false;
    SerialPort *serialPort = nullptr;
    QGridLayout l1,l2,l3,l4;
    QStringList list;
//    localKeyboard *kb = nullptr;


};

#endif // DEVICECMDCTRLDLG_H
