#ifndef USBUPDATEDLG_H
#define USBUPDATEDLG_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDebug>
#include <QStringList>
#include <QComboBox>
#include <QDirIterator>
#include <QFile>
#include <QFileDevice>
#include <QDateTime>
#include <QProcess>
#include <QMessageBox>

namespace Ui {
class USBUpdateDlg;
}

class USBUpdateDlg : public QWidget
{
    Q_OBJECT


public:
    explicit USBUpdateDlg(QWidget *parent = nullptr);
    ~USBUpdateDlg();

    void loadUSBUpdateDemo();
    void connectevent();
    void init();


public slots:
    void on_comboBox_currentitem_changed(const QString &);
    void on_confirm_update();

private:
    Ui::USBUpdateDlg *ui;
};

#endif // USBUPDATEDLG_H
