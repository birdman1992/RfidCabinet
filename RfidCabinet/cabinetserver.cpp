#include "cabinetserver.h"
#include <QString>
#include <QDebug>
#include <QTime>
#include <QUrl>
#include <QtGlobal>
#include <fcntl.h>
#include <unistd.h>
#include "config.h"
//#include "defines.h"
//#include "Device/controldevice.h"
//#define RECV_DEBUG

//#define SERVER_ADDR "http://175.11.185.181"
#define SERVER_ADDR "http://120.77.159.8:8080"
#define API_REG "/spd-web/sarkApi/SmartCheset/saveOrUpdate/"   //注册接口
#define API_INFO_UPLOAD ""    //柜子信息上传接口
#define API_INFO_REQ "/spd-web/sarkApi/SmartCheset/query/"      //柜子信息查询接口
#define API_CLONE_REQ "/spd-web/sarkApi/Cheset/syncCheset/"     //柜子克隆请求接口
#define API_CLONE_SYNC  ""      //柜子克隆数据同步接口
#define API_INSERT_COL "/spd-web/sarkApi/Cheset/doUpdateChesetCol/"//列插入接口
#define API_LOGIN "/spd-web/sarkApi/UserInfo/query/"  //登录接口
#define API_LIST_CHECK "/spd-web/sarkApi/OutStorage/query/goods/" //送货单检查接口
#define API_LIST_STORE "/spd-web/sarkApi/OutStorage/query/"      //存入完毕销单接口
#define API_CAB_BIND "/spd-web/sarkApi/Cheset/register/"     //柜格物品绑定接口
#define API_GOODS_ACCESS  "/spd-web/sarkApi/Cheset/rfid/doGoods/"
//#define API_GOODS_CHECK  "/spd-web/sarkApi/Cheset/doUpdataGoods/"     //盘点接口
#define API_GOODS_CHECK    "/spd-web/sarkApi/Cheset/checkCheset/"      //盘点接口
#define API_CHECK_CREAT     "/spd-web/sarkApi/TakeStockCheset/create/"       //创建盘点
#define API_CHECK_END       "/spd-web/sarkApi/TakeStockCheset/end/"          //结束盘点
#define API_CHECK_TIME "/spd-web/sarkApi/Time/query/"
#define API_REQ_LIST "/spd-web/sarkApi/OutStorage/find/OutStorageCar/"      //查询是否有送货单在途中
#define API_LIST_CHECK_NEW "/spd-web/sarkApi/OutStorage/queryfind/rfid/goods/"     //查询待存送货单接口NEW
#define API_NETSTATE_CHECK "/spd-web/websocket/"    //网络状态检查
#define API_CHECK_TABLES "/spd-web/sarkApi/TakeStockCheset/query/takestockList/"    //查询盘点清单表
#define API_CHECK_INFO "/spd-web/sarkApi/TakeStockCheset/query/takestockGoodsList/"    //查询盘点清单内容
#define API_SEARCH_SPELL "/spd-web/sarkApi/Cheset/query/chesetGoods/"     //首字母搜索物品
#define API_GOODS_REPLY "/spd-web/sarkApi/Cheset/doPleaseGoods/"  //请货
#define API_DAY_REPORT "/spd-web/sarkApi/Cheset/query/consumeDate/"  //日清单


CabinetServer::CabinetServer(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    cur_manager = new UserInfo();
    cabManager = CabinetManager::manager();
    repManager = RepertoryManager::manager();
    epcManager = EpcManager::manager();
    ApiAddress = QString();
    cabId = QString();
    regId = QString();
    rfidStep = 0;
    cur_user = NULL;
    checkList = NULL;
    reply_register = NULL;
    reply_login = NULL;
    reply_check_tables = NULL;
    reply_check_table_info = NULL;
    reply_list_check = NULL;
    reply_cabinet_bind = NULL;
    reply_goods_access = NULL;
    reply_goods_back = NULL;
    reply_goods_check = NULL;
    reply_goods_reply = NULL;
    reply_datetime = NULL;
    reply_list_state = NULL;
    reply_cabinet_info = NULL;
    reply_cabinet_clone = NULL;
    reply_update_col = NULL;
    reply_search_spell = NULL;
    reply_day_report = NULL;
    needClearBeforeClone = false;
    list_access_cache.clear();
    apiState = 0;
    needReqCar = true;
    needSaveAddress = false;
    fWatchdog = -1;
    checkId = -1;
#ifndef SIMULATE_ON
    watchdogStart();
#endif
    checkCabReg();
}

bool CabinetServer::initcabManager()
{
    getServerAddr(cabManager->getServerAddress());
//    if(ApiAddress.isEmpty())
//    {
//        ApiAddress = SERVER_ADDR;
//        cabManager->setServerAddress(ApiAddress);
//    }
//    checkTime();
//    cabManager->getCabinetId();

//    if(cabManager->getCabinetId().isEmpty())
//        cabRegister();
//    requireListState();
//    {
//        regId = "835999";
//        cabManager->setCabinetId(regId);
//    }

    return true;
}

void CabinetServer::checkCabReg()
{
    cabId = cabManager->getCabinetId();
    if(cabId.isEmpty())
    {
        QTimer::singleShot(10000, this, SLOT(checkCabReg()));
        cabRegister();
        emit reqLockWarning(1);
    }
}

void CabinetServer::rfidScanFinish()
{
    if(cur_user == NULL)
        return;

    list_access_cache.clear();

    list_access_cache<<epcManager->storeClearing();
    list_access_cache<<epcManager->fetchClearing(cur_user->cardId);
    list_access_cache<<epcManager->backClearing(cur_user->cardId);

    rfidAccess();
}

void CabinetServer::rfidAccess()
{
    if(list_access_cache.isEmpty())
        return;

    QByteArray qba = list_access_cache.takeFirst();
    if(qba.isEmpty())
    {
        rfidAccess();
        return;
    }

    QString nUrl = ApiAddress+QString(API_GOODS_ACCESS);
    qDebug()<<"[listAccess]"<<nUrl;
    qDebug()<<qba;
    replyCheck(reply_goods_access);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(nUrl);
    reply_goods_access = manager->post(request, qba.toBase64());
    connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));
}

void CabinetServer::waitForListTimeout()
{

}

void CabinetServer::cabRegister()
{
#ifdef NO_SERVER
    qDebug()<<"[Cabinet register]:success"<<regId;
    cabManager->setCabinetId(regId);
#endif
    if(regId.isEmpty())
    {
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        regId = QString::number(qrand()%1000000);
        while(regId.length() < 6)
        {
            regId.insert(0,'0');
        }
        regId.insert(0, "R");
    }
    QByteArray qba = QString("{\"code\":\"%1\"}").arg(regId).toUtf8();
    QString nUrl = ApiAddress+QString(API_REG)+'?'+qba.toBase64();
    qDebug()<<"[cabRegister]"<<nUrl<<qba;

    replyCheck(reply_register);
    reply_register = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_register, SIGNAL(finished()), this, SLOT(recvCabRegister()));
}

void CabinetServer::checkTime()
{
    timeIsChecked = false;

    replyCheck(reply_datetime);

    QString url = ApiAddress + QString(API_CHECK_TIME);
    reply_datetime = manager->get(QNetworkRequest(QUrl(url)));
    qDebug()<<"[checkTime]"<<url;
    connect(reply_datetime, SIGNAL(readyRead()), this, SLOT(recvDateTime()));
//    connect(reply_datetime, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(recvDateTimeError(QNetworkReply::NetworkError)));

    sysClock.stop();
    sysClock.start(60000);
    connect(&sysClock, SIGNAL(timeout()), this, SLOT(sysTimeout()));
    netTimeStart();
}

void CabinetServer::checkSysTime(QDateTime _time)
{
    QProcess pro;
    QString cmd = QString("date -s \"%1\"").arg(_time.toString("yyyy-MM-dd hh:mm:ss"));
    qDebug()<<"[checkSysTime]"<<cmd;
    pro.start(cmd);
    pro.waitForFinished(1000);
    emit timeUpdate();
//    pro.start("clock -w");
    //    pro.waitForFinished(1000);
}

