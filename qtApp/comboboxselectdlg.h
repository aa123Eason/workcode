#ifndef COMBOBOXSELECTDLG_H
#define COMBOBOXSELECTDLG_H

#include <QWidget>
#include <QComboBox>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QPushButton>
#include <QFont>

namespace Ui {
class ComBoBoxSelectDlg;
}

class ComBoBoxSelectDlg : public QWidget
{
    Q_OBJECT

public:
    explicit ComBoBoxSelectDlg(QString title,int rows,int cols,QWidget *parent = nullptr);
    ~ComBoBoxSelectDlg();
    void addButton(int row,int col,QString name);

signals:
    void sendSelectedButton(QString name);

private:
    Ui::ComBoBoxSelectDlg *ui;
    QFont font;
};

#endif // COMBOBOXSELECTDLG_H
