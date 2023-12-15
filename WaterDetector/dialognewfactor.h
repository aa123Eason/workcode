#ifndef DIALOGNEWFACTOR_H
#define DIALOGNEWFACTOR_H

#include <QDialog>
#include "common.h"
#include "cjsonfile.h"

namespace Ui {
class DialogNewFactor;
}

class DialogNewFactor : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewFactor(QString protoName,QWidget *parent = nullptr);
    ~DialogNewFactor();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogNewFactor *ui;
    QString m_ProtoName;
};

#endif // DIALOGNEWFACTOR_H
