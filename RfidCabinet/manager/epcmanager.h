#ifndef EPCMANAGER_H
#define EPCMANAGER_H
#include <QHash>
#include <QStringList>
#include "Structs/cabinetinfo.h"

class EpcManager
{
public:
    static EpcManager* manager();
    bool insert(QString);
    void append(QStringList l);
    void initEpcHash(QStringList l);
    void clearCache();
//    void checkNewAndDel();
    QStringList checkDel();

private:
    static EpcManager* m;
    QStringList cacheList;
    QStringList newList;
    QStringList delList;
    EpcManager();

    QHash<QString, int> hash_epc;  //0:del  1:new  2:old

};

#endif // EPCMANAGER_H
