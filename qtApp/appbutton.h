#ifndef APPBUTTON_H
#define APPBUTTON_H

#include <QFrame>
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QString>

namespace Ui {
class AppButton;
}

class AppButton : public QFrame
{
    Q_OBJECT

public:
    explicit AppButton(QString name="",QString imgUrl="",QWidget *parent = nullptr);
    ~AppButton();

    QString appName;
    QPixmap appPixmap;

    void setName(QString name);
    void setPixmap(QString picUrl);
    void setSize(int w,int h);
private:
    Ui::AppButton *ui;
};

#endif // APPBUTTON_H
