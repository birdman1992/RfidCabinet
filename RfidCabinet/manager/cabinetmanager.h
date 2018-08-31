#ifndef CABINETMANAGER_H
#define CABINETMANAGER_H

#include <QObject>
#include "config.h"

class CabinetManager : public QObject
{
    Q_OBJECT
public:
    explicit CabinetManager(QObject *parent = nullptr);
    CabinetManager* manager();

private:
    static CabinetManager* m;

signals:

public slots:
};

#endif // CABINETMANAGER_H
