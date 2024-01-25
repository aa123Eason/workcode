#ifndef TESHUZHIDELE_H
#define TESHUZHIDELE_H

#include <QDialog>
#include "common.h"

namespace Ui {
class TeshuzhiDele;
}

class TeshuzhiDele : public QDialog
{
    Q_OBJECT

public:
    explicit TeshuzhiDele(QWidget *parent = nullptr);
    ~TeshuzhiDele();

    void TeshuzhiGui_Init();

private:
    Ui::TeshuzhiDele *ui;
};

#endif // TESHUZHIDELE_H
