/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  get_time.c
 *    Description:  This file to get_time
 *                 
 *        Version:  1.0.0(2023年04月15日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月15日 09时49分29秒"
 *                 
 ********************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <stdio.h>
#include <string.h>

int get_time(char *report_time)
{
	time_t timer;

	timer= time(NULL);
	printf("%ld\n", timer);
	struct tm* plocaltime = localtime(&timer);
	sprintf(report_time,"%d-%d-%d %d:%d:%d", plocaltime->tm_year + 1900, plocaltime->tm_mon + 1, plocaltime->tm_mday,
                       plocaltime->tm_hour,plocaltime->tm_min,plocaltime->tm_sec);
     
    return 0;
}