void CabinetServer::requireListState()
{
    QByteArray qba = QString("{\"code\":\"%1\"}").arg(cabManager->getCabinetId()).toUtf8();
    QString url = ApiAddress + QString(API_REQ_LIST) +'?'+ qba.toBase64();

    replyCheck(reply_list_state);
    reply_list_state = manager->get(QNetworkRequest(QUrl(url)));
    qDebug()<<"[requireListState]"<<url;
    qDebug()<<qba;
    connect(reply_list_state, SIGNAL(readyRead()), this, SLOT(recvListState()));

    netTimeStart();
//    sysClock.start(60000);
    //    connect(&sysClock, SIGNAL(timeout()), this, SLOT(sysTimeout()));
}

void CabinetServer::replyCheck(QNetworkReply *reply)
{
    if(reply != NULL)
        reply->deleteLater();
}

void CabinetServer::checkGoodsList()
{
    if(list_goodsList.isEmpty())
    {
        return;
    }

    listCheck(list_goodsList.takeFirst());
}

void CabinetServer::netTimeStart()
{
    netFlag = false;
    QTimer::singleShot(10000,this,SLOT(netTimeout()));
}

//=
void CabinetServer::localCacheAccess()
{
//    list_access_cache = cabManager->getFetchList();

//    accessLoop();
}

void CabinetServer::accessLoop()
{
    if(list_access_cache.isEmpty())
        return;

    QByteArray qba = list_access_cache.takeFirst();
    QString nUrl = ApiAddress+QString(API_GOODS_ACCESS)+"?"+qba;
    qDebug()<<"[accessLoop]"<<nUrl;
    qDebug()<<QByteArray::fromBase64(qba);
    replyCheck(reply_goods_access);
    reply_goods_access = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));
}

QString CabinetServer::getAbbName(QString fullName)
{
    if(fullName.indexOf("一次性") == 0)
    {
        fullName = fullName.remove(0,3);
    }
    if(fullName.indexOf("使用") == 0)
    {
        fullName = fullName.remove(0,2);
    }
    if(fullName.indexOf("医用") == 0)
    {
        fullName = fullName.remove(0,2);
    }

    return fullName;
}

void CabinetServer::watchdogStart()
{
    fWatchdog = -1;
    fWatchdog = open("/dev/watchdog", O_WRONLY);
    if (fWatchdog == -1)
    {
        qDebug()<<"[watchdog] watchdog start failed.";
        return;
    }
    qDebug()<<"[watchdog]"<<"start";
    watdogClock.start(10000);
    connect(&watdogClock, SIGNAL(timeout()), this, SLOT(watchdogTimeout()));
}

void CabinetServer::getServerAddr(QString addr)
{
    qDebug("[getServerAddr]");
    ApiAddress = QString("http://") + addr;
    needSaveAddress = true;
//    cabManager->setServerAddress(ApiAddress);

//    if(cabManager->getCabinetId().isEmpty())
//        cabRegister();
//    {
//        regId = "835999";
//        cabManager->setCabinetId(regId);
//    }
    checkTime();
}

void CabinetServer::userLogin(QString userId)
{
//#ifdef NO_SERVER
//    emit loginRst(true);
//#endif
    QByteArray qba = QString("{\"cardId\":\"%2\",\"departId\":\"%1\"}").arg(cabManager->getCabinetId()).arg(userId).toUtf8();
    QString nUrl = ApiAddress+QString(API_LOGIN)+'?'+qba.toBase64();
    qDebug()<<"[login]"<<nUrl;
    qDebug()<<qba;
    logId = userId;

    replyCheck(reply_login);
    reply_login = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_login, SIGNAL(finished()), this, SLOT(recvUserLogin()));
    apiState = 1;
    netTimeStart();
}

void CabinetServer::listCheck(QString code)
{
    QByteArray qba = QString("{\"barcode\":\"%1\"}").arg(code).toUtf8();
    QString nUrl = ApiAddress+QString(API_LIST_CHECK_NEW)+'?'+qba.toBase64();
    barCode = code;
    qDebug()<<"[listCheck]"<<nUrl;
    replyCheck(reply_list_check);
    reply_list_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_list_check, SIGNAL(finished()), this, SLOT(recvListCheck()));
}

//=
void CabinetServer::cabInfoUpload()
{
//    QByteArray qba = cabManager->creatCabinetJson();
//    QString nUrl = ApiAddress+QString(API_INFO_UPLOAD)+"?"+qba.toBase64();
//    qDebug()<<"[cabInfoUpload]"<<nUrl<<qba;
//    replyCheck(reply_cabinet_info);
//    reply_cabinet_info = manager->get(QNetworkRequest(QUrl(nUrl)));
//    connect(reply_cabinet_info, SIGNAL(finished()), this, SLOT(recvInfoUploadResult()));
}

void CabinetServer::cabInfoReq()
{
    QByteArray qba = QString("{\"chesetCode\"\":%1\"}").arg(cabManager->getCabinetId()).toUtf8();
    QString nUrl = ApiAddress+QString(API_INFO_REQ)+"?"+qba.toBase64();
    qDebug()<<"[cabInfoReq]"<<nUrl<<qba;
}

void CabinetServer::cabCloneReq(QString oldCabinetId)
{
    regId = oldCabinetId;
    QByteArray qba = QString("{\"code\":\"%1\"}").arg(oldCabinetId).toUtf8();
    QString nUrl = ApiAddress+QString(API_CLONE_REQ)+"?"+qba.toBase64();
    replyCheck(reply_cabinet_clone);
    reply_cabinet_clone = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_cabinet_clone, SIGNAL(finished()), this, SLOT(recvCabClone()));
    qDebug()<<"[cabCloneReq]"<<nUrl<<qba;
}

void CabinetServer::cabInfoSync()//同步柜子库存信息
{
    QString cabId = cabManager->getCabinetId();
    QByteArray qba = QString("{\"code\":\"%1\"}").arg(cabId).toUtf8();
    QString nUrl = ApiAddress+QString(API_CLONE_REQ)+"?"+qba.toBase64();
    needClearBeforeClone = true;
    replyCheck(reply_cabinet_clone);
    reply_cabinet_clone = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_cabinet_clone, SIGNAL(finished()), this, SLOT(recvCabSync()));
    qDebug()<<"[cabInfoSync]"<<nUrl<<qba;
}

void CabinetServer::cabColInsert(int pos, int num)
{
    QString cabId = cabManager->getCabinetId();
    QByteArray qba = QString("{\"departCode\":\"%1\",\"col\":[\"%2\",\"%3\"]}").arg(cabId).arg(pos).arg(num).toUtf8();
    QString nUrl = ApiAddress+QString(API_INSERT_COL)+"?"+qba.toBase64();
    replyCheck(reply_update_col);
    reply_update_col = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_update_col, SIGNAL(finished()), this, SLOT(recvColInsert()));
    qDebug()<<"[cabColInsert]"<<nUrl<<qba;
}

void CabinetServer::cabinetBind(int seqNum, int index, QString goodsId)
{
    if(!networkState)
    {
        emit bindRst(false);
        return;
    }
    QString optId = logId;
    QString caseId = QString::number(cabManager->getLockId(seqNum, index));
    QString cabinetId = cabManager->getCabinetId();
    QByteArray qba = QString("{\"optName\":\"%6\",\"goodsId\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\",\"cabinetRow\":%4,\"cabinetCol\":%5}").arg(goodsId).arg(cabinetId).arg(caseId).arg(index).arg(seqNum).arg(optId).toUtf8();
//    QByteArray qba = QString("{\"goodId\":\"%1\",\"chesetCode\":\"%2\",\"caseId\":\"%3\"}").arg(goodsId).arg(cabinetId).arg(caseId).toUtf8();
    QString nUrl = ApiAddress+QString(API_CAB_BIND)+"?"+qba.toBase64();
    qDebug()<<"[cabinetBind]"<<nUrl<<"\n"<<qba;
    replyCheck(reply_cabinet_bind);
    reply_cabinet_bind = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_cabinet_bind, SIGNAL(finished()), this, SLOT(recvCabBind()));
}

