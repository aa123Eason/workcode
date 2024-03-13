#ifndef COMBOBOXSELECTDLG_H
#define COMBOBOXSELECTDLG_H

#include <QWidget>
#include <QComboBox>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QPushButton>
#include <QFont>
#include <QColumnView>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QScrollBar>

namespace Ui {
class ComBoBoxSelectDlg;
}

class ComBoBoxSelectDlg : public QWidget
{
    Q_OBJECT

public:
    explicit ComBoBoxSelectDlg(QString title,int rows,int cols,QWidget *parent = nullptr);
    ~ComBoBoxSelectDlg();
    void addButton(QString name);

signals:
    void sendSelectedButton(QString name);

private:
    Ui::ComBoBoxSelectDlg *ui;
    QFont font;
    QVBoxLayout vlayout;
};

#endif // COMBOBOXSELECTDLG_H
