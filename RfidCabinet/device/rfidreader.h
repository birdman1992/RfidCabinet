#ifndef RFIDREADER_H
#define RFIDREADER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QList>
#include <QStringList>
#include "Qextserial/qextserialport.h"
#include "manager/epcmanager.h"
#include "Structs/rfidchangeinfo.h"

class RfidReader : public QObject
{
    Q_OBJECT
public:
    RfidReader(int address, QString dev, QObject* parent = NULL);
    void rfidScan();
    void setReaderAddress(char address);
    QList<rfidChangeInfo *> getScanAddList();
    QList<rfidChangeInfo *> getScanDelList();

private:
    QList<QByteArray> sendList;
    QList<rfidChangeInfo*> listAdd;
    QList<rfidChangeInfo*> listDel;
    EpcManager* manager_epc;
    QextSerialPort* com;
    QByteArray dataCache;
    bool waitFlag;
    bool nextFlag;
    int curAnt;
    int readerId;
    qint64 write(QByteArray data);
    qint64 writeNext();
    bool epcFilter(QString epc);
    void waitForWrite(QByteArray data);
    void scanOneAnt(int antId);
    void readDataCache();
    void scanFinish();

    void setScanAnt(char address, char antId);
    void rfidInventory(char address, char repeat);
    void getInventoryBuffer(char address);
    void getRealTimeInventory(char address, char repeat);
//    void fastAntInventory();

    QByteArray rfidCmd(char address, char cmd, QByteArray data);
    QByteArray rfidCmd(char address, char cmd, char data);
    char checkSum(QByteArray qba);

    void parPackage(QByteArray qba);
    QString parEpc(QByteArray qba);

signals:
    void scanFinished();

private slots:
    void readData();
};

#endif // RFIDREADER_H
