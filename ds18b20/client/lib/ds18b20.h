/*********************************************************************************
 *      Copyright:    (C) 2023 Kun_<1433729173@qq.com>
 *                    All rights reserved.
 *
 *      Filename:     ds18b20.h
 *      Description:  This ds18b20 head file
 *                 
 *      Version:      2.0.0(2023年04月30日)
 *      Author:       Kun_ <1433729173@qq.com>
 *      ChangeLog:    1, Release initial version on "2023年04月10日 17时25分12秒"
 *                 
 ********************************************************************************/
#ifndef  _DS18B20_H_
#define  _DS18B20_H_


/* 
 * 函数介绍：获取ds18b20的温度
 * 输出参数：将获取到的温度写入temp
 * 返回值：成功返回0，出错返回负数
 * */
int get_temperature(float *temp);



#endif