void CabinetServer::goodsAccess(CaseAddress addr, QString id, int num, int optType)
{qDebug()<<addr.cabinetSeqNum<<id<<num<<optType;
    QString caseId = QString::number(cabManager->getLockId(addr.cabinetSeqNum, addr.caseIndex));
    QString cabinetId = cabManager->getCabinetId();
    QString optName = logId;
    QByteArray qba;

    if(optType == 2)
        qba = QString("{\"optName\":\"%7\",\"li\":[{\"packageBarcode\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\",\"optType\":%4,\"optCount\":%5,\"barcode\":\"%6\"}]}")
             .arg(id).arg(cabinetId).arg(caseId).arg(2).arg(num).arg(barCode).arg(optName).toUtf8();
    else if(optType == 1)
        qba = QString("{\"optName\":\"%6\",\"li\":[{\"packageBarcode\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\",\"optType\":%4,\"optCount\":%5}]}")
             .arg(id).arg(cabinetId).arg(caseId).arg(1).arg(num).arg(optName).toUtf8();
    else if(optType == 3)
        qba = QString("{\"optName\":\"%6\",\"li\":[{\"packageBarcode\":\"%1\",\"chesetCode\":\"%2\",\"goodsCode\":\"%3\",\"optType\":%4,\"optCount\":%5}]}")
             .arg(id).arg(cabinetId).arg(caseId).arg(3).arg(num).arg(optName).toUtf8();

//    QString nUrl = ApiAddress+QString(API_GOODS_ACCESS)+"?"+qba.toBase64();
    QString nUrl = ApiAddress+QString(API_GOODS_ACCESS);
    qDebug()<<"[goodsAccess]"<<nUrl;
    qDebug()<<qba;
    replyCheck(reply_goods_access);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(nUrl);
//    reply_goods_access = manager->get(QNetworkRequest(QUrl(nUrl)));
    reply_goods_access = manager->post(request, qba.toBase64());
    connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));

}

void CabinetServer::listAccess(QStringList list, int optType)//fetch:1  store:2 refund:3  rfidback:16
{
    cJSON* json = cJSON_CreateObject();
    cJSON* jlist = cJSON_CreateArray();

    QString pack_bar;
    int i = 0;

    for(i=0; i<list.count(); i++)
    {
        pack_bar = list.at(i);
        QString pack_id = cabManager->scanDataTrans(pack_bar);
        QByteArray packageBarcode = pack_bar.toLocal8Bit();
        QByteArray chesetCode = cabManager->getCabinetId().toLocal8Bit();
        QByteArray barcode = barCode.toLocal8Bit();
        QByteArray goodsCode = "0";
//        CaseAddress addr = cabManager->checkCabinetByBarCode(pack_id);
//        QByteArray goodsCode = QString::number(cabManager->getLockId(addr.cabinetSeqNum, addr.caseIndex)).toLocal8Bit();

        cJSON* obj = cJSON_CreateObject();
        cJSON_AddItemToObject(obj, "packageBarcode",cJSON_CreateString(packageBarcode.data()));
        cJSON_AddItemToObject(obj, "chesetCode", cJSON_CreateString(chesetCode.data()));
        cJSON_AddItemToObject(obj, "optType", cJSON_CreateNumber(optType));
        cJSON_AddItemToObject(obj, "goodsCode", cJSON_CreateString(goodsCode.data()));
        if(optType == 2)
        {
            cJSON_AddItemToObject(obj, "barcode", cJSON_CreateString(barcode.data()));
        }
        cJSON_AddItemToObject(obj, "optCount", cJSON_CreateNumber(1));
        if(cur_user != NULL)
        {
            QByteArray optCard = cur_user->cardId.toLocal8Bit();
            QByteArray optTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss").toLocal8Bit();
            cJSON_AddItemToObject(obj, "name", cJSON_CreateString(optCard.data()));
            cJSON_AddItemToObject(obj, "takeOutTime", cJSON_CreateString(optTime.data()));
        }
        cJSON_AddItemToArray(jlist, obj);
    }
    cJSON_AddItemToObject(json, "li",jlist);
    QByteArray optId = logId.toLocal8Bit();
    cJSON_AddItemToObject(json,"optName", cJSON_CreateString(optId.data()));
    char* buff = cJSON_Print(json);

    cJSON_Delete(json);
    QByteArray qba = QByteArray(buff);

    if(!networkState)
    {
        cabManager->saveFetchList(qba.toBase64());
        free(buff);
    }
    else
    {
//        QString nUrl = ApiAddress+QString(API_GOODS_ACCESS)+"?"+qba.toBase64();
        QString nUrl = ApiAddress+QString(API_GOODS_ACCESS);
        qDebug()<<"[listAccess]"<<nUrl;
        qDebug()<<qba;
        replyCheck(reply_goods_access);
        QNetworkRequest request;
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setUrl(nUrl);
//        reply_goods_access = manager->get(QNetworkRequest(QUrl(nUrl)));
        reply_goods_access = manager->post(request, qba.toBase64());
        connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));
        free(buff);
    //    qDebug()<<"[list fetch]"<<cJSON_Print(json);
    }
}

void CabinetServer::goodsCheckReq()
{
    QString cabId = cabManager->getCabinetId();
    QByteArray qba = QString("{\"departCode\":\"%1\"}").arg(cabId).toUtf8();
    QString nUrl = ApiAddress+QString(API_CHECK_CREAT)+"?"+qba.toBase64();
    replyCheck(reply_goods_check);
    reply_goods_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_check, SIGNAL(finished()), this, SLOT(recvCheckCreat()));
    qDebug()<<"[goodsCheckCreat]"<<nUrl<<qba;
}

void CabinetServer::goodsCheckFinish()
{
    if(checkId == -1)
        return;
    QString cabId = cabManager->getCabinetId();
    QByteArray qba = QString("{\"departCode\":\"%1\"}").arg(cabId).toUtf8();
    QString nUrl = ApiAddress+QString(API_CHECK_END)+"?"+qba.toBase64();
    replyCheck(reply_goods_check);
    reply_goods_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_check, SIGNAL(finished()), this, SLOT(recvCheckFinish()));
    qDebug()<<"[goodsCheckFinish]"<<nUrl<<qba;
}

void CabinetServer::goodsCheck(QList<CabinetCheckItem *> l, CaseAddress addr)
{
    cJSON* json = cJSON_CreateObject();
    cJSON* packageList = cJSON_CreateArray();

    CabinetCheckItem* item;
    int i = 0;

    QByteArray chesetCode = cabManager->getCabinetId().toLocal8Bit();
    QByteArray goodsCode = QString::number(cabManager->getLockId(addr.cabinetSeqNum, addr.caseIndex)).toLocal8Bit();
    cJSON_AddItemToObject(json, "departCode", cJSON_CreateString(chesetCode.data()));
    cJSON_AddItemToObject(json, "cabinetId", cJSON_CreateString(goodsCode.data()));

    for(i=0; i<l.count(); i++)
    {
        item = l.at(i);
        QByteArray packageBarcode = item->itemId().toLocal8Bit();

        cJSON* codeList = cJSON_CreateArray();
        cJSON* package = cJSON_CreateObject();
        cJSON_AddItemToObject(package, "packageType", cJSON_CreateString(packageBarcode.data()));

        int j = 0;
        for(j=0; j<item->list_fullId.count(); j++)
        {
            QByteArray qba = item->list_fullId.at(j).toLocal8Bit();
            cJSON_AddItemToArray(codeList, cJSON_CreateString(qba.data()));
        }

        cJSON_AddItemToObject(package, "packageCode", codeList);
        cJSON_AddItemToArray(packageList, package);
    }
    cJSON_AddItemToObject(json, "packageList",packageList);
    QByteArray optId = logId.toLocal8Bit();
    cJSON_AddItemToObject(json,"optName", cJSON_CreateString(optId.data()));

    char* buff = cJSON_Print(json);
    cJSON_Delete(json);
    QByteArray qba = QByteArray(buff);

    QString nUrl = ApiAddress+QString(API_GOODS_CHECK);
    qDebug()<<"[goodsCheck]"<<nUrl;
    qDebug()<<qba;
    replyCheck(reply_goods_check);
//    reply_goods_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(nUrl);
    reply_goods_check = manager->post(request, qba.toBase64());
    connect(reply_goods_check, SIGNAL(finished()), this, SLOT(recvGoodsCheck()));
    free(buff);
}

