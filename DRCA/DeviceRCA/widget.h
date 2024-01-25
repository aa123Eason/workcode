#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "httpclinet.h"
#include "serialport.h"
#include "util.h"
#include "common.h"
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QString>
#include <QStringList>
#include <QFont>
#include <QIcon>
#include <QDebug>
#include <QMap>
#include <QStackedWidget>
#include <QHeaderView>
#include <QThread>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QRadioButton>
#include <QCheckBox>
#include <QComboBox>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QPixmap>
#include <QImage>
#include <QHBoxLayout>
#include <QLabel>
#include <QSignalMapper>
#include <QtCore/QObject>
#include <QByteArray>
#include <QDir>


#define CMDINFO "/docu/cmdinfo.json"
#define STATE_ON ":/images/images/checked.png"
#define STATE_OFF ":/images/images/unchecked.png"


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class CHttpWork;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void init();
    void widget_init();
    void page_deviceStatueInit();
    void page_deviceEditInit();
    void page_deviceAddInit();
    void page_deviceLogInit();
    void device_init();
    void connectevent();
    void setTableHeader();
    void setTableContent(const QJsonObject &pDevice,const QJsonObject &pFactor);
    void timeCheckInit(QList<QCheckBox *> &,QGridLayout &timeset,QWidget *w = nullptr);
    void loadtmcksState(QString porName);

signals:
    void startWork();
    void sendfacid(QString &);

public slots:
    void handleResults(const QJsonObject &pDevice,const QJsonObject &pFactor);
    void handleDateTimeout();
    void onSaveTimeset();
    void onSlotRW(QMap<QString,SerialPort *> &);


private slots:
    void onButtonDele(QString id);
    void onButtonEdit(QString id);


private:
    Ui::Widget *ui;
    QThread m_thread,m_threadRW;
    CHttpWork *m_httpWorker = nullptr;
    CHttpWork *m_httpWorkerRW = nullptr;
    Util util;

    QStringList portsList;

    QMap<QString,QString> facnameMap;
    QTimer timer;
    QFont itemfont,headerfont,ckfont;
    QSignalMapper *m_SignalMapper_Edit = nullptr;   // edit
    QSignalMapper *m_SignalMapper_Dele = nullptr;   // delete
    QList<QCheckBox *> edittimecks,addtimecks;
    QList<QCheckBox *> edittypecks,addtypecks;
    QGridLayout el1,el2,el3,el4;
    QGridLayout al1,al2,al3,al4;
    QStringList ontimecks;
    QStringList cmdlist;
};

class CHttpWork : public QObject
{
    Q_OBJECT

public:
    explicit CHttpWork(QObject *parent = nullptr);

    /* 打断线程（注意此方法不能放在槽函数下） */
    void stopWork() {
        qDebug()<<"打断自动监控线程";

        /* 获取锁后，运行完成后即解锁 */
        QMutexLocker locker(&lock);
        isCanRun = false;
    }

    void sendResultReady();
    QStringList getAvailblePorts(const QJsonObject &pDevice);
    QJsonObject checkPortState(QStringList ports);

signals:
    /* 工人工作函数状态的信号 */
    void resultReady(const QJsonObject &pDevice,const QJsonObject &pFactor);
    void sendCMD(QMap<QString,SerialPort *> &);

public slots:
    void doWork();
    void rwWork();
private:
    /* 互斥锁 */
    QMutex lock;
    /* 标志位 */
    bool isCanRun;
    Util util;
};




#endif // WIDGET_H
