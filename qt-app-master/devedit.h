#ifndef DEVEDIT_H
#define DEVEDIT_H

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
    void loadParamtable(QString dev_params);
    QString builddevparams();

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
};

#endif // DEVEDIT_H
