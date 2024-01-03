#ifndef DIALOGPROTO_H
#define DIALOGPROTO_H

#include <QDialog>
#include "common.h"
#include "httpclinet.h"

namespace Ui {
class DialogProto;
}

class httpclinet;

class DialogProto : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProto(QString proto,QWidget *parent = nullptr);
    ~DialogProto();

private slots:
    void textEdit_filled();
private:
    Ui::DialogProto *ui;
    QString m_DevProto;
};

#endif // DIALOGPROTO_H
