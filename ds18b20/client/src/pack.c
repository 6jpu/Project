/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  pack.c
 *    Description:  This pack file 
 *                 
 *        Version:  1.0.0(2023年04月25日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月25日 19时21分23秒"
 *                 
 ********************************************************************************/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include "logger.h"
#include "pack.h"


/*  Description: Get device serial number */
int get_devsn(char *devsn, int size)
{
	char sn[]="007";
	memset(devsn,0,sizeof(devsn));
	snprintf(devsn,16,"RPI%s",sn);

	return 0;
}

/*  Description: Get date/time */
int get_time(char *date_time, int size)
{
	time_t timestamp = time(NULL); // 获取当前时间戳
	struct tm result;
	
	localtime_r(&timestamp, &result);  // 将时间戳转换为本地时间并存储到 result 中
	snprintf(date_time, size, "%d-%02d-%02d %02d:%02d:%02d", 
			result.tm_year + 1900, result.tm_mon + 1, result.tm_mday,
			result.tm_hour, result.tm_min, result.tm_sec);
    
    return 0;
}

/*  Description: Convert data from $pack to string format and save in $buf
 *  String Format: "RPI007,2023-04-13 20:08:08,26.66"
 *  */
int pack_data(packet_t *pack, char *buf, int size)
{
	/* 生成字符串 */    
	memset(buf, 0, size);
	snprintf(buf, size, "%s,%f,%s\n", pack->time, pack->temp, pack->devsn);
	PARSE_LOG_DEBUG("msg_str:%s\n", buf);
	
	return 0;
}

