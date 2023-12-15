#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QTimer"
#include "QDateTime"
#include "QButtonGroup"
#include "QLabel"
#include <QMouseEvent>
#include <QTreeWidget>
#include "common.h"
#include "cjsonfile.h"
#include "QSignalMapper"
#include "QHBoxLayout"
#include "QTableWidgetItem"
#include "dialognewdev.h"
#include "dialogprotocol.h"
#include "cdevice.h"
#include "qfilesystemwatcher.h"
#include "cuploadsetting.h"
#include <QTcpServer>
#include <QTcpSocket>
#include "QTextCodec"
#include "hj212.h"
#include "win_qextserialport.h"
#include <QThread>
#include <QMutexLocker>
#include <QMutex>
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class SerialWorker;
class DialogNewDev;
class DialogProtocol;
class CDevice;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool OpenCom();

    void initLogger();
    void destroyLogger();

    void device_display();
    void factor_widget_clear();
    void device_widget_clear();
    void device_info_widget_clear();
    bool eventFilter(QObject *obj, QEvent *event);	// 添加时间过滤器声明
    void installEvents();
    void serialinfo_display(QJsonObject &pJsonRootObj);
    void setPackageTableHeader();
    void setPackageTableContents();
    void setDataTableHeader();
    void setDataTableContents();
    void setTableHeader();
    void setTableContents(QJsonObject &pJsonRootObj);
    void setUpTableContents(QJsonObject &pJsonRootObj);
    void setDeviceInit(QJsonObject &pJsonRootObj);
    void setUploadInit(QJsonObject &pJsonRootObj);
    void setUploadTableHeader();
    void radioButton_Init();
    void upload_init();
    void ProcessCtrlLogic_Init();
    void AutoManualMeasureThreadCallback();
    void FlushTipStatusTimerCallback();
    void factor_flag_init();
    void get_KeyValue(QJsonObject &pJsonRootObj,QString Key);
    bool ManualMode1(bool stat);
    bool ManualMode2(bool stat);
    void Reset();
    void store_work();
    void upload_work();
    void test_package_work();
    bool SendMeasureCmd(QString address,QString pType);
    bool ReadMeasureResult(CDevice* &pDevice);
    void WriteSingleCoil(QString pAddr,QString pRegAddr,QString pFunc,QString pSetVal);
    void ReadHoldingRegisters(QString pAddr,QString pRegAddr,uint16_t len,QString pRegs);
    bool WriteSingleRegister(QString pAddr,QString pRegAddr,QString pFunc,QString pSetVal);
    bool CommonReadRegister(QString byte_order,QString data_type, QString addr,QString func,QString regisAddr,QString mbLen,QString &pRealData);
    bool WriteMultiRegister(QString pAddr,QString pRegAddr,QString pFunc,QString pSetValList);

