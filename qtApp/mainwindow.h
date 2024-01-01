#ifndef MAINWINDOW_H
#define MAINWINDOW_H



#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include <QListWidget>
#include <QListWidgetItem>
#include <QThread>
#include <QStackedWidget>
#include <QString>
#include <QDateTime>
#include <QEvent>
#include <QMouseEvent>
#include <QProcess>
#include <QMutexLocker>
#include <QMutex>
#include <QGridLayout>
#include <QTimerEvent>
#include <QVBoxLayout>
#include <QDialog>
#include <QList>
#include <QLayoutItem>
#include <QMessageBox>
#include <QGuiApplication>
#include <QCursor>
#include <QDesktopWidget>
#include <QApplication>
#include <QMap>
#include <QThread>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMetaType>
#include <QMetaObject>

#include "common.h"
#include "facpanel.h"
#include "appbutton.h"
#include "devicecmdctrldlg.h"
#include "dataquery.h"
#include "deviceset.h"
#include "updeviceset.h"
#include "httpclient.h"




QT_BEGIN_NAMESPACE
namespace Ui { class MainWIndow; }
QT_END_NAMESPACE

class MainWIndow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWIndow(QWidget *parent = nullptr);
    ~MainWIndow();

    /* 打断线程（注意此方法不能放在槽函数下） */
    void stopWork() {
        // qDebug()<<"打断自动监控线程";

        /* 获取锁后，运行完成后即解锁 */
        QMutexLocker locker(&lock);
        isCanRun = false;
    }

    void widgetInit();//mainwindow init
    void connectevent();
    void addApp(int row,int col,QString name="",QString iconpath="");
    void addLocalApp();
//    void loadAppDlg(QDialog *dlg=nullptr);
    void loadAppDlg(QWidget *w=nullptr);
    void deleteApp();    

    QJsonObject get_rtk_data();
    void autoGetData();
    void rtktableInit();
    void setRtkPanelContent();

    QJsonObject get_connect_stat();

protected:
    bool eventFilter(QObject *obj = nullptr,QEvent *e = nullptr);
    void timerEvent(QTimerEvent *event) override;

signals:
    void appClicked(QString);
    void resultReady(const QString &result);
    void startWork();
    void sendFacPanel(QString str);



public slots:
    void startApp(QString name);
    void handleResults(const QString & results);
    void doWork();

private:
    Ui::MainWIndow *ui;
    /* 互斥锁 */
    QMutex lock;
    /* 标志位 */
    bool isCanRun;
    QGridLayout layout;
    QVBoxLayout appLayout;
    int id1;
    bool isLogin = false;
    DeviceCMDCtrlDlg *deviceCMDCtrlDlg = nullptr;
//    DeviceSet *deviceSet = nullptr;
//    UpDeviceSet *upDeviceSet = nullptr;
//    DataQuery *dataQuery = nullptr;

    QThread pThread;
    HttpClient httpClient;

    QString curAppName;
    QMap<QString,facPanel*> map;

};
#endif // MAINWINDOW_H
