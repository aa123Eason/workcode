#ifndef TESHUZHIADD_H
#define TESHUZHIADD_H

#include <QDialog>
#include "common.h"
#include "httpclinet.h"

namespace Ui {
class TeshuzhiAdd;
}

class TeshuzhiAdd : public QDialog
{
    Q_OBJECT

public:
    explicit TeshuzhiAdd(QWidget *parent = nullptr);
    ~TeshuzhiAdd();

    bool Conf_TeshuzhiUpdate();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::TeshuzhiAdd *ui;
};

#endif // TESHUZHIADD_H
