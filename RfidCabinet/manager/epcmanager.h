#ifndef EPCMANAGER_H
#define EPCMANAGER_H
#include <QHash>
#include <QMap>
#include <QStringList>
#include "device/repertorymanager.h"
#include <Structs/goodslist.h>
#include "manager/cabinetmanager.h"
#include "Structs/cabinetinfo.h"
#include "Structs/epcinfo.h"

class EpcManager
{
public:
    static EpcManager* manager();
    void setActiveCab(QPoint pos);
    void appendWatchList(QStringList l);
    void appendWatchList(QString l);
    void removeWatchList(QStringList l);
    void removeWatchList(QString l);
    QByteArray storeClearing();
    QByteArray fetchClearing(QString opt);
    QByteArray backClearing(QString opt);
    void fetchRst();
    void backRst();

    bool insert(QString);
    void append(QStringList l);
    void initEpcHash();
    void clearCache();
    int epcNum();
    void addStoreList(GoodsList* l);
//    void checkNewAndDel();
    QStringList checkNew();//use before checkDel
    QStringList checkDel();

    QList<GoodsList*> storeLists();
    void clearStoreLists();
private:
    static EpcManager* m;
    RepertoryManager* repManager;
    QPoint activePos;
    QStringList cacheList;
    QStringList newList;
    QStringList delList;
    QStringList backList;
    QList<GoodsList*> storeList;
    EpcManager();
    void rfidClearing();//rfid结算
    void appendRepInfo(Goods* gInfo, QPoint pos);

/*
|0|1|2|3|4|5|6|7|
   0:1
0.旧物品:新物品
1.未被扫描:已被扫描
2.无标记:暂时取走
*/
    QHash<QString, int> hash_epc;
    QMap<QString, EpcInfo*> map_epc;
};

#endif // EPCMANAGER_H
