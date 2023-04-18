/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlite.h
 *    Description:  This file sqlite head file
 *
 *        Version:  1.0.0(2023年04月18日)
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
 
/*
 * 函数介绍： 创建名为temp_data的数据库并创建名为TEMP的表格
 * 返回值：正常退出返回0，创建失败返回负数
 */
extern int sqlite_init();

/*
 * 函数介绍：向TEMP的data插入结构体数据
 * 输出参数：myData类型结构体
 * 返回值：正常退出返回0，出错返回负数
 */
extern int sqlite_insert(myData data);

/* 
 * 函数介绍：自动选择表中最后一行数据存入传入参数数组并打印
 * 输出参数：选择数据存入str中
 * 返回值：正常退出返回0，出错返回负数
 */
extern int sqlite_select(char *str);

/*
 * 函数介绍：自动选择表中最后一行数据删除
 * 返回值：正常退出返回0，出错返回负数
 */
extern int sqlite_delete();
 
#endif

