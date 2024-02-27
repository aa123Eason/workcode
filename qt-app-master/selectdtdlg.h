#ifndef SELECTDTDLG_H
#define SELECTDTDLG_H

#include <QDialog>
#include <QDateTime>
#include <QTime>
#include <QDate>
#include <QCalendarWidget>
#include <QDebug>
#include <QString>
#include <QEvent>
#include <QMouseEvent>
#include <QProcess>


namespace Ui {
class SelectDTDlg;
}

class SelectDTDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SelectDTDlg(QWidget *parent = 0);
    ~SelectDTDlg();

    void init();
    void connectevent();


signals:
    void senddt(QDateTime &);

protected:
    bool eventFilter(QObject *, QEvent *) override;

private:
    Ui::SelectDTDlg *ui;
    QDateTime dt;
};

#endif // SELECTDTDLG_H
