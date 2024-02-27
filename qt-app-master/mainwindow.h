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
#include <QHBoxLayout>
#include "addrcdevice.h"
#include "editrcdevice.h"
#include "serialport.h"
#include <QMap>
#include <QListWidget>
#include <QListWidgetItem>
#include "rclogdlg.h"
#include "xlsxworksheet.h"
#include "xlsxformat.h"
#include "xlsxdocument.h"
#include <QFileDialog>
#include "selectdtdlg.h"


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
class AddRCDevice;
class RCLogDlg;
class SelectDTDlg;

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

    enum RC_RWSTATE
    {
        RC_WRITE = false,
        RC_READ = true
    };

    bool FactorGui_Init(QString pDev_ID);//因子配置初始化
    void setTableFaHeader();//绘制因子表格的表头
    bool DevGui_Init();//设备配置初始化
    void setTableDevHeader();//绘制设备配置表格的表头
    void setTableUpHeader();//绘制上位机配置表格的表头
    bool UploadGui_Init();//上位机设置初始化
    void setTableTeHeader();//绘制特殊值配置表格
    QJsonObject Conf_RootObjGet();//获取QDesktop.json文件中的内容
    void Analog_RenewDevParam();//更新模拟量的配置参数
    void ConfFactor_Filled(QString pTemp,bool isAnalogProtocl);
    bool TeshuzhiGui_Init();//特殊值初始化
    void loadTesshuzhiLocal();//将配置好的特殊值填入表格
    bool Conf_TeshuzhiUpdate();//特殊值更新
    void ConfNode_Init();//QDesktop.json文件初始化
    void ConfFile_Init();//QDesktop.json文件初始化
    void Widget_Init();//主界面主要控件初始化
    // void Teshuzhi_EditInit(bool ok);
    void UpSetting_EditInit(bool ok);//上位机设置初始化
    void setTableContents(const QJsonObject &pJsonObj,const QJsonObject & pConfJsonTeList);//实时数据显示界面内容填充
    void delfactor(int differ, const QJsonObject & results,const QJsonObject & pConfJsonTeList);//删除因子
    void addfactor( int differ, const QJsonObject & results,const QJsonObject & pConfJsonTeList);//添加因子
    void label_clear();//软件关闭后，将控件List中的所有成员清除
    void qingqiu(int page);//数据查询：实时数据查询
    void setTableHeader();//数据查询:实时数据查询表格，表头绘制
    void setTableContents(QJsonArray &history_real_time_data);//数据查询:实时数据查询表格，查询结果填充
    void qingqiuHisData(int page, QString type);//数据查询:查询分钟、小时、日、月历史数据
    void setHisTableContents(QJsonArray &history_real_time_data);//数据查询:分钟、小时、日、月历史数据查询结果填充
    void qingqiuMessage(int page);//数据查询：传输报文查询
    void setMsgTableHeader();//数据查询：传输报文表格表头绘制
    void setMsgTableContents(QJsonArray &history_real_time_data);//数据查询：传输报文查询结果填充

    bool eventFilter(QObject *obj, QEvent *event);	// 添加时间过滤器声明
    void installEvents();//配置事件过滤器初始化参数

    void deletecurrid(QString deviceid,QString facname);//删除当前因子
    void buildLocalJson();
    void writeDevParams();//将模拟量设备参数的值记入本地json文件
    QJsonObject loadlocalJson(int gettype,QString dev_id);//从本地json文件在读取设备参数、因子和特殊值
    void usbUpdateEvent();//数采仪软件的U盘自动更新函数
    bool checkUSBDevice();//检测是否有U盘插入
    void showModifiedTime();//显示最近一般数采仪软件的更新时间
    void setDeviceTableHeader();//绘制设备表格表头
    void setDeviceTableContent();//将设备参数填入表格
    void checkRCCOMSTate(QMap<QString,bool> &,const QJsonObject &);//检测所有设备反控端口是否已连接
    void addRCPorts();//添加反控参数
    void rcReadWrite();//反控设备的数据读写信号控制
    void exportData(int curPage);

