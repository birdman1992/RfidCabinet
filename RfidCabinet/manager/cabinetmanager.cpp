#include "cabinetmanager.h"

CabinetManager* CabinetManager::m = new CabinetManager;

CabinetManager::CabinetManager(QObject *parent) : QObject(parent)
{

}

CabinetManager *CabinetManager::manager()
{
    return m;
}
