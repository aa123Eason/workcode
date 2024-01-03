#ifndef FACEDIT_H
#define FACEDIT_H

#include <QDialog>
#include "common.h"
#include "httpclinet.h"

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
    bool ConfFactor_Filled(QString id);
    bool Conf_FactorUpdate(QString pKey);

signals:
    void editSuccess();

private slots:
    void on_pushButton_cancel_clicked();
    void on_pushButton_Saved_clicked();

private:
    Ui::FacEdit *ui;

    QString m_FcodeID;
};

#endif // FACEDIT_H
