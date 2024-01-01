#ifndef DATAQUERY_H
#define DATAQUERY_H

#include <QWidget>
#include <QStackedWidget>
#include <QDebug>
#include "httpclient.h"
#include <QJsonObject>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMap>
#include <QString>
#include <QList>
#include <QMessageBox>

#include "common.h"


namespace Ui {
class DataQuery;
}

class DataQuery : public QWidget
{
    Q_OBJECT

public:
    explicit DataQuery(QWidget *parent = nullptr);
    ~DataQuery();
    void init();
    void connectevent();
    void getFactors();
    void getCommonFactors();
    void getUpsetAddr();

    QJsonObject query_rtk();
    QJsonObject query_min();
    QJsonObject query_hour();
    QJsonObject query_day();
    QJsonObject query_month();
    QJsonObject query_trans();

signals:

public slots:
    void onQuery();
    void onExport();
    void onReceiveFac(QString facname);

private:
    Ui::DataQuery *ui;
    QJsonObject fullFactorsInfo;
    QJsonObject fullDevicesInfo;

};

#endif // DATAQUERY_H
