#ifndef DEVEDIT_H
#define DEVEDIT_H

#include <QDialog>
#include "common.h"
#include "httpclinet.h"
#include <QButtonGroup>
#include "dialogproto.h"
#include "util.h"

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

    void DevEdit_Init(QString dev_id);

private slots:
    void typeRadioBtnClicked();
    void on_pushButton_Detail_clicked();
    void on_pushButton_UpdateDev_clicked();

    void on_pushButton_Cancel_clicked();

private:
    Ui::DevEdit *ui;
    Util util;
    QButtonGroup *interGroup = nullptr;
    QMap<QString,QStringList> map;
    QMap<QString,QString> namemap;
};

#endif // DEVEDIT_H
