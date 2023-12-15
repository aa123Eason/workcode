#ifndef MSGBOX_H
#define MSGBOX_H

#include <QObject>
#include <QEventLoop>
#include <QMessageBox>
#include <QApplication>
#include <QTimer>

class msgBox: public QObject
{
    Q_OBJECT

public:
    explicit msgBox(QObject *parent = nullptr);

    // messageBox类型
    enum Type { Information, Warning, Critical, Question};

private:
    const QString title;
    const QString msg;
    int type;
    void readyShow(void);

public:
    msgBox(const QString &title, const QString &msg, const int type);
    static void show(const QString &title, const QString &msg, const int type);
    static void information(const QString &title, const QString &msg);
    static void warning(const QString &title, const QString &msg);
    static void critical(const QString &title, const QString &msg);
    static QMessageBox::StandardButton question(const QString &title, const QString &msg);

public slots:
    void onShow();

};

#endif // MSGBOX_H
