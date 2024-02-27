#ifndef FACTORADD_H
#define FACTORADD_H

#include <QDialog>
#include <QButtonGroup>
#include "common.h"
#include "httpclinet.h"
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QFileInfo>
#include <QString>
#include <QDebug>
#include <QFileInfoList>
#include <QScrollBar>
#include <QAbstractItemView>

namespace Ui {
class FactorAdd;
}

class httpclinet;

class FactorAdd : public QDialog
{
    Q_OBJECT

public:
    explicit FactorAdd(QString pDevID,QString devtype,QWidget *parent = nullptr);
    ~FactorAdd();
    bool Conf_FactorUpdate(QString pKey,QJsonObject &jFac);
    bool findSameALias(QString devid,QString alias);
    void writeinLocalJson(QString filename,QJsonObject &obj,QString pKey);
    void writeDevParams(QJsonObject &obj);
signals:
    void addSuccess();

private slots:
    void decRadioBtnClicked();
    void mfRadioBtnClicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void analogRadioBtnClicked();

private:
    Ui::FactorAdd *ui;
    QString m_DevId;
    QString m_DevType;

    QButtonGroup *GroupF1 = nullptr;
//    localKeyboard *kb = nullptr;
};

#endif // FACTORADD_H