void CabinetServer::goodsCheck(QStringList l, CaseAddress)
{
    cJSON* json = cJSON_CreateObject();
    cJSON* jlist = cJSON_CreateArray();
    int i = 0;

    QByteArray optId = logId.toLocal8Bit();
    cJSON_AddItemToObject(json,"optName", cJSON_CreateString(optId.data()));

    QByteArray chesetCode = cabManager->getCabinetId().toLocal8Bit();
    cJSON_AddItemToObject(json, "departCode", cJSON_CreateString(chesetCode.data()));

    for(i=0; i<l.count(); i++)
    {
        QByteArray packageBarcode = l.at(i).toLocal8Bit();
        cJSON_AddItemToArray(jlist, cJSON_CreateString(packageBarcode.data()));
    }
    cJSON_AddItemToObject(json, "packageCode",jlist);
    char* buff = cJSON_Print(json);
    cJSON_Delete(json);
    QByteArray qba = QByteArray(buff);

    QString nUrl = ApiAddress+QString(API_GOODS_CHECK);
    qDebug()<<"[goodsCheck]"<<nUrl;
    qDebug()<<qba;
    replyCheck(reply_goods_check);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(nUrl);
    reply_goods_check = manager->post(request, qba.toBase64());
//    reply_goods_check = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_check, SIGNAL(finished()), this, SLOT(recvGoodsCheck()));
    free(buff);
}

void CabinetServer::goodsListStore(QList<CabinetStoreListItem *> l)
{
    qDebug("goodsListStore");
    qDebug()<<l.count();

//    return;

    cJSON* json = cJSON_CreateObject();
    cJSON* jlist = cJSON_CreateArray();

    CabinetStoreListItem* goodsItem;
    int i = 0;
    int optType = 2;

    for(i=0; i<l.count(); i++)
    {
        goodsItem = l.at(i);
        qDebug()<<goodsItem->itemId()<<goodsItem->itemNum();
        QString pack_id = goodsItem->itemId();
        QByteArray packageBarcode = goodsItem->itemId().toLocal8Bit();
        QByteArray chesetCode = cabManager->getCabinetId().toLocal8Bit();
//        CaseAddress addr = cabManager->checkCabinetByBarCode(pack_id);
        QByteArray barcode = barCode.toLocal8Bit();
//        QByteArray goodsCode = QString::number(cabManager->getLockId(addr.cabinetSeqNum, addr.caseIndex)).toLocal8Bit();
        cJSON* obj = cJSON_CreateObject();
        cJSON_AddItemToObject(obj, "packageBarcode",cJSON_CreateString(packageBarcode.data()));
        cJSON_AddItemToObject(obj, "chesetCode", cJSON_CreateString(chesetCode.data()));
        cJSON_AddItemToObject(obj, "optType", cJSON_CreateNumber(optType));
//        cJSON_AddItemToObject(obj, "goodsCode", cJSON_CreateString(goodsCode.data()));
        cJSON_AddItemToObject(obj, "optCount", cJSON_CreateNumber(goodsItem->itemNum()));
        cJSON_AddItemToObject(obj, "barcode", cJSON_CreateString(barcode.data()));
        cJSON_AddItemToArray(jlist, obj);
    }
    cJSON_AddItemToObject(json, "li",jlist);

    QByteArray optId = logId.toLocal8Bit();
    cJSON_AddItemToObject(json,"optName", cJSON_CreateString(optId.data()));

    char* buff = cJSON_Print(json);
    cJSON_Delete(json);
    QByteArray qba = QByteArray(buff);

//    QString nUrl = ApiAddress+QString(API_GOODS_ACCESS)+"?"+qba.toBase64();
    QString nUrl = ApiAddress+QString(API_GOODS_ACCESS);
    qDebug()<<"[goodsListStore]"<<nUrl;
    qDebug()<<qba;
    replyCheck(reply_goods_access);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setUrl(nUrl);
//    reply_goods_access = manager->get(QNetworkRequest(QUrl(nUrl)));
    reply_goods_access = manager->post(request, qba.toBase64());
    connect(reply_goods_access, SIGNAL(finished()), this, SLOT(recvListAccess()));
    free(buff);
}

void CabinetServer::goodsCarScan()
{
    needReqCar = true;//打开定时查询
}

void CabinetServer::goodsBack(QString)
{
//    QByteArray qba = QString("{\"barcode\":\"%1\"}").arg(goodsId).toUtf8();
//    QString nUrl = ApiAddress+QString(API_GOODS_BACK)+'?'+qba.toBase64();
//    qDebug()<<"[ck]"<<nUrl;
    return;
//    reply_goods_back = manager->get(QNetworkRequest(QUrl(nUrl)));
//    connect(reply_goods_back, SIGNAL(finished()), this, SLOT(recvGoodsBack()));
}

void CabinetServer::requireCheckTables(QDate start, QDate finish)
{
    QString cabId = cabManager->getCabinetId();
    QByteArray qba = QString("{\"departCode\":\"%1\",\"sTime\":\"%2 00:00:00\",\"eTime\":\"%3 23:59:59\"}").arg(cabId).arg(start.toString("yyyy-MM-dd")).arg(finish.toString("yyyy-MM-dd")).toUtf8();
    QString nUrl = ApiAddress+QString(API_CHECK_TABLES)+"?"+qba.toBase64();
    replyCheck(reply_check_tables);
    reply_check_tables = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_check_tables, SIGNAL(finished()), this, SLOT(recvCheckTables()));
    qDebug()<<"[requireCheckTables]"<<nUrl<<qba;
}

void CabinetServer::requireCheckTableInfo(QString id)
{
    QString cabId = cabManager->getCabinetId();
    QByteArray qba = QString("{\"id\":\"%1\"}").arg(id.toInt()).toUtf8();
    QString nUrl = ApiAddress+QString(API_CHECK_INFO)+"?"+qba.toBase64();
    replyCheck(reply_check_table_info);
    reply_check_table_info = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_check_table_info, SIGNAL(finished()), this, SLOT(recvCheckTableInfo()));
    qDebug()<<"[requireCheckTableInfo]"<<nUrl<<qba;
}

//获取日清单信息
void CabinetServer::requireListInfo(QDate sDate, QDate eDate)
{
    QString cabId = cabManager->getCabinetId();
    QByteArray qba = QString("{\"departCode\":\"%1\", \"sTime\":\"%2\",\"eTime\":\"%3\"}").arg(cabId).arg(sDate.toString("yyyy-MM-dd")).arg(eDate.toString("yyyy-MM-dd")).toUtf8();
    QString nUrl = ApiAddress+QString(API_DAY_REPORT)+"?"+qba.toBase64();
    replyCheck(reply_day_report);
    reply_day_report = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_day_report, SIGNAL(finished()), this, SLOT(recvDayReportInfo()));
    qDebug()<<"[requireCheckTableInfo]"<<nUrl<<qba;
}

void CabinetServer::searchSpell(QString spell)
{
    QByteArray qba = QString("{\"spell\":\"%1\", \"departCode\":\"%2\"}").arg(spell).arg(cabManager->getCabinetId()).toLocal8Bit();
    QString nUrl = ApiAddress+QString(API_SEARCH_SPELL)+"?"+qba.toBase64();
    replyCheck(reply_search_spell);
    reply_search_spell = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_search_spell, SIGNAL(finished()), this, SLOT(recvSearchSpell()));
    qDebug()<<"[searchSpell]"<<nUrl<<qba;
}

