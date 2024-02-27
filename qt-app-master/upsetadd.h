#ifndef UPSETADD_H
#define UPSETADD_H

#include <QDialog>
#include "common.h"
#include "httpclinet.h"
#include <QButtonGroup>

class httpclinet;

namespace Ui {
class UpsetAdd;
}

class UpsetAdd : public QDialog
{
    Q_OBJECT

public:
    explicit UpsetAdd(QWidget *parent = nullptr);
    ~UpsetAdd();

    void UpAdd_filled();
    void setdefaultvalue();

private slots:
    void on_pushButton_confirm_clicked();
    void on_pushButton_cancel_clicked();

private:
    Ui::UpsetAdd *ui;
//    localKeyboard *kb = nullptr;
};

#endif // UPSETADD_H
