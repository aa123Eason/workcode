#ifndef TESHUZHIADD_H
#define TESHUZHIADD_H

#include <QDialog>
#include "common.h"
#include "httpclinet.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QComboBox>
#include <QStringList>
#include <QString>
#include <QDebug>
#include <QDialogButtonBox>
#include <QFile>
#include "util.h"
#include <QByteArray>

namespace Ui {
class TeshuzhiAdd;
}

class TeshuzhiAdd : public QDialog
{
    Q_OBJECT

public:
    explicit TeshuzhiAdd(QWidget *parent = nullptr);
    ~TeshuzhiAdd();

    void loadAvailbleFacs();
    void connectevent();
    bool Conf_TeshuzhiUpdate();
    void writeinLocalJson(QJsonObject &);
    QJsonObject facsMatchDevid(QString code);

signals:
    void addSuccess();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::TeshuzhiAdd *ui;
    httpclinet h;
    Util util;
    QMap<QString,QString> nameMap,avaFacMap;
//    localKeyboard *kb = nullptr;

};

#endif // TESHUZHIADD_H
