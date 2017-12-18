#include "devicemanager.h"

DeviceManager::DeviceManager(QObject *parent) : QObject(parent)
{
    win_controler = new DeviceSimulator();
    win_controler->show();
}

DeviceManager::~DeviceManager()
{
    delete win_controler;
}