void CabinetServer::replyRequire(QList<GoodsCheckInfo *> l)
{
    QByteArray optName = cur_user->cardId.toLocal8Bit();
    QByteArray departCode = cabManager->getCabinetId().toLocal8Bit();

    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "optName", cJSON_CreateString(optName.data()));
    cJSON_AddItemToObject(json, "departCode", cJSON_CreateString(departCode.data()));
    cJSON* StoreGoodsModel = cJSON_CreateArray();
    foreach(GoodsCheckInfo* info, l)
    {
        cJSON* goods = cJSON_CreateObject();
        cJSON_AddItemToObject(goods, "goodsId", cJSON_CreateString(info->id.toLocal8Bit()));
        cJSON_AddItemToObject(goods, "goodsCount", cJSON_CreateNumber(info->num_pack*info->type));
        cJSON_AddItemToObject(goods, "packageBarcode", cJSON_CreateString(info->packageBarCode.toLocal8Bit()));
        cJSON_AddItemToArray(StoreGoodsModel, goods);
    }
    cJSON_AddItemToObject(json, "li", StoreGoodsModel);
    QByteArray qba = QByteArray(cJSON_Print(json));
    cJSON_Delete(json);
    qDeleteAll(l.begin(), l.end());
    QString nUrl = ApiAddress+QString(API_GOODS_REPLY)+"?"+qba.toBase64();
    replyCheck(reply_goods_reply);
    reply_goods_reply = manager->get(QNetworkRequest(QUrl(nUrl)));
    connect(reply_goods_reply, SIGNAL(finished()), this, SLOT(recvGoodsReply()));
    qDebug()<<"[replyRequire]"<<nUrl<<qba;
}

void CabinetServer::recvCabRegister()
{
    QByteArray qba = QByteArray::fromBase64(reply_register->readAll());
    reply_register->deleteLater();
    reply_register = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCabRegister]"<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");

    if(json_rst->type == cJSON_True)
    {
        qDebug()<<"[Cabinet register]:success"<<regId;
        cabManager->setCabinetId(regId);
        qDebug()<<"reg"<<cabManager->getCabinetId();
        emit idUpdate();
        emit regResult(true);
        emit reqLockWarning(4);
    }
    else
    {
        emit regResult(false);
        regId = QString();
//        cabRegister();
    }
    cJSON_Delete(json);
}

void CabinetServer::recvUserLogin()
{
    QByteArray qba = QByteArray::fromBase64(reply_login->readAll());
    reply_login->deleteLater();
    reply_login = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<cJSON_Print(json);

    if(!json)
        return;
    apiState = 0;
    netFlag = true;
    emit netState(true);
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        UserInfo* info = new UserInfo;
        cJSON* json_data = cJSON_GetObjectItem(json,"data");
        if(cJSON_GetArraySize(json_data) <= 0)
        {
            cJSON_Delete(json);
            return;
        }

        cJSON* json_info = cJSON_GetArrayItem(json_data,0);
        info->id = cJSON_GetObjectItem(json_info,"id")->valueint;
        info->cardId = QString(cJSON_GetObjectItem(json_info,"cardId")->valuestring);
        info->departId = QString(cJSON_GetObjectItem(json_info,"departId")->valuestring);
        info->identityId = QString(cJSON_GetObjectItem(json_info,"identityId")->valuestring);
        info->name = QString(cJSON_GetObjectItem(json_info,"name")->valuestring);
        info->power = cJSON_GetObjectItem(json_info,"power")->valueint;
        info->tel = QString(cJSON_GetObjectItem(json_info,"tel")->valuestring);
        qDebug()<<"[recvUserLogin]"<<info->cardId<<info->power;
        cur_user = info;
        emit loginRst(info);
        cabManager->addUser(info);
//        cabManager->wakeUp(TIMEOUT_BASE);
        networkState = true;
        requireListState();//触发查询待存送货单
    }
    else
    {
        cJSON_Delete(json);
        return;
    }
    cJSON_Delete(json);
}

QByteArray test = QByteArray("{\
                             \"callTime\": 0,\
                             \"success\": true,\
                             \"errorCode\": \"5000\",\
                             \"msg\": \"\",\
                             \"data\": {\
                               \"goods\": [     \
                                 {\
                                   \"name\": \"3M高强度外壳胶带（丝绸布胶带）\",\
                                   \"goodsId\": \"A0201002349\",\
                                   \"size\": \"1538-0\",\
                                   \"unit\": \"卷\",\
                                   \"packageCount\": 20,\
                                   \"singlePrice\": 10.00,\
                                   \"packageBarcode\": \"A0201002349\",\
                                   \"packageType\": 1,\
                                   \"batchNumber\": 20170523,\
                                   \"inStorageId\": 42,\
                                   \"lifeTime\": null,\
                                   \"roomName\": \"丝绸\",\
                                   \"producerName\": null\
                                 },\
                                {\
                                \"name\": \"BD真空测试包\",\
                                \"goodsId\": \"EO501002219\",\
                                \"size\": \"1538-0\",\
                                \"unit\": \"个\",\
                                \"packageCount\": 20,\
                                \"singlePrice\": 10.00,\
                                \"packageBarcode\": \"EO501002219\",\
                                \"packageType\": 1,\
                                \"batchNumber\": 20170523,\
                                \"inStorageId\": 42,\
                                \"lifeTime\": null,\
                                \"roomName\": \"丝绸\",\
                                \"producerName\": null\
                                }\
                               ],\
                               \"store\": {\
                                 \"id\": 0,\
                                 \"barcode\": \"T220170523094959259\",\
                                 \"carId\": 3,\
                                 \"totalPrice\": 200.00,\
                                 \"hosId\": 0,\
                                 \"departId\": 0,\
                                 \"warehouseId\": 0,\
                                 \"deliveryMan\": \"yyx\",\
                                 \"state\": 0,\
                                 \"remark\": null,\
                                 \"optUser\": null,\
                                 \"optTime\": null,\
                                 \"hosName\": \"安化县人民医院\",\
                                 \"departName\": \"内科(智能柜)\",\
                                 \"warehouseName\": \"安化一人民医院中心库\",\
                                 \"nowTime\": \"2017-05-23 09:53:03\",\
                                 \"goods\": null\
                               }\
                             }\
                           }");

void CabinetServer::recvListCheck()
{
    QByteArray qba = QByteArray::fromBase64(reply_list_check->readAll());
//    QByteArray qba = test;
    reply_list_check->deleteLater();
    reply_list_check = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvListCheck]"<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        Goods* info;
        GoodsList* list = new GoodsList;
        list->chesetCode = cabManager->getCabinetId();
        list->optName = logId;
        cJSON* json_data = cJSON_GetObjectItem(json,"data");
        if(json_data->type == cJSON_NULL)
        {
            emit listRst(list);
            return;
        }

        cJSON* json_goods = cJSON_GetObjectItem(json_data,"goods");
        int listCount = cJSON_GetArraySize(json_goods);
        if(listCount <= 0)
        {
            cJSON_Delete(json);
            return;
        }
//读取定数包列表
        for(int i=0; i<listCount; i++)
        {
            info = new Goods;
            cJSON* json_info = cJSON_GetArrayItem(json_goods,i);
            info->batchNumber = cJSON_GetObjectItem(json_info,"batchNumber")->valueint;
            info->goodsId = QString::fromUtf8(cJSON_GetObjectItem(json_info,"goodsId")->valuestring);
            info->inStorageId = cJSON_GetObjectItem(json_info,"inStorageId")->valueint;
            info->name = QString::fromUtf8(cJSON_GetObjectItem(json_info,"name")->valuestring);
            info->abbName = QString::fromUtf8(cJSON_GetObjectItem(json_info,"abbName")->valuestring);
            info->packageBarcode = QString::fromUtf8(cJSON_GetObjectItem(json_info,"packageBarcode")->valuestring);
            info->packageType = cJSON_GetObjectItem(json_info, "packageType")->valueint;
            info->roomName = QString::fromUtf8(cJSON_GetObjectItem(json_info,"roomName")->valuestring);
            info->singlePrice = cJSON_GetObjectItem(json_info,"singlePrice")->valueint;
            info->size = QString::fromUtf8(cJSON_GetObjectItem(json_info,"size")->valuestring);
//            info->takeCount = cJSON_GetObjectItem(json_info,"takeCount")->valueint;
            info->takeCount = cJSON_GetObjectItem(json_info,"outCount")->valueint;//packageCount
            info->totalNum = info->takeCount;
            info->unit = QString::fromUtf8(cJSON_GetObjectItem(json_info,"unit")->valuestring);

            if(info->abbName == info->name)
            {
                info->abbName = getAbbName(info->name);
            }

            qDebug()<<"[goods]"<<info->name<<info->goodsId<<info->takeCount<<info->unit;
            list->addGoods(info);
        }

        cJSON* json_rfids = cJSON_GetObjectItem(json_data,"rfidCode");
        listCount = cJSON_GetArraySize(json_rfids);
        if(listCount <= 0)
        {
            cJSON_Delete(json);
            return;
        }
