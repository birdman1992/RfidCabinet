#include "epcinfo.h"

EpcInfo::EpcInfo()
{

}

EpcInfo::EpcInfo(Goods *gInfo, QString rfidCode, int state, QPoint posInfo)
{
    epcId = rfidCode;
    goodsName = gInfo->name;
    abbName = gInfo->abbName;
    goodsId = gInfo->goodsId;
    pos = posInfo;
    epcState = state;
}
