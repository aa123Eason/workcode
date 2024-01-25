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
#include <QCursor>

namespace Ui {
class localKeyboard;
}

class localKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit localKeyboard(QWidget *widget = nullptr,QWidget *parent = nullptr);
    ~localKeyboard();
    void init();
    void keymap();

protected:
    bool eventFilter(QObject *o = nullptr,QEvent *e = nullptr);
    void closeEvent(QCloseEvent *event) override;
    //鼠标点击事件
    void mousePressEvent(QMouseEvent* ev)override;

    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent*ev)override;



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
    QWidget *curWidget = nullptr;
};

#endif // LOCALKEYBOARD_H