//读取rfid列表
        for(int i=0; i<listCount; i++)
        {
            cJSON* json_rfid = cJSON_GetArrayItem(json_rfids,i);
            QString packageBarcode = QString::fromUtf8(cJSON_GetObjectItem(json_rfid,"packageBarcode")->valuestring);
            QString rfid = QString::fromUtf8(cJSON_GetObjectItem(json_rfid,"rfidCode")->valuestring);
            list->addRfid(packageBarcode, rfid);
        }

        cJSON* json_list_info = cJSON_GetObjectItem(json_data,"store");
        list->barcode = QString::fromUtf8(cJSON_GetObjectItem(json_list_info, "barcode")->valuestring);
        if(cabManager->getCabinetId() == QString::fromUtf8(cJSON_GetObjectItem(json_list_info, "departName")->valuestring))
        {
//            emit listRst(list);
            //把rfid加入关注列表
            epcManager->addStoreList(list);
//            emit doorStareChanged(true);
        }
        else
        {
            delete list;
//            list = new GoodsList;
//            emit listRst(list);
        }
    }
    cJSON_Delete(json);
    checkGoodsList();
}

void CabinetServer::recvCabBind()
{
    QByteArray qba = QByteArray::fromBase64(reply_cabinet_bind->readAll());
    reply_cabinet_bind->deleteLater();
    reply_cabinet_bind = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCabBind]"<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        qDebug()<<"bind success";
        emit bindRst(true);
    }
    else
    {
        emit bindRst(false);
    }
    cJSON_Delete(json);
}

void CabinetServer::recvGoodsAccess()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_access->readAll());
    reply_goods_access->deleteLater();
    reply_goods_access = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<cJSON_Print(json);

    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        qDebug()<<"ACCESS success";
        cJSON* data = cJSON_GetObjectItem(json, "data");
        QString goodsId = QString::fromUtf8(cJSON_GetObjectItem(data,"goodsId")->valuestring);
        int goodsNum = cJSON_GetObjectItem(data, "packageCount")->valueint;
        emit goodsNumChanged(goodsId, goodsNum);
    }
    else
    {
        emit accessFailed(QString(cJSON_GetObjectItem(json,"msg")->valuestring));
    }
    cJSON_Delete(json);
}

void CabinetServer::recvListAccess()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_access->readAll());
    reply_goods_access->deleteLater();
    reply_goods_access = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvListAccess]";
    qDebug()<<cJSON_Print(json);

    if(!json)
    {
        rfidAccess();
        return;
    }

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        goodsCarScan();

        if(list_access_cache.count() == 2)
        {
            qDebug()<<"STORE success";
        }
        else if(list_access_cache.count() == 1)
        {
            qDebug()<<"FETCH success";
            epcManager->fetchRst();
        }
        else if(list_access_cache.count() == 0)
        {
            qDebug()<<"back success";
            epcManager->backRst();
        }

//        cJSON* data = cJSON_GetObjectItem(json, "data");
//        int listCount = cJSON_GetArraySize(data);
//        if(listCount <= 0)
//        {
//            cJSON_Delete(json);
//            return;
//        }
//        int i=0;
//        for(i=0; i<listCount; i++)
//        {
//            cJSON* item = cJSON_GetArrayItem(data, i);
//            QString goodsId = QString::fromUtf8(cJSON_GetObjectItem(item,"goodsId")->valuestring);
//            int goodsType = cJSON_GetObjectItem(item, "goodsType")->valueint;
//            int goodsNum = cJSON_GetObjectItem(item, "packageCount")->valueint;
//            float goodsPrice = cJSON_GetObjectItem(item, "price")->valuedouble;

//            if(goodsType<10)
//                goodsId += "-0"+QString::number(goodsType);
//            else
//                goodsId += "-"+QString::number(goodsType);

//            qDebug()<<goodsId<<goodsNum;

//            emit goodsNumChanged(goodsId, goodsNum);
//            emit accessSuccess(QString(cJSON_GetObjectItem(item,"msg")->valuestring));
//            emit updateGoodsPrice(goodsPrice, goodsPrice*goodsType);
//        }
    }
    else
    {
        emit accessFailed(QString(cJSON_GetObjectItem(json,"msg")->valuestring));
    }
    cJSON_Delete(json);

    rfidAccess();
//    accessLoop();
}

void CabinetServer::recvGoodsCheck()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_check->readAll());
    reply_goods_check->deleteLater();
    reply_goods_check = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvGoodsCheck]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        emit goodsCheckRst(QString());
    }
    else
    {
        emit goodsCheckRst(QString(cJSON_GetObjectItem(json, "msg")->valuestring));
    }

    cJSON_Delete(json);
}

void CabinetServer::recvCheckCreat()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_check->readAll());
    reply_goods_check->deleteLater();
    reply_goods_check = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCheckCreat]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        emit checkCreatRst(true);
        cJSON* jData = cJSON_GetObjectItem(json, "data");
        checkId = cJSON_GetObjectItem(jData,"id")->valueint;
    }
    else
    {
        emit checkCreatRst(false);
    }

    cJSON_Delete(json);
}

void CabinetServer::recvCheckFinish()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_check->readAll());
    reply_goods_check->deleteLater();
    reply_goods_check = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCheckFinish]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        qDebug()<<"check finished.";
        if(checkList != NULL)
            delete checkList;
        checkList = new CheckList();
        cJSON* jData = cJSON_GetObjectItem(json,"data");
        checkList->setCheckDateTime(QString(cJSON_GetObjectItem(jData, "strTime")->valuestring));
        checkList->departCode = QString(cJSON_GetObjectItem(jData, "departCode")->valuestring);
//        checkList->departCode = QString(cJSON_GetObjectItem(jData, "departCode")->valuestring);
        cJSON* jInfo = cJSON_GetObjectItem(jData, "goodsChesetEntityList");
        int listSize = cJSON_GetArraySize(jInfo);
        for(int i=0; i<listSize; i++)
        {
            cJSON* jItem = cJSON_GetArrayItem(jInfo, i);
            if(jItem == NULL)
                break;

            GoodsCheckInfo* info = new GoodsCheckInfo();
            info->id = QString(cJSON_GetObjectItem(jItem, "goodsId")->valuestring);
            info->name = QString(cJSON_GetObjectItem(jItem, "goodsName")->valuestring);
            info->goodsSize = QString(cJSON_GetObjectItem(jItem, "size")->valuestring);
            info->num_in = cJSON_GetObjectItem(jItem, "inCount")->valueint;
            info->num_out = cJSON_GetObjectItem(jItem, "outCount")->valueint;
            info->num_back = cJSON_GetObjectItem(jItem, "backCount")->valueint;
            info->num_cur = cJSON_GetObjectItem(jItem, "currCount")->valueint;
            checkList->addInfo(info);
        }
        emit curCheckList(checkList);
        checkFinish(true);
    }
    else
    {
        checkFinish(false);
//        qDebug()<<"check not finished, resend after 30s.";
//        QTimer::singleShot(3000, this, SLOT(goodsCheckFinish()));
    }

    cJSON_Delete(json);
}

void CabinetServer::recvGoodsBack()
{

}

void CabinetServer::recvDateTime()
{
    QByteArray qba = QByteArray::fromBase64(reply_datetime->readAll());
    reply_datetime->deleteLater();
    reply_datetime = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<cJSON_Print(json);

    if(!json)
        return;

    netFlag = true;
    cJSON* rst = cJSON_GetObjectItem(json, "success");

    if(rst->type != cJSON_True)
    {
        qDebug("[check time] failed");
        return;
    }
    timeIsChecked = true;

    rst = cJSON_GetObjectItem(json, "data");

//    cJSON* jsTime = cJSON_GetObjectItem(rst, "datetime_1");
    QString str(rst->valuestring);
//    qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    checkSysTime(QDateTime::fromString(str,"yyyy-MM-dd hh:mm:ss"));
    //    qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    cJSON_Delete(json);

    return;
//    if(!needSaveAddress)
//        return;
//    needSaveAddress = false;
//    cabManager->setServerAddress(ApiAddress);

//    if(cabManager->getCabinetId().isEmpty())
//        cabRegister();
//    {
//        regId = "835999";
//        cabManager->setCabinetId(regId);
//    }
}

