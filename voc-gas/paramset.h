#ifndef PARAMSET_H
#define PARAMSET_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMap>
#include "factorinfo.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include "common.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>

namespace Ui {
class ParamSet;
}

class ParamSet : public QMainWindow
{
    Q_OBJECT

public:
    explicit ParamSet(QWidget *parent = nullptr);
    ~ParamSet();

    void setUserTableHeader();
    void setUserTableContents();

    void setTableHeader();
    void setTableContents();

    void setTableFactorHeader();
    void setTableFactorContents();

    void setTableUpHeader();
    void setTableUpContents();

    bool Save_FactorSet();
    bool Save_CommSet();
    bool Save_UserSet();
    bool Save_SysSet();
    bool Save_FanSet();
    void System_Display();
    void Fanchui_Display();

    void loadfactorsInfo();

signals:
    void sendUpLoadType(int);
    void sendChangeFactors(bool);
    void sendCMDStr(QString str);
    void sendlogmsg(QString msg);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_12_clicked();
    void onReceiveUpLoadType(int);
    void onPrintlog(QString msg);



private:
    Ui::ParamSet *ui;
    QSqlDatabase curdb;
    QMap<QString,FactorInfo *> curMap;

};

#endif // PARAMSET_H
