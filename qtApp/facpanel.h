#ifndef FACPANEL_H
#define FACPANEL_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QDebug>
#include <QMetaType>
#include <QMetaObject>
#include <QVariant>



namespace Ui {
class facPanel;
}



class facPanel : public QWidget
{
    Q_OBJECT
public:
    explicit facPanel(QWidget *parent = nullptr);
    ~facPanel();

    void Init();

    void setcode(QString code);
    void setname(QString name);
    void setvalue(QString value);
    void setunit(QString unit);
    void setstate(QString state);





private:
    Ui::facPanel *ui;
};


#endif // FACPANEL_H
