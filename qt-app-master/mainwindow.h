#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutexLocker>
#include <QMutex>
#include <QThread>
#include <QMouseEvent>
#include "common.h"
#include "httpclinet.h"
#include <string>
#include <iostream>
#include <functional>
#include <QButtonGroup>
#include "devadd.h"
#include "factoradd.h"
#include "dialogproto.h"
#include "upsetadd.h"
#include "teshuzhiadd.h"
#include "teshuzhidele.h"
#include <QSignalMapper>
#include "dialogdevprop.h"
#include "devedit.h"
#include "facedit.h"
#include "devicecmdctrldlg.h"
#include <QPointer>
#include <QProcess>
#include <QJsonValue>
#include <QFileInfoList>
#include <QDir>
#include "usbupdatedlg.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

typedef QMap<QString, QString> mapStrString;

class DevEdit;
class CHttpWork;
class DevDele;
class DevAdd;
class FactorDele;
class FactorAdd;
class UpsetDele;
class UpsetAdd;
class TeshuzhiAdd;
class TeshuzhiDele;
class DialogDevProp;
class DeviceCMDCtrlDlg;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum GET_TYPE
    {
        GET_DEVICE = 0,
        GET_FACTORS,
        GET_SPECIALS
    };

    bool FactorGui_Init(QString pDev_ID);
    void setTableFaHeader();
    bool DevGui_Init();
    void setTableDevHeader();
    void setTableUpHeader();
    bool UploadGui_Init();
    void setTableTeHeader();
    QJsonObject Conf_RootObjGet();
    void Analog_RenewDevParam();
    void ConfFactor_Filled(QString pTemp,bool isAnalogProtocl);
    bool TeshuzhiGui_Init();
    void loadTesshuzhiLocal();
    bool Conf_TeshuzhiUpdate();
    void ConfNode_Init();
    void ConfFile_Init();
    void Widget_Init();
    // void Teshuzhi_EditInit(bool ok);
    void UpSetting_EditInit(bool ok);
    void setTableContents(const QJsonObject &pJsonObj,const QJsonObject & pConfJsonTeList);
    void delfactor(int differ, const QJsonObject & results,const QJsonObject & pConfJsonTeList);
    void addfactor( int differ, const QJsonObject & results,const QJsonObject & pConfJsonTeList);
    void label_clear();
    void qingqiu(int page);
    void setTableHeader();
    void setTableContents(QJsonArray &history_real_time_data);
    void qingqiuHisData(int page, QString type);
    void setHisTableContents(QJsonArray &history_real_time_data);
    void qingqiuMessage(int page);
    void setMsgTableHeader();
    void setMsgTableContents(QJsonArray &history_real_time_data);

    bool eventFilter(QObject *obj, QEvent *event);	// 添加时间过滤器声明
    void installEvents();

    void deletecurrid(QString deviceid,QString facname);
    void writeDevParams();
    QJsonObject loadlocalJson(int gettype,QString dev_id);
    void usbUpdateEvent();
    bool checkUSBDevice();


