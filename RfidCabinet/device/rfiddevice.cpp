#include "rfiddevice.h"
#include <QMetaType>
extern "C"{
#include "ber_test/inc/RfidApi.h"
}
#include "config.h"
#include <QDebug>

RfidDevice::RfidDevice(QObject *parent) : QThread(parent)
{
//    runFlag = false;

    qRegisterMetaType<QList<rfidChangeInfo*> >("QList<rfidChangeInfo*>");

    rInfo = (PMDYINFO)malloc(sizeof(MDYINFO)*MHI);
    memset(rInfo,0x00,sizeof(sizeof(MDYINFO)*MHI));

    setbuf(stdout,NULL);
#ifdef RUN_IN_ARM
    cfd = open_com(QByteArray(DEV_Rfid).data());
    if(cfd > 0)
        qDebug()<<DEV_Rfid<<"open success!";
#else
    cfd = open_com(QByteArray(DEV_RFID_PC).data());
    if(cfd > 0)
        qDebug()<<DEV_RFID_PC<<"open success!";
#endif
    CryptTables();				// 初始化hash
    RfidHash = inithashtable(MHI);
}

RfidDevice::~RfidDevice()
{
    if(this->isRunning())
        stopScan();

    free(rInfo);
    FreeHash(RfidHash);
}

void RfidDevice::startScan()
{
    qDebug()<<"[startScan]";
    if(!this->isRunning())
        this->start();
}

void RfidDevice::stopScan()
{
    qDebug()<<"[stopScan]";
    runFlag = false;
//    this->quit();
//    this->wait();
}


//void RfidDevice::run()
//{
//    int i;
//    runFlag = true;

//    while(runFlag)
//    {
//        sleep(5);
//        RFID_DATA *epc = (RFID_DATA *)malloc(sizeof(RFID_DATA));
//        GetEpc(epc);
//        if(epc != NULL)
//        {
//            while (epc->next != NULL)
//            {
//                epc = epc->next;
//                printf("id[ANT%d][%d]: ",epc->ant_num,epc->dc_type);
//                for(i=0;i<12;i++)
//                    printf("%02X  ",epc->id[i]);
//                printf("\n");
//            }
//        }
//        printf("\n");
//        printf("\n");
//        FreeRS(epc);
//    }
//    return;
//}

void RfidDevice::run()
{
//    int i;
    QByteArray id;
    runFlag = true;

//    while(runFlag)
    {
        sleep(1);
        RFID_DATA *epc = (RFID_DATA *)malloc(sizeof(RFID_DATA));
        epc->next = NULL;
        GetEpc(epc);
        RFID_DATA *epc_p = epc;

        if(epc_p != NULL)
        {
            while (epc_p->next != NULL)
            {
                epc_p = epc_p->next;
                if(epc_p->dc_type == 1)
                {
                    id = QByteArray((const char*)epc_p->id, 12);
                    rfidChangeInfo* info = new rfidChangeInfo(epc_p->ant_num,id);
                    inList<<info;
//                    qDebug()<<"[RFID in]"<<"ant:"<<epc->ant_num<<id.toHex();
                }
                else if(epc_p->dc_type == 0)
                {
                    id = QByteArray((const char*)epc_p->id, 12);
                    rfidChangeInfo* info = new rfidChangeInfo(epc_p->ant_num,id);
                    outList<<info;
//                    qDebug()<<"[RFID out]"<<"ant:"<<epc->ant_num<<id.toHex();
                }
//                printf("id[ANT%d][%d]: ",epc->ant_num,epc->dc_type);
//                for(i=0;i<12;i++)
//                    printf("%02X  ",epc->id[i]);
//                printf("\n");
            }
        }
//        printf("\n");
//        printf("\n");
        FreeRS(epc);
    }
    if(!inList.isEmpty())
    {
        emit rfidIn(inList);
//        qDebug()<<"[RFID in]"<<inList;
        inList.clear();
    }
    if(!outList.isEmpty())
    {
        emit rfidOut(outList);
//        qDebug()<<"[RFID out]"<<outList;
        outList.clear();
    }
    qDebug()<<"[RfidDevice]:scan finished.";
    return;
}

void RfidDevice::insertRfid(QStringList list_id)
{
    QByteArray qba;
    QString str;

    foreach(str, list_id)
    {
        qba = QByteArray::fromHex(str.toLocal8Bit());
        writeDataToHashTable((unsigned char*)qba.data());
    }
}