private slots:
    void heartbeat_work();
    void slots_RadioButton_Logger();
    void logSlot(const QString &message, int level);
    void HandleDateTimeout();
    void SetIndex(int id);
    void on_pushButtonSerialSaved_clicked();
    void btnClicked_DevEdit(QString pDevName);
    void btnClicked_DevDele(QString pDevName);
    void AutoManualModeTimerCallback();
    void btnClicked_UploadDele(QString pServerAddr);
    void newDev_ExitWin();
    void CheckNetworkStatus();
    void factor_display();

    /* 连接 */
    void toConnect();
    /* 断开连接 */
    void toDisConnect();
    /* 已连接 */
    void connected();
    /* 已断开连接 */
    void disconnected();
    /* 重新连接 */
    void reconnect();
    /* 接收到消息 */
    void receiveMessages();
    /* 发送消息 */
    void sendMessages(QTcpSocket* socket, QString message);
    /* 连接状态改变槽函数 */
    void socketStateChange(QAbstractSocket::SocketState state);
    /* 用于接收工人是否在工作的信号 */
    void handleResults(const QString &);

    void start_measure();
    void read_value();

    void on_pushButtonProtoDev_clicked();
    void on_pushButtonAddDev_clicked();
    void on_pushButtonFreshDev_clicked();
    void on_pushButtonUpReset_clicked();
    void on_pushButtonUpSaved_clicked();
    void on_pushButton_63_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_15_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_17_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_18_clicked();
    void on_pushButton_19_clicked();
    void on_pushButton_20_clicked();
    void on_pushButton_21_clicked();
    void on_pushButton_22_clicked();
    void on_pushButton_23_clicked();
    void on_pushButton_24_clicked();
    void on_pushButton_25_clicked();
    void on_pushButton_26_clicked();
    void on_pushButton_27_clicked();
    void on_pushButton_28_clicked();
    void on_pushButton_29_clicked();
    void on_pushButton_30_clicked();
    void on_pushButton_31_clicked();
    void on_pushButton_36_clicked();
    void on_pushButton_37_clicked();
    void on_pushButton_38_clicked();
    void on_pushButton_39_clicked();
    void on_pushButton_40_clicked();
    void on_pushButton_41_clicked();
    void on_pushButton_42_clicked();
    void on_pushButton_43_clicked();
    void on_pushButton_44_clicked();
    void on_pushButton_45_clicked();
    void on_pushButton_46_clicked();
    void on_pushButton_47_clicked();
    void on_pushButton_48_clicked();
    void on_pushButton_49_clicked();
    void on_pushButton_51_clicked();
    void on_pushButton_50_clicked();
    //void on_pushButton_62_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_54_clicked();
    void on_pushButton_W1_clicked();
    void on_pushButton_W2_clicked();
    void on_pushButton_W3_clicked();
    void on_pushButton_W4_clicked();
    void on_pushButton_W5_clicked();
    void on_pushButton_W6_clicked();
    void on_pushButton_W7_clicked();
    void on_pushButton_W8_clicked();
    void on_pushButton_W9_clicked();
    void on_pushButton_W10_clicked();
    void on_pushButton_W11_clicked();
    void on_pushButton_W12_clicked();
    void on_pushButton_W13_clicked();
    void on_pushButton_W14_clicked();
    void on_pushButton_W15_clicked();
    void on_pushButton_W16_clicked();
    void on_pushButton_W17_clicked();
    void on_pushButton_W18_clicked();
    void on_pushButton_W19_clicked();
    void on_pushButton_W20_clicked();
    void on_pushButton_W21_clicked();
    void on_pushButton_W21_1_clicked();
    void on_pushButton_W22_clicked();
    void on_pushButton_W23_clicked();
    void on_pushButton_W24_clicked();
    void on_pushButton_W25_clicked();
    void on_pushButton_W26_clicked();
    void on_pushButton_171_clicked();
    void on_pushButton_172_clicked();
    void on_pushButton_173_clicked();
    void on_pushButton_174_clicked();
    void on_pushButton_175_clicked();
    void on_pushButton_75_clicked();
    void on_pushButton_76_clicked();
    void on_pushButton_77_clicked();
    void on_pushButton_71_clicked();
    void on_pushButton_72_clicked();
    void on_pushButton_74_clicked();
    void on_pushButton_73_clicked();
    void on_pushButton_52_clicked();
    void on_pushButton_53_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_32_clicked();
    void on_pushButton_34_clicked();
    void on_pushButton_33_clicked();
    void on_pushButton_35_clicked();
    void on_pushButton_62_clicked();
    void on_pushButton_W3_2_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_55_clicked();
    void on_pushButton_56_clicked();
    void on_pushButton_57_clicked();
    void on_pushButton_58_clicked();

private:
    Ui::MainWindow *ui;

    QTimer *m_pDateTimer = nullptr;
    QTimer *m_AutoManulModeTimer = nullptr;
    QTimer *m_TimerHeartbeat = nullptr;
    QTimer *m_TimerCheck = nullptr;
    QButtonGroup *m_GroupMember = nullptr;
    QList<QLabel *> labelList;
    QSignalMapper *m_SignalMapperEdit = nullptr;
    QSignalMapper *m_SignalMapperDele = nullptr;
    QSignalMapper *m_SignalMapperUpDele = nullptr;
    DialogNewDev *m_DialogNewDev = nullptr;
    DialogProtocol *m_DialogProtoDev = nullptr;
    QMap<QString,QPushButton *> m_DevBtnEditMap;
    QMap<QString,QPushButton *> m_DevBtnDeleMap;
    QMap<QString,QPushButton *> m_UploadBtnDeleMap;

    QList<QWidget *> widget_list;
    QList<QLabel *> NAME_list;
    QList<QLabel *> DATA_list;

