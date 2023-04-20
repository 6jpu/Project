/********************************************************************************
 *       Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  get_time.h
 *       Description:  This file is head file for get_time.c
 *
 *       Version:  1.0.0(2023年04月18日)
 *       Author:  Kun_ <1433729173@qq.com>
 *       ChangeLog:  1, Release initial version on "2023年04月15日 10时29分09秒"
 *                 
 ********************************************************************************/
#ifndef  _GET_TIME_H_
#define  _GET_TIME_H_


/* 
 * 函数介绍：获取当前时间
 * 输出参数：将时间以YYYY-MM-DD HH:MM:SS 格式的字符串写入repoet_time
 * 返回值：成功返回0，出错返回负数
 * */
extern int get_time(char *report_time);

 
#endif