private slots:

    void refresh_AnalogDevParam();//更新模拟量参数1
    void refresh_SpecialsDevParam();//更新模拟量参数2
    void onButtonDevDele(QString id);//删除设备
    void onButtonDevMore(QString id);//编辑设备
    void onButtonDevFactor(QString id);//查看设备因子
    void handleDateTimeout();//实时更新时间
    void handleResults(QString item,const QJsonObject &results);//自动运行线程：读取并展示实时因子数据，连接状态和反控状态，自动执行反控命令收发
    void ClearTable();//数据查询：勾选不同的复选框，指定哪些控件显示，哪些控件隐藏
    void ShowNextPage();//数据查询：显示下一页
    void ShowNewPage();//数据查询：显示指定页
    void ShowLastPage();//数据查询:显示上一页
    void onButtonDele(QString id);//删除反控设备
    void onButtonEdit(QString id);//编辑反控设备

    void OpenSysInfo();//打开系统配置
    void OpenNeworkSetting();//打开网络设置
    void OpenComDebug();//打开串口调试
    void OpenBufa();//打开数据补发
    void OpenMasterSet();//打开上位机设置
    void OpenTimeSet();//打开时间设置
    void OpenData_Query();//打开数据查询
    void OpenDev_Setting();//打开设备配置
    void OpenTeshuzhi();//打开特殊值

    void onButtonTeDele(QString pFactor);//特殊值删除
    void onButtonTeSaved(QString pFactor);//特殊值保存
    void on_login_clicked();//用户登录1
    void on_main_clicked();//主界面
    void on_data_clicked();//实时数据
    void on_Login_clicked();//用户登录2
    void on_Return_clicked();//返回实时数据界面
    void on_pushButtonQuery_clicked();//数据查询
    void on_pushButtonModify_clicked();//修改本机IP
    void on_confirmBtn_clicked();//串口调试：确认
    void on_pushButton_Bufa_clicked();//数据补发
    void on_pushButton_setlocal_clicked();//设置本机时间
    void on_pushButton_setself_clicked();//设置自定义时间
    void on_pushButton_clicked();//新增上位机
    void on_pushButton_3_clicked();//编辑上位机
    void on_pushButtonFind_clicked();//查询
    void on_pushButtonExport_clicked();//导出
    void on_pushButton_AddDev_clicked();//新增设备
    void on_pushButton_Addf_clicked();//新增因子
    void on_pushButton_Updatef_clicked();//修改因子
    void on_pushButton_AddFresh_clicked();//编辑设备
    void on_pushButton_AddT_clicked();//新增特殊值
    void on_pushButton_UpdateT_clicked();//修改特殊值
    void onButtonDevParam(QString pFactorKey);//配置设备参数
    void onButtonUpDele(QString ipaddr);//删除上位机
    void onButtonUpSaved(QString ipaddr);//保存上位机配置
    void on_pushButton_ReturnDev_clicked();//返回设备配置

    void onButtonFaEdit(QString id);//编辑设备因子
    void onButtonFaDele(QString id);//删除设备因子

    void onReceiveDeviceCMDCtrl();//没用
    void openModbus();//打开Modbus配置窗口

signals:
    /* 工人开始工作（做些耗时的操作 ） */
    void startWork();
    void sendCurDT(QDateTime &);
    void sendUSBState(bool);
    void sendRCRW(bool isRead);
    void sendlog(QString logStr);

public slots:
    void onSlotRW(bool isRead);

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

    QSignalMapper *m_SignalMapper_Edit = nullptr;   // edit
    QSignalMapper *m_SignalMapper_Dele = nullptr;   // delete

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

    QList<QLineEdit *> lineEditList;
    QList<QTextEdit *> textEditList;
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
//    localKeyboard *kb = nullptr;
    AddRCDevice *addRC = nullptr;
    EditRCDevice *editRC = nullptr;
    RCLogDlg *logdlg =  nullptr;


    QFont itemfont,headerfont,ckfont;

    QList<QCheckBox *> edittimecks,addtimecks;
    QList<QCheckBox *> edittypecks,addtypecks;
    QGridLayout el1,el2,el3,el4;
    QGridLayout al1,al2,al3,al4;
    QStringList ontimecks;
    QStringList cmdlist;
    QXlsx::Document xlsx;


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
