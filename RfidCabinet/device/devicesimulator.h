#ifndef DEVICESIMULATOR_H
#define DEVICESIMULATOR_H

#include <QWidget>
#include <QButtonGroup>

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

signals:
    void cardReaderData(QByteArray);
    void rfidInRange(QByteArray);
    void rfidOutRange(QByteArray);

private:
    Ui::DeviceSimulator *ui;
    QButtonGroup* group_card;
    QButtonGroup* group_rfid;

    void saveInfoToButton(QString info, QButtonGroup* group);
    void initCardGroup();
    void initRfidGroup();
};

#endif // DEVICESIMULATOR_H
