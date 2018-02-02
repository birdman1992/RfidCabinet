#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QList>
#include "devicesimulator.h"
#include "Qextserial/qextserialport.h"
#include "device/rfiddevice.h"
#include "Structs/rfidchangeinfo.h"

class DeviceManager : public QObject
{
    Q_OBJECT
public:
    explicit DeviceManager(QObject *parent = NULL);
    ~DeviceManager();

private:
    DeviceSimulator* win_controler;
    RfidDevice* devRfid;
    void deviceInit();
    QextSerialPort *comCtrlInit(QString devName, int baudRate, int dataBits, int Parity, int stopBits);

signals:
    void rfidIn(QList<rfidChangeInfo*>);
    void rfidOut(QList<rfidChangeInfo*>);

public slots:
    void recvDoorState(bool isopen);
    void insertRfid(QStringList);
    void test();
private slots:

};

#endif // DEVICEMANAGER_H