void CabinetServer::recvDateTimeError(QNetworkReply::NetworkError code)
{
    qDebug()<<code;
//    cabManager->clearcabManager();
}

void CabinetServer::recvListState()
{
    QByteArray qba = QByteArray::fromBase64(reply_list_state->readAll());
    reply_list_state->deleteLater();
    reply_list_state = NULL;


    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvListState]"<<cJSON_Print(json);

    if(!json)
    {
        cJSON_Delete(json);
        return;
    }
    netFlag = true;
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        cJSON* json_data = cJSON_GetObjectItem(json,"data");
        if(json_data->type == cJSON_NULL)
        {
            cJSON_Delete(json);
            return;
        }
        int listSize = cJSON_GetArraySize(json_data);
        int i = 0;

        for(i=0; i<listSize; i++)
        {
            cJSON* item = cJSON_GetArrayItem(json_data, i);
//            GoodsCar car;
            QString listId = QString(cJSON_GetObjectItem(item, "barcode")->valuestring);
//            car.rfid = QString(cJSON_GetObjectItem(item, "code")->valuestring);
            qDebug()<<"[newGoodsCar]"<<listId;
            list_goodsList<<listId;
//            emit newGoodsCar(car);
//            needReqCar = false;
        }
    }

    cJSON_Delete(json);
    checkGoodsList();
}

void CabinetServer::recvInfoUploadResult()
{

}

void CabinetServer::recvCabClone()
{
    QByteArray qba = QByteArray::fromBase64(reply_cabinet_clone->readAll());
    reply_cabinet_clone->deleteLater();
    reply_cabinet_clone = NULL;


    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCabClone]"<<cJSON_Print(json);
//    return;
    if(!json)
    {
        cJSON_Delete(json);
        return;
    }
    netFlag = true;
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        cJSON* json_data = cJSON_GetObjectItem(json,"data");
        if(json_data->type == cJSON_NULL)
        {
            cJSON_Delete(json);
            return;
        }
        int listSize = cJSON_GetArraySize(json_data);
        int i = 0;

        for(i=0; i<listSize; i++)
        {
            cJSON* item = cJSON_GetArrayItem(json_data, i);
            GoodsInfo* info = new GoodsInfo();

            int row = QString(cJSON_GetObjectItem(item, "cabinetRow")->valuestring).toInt();
            int col = QString(cJSON_GetObjectItem(item, "cabinetCol")->valuestring).toInt();
            info->abbName = QString(cJSON_GetObjectItem(item, "abbName")->valuestring);
            info->name = QString(cJSON_GetObjectItem(item, "goodsName")->valuestring);
            info->num = cJSON_GetObjectItem(item,"packageCount")->valueint;
            info->outNum = 0;
            info->id = QString(cJSON_GetObjectItem(item, "goodsId")->valuestring);
            info->goodsType = cJSON_GetObjectItem(item, "goodsType")->valueint;
            info->unit = QString(cJSON_GetObjectItem(item, "unit")->valuestring);
            info->Py = cabManager->getPyCh(info->name);//qDebug()<<"[PY]"<<info->Py;
            info->packageId = info->id;

            if(info->goodsType<10)
                info->packageId += "-0"+QString::number(info->goodsType);
            else
                info->packageId += "-"+QString::number(info->goodsType);

            if(info->abbName.isEmpty())
                info->abbName = getAbbName(info->name);
//            qDebug()<<"[newGoods]"<<row<<col<<info->name<<info->abbName<<info->id<<info->packageId<<info->num<<info->unit;
//            cabManager->insertGoods(info, row, col);
        }
    }
    else
    {
        emit cloneResult(false,"柜子编码无效");
        return;
    }
    emit cloneResult(true,"智能柜数据克隆成功");
    cJSON_Delete(json);
}

void CabinetServer::recvCabSync()
{
    QByteArray qba = QByteArray::fromBase64(reply_cabinet_clone->readAll());
    reply_cabinet_clone->deleteLater();
    reply_cabinet_clone = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCabSync]"<<cJSON_Print(json);
//    return;
    if(!json)
    {
        cJSON_Delete(json);
        emit cabSyncResult(false);
        return;
    }
    netFlag = true;
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        cJSON* json_data = cJSON_GetObjectItem(json,"data");
        if(json_data->type == cJSON_NULL)
        {
            cJSON_Delete(json);
            return;
        }
        if(needClearBeforeClone)
        {
            needClearBeforeClone = false;
            repManager->clearRepertory();
            emit insertRst(true);
        }
        int listSize = cJSON_GetArraySize(json_data);
        int i = 0;

        for(i=0; i<listSize; i++)
        {
            cJSON* item = cJSON_GetArrayItem(json_data, i);
            GoodsInfo* info = new GoodsInfo();

            int row = QString(cJSON_GetObjectItem(item, "cabinetRow")->valuestring).toInt();
            int col = QString(cJSON_GetObjectItem(item, "cabinetCol")->valuestring).toInt();
            info->abbName = QString(cJSON_GetObjectItem(item, "abbName")->valuestring);
            info->name = QString(cJSON_GetObjectItem(item, "goodsName")->valuestring);
            info->num = cJSON_GetObjectItem(item,"packageCount")->valueint;
            info->outNum = 0;
            info->id = QString(cJSON_GetObjectItem(item, "goodsId")->valuestring);
            info->goodsType = cJSON_GetObjectItem(item, "goodsType")->valueint;
            info->unit = QString(cJSON_GetObjectItem(item, "unit")->valuestring);
            info->Py = cabManager->getPyCh(info->name);//qDebug()<<"[PY]"<<info->Py;
            info->packageId = info->id;

            if(info->goodsType<10)
                info->packageId += "-0"+QString::number(info->goodsType);
            else
                info->packageId += "-"+QString::number(info->goodsType);

            if(info->abbName.isEmpty())
                info->abbName = getAbbName(info->name);

            qDebug()<<"[newGoods]"<<row<<col<<info->name<<info->abbName<<info->id<<info->packageId<<info->num<<info->unit;
//            cabManager->syncGoods(info, row, col);
        }
    }
    else
    {
        emit cabSyncResult(false);
        return;
    }
    emit cabSyncResult(true);
    cJSON_Delete(json);
}

void CabinetServer::recvColInsert()
{
    QByteArray qba = QByteArray::fromBase64(reply_update_col->readAll());
    reply_update_col->deleteLater();
    reply_update_col = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvColInsert]"<<cJSON_Print(json);
//    return;
    if(!json)
    {
        cJSON_Delete(json);
        return;
    }
    netFlag = true;
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
//        cabManager->clearGoodscabManager();
        needClearBeforeClone = true;
        cabInfoSync();
    }
    else
    {
        emit insertRst(false);
    }
}

void CabinetServer::recvCheckTables()
{
    QByteArray qba = QByteArray::fromBase64(reply_check_tables->readAll());
    reply_check_tables->deleteLater();
    reply_check_tables = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCheckTables]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        cJSON* jData = cJSON_GetObjectItem(json, "data");
        int listSize = cJSON_GetArraySize(jData);
        QList<CheckTableInfo*> l;

        for(int i=0; i<listSize; i++)
        {
            cJSON* jItem = cJSON_GetArrayItem(jData, i);
            if(jItem == NULL)
                break;

            CheckTableInfo* info = new CheckTableInfo();
            info->id = QString::number(cJSON_GetObjectItem(jItem, "id")->valueint);
            info->sTime = QString(cJSON_GetObjectItem(jItem, "sTime")->valuestring);
            info->eTime = QString(cJSON_GetObjectItem(jItem, "eTime")->valuestring);

            l<<info;
        }
        emit checkTables(l);
    }
    else
    {

    }

    cJSON_Delete(json);
}

void CabinetServer::recvCheckTableInfo()
{
    QByteArray qba = QByteArray::fromBase64(reply_check_table_info->readAll());
    reply_check_table_info->deleteLater();
    reply_check_table_info = NULL;

    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvCheckTableInfo]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        if(checkList != NULL)
            delete checkList;
        checkList = new CheckList();
        cJSON* jData = cJSON_GetObjectItem(json,"data");
