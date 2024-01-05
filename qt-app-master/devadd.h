#ifndef DEVADD_H
#define DEVADD_H

#include <QDialog>
#include <common.h>
#include "httpclinet.h"
#include "QButtonGroup"
#include "util.h"

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

private slots:
    void typeRadioBtnClicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void onCurrentDevTypeChanged(const QString &);

private:
    Ui::DevAdd *ui;
    QButtonGroup *interGroup = nullptr;
    Util util;
    QMap<QString,QStringList> map;
    QMap<QString,QString> namemap;
};

#endif // DEVADD_H
