#ifndef REPERTORYMANAGER_H
#define REPERTORYMANAGER_H

#include <QObject>
#include <QSettings>
#include <QStringList>
#include "Structs/cabinetinfo.h"

class RepertoryManager : public QObject
{
    Q_OBJECT
public:
    static RepertoryManager* manager();
    void clearRepertory();
    void rfidIn(GoodsInfo *info);//物品放入
    void rfidOut(GoodsInfo *info);

private:
    explicit RepertoryManager(QObject *parent = NULL);
    static RepertoryManager* m;
    QString getRepFile(QPoint pos);
    int rfidAppend(QString& rfids, QString rfid);
    int rfidRemove(QString& rfids, QString rfid);



signals:

public slots:
};

#endif // REPERTORYMANAGER_H
