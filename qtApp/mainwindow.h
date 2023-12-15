#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include <QListWidget>
#include <QListWidgetItem>
#include <QThread>
#include <QStackedWidget>
#include <QString>
#include <QDateTime>
#include <QEvent>
#include <QMouseEvent>
#include <QProcess>



#include "facpanel.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWIndow; }
QT_END_NAMESPACE

class MainWIndow : public QWidget
{
    Q_OBJECT

public:
    MainWIndow(QWidget *parent = nullptr);
    ~MainWIndow();
    void widgetInit();//mainwindow init
    void connectevent();


signals:



public slots:

private:
    Ui::MainWIndow *ui;
};
#endif // MAINWINDOW_H
