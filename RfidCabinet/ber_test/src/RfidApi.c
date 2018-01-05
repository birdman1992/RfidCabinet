/*
 * main.c
 *
 *  Created on: 2010-3-27
 *      Author: Lovenix
 */
#include "../inc/RfidApi.h"
int cfd ;
struct HashItem* RfidHash;

/***写串口函数****/
int _write(int fd, void *buffer, int length)
{
    int bytes_left;
    int written_bytes;
    char *ptr;

    ptr = buffer;
    bytes_left = length;
    while (bytes_left > 0)
    {
        /********开始写 ***********/
        written_bytes=write(fd, ptr, bytes_left);
        if (written_bytes <= 0) // 出错了
        {
            if (errno==EINTR) // 中断错误继续写
                written_bytes = 0;
            else
                // 其他错误退出
                return (-1);
        }
        bytes_left -= written_bytes;
        ptr += written_bytes; // 从剩下的地方继续写
    }
    return (0);
}

int ReadDevice(int hComm,RFID_DATA *epc,int ant,time_t seconds,int(*DataProcess)(RFID_DATA *,unsigned char *,int,time_t))
{
    unsigned char temp;
    int count,start,pLen=0;
    fd_set readset; // 设置文件描述符集
    struct timeval tv;
    int MaxFd,nFd; // 文件描述符个数
    unsigned char buf[256] = {0};
	count = start = 0;
	{
		do{
			FD_ZERO( &readset); // 文件描述符置0
			if (hComm >= 0)
				FD_SET(hComm, &readset);
			MaxFd = hComm + 1; // 最大文件描述符数
			tv.tv_sec = 0;
			tv.tv_usec = 800000;
			do {
				nFd = select(MaxFd, &readset, 0, 0, &tv); // 检测状态
			} while (nFd==-1 && errno==EINTR); // 是否中断
			if (nFd <= 0)
				hComm = -1;
			else if(FD_ISSET(hComm, &readset)) // FD_ISSET检测fd是否设置
			{
				if(read(hComm, &temp, 1)>0)
				{
					if(temp==0xA0)			// pack head
						start = 1;
					if(start)
					{
						buf[count++] = temp;
						if(count == 2)
							pLen = temp+2;
						if(count == pLen)
						{
							if(DataProcess(epc,buf,ant,seconds) != 0)
								return 0;
							start = count = pLen = 0;
						}
					}
				}
				else
					break;
			}
		}while(hComm >= 0);
	}
    return 1;
}

/***@brief  设置串口通信速率
 *@param  fd     类型 int  打开串口的文件句柄
 *@param  speed  类型 int  串口速度
 *@return  void
 */
#define BAUDRATE B115200
int open_com(char *DevPath)
{
    int fd;
    struct termios newtio;
    fd = open(DevPath, O_RDWR|O_NOCTTY|O_NDELAY);
    if (fd == -1)
    {
        printf("can't open %s!\n",DevPath);

        return (-1);
    }
    tcgetattr(fd,&newtio);
    bzero(&newtio,sizeof(newtio));
    //setting c_cflag
    newtio.c_cflag |= (CLOCAL|CREAD);
    newtio.c_cflag &= ~PARENB;
    newtio.c_cflag &= ~PARODD;
    newtio.c_cflag &= ~CSTOPB;
    newtio.c_cflag &= ~CSIZE;
    newtio.c_cflag |= CS8;
    newtio.c_oflag |= OPOST;
    //newtio.c_lflag = 0;
    //newtio.c_cc[VTIME] = 5;
    //newtio.c_cc[VMIN] = 0;
    //setting c_iflag
    newtio.c_iflag &=~ (IXON|IXOFF|IXANY);
    cfsetispeed(&newtio,BAUDRATE);
    cfsetospeed(&newtio,BAUDRATE);
    //printf("speed=%d\n",baud);
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
    return(fd);
}

void r_printf(const char *format,...)
{
    printf("%s",format);
#ifdef RDEBUG
	time_t now;
	struct tm  *timenow;
	time(&now);
	timenow = localtime(&now);
	printf("RIFD[%02d:%02d:%02d] ",timenow->tm_hour,timenow->tm_min,timenow->tm_sec);
    va_list args;
    va_start(args,format);
    vprintf(format,args);
    va_end(args);
#endif
}

int FreeRS(RFID_DATA *epc)		// free result
{
	if (epc == NULL)
		return 0;
	RFID_DATA  *ptr = epc;
	RFID_DATA  *curr;
	while (ptr->next != NULL)
	{
		curr = ptr;
		ptr = ptr->next;
		free(curr);
	}
	free(ptr);
	return 1;
}

