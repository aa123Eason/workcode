#include "msgbox.h"

static QMessageBox::StandardButton result;

msgBox::msgBox(QObject *parent):QObject (parent)
{

}

void msgBox::readyShow()
{
    this->moveToThread(qApp->thread());
    QTimer::singleShot(0, this, SLOT(onShow()));
}

msgBox::msgBox(const QString &title, const QString &msg,const int type):title(title),msg(msg),type(type) {

}

void msgBox::show(const QString &title, const QString &msg, const int type)
{
    QEventLoop eventLoop;
    auto messageBox = new msgBox(title, msg, type);
    connect(messageBox, SIGNAL(destroyed()), &eventLoop, SLOT(quit()));
    messageBox->readyShow();
    eventLoop.exec();
}

void msgBox::information(const QString &title, const QString &msg) {
    show(title, msg, msgBox::Information);
}

void msgBox::warning(const QString &title, const QString &msg) {
    show(title, msg, msgBox::Warning);
}

void msgBox::critical(const QString &title, const QString &msg) {
    show(title, msg, msgBox::Critical);
}

QMessageBox::StandardButton msgBox::question(const QString &title, const QString &msg) {
    show(title, msg, msgBox::Question);
    return result;
}

void msgBox::onShow()
{
    switch (type)
    {
    case msgBox::Critical:
        QMessageBox::critical(NULL, title, msg, QString::fromLocal8Bit("ok"));
        break;
    case msgBox::Warning:
        QMessageBox::warning(NULL, title, msg, QString::fromLocal8Bit("ok"));
        break;
    case msgBox::Question:
        result = QMessageBox::question(NULL, title, msg, QMessageBox::Yes | QMessageBox::No);
        break;
    case msgBox::Information:
    default:
        QMessageBox::information(NULL, title, msg, QString::fromLocal8Bit("ok"));
        break;
    }

    this->deleteLater();
}
