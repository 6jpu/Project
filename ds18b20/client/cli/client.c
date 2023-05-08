/*********************************************************************************
 *       Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *       Description:  This file socket client
 *                 
 *       Version:  2.0.0(2023年04月30日)
 *       Author:  Kun_ <1433729173@qq.com>
 *       ChangeLog:  1, Release initial version on "2023年04月07日 17时00分57秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>
#include "mysqlite.h"
#include "logger.h"
#include "pack.h"
#include "socket.h"


void print_usage(char *proname)
{
    printf("%s usage :[hostname] [port]\n",proname );
    printf("-h(--hostname): sepcify Server IP address\n");
    printf("-p(--port): sepcify Server port\n");
    printf("-t(--time_interval): set data reporting time interval\n");
    printf("-H(--help): print this help information.\n");

    return ;

}

int main(int argc,char **argv)
{
    int                   ch;
	int                   msg_str_bytes;
	int                   sample_flag;       //sample_flag为0为未采样，为1则已采样
    int                   sockfd = -1;
    int                   rv = -1;
    int                   port = 6666;       //默认端口
    int                   set_time = 3;      //设置上报时间间隔，默认为三秒
    char                  msg_str[64];
    char                  buf[256];
    char                  hostname[32] = "127.0.0.1";		//默认IP
    char                  db_name[32] = "temp_data.db";
    socket_t              sock;
    packet_t              pack;
    time_t                current_time;      //当前时间戳
    time_t                pretime = 0;       //上次采样时间戳
 

    struct option         opts[] = 
    {
        {"hostname", required_argument, NULL, 'h'},
        {"port", required_argument, NULL, 'p'},
        {"time_interval", required_argument, NULL, 't'},
        {"help", no_argument, NULL, 'H'},
        {NULL, 0, NULL, 0}
    };


    while ((ch=getopt_long(argc, argv, "h:p:t:H", opts, NULL)) != -1)
    {
        switch (ch)
        {
            case 'h':
                strcpy(hostname,optarg);
                break;
            case 'p':
                port=atoi(optarg);
                break;
            case 't':
                set_time=atoi(optarg);
                break;
            case 'H':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 0;
        }
    }

    if ( (socket_init(&sock, hostname, port)) < 0)
    {
        PARSE_LOG_ERROR("socket initialization failure.\n");
        return -1;
    }


    /* 创建数据库表格 */
    if (  db_init( db_name ) < 0)
    {   
        PARSE_LOG_ERROR("Failed initial database\n");
        return -2; 
    }


    while (1)
    {
        sample_flag = 0;

        /* 判断是否到了采样时间 */
        current_time = time(NULL);
        if ( current_time-pretime >= set_time )
        {
            /* 采样 */
            if( sample_temperature( &pack ) < 0 )
            {
                PARSE_LOG_ERROR("Failed sample data\n");
                return -3;
            }
            sample_flag = 1;
            pretime = current_time;
        }

     	/* 判断socket连接，若未连上则连接 */
    	if ( socket_diag( &sock ) < 0 )
    	{
            PARSE_LOG_WARN("Socket diagnose failure\n");
    	}

    	if ( !sock.connected )
    	{
            if ( (socket_connect(&sock)) < 0 )
            {
            	PARSE_LOG_WARN("socket connect failure.\n");
            }
        }

    	/* 如果还是断开连接就存入数据库 */
    	if ( !sock.connected )
    	{
            if ( sample_flag )
            {
                if ( db_insert( pack ) < 0 )
                {
                    PARSE_LOG_ERROR("Insert data into database failure\n");
                    return -5;
                }
            }

            continue;
        }

     	/* 下面为socket 连接上的情况: */
    	/* 1.如果有采样就发送采样数据 */
    	if ( sample_flag )
    	{
            pack_data( &pack, msg_str, sizeof(msg_str));
            if ( socket_write( &sock, msg_str, strlen(msg_str)) < 0 )
            {
                if ( db_insert( pack ) < 0 )
                {
                    PARSE_LOG_ERROR("Insert data into database failure\n");
                    return -5;
                } 
                socket_close( &sock );
            }
        }

        /* 2.如果数据库中有数据就发送数据库中数据 */
        memset( msg_str, 0, sizeof(msg_str));
        if ( !db_select(msg_str, &msg_str_bytes) )
        {
             PARSE_LOG_DEBUG("select %d bytes msg_str from DB:%s\n",msg_str_bytes, msg_str);
             if ( socket_write( &sock, msg_str, strlen(msg_str)) < 0 )
             {
                 socket_close(&sock);
             }
             else
             {
                 if ( db_delete() < 0 )
                 {
                     return -6;
                 }
             }
        }
	}

	db_close();

	return 0;

}
