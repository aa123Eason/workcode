#ifndef FACPANEL_H
#define FACPANEL_H

#include <QWidget>

namespace Ui {
class facPanel;
}

class facPanel : public QWidget
{
    Q_OBJECT

public:
    explicit facPanel(QWidget *parent = nullptr);
    ~facPanel();

private:
    Ui::facPanel *ui;
};

#endif // FACPANEL_H