int EchoProcess(RFID_DATA *epc, unsigned char* buf,int ant,time_t seconds)
{
//    int i = 0;
//    printf("ECHO: ");
//    for(i=0; i<6; i++)
//        printf("%02X  ",buf[i]);
//    printf("\n");
	return 1;
}

int EpcProcess(RFID_DATA *epc, unsigned char* buf,int ant,time_t seconds)
{
	RFID_DATA* DataLink = NULL;
	unsigned long local;
	unsigned char id[12] = {0};
	if(buf[1] == 0x13)
	{
		memcpy(id,&buf[7],12);
		if((local=GetHashTablePos(id, RfidHash,MHI,12)) == 0)	// not storage
		{
//            printf("GetHashTablePos:%d\n",local);
			local = InsertHash(id, RfidHash,MHI,12);
			
			DataLink = (RFID_DATA*)malloc(sizeof(RFID_DATA));
			memcpy(DataLink->id,id,12);
			
			DataLink->ant_num = rInfo[local].ant_num = ant;
			DataLink->dc_type = 1;
//			DataLink->second = seconds;
			DataLink->next = epc->next;
			epc->next = DataLink;
			
			rInfo[local].index = seconds;
			memcpy(rInfo[local].epc,id,12);
		}
		else if(rInfo[local].ant_num == ant)		// ant channel same
        {
//            printf("GetHashTablePos:%d\n",local);
            rInfo[local].index = seconds;		// update time
        }

#if 0			
			printf("EPC(%lu): ",local);
			for(i=0; i<12; i++)
				printf("%02X  ",id[i]);
			printf("\n");
#endif
	}
	else
		return (buf[4]+1);
	return 0;
}

int GetEpc(RFID_DATA *epc)
{
	int i = 0;
	unsigned char set_work_ant[6] = { 0xA0,0x04,0x01,0x74,0x00,0x00 };
	unsigned char real_time_inventory[6] = { 0xA0,0x04,0x01,0x89,0x01,0xD1 };
	time_t seconds;
	RFID_DATA* DataLink = NULL;
    seconds = time(NULL); 

	for(i=0; i<4; i++)
	{
		set_work_ant[4] = i;
		set_work_ant[5] = 231-i;		// crc bit
		_write(cfd,set_work_ant,6);		// set work ant
		ReadDevice(cfd,epc,i,seconds,EchoProcess);
		_write(cfd,real_time_inventory,6);		
		ReadDevice(cfd,epc,i,seconds,EpcProcess);
	}
	for(i=0; i<MHI; i++)
	{
        if((rInfo[i].index!=0) && (rInfo[i].index!=(unsigned long)seconds))		// time not update
		{
			DataLink = (RFID_DATA*)malloc(sizeof(RFID_DATA));
			DataLink->dc_type = 0;		// delete
			memcpy(DataLink->id,rInfo[i].epc,12);
			DataLink->ant_num = rInfo[i].ant_num;
			DataLink->next = epc->next;
			epc->next = DataLink;
			DelHashTablePos(rInfo[i].epc, RfidHash,MHI,12);
//            printf("DelHashTablePos:%d\n",i);
			rInfo[i].index  = 0;
		}
	}
	return 1;
}

int mainLoop(void)
{
	int i;
	rInfo = (PMDYINFO)malloc(sizeof(MDYINFO)*MHI);
    memset(rInfo,0x00,sizeof(sizeof(MDYINFO)*MHI));

	setbuf(stdout,NULL);
	if(FileCheck(CONFIG_FILE) == 0)		// 检测配置文件
	{
		if(!DefaultConfigSet())
		{
			printf("Create configure file error!\n");
			exit(0);
		}
	}
	char test[128] = {0};
	GetConfigArg(5,test);
	printf("get line: %s\n",test);

	cfd = open_com("/dev/ttyUSB0");
	CryptTables();				// 初始化hash
	RfidHash = inithashtable(MHI);
	
	while(1)
	{	
		RFID_DATA *epc = (RFID_DATA *)malloc(sizeof(RFID_DATA));
		GetEpc(epc);
		if(epc != NULL)
		{
			while (epc->next != NULL)
			{
				epc = epc->next;
				printf("id[ANT%d][%d]: ",epc->ant_num,epc->dc_type);
				for(i=0;i<12;i++)
					printf("%02X  ",epc->id[i]);
				printf("\n");
			}
		}
		printf("\n");
		printf("\n");
		FreeRS(epc);
		sleep(5);
	}
	free(rInfo);
	FreeHash(RfidHash);
	return 0;
}

