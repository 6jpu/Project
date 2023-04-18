/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file is head file for ds18b20.c
 *                 
 *        Version:  1.0.0(2023年04月18日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月10日 17时25分12秒"
 *                 
 ********************************************************************************/
#ifndef  _DS18B20_H_
#define  _DS18B20_H_

#include <stdint.h>

/* 
 * 函数介绍：获取ds18b20的温度
 * 输出参数：将获取到的温度写入temp
 * 返回值：成功返回0，出错返回负数
 * */
extern int get_temperature(float *temp);

/* 
 * 函数介绍：获取ds18b20d的产品序列号
 * 输出参数：将获取到序列号写入sn
 * 返回值：成功返回0，出错返回负数
 * */
extern int get_sn(char *sn);
 
#endif
