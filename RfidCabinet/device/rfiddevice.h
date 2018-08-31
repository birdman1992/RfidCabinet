#ifndef RFIDDEVICE_H
#define RFIDDEVICE_H

#include <QObject>
#include <QThread>
#include <QByteArray>
#include <QList>
#include <QStringList>
#include "device/rfidreader.h"
#include "Structs/rfidchangeinfo.h"
#include "manager/epcmanager.h"
//struct RFID_DATA;


class RfidDevice : public QThread
{
    Q_OBJECT
public:
    explicit RfidDevice(QObject *parent = NULL);
    ~RfidDevice();

    void startScan();
    void stopScan();
    void insertRfid(QStringList);
private:
    void run();

    bool runFlag;
    RfidReader* reader;
    EpcManager* manager_epc;
    QList<rfidChangeInfo*> inList;
    QList<rfidChangeInfo*> outList;
signals:
    void rfidIn(QList<rfidChangeInfo*>);
    void rfidOut(QList<rfidChangeInfo*>);

public slots:
//    int rfidScan();
    void rfidReadyread();
};

#endif // RFIDDEVICE_H
