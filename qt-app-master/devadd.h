#ifndef DEVADD_H
#define DEVADD_H

#include <QDialog>
#include <common.h>
#include "httpclinet.h"
#include "QButtonGroup"
#include "util.h"
#include <QDir>
#include <QEvent>
#include <QMouseEvent>
#include <QObjectList>
#include "comboboxselectdlg.h"

//添加设备


namespace Ui {
class DevAdd;
}

class httpclinet;

class DevAdd : public QDialog
{
    Q_OBJECT

public:
    explicit DevAdd(QWidget *parent = nullptr);
    ~DevAdd();
    void connectevent();
    void buildLocalJson(QJsonObject &obj);//将设备参数记入本地json文件
    bool compare2devices(QJsonObject &nowobj,QJsonObject &refobj,QString &idcode);//对比两个设备参数json
    void installEvents();//事件过滤器初始配置

private slots:
    void typeRadioBtnClicked();//串口配置和网络配置切换
    void on_pushButton_clicked();//确认新增设备
    void on_pushButton_2_clicked();//取消新增
    void onCurrentDevTypeChanged(const QString &);

protected:
    bool eventFilter(QObject *, QEvent *) override;

private:
    Ui::DevAdd *ui;
    QButtonGroup *interGroup = nullptr;
    Util util;
    QMap<QString,QStringList> map;
    QMap<QString,QString> namemap;
    ComBoBoxSelectDlg *dlgbox = nullptr;
//    localKeyboard *kb = nullptr;
};

#endif // DEVADD_H
