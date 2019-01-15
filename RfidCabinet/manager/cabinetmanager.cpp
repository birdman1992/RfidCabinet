#include "cabinetmanager.h"

CabinetManager* CabinetManager::m = new CabinetManager;

CabinetManager::CabinetManager(QObject *parent) : QObject(parent)
{

}

CabinetManager* CabinetManager::manager()
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

void CabinetManager::setAntPow(int pow)
{
    setConfig("AntPow", pow);
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
    QString ret = getConfig("ServerAddress", QString()).toString();
    if(ret.isEmpty())
    {
        ret = QString(DEFAULT_SERVER);
        setConfig("ServerAddress", ret);
    }
    return ret;
}

QString CabinetManager::getCabinetId()
{
    return getConfig("CabinetId", QString()).toString();
}

int CabinetManager::getAntPow()
{
    return getConfig("AntPow", 0x14).toInt();
}

int CabinetManager::getLockId(int col, int row)
{
    QString layout = getConfig("layout", QString()).toString();
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
    QString layout = getConfig("layout", QString()).toString();
    if(layout.isEmpty())
        return 0;

    return layout.split("#", QString::SkipEmptyParts).count();
}

int CabinetManager::cabinetRowCount(int col)
{
    QString layout = getConfig("layout", QString()).toString();
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
    return ChineseLetterHelper::GetFirstLettersAll(name);
}

void CabinetManager::addUser(UserInfo* user)
{
    if(user == NULL)
    {
        qDebug()<<"[addUser]"<<"user info is null";
    }

    if(checkUserLocal(user->cardId) != NULL)
        return;
    qDebug()<<"[addUser]"<<user->cardId;

    QSettings settings(FILE_CONFIG_CABINET_LAYOUT,QSettings::IniFormat);
    settings.beginGroup(QString("Users"));
    int index = settings.beginReadArray("user");
    settings.endArray();
    settings.beginWriteArray("user");
    settings.setArrayIndex(index);
    settings.setValue("name",QVariant(user->name));
    settings.setValue("power", QVariant(user->power));
    settings.setValue("cardId",QVariant(user->cardId));
    settings.endArray();
    settings.endGroup();
}

UserInfo *CabinetManager::checkUserLocal(QString userId)
{
    UserInfo* ret = NULL;
    QSettings settings(FILE_CONFIG_CABINET_LAYOUT,QSettings::IniFormat);
    int i = 0;
    settings.beginGroup(QString("Users"));
    int index = settings.beginReadArray("user");

    for(i=0; i<index; i++)
    {
        settings.setArrayIndex(i);
//        qDebug()<<userId<<settings.value("cardId", QString()).toString();
        if(userId == settings.value("cardId", QString()).toString())
        {
            ret = new UserInfo;
            qDebug()<<settings.value("name", QString()).toString();
            ret->name = settings.value("name", QString()).toString();
            ret->power = settings.value("power", -1).toInt();
            ret->cardId = settings.value("cardId", QString()).toString();
            break;
        }
    }
    settings.endArray();
    settings.endGroup();
    return ret;
}

bool CabinetManager::checkManagers(QString id)
{
    QFile fManager("/home/config/managers.ini");

    if(!fManager.exists())
        return false;

    fManager.open(QFile::ReadOnly);
    QString managerStr = QString(fManager.readAll());
    fManager.close();

    QStringList managers = managerStr.split(' ');
//    qDebug()<<"[managers]"<<managers<<userId<<managers.indexOf(userId);
//    qDebug()<<managers.at(7);

    if(managers.isEmpty())
        return false;
    if(managers.indexOf(id) == -1)
        return false;
    else
        return true;
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
    QSettings settings(FILE_CONFIG_CABINET_LAYOUT, QSettings::IniFormat);
    settings.setValue(key, value);
    settings.sync();
}

QVariant CabinetManager::getConfig(QString key, QVariant defaultRet)
{
    QSettings settings(FILE_CONFIG_CABINET_LAYOUT, QSettings::IniFormat);
    return settings.value(key, defaultRet);
}

void CabinetManager::removeConfig(QString path)
{
    QSettings settings(FILE_CONFIG_CABINET_LAYOUT, QSettings::IniFormat);
    settings.remove(path);
}
/********base functions end*******/
