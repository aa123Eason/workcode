#ifndef UPDEVICESET_H
#define UPDEVICESET_H

#include <QWidget>

namespace Ui {
class UpDeviceSet;
}

class UpDeviceSet : public QWidget
{
    Q_OBJECT

public:
    explicit UpDeviceSet(QWidget *parent = nullptr);
    ~UpDeviceSet();

private:
    Ui::UpDeviceSet *ui;
};

#endif // UPDEVICESET_H
