#ifndef RFIDREADER_H
#define RFIDREADER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QList>
#include "Qextserial/qextserialport.h"

class RfidReader : public QObject
{
    Q_OBJECT
public:
    RfidReader(QString dev, QObject* parent = NULL);
    void rfidScan();

private:
    QList<QByteArray> sendList;
    QextSerialPort* com;
    qint64 write(QByteArray data);
    qint64 writeNext();
    void waitForWrite(QByteArray data);
    void scanOneAnt(int antId);

private slots:
    void readData();
};

#endif // RFIDREADER_H
