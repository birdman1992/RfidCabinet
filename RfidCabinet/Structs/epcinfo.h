#ifndef EPCINFO_H
#define EPCINFO_H
#include <QString>
#include <QPoint>
#include "Structs/goodslist.h"

class EpcInfo
{
public:
    EpcInfo();
    EpcInfo(Goods* gInfo, QString rfidCode, int state, QPoint posInfo);
    QString epcId;
    QString goodsName;
    QString goodsId;
    QString abbName;
    QPoint pos;
    int epcState;
};

#endif // EPCINFO_H
