#ifndef DEVICESET_H
#define DEVICESET_H

#include <QWidget>

namespace Ui {
class DeviceSet;
}

class DeviceSet : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceSet(QWidget *parent = nullptr);
    ~DeviceSet();

private:
    Ui::DeviceSet *ui;
};

#endif // DEVICESET_H
