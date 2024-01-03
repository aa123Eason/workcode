#ifndef FACTORADD_H
#define FACTORADD_H

#include <QDialog>
#include <QButtonGroup>
#include "common.h"
#include "httpclinet.h"

namespace Ui {
class FactorAdd;
}

class httpclinet;

class FactorAdd : public QDialog
{
    Q_OBJECT

public:
    explicit FactorAdd(QString pDevID,QWidget *parent = nullptr);
    ~FactorAdd();
    bool Conf_FactorUpdate(QString pKey);

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

    QButtonGroup *GroupF1 = nullptr;
};

#endif // FACTORADD_H
