#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "paramset.h"
#include "common.h"
#include "win_qextserialport.h"
#include <QProcess>
#include "hj212.h"
#include "factorinfo.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMap>
#include <QList>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QToolTip>
#include <QDir>

#include "msgbox.h"
#include "Crc16Class.h"

#include "chart.h"
#include <QtCharts/QChartView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QBrush>
#include "historychartview.h"
#include "historydataquery.h"
#include <QJsonDocument>
#include <QJsonObject>



QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class SerialWorker;
class ParamSet;
//class FactorInfo;

//class FactorInfo
//{
//public:
//    FactorInfo(QString name,QString value,QString state,QString unit,bool display,bool upload);
//    ~FactorInfo();

//    QString m_name;
//    QString m_value;
//    QString m_state;
//    QString m_unit;
//    bool m_display;
//    bool m_upload;
//    uint16_t m_Chan;
//    float m_RangeUpper;
//    float m_RangeLower;
//    float m_LC;
//};

class SerialWorker : public QObject
{
    Q_OBJECT

public:
    explicit SerialWorker(Win_QextSerialPort *ser,QObject *parent = nullptr);

    /* 打断线程（注意此方法不能放在槽函数下） */
    void stopWork() {
        // qDebug()<<"打断自动监控线程";

        /* 获取锁后，运行完成后即解锁 */
        QMutexLocker locker(&lock);
        isCanRun = false;
    }

    void VocsHandler();
    void UploadHandler1();
    void skybluework();
    void lunanwork();
    // void UploadHandler2();
    void getuploadstate();
    void writeinLog(QString);
    void setFacState(QString name,QString stateNote);



signals:
    /* 工人工作函数状态的信号 */
    void resultReady(const QString &result);
    void sendFluParams(QMap<QString,QString> &paramsMap);

public slots:
    void doWork1();
    void doWork2();
    void doWork3();
    void doWork4();
private:
    Win_QextSerialPort *serial = nullptr;
    /* 互斥锁 */
    QMutex lock;
    /* 标志位 */
    bool isCanRun;

    bool isUpLoadWet = true;
    bool isUpLoadDry = true;
    QMap<QString,QString> flusmap;

};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setTableHeader();
    void setTableContents();
    bool eventFilter(QObject *obj, QEvent *event);
    void Widget_Init();
    void Setting_Init();
    void Setting_Check(QString filename);
    void initLogger();
    void editLogger();
    void destroyLogger();
    void InitComm();
    void InitSysSetting();
    void InitFactorMaps();
    //重写定时器的事件   虚函数 子类重写父类的虚函数
    virtual void timerEvent(QTimerEvent *);

    bool OpenCom(Win_QextSerialPort *m_pSerialCom,QString pItemName,QString pItemBaud,QString pItemDataBit,QString pItemStopBit,QString pItemParity);
    void printFactors(QMap<QString,FactorInfo *>);
    int countFactordisplay(QMap<QString,FactorInfo *>);
    bool datebaseinit();
    void chartinit();
    void connectMarkers();
    void disconnectMarkers();

    QString queryFacCode(QString facName);



private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void on_pushButton_4_clicked();
    void HandleDateTimeout();
    void on_pushButton_Set_clicked();
    void handleResults(const QString & results);
    void handleMarkersClicked();
    void onReceiveFluParamsMap(QMap<QString,QString>&);

signals:
    /* 工人开始工作（做些耗时的操作 ） */
    void startWork1();
    void startWork2();
    void startWork3();
    void startWork4();
    void sendGlobalMapAndList(QStringList &g_FactorsNameList,QMap<QString, FactorInfo*> &map_Factors);

private:
    Ui::MainWindow *ui;
    QTimer *m_pDateTimer = nullptr;
    Logger *logger;
    QProcess my_Process;
    Chart *chart = nullptr;
    HistoryChartView *histoyChartView = nullptr;
    HistoryDataQuery *historyDateQuery = nullptr;

    int id1; //定时器1的唯一标示

    Win_QextSerialPort *m_pSerialCom1 = nullptr; // 声明串口
    QThread serialThread_1;
    SerialWorker *serialWorker1 = nullptr;

    Win_QextSerialPort *m_pSerialCom2 = nullptr; // 声明串口
    QThread serialThread_2;
    SerialWorker *serialWorker2 = nullptr;

    Win_QextSerialPort *m_pSerialCom3 = nullptr; // 声明串口
    QThread serialThread_3;
    SerialWorker *serialWorker3 = nullptr;

    Win_QextSerialPort *m_pSerialCom4 = nullptr; // 声明串口
    QThread serialThread_4;
    SerialWorker *serialWorker4 = nullptr;

    QSqlDatabase db;
    QStringList seqlist,nameseqlist;
    QList<FactorInfo *> facseqlist;


};
#endif // MAINWINDOW_H
