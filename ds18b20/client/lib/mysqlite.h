/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlite.h
 *    Description:  This file mysqlite head file
 *
 *        Version:  1.0.0(2023年04月18日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月10日 21时01分23秒"
 *                 
 ********************************************************************************/
#ifndef  _MYSQLITE_H_
#define  _MYSQLITE_H_

#include "pack.h"
 
/*
 * 函数介绍： 创建名为输入参数db_name的数据库并创建名为TEMP的表格
 * 返回值：正常退出返回0，创建失败返回负数
 */
int db_init(char *db_name);

/*
 * 函数介绍：向TEMP的data插入结构体数据
 * 输出参数：packet_t类型结构体
 * 返回值：正常退出返回0，出错返回负数
 */
int db_insert(packet_t pack);

/* 
 * 函数介绍：自动选择表中最后一行数据存入传入参数数组并打印
 * 输出参数：选择数据存入str中
 * 返回值：正常退出返回0，出错返回负数
 */
int db_select(char *str);

/*
 * 函数介绍：自动选择表中最后一行数据删除
 * 返回值：正常退出返回0，出错返回负数
 */
int db_delete(void);

/* 
 * 函数介绍：使用完数据库后关闭数据库
 * 返回值：正常退出返回0，出错返回负数
 */
int db_close(void);
 
#endif

