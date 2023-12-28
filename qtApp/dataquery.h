#ifndef DATAQUERY_H
#define DATAQUERY_H

#include <QWidget>
#include <QStackedWidget>
#include <QDebug>


namespace Ui {
class DataQuery;
}

class DataQuery : public QWidget
{
    Q_OBJECT

public:
    explicit DataQuery(QWidget *parent = nullptr);
    ~DataQuery();
    void init();
    void  connectevent();


private:
    Ui::DataQuery *ui;
};

#endif // DATAQUERY_H
