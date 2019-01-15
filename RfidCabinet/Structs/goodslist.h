#ifndef GOODSLIST_H
#define GOODSLIST_H
#include <qstring.h>
#include <qlist.h>
#include <QMap>

#include <qstring.h>
#include <QStringList>
#include <qlist.h>
#include <QMap>
#include <QPoint>

//class goodsCode{
//public:
//    goodsCode(QString cPackage, QString cRfid)
//    {
//        packageBarcode = cPackage;
//        rfidCode = cRfid;
//    }
//    QString packageBarcode;
//    QString rfidCode;
//};

class Goods
{
public:
    Goods();
    Goods(QString _goodsId, QString goodsName, int goodsNum);
    Goods(Goods* goods);
    bool addRfid(QString rfid);
    bool storeRfid(QString rfid);
    QStringList codes;
    QString name;
    QString abbName;
    QString goodsId;
    QString size;//规格
    QString unit;
    QString packageBarcode;
    QStringList rfidCodes;
    QStringList storeRfidCodes;
    QString roomName;

    QPoint pos;
    int packageType;
    int batchNumber;
    int takeCount;
    int singlePrice;
    int inStorageId;
    int totalNum;
    int curNum;//未写入柜子信息的已存入数量
    int storeNum;//写入柜子信息的已存入数量
    bool finish;
};

//此处goodsId参数为物品包层次的区分，实际为packagebarcode
class GoodsList
{
public:
    GoodsList();
    ~GoodsList();
    void addGoods(Goods* _goods);
    void addRfid(QString goodsId, QString rfid);
    bool storeRfid(QString rfid);
    void goodsIn(QString goodsId,int num);
    void goodsOut(QString goodsId, int num);
    bool goodsIsRepeat(Goods* _goods, int *index=NULL);
    Goods* getGoodsById(QString goodsId);
    bool isFinished();
    bool listCheck();

    QMap<QString, Goods*> map_goods;
    QString barcode;
    QString chesetCode;
    QString optName;
    QList<Goods*> list_goods;
};

#endif // GOODSLIST_H