//    QList<QWidget *> DevWidget_list;
    QList<QLabel *> DevKey_list;
    QList<QLabel *> DevVal_list;
    QList<QPushButton *> DevButton_list;

    QButtonGroup *m_HeartBreak = nullptr;
    QButtonGroup *m_RealtimeUpload = nullptr;
    int m_UploadInternal = 3000;
    QString m_MN = HJ212_DEF_VAL_MN;

    QList<QHostAddress> IPlist;
    QList<qint32> Portlist;
    QList<QString>Passwordlist;
    /* 通信套接字 */
    QList<QTcpSocket *> SocketList;

    enum SystemModule {
        MODULE_JC = 0,
        MODULE_SB = 1,
        MODULE_SC = 2,
        MODULE_QX = 3,
        MODULE_SJ = 4,
        MODULE_RZ = 5,
        MODULE_KZ = 6,
        MODULE_MS = 7,
        MODULE_PLC = 8
    };

    /* 全局线程 */
    Win_QextSerialPort *m_pSerialCom = nullptr; // 声明串口
    QThread serialThread_1;
    SerialWorker *serialWorker = nullptr;
    QButtonGroup *groupButtonLog;
    Logger *logger;

    QString m_port = "";
    QString m_stop_bit;
    QString m_data_bit;
    QString m_baudrate;
    QString m_parity;

    QSqlDatabase db;

signals:
    /* 工人开始工作（做些耗时的操作 ） */
    void startWork();

};

class SerialWorker : public QObject
{
    Q_OBJECT

public:
    explicit SerialWorker(Win_QextSerialPort *ser,QObject *parent = nullptr);

    void ReadPLCWarningInfo(QString coil, QString pCoils);
    void ReadPLCProcessInfo(QString coil, QString pCoils);
    void ReadPLCProgramRunStep(QString reg, QString pRegs);
    void ReadPLCValveStat(QString coil, QString pCoils);
    bool CheckWarningSignalFromPLC();
    void ReadDeviceStatusInfo(CDevice* &pDevice);
    bool ReadMeasureResult(CDevice* &pDevice);
    void ReadDeviceWarningInfo(CDevice* &pDevice);
    bool SendMeasureCmd(QString address,QString pType);
    void StartStopRunMode(QString reg, bool onOff);
    void ReadWaterPressure(QString reg, QString pRegs);
    void SendMeasuredDoneSignalToPlc(QString reg, uint16_t val);
    void MeasureFive();

    /* 打断线程（注意此方法不能放在槽函数下） */
    void stopWork() {
        QLOG_INFO()<<"打断自动监控线程"<<endl;

        /* 获取锁后，运行完成后即解锁 */
        QMutexLocker locker(&lock);
        isCanRun = false;
    }

    void WriteSingleCoil(QString pAddr,QString pRegAddr,QString pFunc,QString pSetVal);
    void ReadHoldingRegisters(QString pAddr,QString pRegAddr,uint16_t len,QString pRegs);
    void ReadCoils(QString pAddr,QString coil,uint16_t len,QString pCoils);
    bool WriteSingleRegister(QString pAddr,QString pRegAddr,QString pFunc,QString pSetVal);
    bool CommonReadRegister(QString byte_order,QString data_type, QString addr,QString func,QString regisAddr,QString mbLen,QString &pRealData);
    bool WriteMultiRegister(QString pAddr,QString pRegAddr,QString pFunc,QString pSetValList);

signals:
    /* 工人工作函数状态的信号 */
    void resultReady(const QString &result);

public slots:
    void doWork1();
private:
    Win_QextSerialPort *serial;
    /* 互斥锁 */
    QMutex lock;
    /* 标志位 */
    bool isCanRun;
};


#endif // MAINWINDOW_H
