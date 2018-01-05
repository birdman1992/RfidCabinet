#include "rfidchangeinfo.h"

rfidChangeInfo::rfidChangeInfo(int ant, QByteArray id)
{
    antId = ant;
    rfid = QByteArray(id);
}
