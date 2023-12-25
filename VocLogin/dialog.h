#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QProcess>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QTextEdit>
#include <QEvent>
#include <QMouseEvent>
#include <windows.h>
#include <QDateEdit>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();

    void init();
    bool datebaseinit();
    bool writeindb(QSqlDatabase &db,QString name);

protected:
    bool eventFilter(QObject *, QEvent *);

public slots:
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void showResult();
    void showState(QProcess::ProcessState);
    void showError();
    void showFinished(int,QProcess::ExitStatus);

private:
    Ui::Dialog *ui;
    QProcess *my_Process = nullptr;
    QSqlDatabase db;
};
#endif // DIALOG_H
