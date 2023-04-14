/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlite.h
 *    Description:  This file sqlite head file
 *
 *        Version:  1.0.0(2023年04月10日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月10日 21时01分23秒"
 *                 
 ********************************************************************************/
#ifndef  _SQLITE_H_
#define  _SQLITE_H_

typedef struct{
	char       report_time[64];
	float      temp;
	char       sn[64];
}myData;
 

extern int sqlite_init();
extern int sqlite_insert(myData data);
extern int sqlite_select();
extern int sqlite_delete();
 
#endif

