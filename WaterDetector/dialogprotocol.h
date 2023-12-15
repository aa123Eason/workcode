#ifndef DIALOGPROTOCOL_H
#define DIALOGPROTOCOL_H

#include <QDialog>
#include "cjsonfile.h"
#include "common.h"
#include "QSignalMapper"
#include "dialogprotoadd.h"
#include "dialognewfactor.h"

namespace Ui {
class DialogProtocol;
}

class DialogProtoAdd;
class DialogNewFactor;

class DialogProtocol : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProtocol(QWidget *parent = nullptr);
    ~DialogProtocol();
    void guiInit();
    void setTableModbusHeader();
    void setTableModbusContents(QJsonArray &pJsonArray);

private slots:
    void btnClicked_FacEdit(QString protoName);
    void btnClicked_FacDele(QString protoName);
    void on_comboBox_2_currentTextChanged(const QString &arg1);
    void on_pushButtonProtoAdd_clicked();
    void on_pushButtonProtoDele_clicked();
    void on_pushButtonProtoFresh_clicked();
    void on_pushButtonFaAdd_clicked();
    void on_pushButtonFaFresh_clicked();

private:
    Ui::DialogProtocol *ui;
    QSignalMapper *m_SignalMapperEdit;
    QSignalMapper *m_SignalMapperDele;
    DialogProtoAdd *m_DialogProtoAdd;
    DialogNewFactor *m_DialogNewFactor;

    QMap<QString,QPushButton *> m_DevBtnEditMap;
    QMap<QString,QPushButton *> m_DevBtnDeleMap;
};

#endif // DIALOGPROTOCOL_H
