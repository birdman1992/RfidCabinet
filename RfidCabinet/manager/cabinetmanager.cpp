#include "cabinetmanager.h"

CabinetManager* CabinetManager::m = new CabinetManager;

CabinetManager::CabinetManager(QObject *parent) : QObject(parent)
{

}

static CabinetManager *CabinetManager::manager()
{
    return m;
}

bool CabinetManager::setServerAddress(QString addr)
{
    QStringList list_params = addr.split(":");
    if(list_params.count() != 2)
        return false;

    QString ip = list_params.at(0);
    QString port = list_params.at(1);
    QRegExp addressExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    QRegExp portExp("^([0-9]|[1-9]\\d|[1-9]\\d{2}|[1-9]\\d{3}|[1-5]\\d{4}|6[0-4]\\d{3}|65[0-4]\\d{2}|655[0-2]\\d|6553[0-5])$");

    if(!(addressExp.exactMatch(ip) || portExp.exactMatch(port)))//非法ip或端口
    {
        return false;
    }

    setConfig("ServerAddress", addr);
    return true;
}

void CabinetManager::setCabCtrlWorld(QPoint caseAddr, QPoint ctrlWord)
{
    QString key = QString("CtrlWord/cabinet%1").arg(caseAddr.x());
    QByteArray word = getConfig(key, defaultCtrlWord(caseAddr.x())).toByteArray();
    if(caseAddr.y()*2 > word.size())
        return;
    word[caseAddr.y()*2] = ctrlWord.x();
    word[caseAddr.y()*2+1] = ctrlWord.y();
    setConfig(key, word);
}

void CabinetManager::setCabinetId(QString id)
{
    setConfig("CabinetId", id);
}

QPoint CabinetManager::getCabCtrlWorld(QPoint caseAddr)
{
    QString key = QString("CtrlWord/cabinet%1").arg(caseAddr.x());
    QByteArray word = getConfig(key, defaultCtrlWord(caseAddr.x())).toByteArray();
    QPoint ret;
    ret.setX(word[caseAddr.y()*2]);
    ret.setY(word[caseAddr.y()*2]);
    return ret;
}

QString CabinetManager::getServerAddress()
{
    getConfig("ServerAddress", DEFAULT_SERVER);
}

QString CabinetManager::getCabinetId()
{
    getConfig("CabinetId", QString());
}

int CabinetManager::getLockId(int col, int row)
{
    QString layout = getConfig("layout", QString());
    QStringList layouts = layout.split("#", QString::SkipEmptyParts);
    int ret = 0;

    for(int i=0; (i<layouts.count())&&(i<col); i++)
    {
        ret += layouts.at(i).count();
    }
    ret += row;
    return ret;
}

int CabinetManager::cabinetColCount()
{
    QString layout = getConfig("layout", QString());
    if(layout.isEmpty())
        return 0;

    return layout.split("#", QString::SkipEmptyParts).count();
}

int CabinetManager::cabinetRowCount(int col)
{
    QString layout = getConfig("layout", QString());
    if(layout.isEmpty())
        return 0;

    QStringList layouts = layout.split("#", QString::SkipEmptyParts);
    if(col >= layouts.count())
        return 0;

    return layouts.at(col).length();
}

void CabinetManager::clearConfig()
{

}

QString CabinetManager::getPyCh(QString name)
{
    return ChineseLetterHelper::GetFirstLettersAll(str);
}

void CabinetManager::addUser(QString id)
{

}

bool CabinetManager::checkManagers(QString id)
{

}

QString CabinetManager::scanDataTrans(QString data)
{

}

void CabinetManager::saveFetchList(QByteArray data)
{

}

bool CabinetManager::sleepFlagTimeout()
{
    return false;
}

QByteArray CabinetManager::defaultCtrlWord(int col)
{
    QString layout = getConfig("layout", QString()).toString();
    if(layout.isEmpty())
        return QByteArray();

    int rowCount = layout.split("#", QString::SkipEmptyParts).at(col).length();
    return QByteArray(rowCount*2, 0x0);
}

/********base functions*******/
void CabinetManager::setConfig(QString key, QVariant value)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.setValue(key, value);
    settings.sync();
}

QVariant CabinetManager::getConfig(QString key, QVariant defaultRet)
{
    QSettings settings(configPath, QSettings::IniFormat);
    return settings.value(key, defaultRet);
}

void CabinetManager::removeConfig(QString path)
{
    QSettings settings(configPath, QSettings::IniFormat);
    settings.remove(path);
}
/********base functions end*******/
