#ifndef DEVEDIT_H
#define DEVEDIT_H

//编辑设备参数

#include <QDialog>
#include "common.h"
#include "httpclinet.h"
#include <QButtonGroup>
#include "dialogproto.h"
#include "util.h"
#include <QComboBox>
#include <QStringList>
#include <QString>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QDateTime>

namespace Ui {
class DevEdit;
}

class DialogProto;

class DevEdit : public QDialog
{
    Q_OBJECT

public:
    explicit DevEdit(QString dev_id,QWidget *parent = nullptr);
    ~DevEdit();

    void DevEdit_Init(QString dev_id);//初始化函数：读取swagger接口的数据，填入相应的空间中
    void loadParamtable(QString dev_params);//读取不同的设备参数
    QString builddevparams();//建立设备参数
    void writeinfile(QString filepath,QJsonObject &obj);
    void writeloglocal(QString);

private slots:
    void typeRadioBtnClicked();
    void on_pushButton_Detail_clicked();
    void on_pushButton_UpdateDev_clicked();
    void onComboBoxProtoCurrentChanged(const QString &text);
    void on_pushButton_Cancel_clicked();


private:
    Ui::DevEdit *ui;
    Util util;
    QButtonGroup *interGroup = nullptr;
    QMap<QString,QStringList> map;
    QMap<QString,QString> namemap;
    QFont font;
    QString strx;
//    localKeyboard *kb = nullptr;
};

#endif // DEVEDIT_H
