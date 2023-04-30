/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  get_time.c
 *    Description:  This file to get time
 *                 
 *        Version:  2.0.0(2023年04月30日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月15日 09时49分29秒"
 *                 
 ********************************************************************************/
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "get_time.h"


int get_time(char *report_time)
{
	time_t timer;

	timer = time(NULL);
	struct tm* plocaltime = localtime(&timer);
	sprintf(report_time, "%d-%02d-%02d %02d:%02d:%02d",
			plocaltime->tm_year + 1900, plocaltime->tm_mon + 1, plocaltime->tm_mday,
			plocaltime->tm_hour, plocaltime->tm_min, plocaltime->tm_sec);
     
	return 0;
}

