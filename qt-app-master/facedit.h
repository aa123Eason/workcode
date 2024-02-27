#ifndef FACEDIT_H
#define FACEDIT_H

#include <QDialog>
#include "common.h"
#include "httpclinet.h"
#include <QFile>
#include <QJsonDocument>
#include <QByteArray>
#include <QJsonObject>
#include <QDir>

namespace Ui {
class FacEdit;
}

class FacEdit : public QDialog
{
    Q_OBJECT

public:
    explicit FacEdit(QString id,QWidget *parent = nullptr);
    ~FacEdit();

    void FacEdit_Init(QString id);
    void loadinfo(QString path);
    bool ConfFactor_Filled(QString id);
    bool Conf_FactorUpdate(QString pKey);

    void writeinLocalJson(QString filename,QJsonObject &obj,QString pKey);
    void writeDevParams(QJsonObject &mainObj);
signals:
    void editSuccess();

private slots:
    void on_pushButton_cancel_clicked();
    void on_pushButton_Saved_clicked();

private:
    Ui::FacEdit *ui;

    QString m_FcodeID;
    QString m_Fcode;
//    localKeyboard *kb = nullptr;

};

#endif // FACEDIT_H
