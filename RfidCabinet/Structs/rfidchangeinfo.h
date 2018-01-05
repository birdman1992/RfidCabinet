#ifndef RFIDCHANGEINFO_H
#define RFIDCHANGEINFO_H
#include <QByteArray>


class rfidChangeInfo
{
public:
    rfidChangeInfo(int ant, QByteArray id);
    int antId;
    QByteArray rfid;
};

#endif // RFIDCHANGEINFO_H
