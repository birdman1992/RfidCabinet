#ifndef RFIDDEVICE_H
#define RFIDDEVICE_H

#include <QObject>
#include <QThread>
#include <QByteArray>
//struct RFID_DATA;


class RfidDevice : public QThread
{
    Q_OBJECT
public:
    explicit RfidDevice(QObject *parent = NULL);
    ~RfidDevice();

    void startScan();
    void stopScan();
private:
    void run();
    bool runFlag;
signals:
    void rfidIn(int antId,QByteArray rfid);
    void rfidOut(int antId,QByteArray rfid);

public slots:
//    int rfidScan();
};

#endif // RFIDDEVICE_H
