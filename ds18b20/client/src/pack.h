/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  pack.h
 *    Description:  This pack head file 
 *
 *        Version:  1.0.0(2023年04月26日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月26日 11时13分54秒"
 *                 
 ********************************************************************************/
#ifndef  _PACK_H_
#define  _PACK_H_

#include "ds18b20.h"
#define DEVSN_LEN 10

typedef struct packet_s
{
    float temp; /*  temperature */
    char  time[32]; /*  date/time: YYYY-MM-DD hh:mm:ss */
    char  devsn[DEVSN_LEN+1]; /*  SN format: RPI2304001 */
} packet_t; 


/*  Description: Get device serial number */
int get_devsn(char *devsn, int size);

/*  Description: Get date/time */
int get_time(char *date_time, int size);

/*  Description: Call get_devsn and get_time */
static inline int sample_temperature(packet_t *pack)
{
	if ( !pack )
	{
		return -1;
	}
	memset(pack, 0, sizeof(*pack));
	if( get_temperature(&pack->temp) < 0)
	{	
		return -2;
	}
	get_time(pack->time, sizeof(pack->time));
	get_devsn(pack->devsn, sizeof(pack->devsn));
	return 0;
}

/*  Description: Convert data from $pack to string format and save in $buf
 *  String Format: "2023-04-13 20:08:08,26.66,RPI007"
 *  */
int pack_data(packet_t *pack, char *buf, int size);

#endif
