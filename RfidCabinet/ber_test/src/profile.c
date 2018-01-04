/*
 * profile.c
 *
 *  Created on: 2015-9-22
 *      Author: lovenix
 */
#include "../inc/RfidApi.h"
#include "../inc/profile.h"

int DefaultConfigSet(void)			// 生成默认配置文件
{
	FILE* fp;
	if((fp = fopen(CONFIG_FILE, "ab")) == NULL)
		return 0;
	fprintf(fp, "%s", "SERVER_IP\t222.240.214.168\t\r\n");
	fprintf(fp, "%s", "ROBOT_PORT\t4016\t\r\n");
	fprintf(fp, "%s", "MOBILE_PORT\t4020\t\r\n");
	fprintf(fp, "%s", "CLI_TIME_OUT\t15\t\r\n");
	fprintf(fp, "%s", "COM\t/dev/ttyUSB0\t\r\n");
	fclose(fp);
	return 1;
}

int GetConfigArg(int num, char* buf)		// 获取某行配置参数
{
	int i = 0;
    long temps = 0;
    char* p = NULL;
	FILE* fp;
	char temp[32];
	fp = fopen(CONFIG_FILE, "rb");
	for (i = 0; i < num - 1; i++)
	{
		p = fgets(&temp[0], 32, fp);
		temps = ftell(fp);                    // 设置文件偏移量
		memset(temp, 0x00, 32);
		if (feof(fp) != 0)
			break;
	}
	i = 0;
	p = fgets(&temp[0], 32, fp);
	while (temp[i++] != 0x09)
		;
	while (temp[i] != 0x09)
		*(buf++) = temp[i++];
	*buf = '\0';
	fclose(fp);
	return (1);
}

int ConfigModif(int num, char* con)		// 修改配置文件
{
	int i, j;
    long temps = 0;;
    char* p = NULL;
	FILE* fp;
	FILE* ftemp;
	char temp[32] = { 0 };
	char head[5] = { 0 };
	fp = fopen(CONFIG_FILE, "rb");
	ftemp = tmpfile();
	for (i = 0; i < num - 1; i++)
	{
		p = fgets(&temp[0], 32, fp);
		temps = ftell(fp);                    // 设置文件偏移量
		fprintf(ftemp, "%s", temp);
		memset(temp, 0x00, 32);
		if (feof(fp) != 0)
			break;
	}
	p = fgets(&temp[0], 32, fp);
	j = i = 0;
	while (temp[i] != 0x09)
		head[j++] = temp[i++];
	head[j] = '\0';
	memset(temp, 0x00, 32);
	snprintf(temp, sizeof(temp), "%s%s%s%s", head, "\t", con, "\t\r\n");
	fprintf(ftemp, "%s", temp);
	while (1)
	{
		p = fgets(&temp[0], 32, fp);
		temps = ftell(fp);                    // 设置文件偏移量
		fprintf(ftemp, "%s", temp);
		memset(temp, 0x00, 32);
		if (feof(fp) != 0)
			break;
	}
	fclose(fp);
	if (remove(CONFIG_FILE) == -1)
		printf("Profile: Remove configure file error!\n");
	if ((fp = fopen(CONFIG_FILE, "w")) == NULL)
		printf("Profile: Create new file error!\n");
	if (fseek(ftemp, 0, SEEK_SET) != 0)
		printf("Profile: lseek error!\n");
	while (1)
	{
		 p = fgets(&temp[0], 32, ftemp);
		 temps = ftell(ftemp);                    // 设置文件偏移量
		fprintf(fp, "%s", temp);
		memset(temp, 0x00, 32);
		if (feof(ftemp) != 0)
			break;
	}
	fclose(ftemp);
	fclose(fp);
	return (1);
}

int FileCheck()   		// 检测配置文件是否存在
{
	int fd;
	struct stat buf;
	if ((fd = open(CONFIG_FILE, O_RDONLY)) == -1)
	{
		if((fd = open(CONFIG_FILE, O_RDONLY | O_EXCL | O_CREAT, S_IRUSR | S_IWUSR)) == -1)		// create file
		{
			printf("Open configure file failed!\n");
			exit(0);
		}
	}
	fstat(fd, &buf);
	close(fd);
	if (buf.st_size == 0)
		return 0;
	else
		return 1;
}

