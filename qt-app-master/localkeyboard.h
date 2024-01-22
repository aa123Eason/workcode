#ifndef LOCALKEYBOARD_H
#define LOCALKEYBOARD_H

#include <QWidget>
#include <QPushButton>
#include <QSignalMapper>
#include <QEvent>
#include <QStringList>
#include <QString>
#include <QList>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QWindow>
#include <QVector>

namespace Ui {
class localKeyboard;
}

class localKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit localKeyboard(QWidget *parent = nullptr);
    ~localKeyboard();
    void init();
    void keymap();

protected:
    bool eventFilter(QObject *o = nullptr,QEvent *e = nullptr);


public slots:
    void slotKeyButtonClicked();
    void slotKeyLetterButtonClicked();
    void slotKeyNumberButtonClicked();

private:
    Ui::localKeyboard *ui;
    QMap<QString,QPushButton*> keyMap;
    QMap<QString,Qt::Key> sysKeyMap;
    QVector<QPushButton *> letterBtns;
    QVector<QPushButton *> numberBtns;
    QVector<QPushButton *> funcBtns;
};

#endif // LOCALKEYBOARD_H
