#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include "devicesimulator.h"

class DeviceManager : public QObject
{
    Q_OBJECT
public:
    explicit DeviceManager(QObject *parent = NULL);
    ~DeviceManager();

private:
    DeviceSimulator* win_controler;

signals:

public slots:
};

#endif // DEVICEMANAGER_H
