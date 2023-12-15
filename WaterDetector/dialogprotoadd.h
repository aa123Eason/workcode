#ifndef DIALOGPROTOADD_H
#define DIALOGPROTOADD_H

#include <QDialog>
#include "common.h"
#include "cjsonfile.h"

namespace Ui {
class DialogProtoAdd;
}

class DialogProtoAdd : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProtoAdd(QWidget *parent = nullptr);
    ~DialogProtoAdd();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogProtoAdd *ui;
};

#endif // DIALOGPROTOADD_H
