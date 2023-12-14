#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
//    void showResult();
//    void showState(QProcess::ProcessState);
//    void showError();
//    void showFinished(int,QProcess::ExitStatus);

private:
    Ui::Dialog *ui;
    QProcess my_Process;
};
#endif // DIALOG_H
