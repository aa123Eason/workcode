#ifndef DIALOGNEWDEV_H
#define DIALOGNEWDEV_H

#include <QDialog>
#include "cjsonfile.h"
#include "common.h"

namespace Ui {
class DialogNewDev;
}

class DialogNewDev : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewDev(QWidget *parent = nullptr);
    ~DialogNewDev();

    void guiInit();

signals:
    void exitWin();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogNewDev *ui;
};

#endif // DIALOGNEWDEV_H
