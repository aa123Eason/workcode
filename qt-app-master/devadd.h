#ifndef DEVADD_H
#define DEVADD_H

#include <QDialog>
#include <common.h>
#include "httpclinet.h"
#include "QButtonGroup"

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

private slots:
    void typeRadioBtnClicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::DevAdd *ui;
    QButtonGroup *interGroup = nullptr;
};

#endif // DEVADD_H
