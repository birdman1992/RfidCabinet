#include "epcmanager.h"

EpcManager* EpcManager::m = new EpcManager;
EpcManager *EpcManager::manager()
{
    return m;
}

EpcManager::EpcManager()
{

}
