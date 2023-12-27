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



#include "facpanel.h"
#include "appbutton.h"
#include "devicecmdctrldlg.h"


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
    void loadAppDlg(QDialog *dlg=nullptr);
    void deleteApp();

protected:
    bool eventFilter(QObject *obj = nullptr,QEvent *e = nullptr);
    void timerEvent(QTimerEvent *event) override;

signals:
    void appClicked(QString);


public slots:
    void startApp(QString name);

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
};
#endif // MAINWINDOW_H
