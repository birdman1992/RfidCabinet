#ifndef RFIDANT_H
#define RFIDANT_H

#include <QObject>
#include <QStringList>
#include "RfidCell/rfidarea.h"

class RfidAnt : public QObject
{
    Q_OBJECT
public:
    explicit RfidAnt(int _antId, RfidArea* area, QObject *parent = 0);
    QStringList addId(QStringList &list);
    QStringList removeId(QStringList list);
    bool addId(QString _id);
    bool removeId(QString _id);
    int getAntId();
    QPoint getPos();

private:
    int antId;
    RfidArea* area;

    QStringList diffList(QStringList newList, QStringList fixList);
    QStringList sameList(QStringList newList, QStringList fixList);

signals:

public slots:

};

#endif // RFIDANT_H
