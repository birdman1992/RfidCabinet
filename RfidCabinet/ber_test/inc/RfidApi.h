/*
 * main.h
 *
 *  Created on: 2010-4-4
 *      Author: knuth
 */

#ifndef MAIN_H_
#define MAIN_H_

#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<stdarg.h>
#include 	<errno.h>
#include 	<math.h>
#include 	<sys/types.h>
#include 	<netinet/in.h>
#include 	<sys/socket.h>
#include 	<sys/wait.h>
#include 	<unistd.h>
#include 	<arpa/inet.h>
#include 	<sys/mman.h>
#include 	<syscall.h>
#include 	<fcntl.h>
#include 	<sys/epoll.h>
#include 	<sys/time.h>
#include 	<sys/resource.h>
#include 	<pthread.h>
#include 	<sys/stat.h>
#include 	<locale.h>
#include 	<wchar.h>
#include 	<semaphore.h>
#include 	<signal.h>
#include 	<setjmp.h>
#include 	<time.h>
#include 	<limits.h>
#include 	<ctype.h>
#include 	<float.h>
#include 	<stddef.h>
#include	<netdb.h>
#include 	<iconv.h>
#include 	<assert.h>
#include	<termios.h>
#include 	"hash.h"
#include 	"profile.h"

#define ID_LEN	16


#define NIPQUAD(addr) \
((unsigned char *)&addr)[0], \
((unsigned char *)&addr)[1], \
((unsigned char *)&addr)[2], \
((unsigned char *)&addr)[3]
#define NIPQUAD_FMT "%u.%u.%u.%u"

#define HI_8BIT(A) ((A&0xFF00)>>8)
#define LO_8BIT(A) (A&0xFF)
#define MHI	60000
//int WriteConfig(CON_DATA *head);

typedef struct mi{
    unsigned long index;
    unsigned char epc[12];
    unsigned char ant_num;
}MDYINFO,*PMDYINFO;			// modify  info

PMDYINFO rInfo;
typedef struct rfid_info{
    unsigned char ant_num;
    unsigned char id[12];
    unsigned char dc_type; 		// data change type		0->delete  1->new add
    time_t second;
    struct rfid_info *next;
}RFID_DATA;
extern int cfd ;
extern struct HashItem* RfidHash;

extern int _write(int fd, void *buffer, int length);
extern int ReadDevice(int hComm,RFID_DATA *epc,int ant,time_t seconds,int(*DataProcess)(RFID_DATA *,unsigned char *,int,time_t));
extern int open_com(char *DevPath);
extern void r_printf(const char *format,...);
extern int FreeRS(RFID_DATA *epc);
extern int EchoProcess(RFID_DATA *epc, unsigned char* buf,int ant,time_t seconds);
extern int EpcProcess(RFID_DATA *epc, unsigned char* buf,int ant,time_t seconds);
extern int GetEpc(RFID_DATA *epc);
extern int mainLoop(void);

#endif /* MAIN_H_ */

