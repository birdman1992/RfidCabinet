#ifndef EPCMANAGER_H
#define EPCMANAGER_H
#include <QHash>
#include <QStringList>
#include "Structs/cabinetinfo.h"

class EpcManager
{
public:
    static EpcManager* manager();
    bool append();

private:
    static EpcManager* m;
    EpcManager();

    QHash<QString, GoodsInfo*> hash_epc;

};

#endif // EPCMANAGER_H
