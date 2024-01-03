#ifndef DIALOGDEVPROP_H
#define DIALOGDEVPROP_H

#include <QDialog>
#include "common.h"
namespace Ui {
class DialogDevProp;
}

class DialogDevProp : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDevProp(QString pFactorKey,QWidget *parent = nullptr);
    ~DialogDevProp();

    void showTableContent(QString pFactorKey);
private:
    Ui::DialogDevProp *ui;

};

#endif // DIALOGDEVPROP_H
