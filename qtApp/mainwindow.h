#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWIndow; }
QT_END_NAMESPACE

class MainWIndow : public QWidget
{
    Q_OBJECT

public:
    MainWIndow(QWidget *parent = nullptr);
    ~MainWIndow();

private:
    Ui::MainWIndow *ui;
};
#endif // MAINWINDOW_H
