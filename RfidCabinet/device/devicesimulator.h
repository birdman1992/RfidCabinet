#ifndef DEVICESIMULATOR_H
#define DEVICESIMULATOR_H

#include <QWidget>
#include <QButtonGroup>
#include "device/Qextserial/qextserialport.h"
#include "Structs/rfidchangeinfo.h"

namespace Ui {
class DeviceSimulator;
}

class DeviceSimulator : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceSimulator(QWidget *parent = 0);
    ~DeviceSimulator();

private slots:
    void on_card_submit_clicked();

    void on_rfidIn_clicked();

    void on_rfidOut_clicked();

    void on_rfidCheck_clicked();

signals:
    void cardReaderData(QByteArray);
//    void rfidInRange(QByteArray);
//    void rfidOutRange(QByteArray);
    void rfidIn(QList<rfidChangeInfo*>);
    void rfidOut(QList<rfidChangeInfo*>);
    void rfidNeedCheck();

private:
    Ui::DeviceSimulator *ui;
    QButtonGroup* group_card;
    QButtonGroup* group_rfid;

    void saveInfoToButton(QString info, QButtonGroup* group);
    void initCardGroup();
    void initRfidGroup();
};

#endif // DEVICESIMULATOR_H