private slots:

    void refresh_AnalogDevParam();
    void refresh_SpecialsDevParam();
    void onButtonDevDele(QString id);
    void onButtonDevMore(QString id);
    void onButtonDevFactor(QString id);
    void handleDateTimeout();
    void handleResults(QString item,const QJsonObject &results);
    void ClearTable();
    void ShowNextPage();
    void ShowNewPage();
    void ShowLastPage();

    void OpenSysInfo();
    void OpenNeworkSetting();
    void OpenComDebug();
    void OpenBufa();
    void OpenMasterSet();
    void OpenTimeSet();
    void OpenData_Query();
    void OpenDev_Setting();
    void OpenTeshuzhi();

    void onButtonTeDele(QString pFactor);
    void onButtonTeSaved(QString pFactor);
    void on_login_clicked();
    void on_main_clicked();
    void on_data_clicked();
    void on_Login_clicked();
    void on_Return_clicked();
    void on_pushButtonQuery_clicked();
    void on_pushButtonModify_clicked();
    void on_confirmBtn_clicked();
    void on_pushButton_Bufa_clicked();
    void on_pushButton_setlocal_clicked();
    void on_pushButton_setself_clicked();
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButtonFind_clicked();
    void on_pushButtonExport_clicked();
    void on_pushButton_AddDev_clicked();
    void on_pushButton_Addf_clicked();
    void on_pushButton_Updatef_clicked();
    void on_pushButton_AddFresh_clicked();
    void on_pushButton_AddT_clicked();
    void on_pushButton_UpdateT_clicked();
    void onButtonDevParam(QString pFactorKey);
    void onButtonUpDele(QString ipaddr);
    void onButtonUpSaved(QString ipaddr);
    void on_pushButton_ReturnDev_clicked();

    void onButtonFaEdit(QString id);
    void onButtonFaDele(QString id);

    void onReceiveDeviceCMDCtrl();
    void openModbus();

signals:
    /* 工人开始工作（做些耗时的操作 ） */
    void startWork();
    void sendCurDT(QDateTime &);
    void sendUSBState(bool);

private:
    Ui::MainWindow *ui;
    QSignalMapper *m_SignalMapper = nullptr;
    QSignalMapper *m_SignalMapper_Te = nullptr;
    QSignalMapper *m_SignalMapper_Ted = nullptr;
    QSignalMapper *m_SignalMapper_Up = nullptr;
    QSignalMapper *m_SignalMapper_Upd = nullptr;
    QSignalMapper *m_SignalMapper_DevM = nullptr;   // more
    QSignalMapper *m_SignalMapper_DevF = nullptr;   // factor
    QSignalMapper *m_SignalMapper_DevD = nullptr;   // dele

    QSignalMapper *m_SignalMapper_FaEdit = nullptr;
    QSignalMapper *m_SignalMapper_FaDele = nullptr;

    bool m_LoginStatus = false;
    QTimer *m_pDateTimer = nullptr;
    QButtonGroup *hexGroup = nullptr;

    int m_CurPage;
    int m_TotalPage;
    QJsonArray m_history_rtd;
    QJsonArray m_history_data;
    QJsonArray m_history_message;

    QThread m_thread;
    CHttpWork *m_httpWorker = nullptr;
    QJsonArray m_JsonArray;
    QJsonObject m_Json_DevFactor;
    QJsonObject m_JsonProto;

    QList<QWidget *> widget_list;
    QList<QLabel *> NAME_list;
    QList<QLabel *> DATA_list;
    QList<QLabel *> UINT_list;
    QList<QLabel *> CODE_list;
    QList<QPushButton *> BUTTON_list;

    QList<QLabel *> NO_list;
    QList<QLabel *> ADDR_list;
    QList<QLabel *> STATE_list;

    QList<QLabel *> labelList;
    QList<std::function<void()>> funcList;
    DeviceCMDCtrlDlg* devicecmddlg = nullptr;
    Util util;
    QMap<QString,QStringList> map;
    QMap<QString,QString> namemap;
    QMap<QString,QString> facnamemap;
    QMap<QString,QString> specialsMap;
    bool isUsbOn = false;
    USBUpdateDlg *usbdlg = nullptr;

};

class CHttpWork : public QObject
{
    Q_OBJECT

public:
    // explicit CHttpWork(QObject *parent = nullptr);

    /* 打断线程（注意此方法不能放在槽函数下） */
    void stopWork() {
        qDebug()<<"打断自动监控线程";

        /* 获取锁后，运行完成后即解锁 */
        QMutexLocker locker(&lock);
        isCanRun = false;
    }

signals:
    /* 工人工作函数状态的信号 */
    void resultReady(QString item,const QJsonObject &result);

public slots:
    void doWork1();
private:
    /* 互斥锁 */
    QMutex lock;
    /* 标志位 */
    bool isCanRun;
};

#endif // MAINWINDOW_H
