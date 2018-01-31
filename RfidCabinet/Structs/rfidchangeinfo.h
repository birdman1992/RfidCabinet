#ifndef RFIDCHANGEINFO_H
#define RFIDCHANGEINFO_H
#include <QByteArray>
#include <QPoint>


class rfidChangeInfo
{
public:
    rfidChangeInfo(int ant, QByteArray id);
    int antId;
    QPoint pos;
    QByteArray rfid;
};

#endif // RFIDCHANGEINFO_H