//        checkList->setCheckDateTime(QString(cJSON_GetObjectItem(jData, "strTime")->valuestring));
//        checkList->departCode = QString(cJSON_GetObjectItem(jData, "departCode")->valuestring);
//        checkList->departCode = QString(cJSON_GetObjectItem(jData, "departCode")->valuestring);
        cJSON* jInfo = jData;//cJSON_GetObjectItem(jData, "goodsChesetEntityList");
        int listSize = cJSON_GetArraySize(jInfo);

        for(int i=0; i<listSize; i++)
        {
            cJSON* jItem = cJSON_GetArrayItem(jInfo, i);
            if(jItem == NULL)
                break;

            GoodsCheckInfo* info = new GoodsCheckInfo();
            info->id = QString(cJSON_GetObjectItem(jItem, "goodsId")->valuestring);
            info->name = QString(cJSON_GetObjectItem(jItem, "goodsName")->valuestring);
            info->goodsSize = QString(cJSON_GetObjectItem(jItem, "size")->valuestring);
            info->num_in = cJSON_GetObjectItem(jItem, "inCount")->valueint;
            info->num_out = cJSON_GetObjectItem(jItem, "outCount")->valueint;
            info->num_back = cJSON_GetObjectItem(jItem, "backCount")->valueint;
            info->num_cur = cJSON_GetObjectItem(jItem, "currCount")->valueint;
            checkList->addInfo(info);
        }
        emit curCheckList(checkList);
    }
    else
    {
//        qDebug()<<"check not finished, resend after 30s.";
//        QTimer::singleShot(3000, this, SLOT(goodsCheckFinish()));
    }

    cJSON_Delete(json);
}

void CabinetServer::recvSearchSpell()
{
    QByteArray qba = QByteArray::fromBase64(reply_search_spell->readAll());
    reply_search_spell->deleteLater();
    reply_search_spell = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvSearchSpell]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        if(checkList != NULL)
            delete checkList;
        checkList = new CheckList();
        cJSON* jData = cJSON_GetObjectItem(json,"data");
        cJSON* jInfo = jData;
        int listSize = cJSON_GetArraySize(jInfo);

        for(int i=0; i<listSize; i++)
        {
            cJSON* jItem = cJSON_GetArrayItem(jInfo, i);
            if(jItem == NULL)
                break;

            GoodsCheckInfo* info = new GoodsCheckInfo();
            info->id = QString(cJSON_GetObjectItem(jItem, "b2bNum")->valuestring);//物品ID
            info->packageBarCode = QString(cJSON_GetObjectItem(jItem, "packageBarcode")->valuestring);
            info->name = QString(cJSON_GetObjectItem(jItem, "name")->valuestring);//物品名
            info->goodsSize = QString(cJSON_GetObjectItem(jItem, "size")->valuestring);//规格
            info->num_cur = cJSON_GetObjectItem(jItem, "goodsCount")->valueint;//库存
            info->unit = cJSON_GetObjectItem(jItem, "unit")->valueint;//单位
            info->producerName = cJSON_GetObjectItem(jItem, "producerName")->valuestring;//生产商
            info->type = QString(cJSON_GetObjectItem(jItem, "packageType")->valuestring).toInt();//type
            checkList->addInfo(info);
        }
        emit curSearchList(checkList);
    }
    else
    {

    }

    cJSON_Delete(json);
}

void CabinetServer::recvGoodsReply()
{
    QByteArray qba = QByteArray::fromBase64(reply_goods_reply->readAll());
    reply_goods_reply->deleteLater();
    reply_goods_reply = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvGoodsReply]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        emit goodsReplyRst(true, "请货成功");
    }
    else
    {
        emit goodsReplyRst(false, QString(cJSON_GetObjectItem(json, "msg")->valuestring));
    }

    cJSON_Delete(json);
}

void CabinetServer::recvDayReportInfo()
{
    QByteArray qba = QByteArray::fromBase64(reply_day_report->readAll());
    reply_day_report->deleteLater();
    reply_day_report = NULL;
    cJSON* json = cJSON_Parse(qba.data());
    qDebug()<<"[recvDayReportInfo]"<<cJSON_Print(json);
    if(!json)
        return;

    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    QList<DayReportInfo*> rst;
    if(json_rst->type == cJSON_True)
    {
        cJSON* data = cJSON_GetObjectItem(json, "data");
        int arraySize = cJSON_GetArraySize(data);
        for(int i=0; i<arraySize; i++)
        {
            cJSON* item = cJSON_GetArrayItem(data, i);
            DayReportInfo* info = new DayReportInfo;
            info->goodsId = QString(cJSON_GetObjectItem(item, "goodsId")->valuestring);
            info->goodsName = QString(cJSON_GetObjectItem(item, "goodsName")->valuestring);
            info->size = QString(cJSON_GetObjectItem(item, "size")->valuestring);
            info->proName = QString(cJSON_GetObjectItem(item, "proName")->valuestring);
            info->goodsCount = QString(cJSON_GetObjectItem(item, "goodsCount")->valuestring);
            info->optTime = QString(cJSON_GetObjectItem(item, "optTime")->valuestring);
            info->traceId = QString(cJSON_GetObjectItem(item, "traceId")->valuestring);
            info->unit = QString(cJSON_GetObjectItem(item, "unit")->valuestring);
            info->batchNumber = QString(cJSON_GetObjectItem(item, "batchNumber")->valuestring);
            info->optName = QString(cJSON_GetObjectItem(item, "optName")->valuestring);
            info->supplyName = QString(cJSON_GetObjectItem(item, "supplyName")->valuestring);
            info->price = cJSON_GetObjectItem(item, "price")->valuedouble;
            info->sumCount = cJSON_GetObjectItem(item, "sumCount")->valuedouble;
            info->state = cJSON_GetObjectItem(item, "state")->valueint;
            rst<<info;
        }
        emit dayReportRst(rst, QString(cJSON_GetObjectItem(json, "msg")->valuestring));
    }
    else
    {
        emit dayReportRst(rst, QString(cJSON_GetObjectItem(json, "msg")->valuestring));
    }

    cJSON_Delete(json);
}

void CabinetServer::netTimeout()
{
    if(netFlag)
        networkState = true;
    else
        networkState = false;

    qDebug()<<"netstate"<<networkState;
    emit netState(networkState);
    if(networkState)
    {
        localCacheAccess();
    }
    else
    {
        if(apiState == 1)//登录
        {
            if(cabManager->checkManagers(logId))
            {
                cur_manager->name = "管理员";
                cur_manager->power = 0;
                cur_manager->cardId = logId;
                cur_user = cur_manager;

//                cabManager->wakeUp(TIMEOUT_FETCH);
                emit loginRst(cur_user);
            }
            else
            {
                cur_user = cabManager->checkUserLocal(logId);
                qDebug()<<"check null";
                if(cur_user == NULL)
                {
                    apiState = 0;
                    return;
                }
                qDebug()<<"check"<<cur_user->cardId;
//                cabManager->wakeUp(TIMEOUT_FETCH);
                emit loginRst(cur_user);
            }
        }
        apiState = 0;
    }
}

int CabinetServer::watchdogTimeout()
{
    int ret = 0;
//    qDebug()<<"[watchdog]"<<"write";
    if (fWatchdog != -1)
    {
        ret = write(fWatchdog, "a", 1);

    }
    return ret;
}

void CabinetServer::sysTimeout()
{
    if(timeIsChecked)
    {
        emit timeUpdate();
    }
    else
    {
        sysClock.stop();
        disconnect(&sysClock, SIGNAL(timeout()), this, SLOT(sysTimeout()));
//        qDebug("checksss");
        checkTime();

        if(cabManager->sleepFlagTimeout())
        {
            qDebug("[lock]");
            emit sysLock();
        }
        return;
    }
//    if(needReqCar&& cabManager->state == STATE_NO)
//        requireListState();

    if(cabManager->sleepFlagTimeout())
    {
        qDebug("[lock]");
        emit sysLock();
    }
}

void CabinetServer::updateAddress()
{
    getServerAddr(cabManager->getServerAddress());
}

