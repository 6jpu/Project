/*********************************************************************************
 *       Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *       Description:  This file socket client
 *                 
 *       Version:  2.0.0(2023年04月20日)
 *       Author:  Kun_ <1433729173@qq.com>
 *       ChangeLog:  1, Release initial version on "2023年04月07日 17时00分57秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
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
    int                   model = 1;         //model为0则未到上报时间，为1则已到上报时间
    time_t                current_timer;     //当前时间戳
    time_t                report_timer;      //采样时间戳
    long                  sleep_timer;
    myData                data;
    int                   sockfd = -1;
    int                   rv = -1;
    int                   port = 6666;		//默认端口
    char                  msg_str[1024];
    char                  buf[1024];
    int                   set_time = 3;      //设置上报时间间隔，默认为三秒
    char                  hostname[] = "127.0.0.1";		//默认IP
    char                 *report_time;     //上报数据时间
 

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
                hostname=optarg;
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
        }
    }

    if ( (sockfd=socket_init(hostname,port)) < 0)
    {
        PARSE_LOG_WARN("socket failure.\n");
    }


    /* 创建数据库表格 */
    if ( sqlite_init() < 0)
    {   
        PARSE_LOG_ERROR("Failed initial database\n");
        return -3; 
    }


    while (1)
    {                       

Report:     
        /* 获取上报时间 */
        report_timer = time(NULL); 
        get_time(data.report_time);

        current_timer = time(NULL);
        sleep_timer = current_timer-report_timer;
        if ( sleep_timer < set_time )
        {
            /* model为0则未到上报时间，为1则已到上报时间 */
            model = 0;
        }

        if ( model == 0 )
        {
            /* 如果还没到上报时间则先发送数据库里的内容 */
            memset(msg_str,0,sizeof(msg_str));
            while ( sqlite_select(msg_str) == 0 )
            {
                /* 上报客户端 */
                rv=write(sockfd, msg_str, strlen(msg_str));
                if ( rv < 0 ) 
                {   
                    /* 上报数据失败则存入数据库 */
                    PARSE_LOG_WARN("Write to Server by sockfd[%d] failure : %s\n", 
                            sockfd, strerror(errno));
                    close(sockfd);
                    goto DB;

                }    

                memset(buf, 0, sizeof(buf));
                rv=read(sockfd, buf, sizeof(buf));
                if (rv < 0)
                {   
                    /* 未收到确认回复则存入数据库 */
                    PARSE_LOG_WARN("Read from Server by sockfd[%d] failure : %s\n",sockfd, 
                            strerror(errno));
                    close(sockfd);
                    goto DB;

                }   
                else if (rv == 0)
                {   
                    /* 服务器断开则存入数据库 */
                    PARSE_LOG_WARN("Scoket [%d] get disconnected\n", sockfd);
                    close(sockfd);
                    goto DB;
                }   
                else if(rv > 0)
                {   
                    PARSE_LOG_INFO("Read %d bytes data from Server : %s\n", rv, buf);
                }    


                /* 从数据库中删除已发送的数据 */
                if ( sqlite_delete() < 0 )
                {
                    PARSE_LOG_ERROR("Delete data from database failure\n");
                    return -3;
                }
            }
        }



        current_timer = time(NULL);
        sleep_timer = current_timer-report_timer;
        if ( sleep_timer < set_time )
        {
            sleep( set_time-sleep_timer );
            model = 1;
        }
        else
        {
            model = 1;
        }

        if ( model == 1)
        {
            /* 如果到上报时间则先采样上报数据 */ 
            /* 获取上报温度 */
            if ( get_temperature(&data.temp) < 0 )
            {
                PARSE_LOG_ERROR("get temperature failure.\n");
                return -2;
            }
            /* 获取上报产品序列号 */
            if ( get_sn(data.sn) < 0)
            {
                PARSE_LOG_ERROR("get sn failure.\n");
                return -2;
            }
            /* 生成字符串 */    
            memset(msg_str, 0, sizeof(msg_str));
            snprintf(msg_str, sizeof(msg_str), "%s,%f,%s\n", data.report_time, data.temp, data.sn);
            PARSE_LOG_DEBUG("msg_str:%s\n", msg_str);
            /* 上报客户端 */
            rv=write(sockfd, msg_str, strlen(msg_str));
            if ( rv < 0 ) 
            {   
                /* 上报数据失败则存入数据库 */
                PARSE_LOG_WARN("Write to Server by sockfd[%d] failure : %s\n", sockfd, strerror(errno));
                close(sockfd);
                break;

            }       

            memset(buf, 0, sizeof(buf));
            rv=read(sockfd, buf, sizeof(buf));
            if (rv < 0)
            {
                /* 未收到确认回复则存入数据库 */
                PARSE_LOG_WARN("Read from Server by sockfd[%d] failure : %s\n",sockfd, 
                        strerror(errno));
                close(sockfd);
                break;

            }
            else if (rv == 0)
            {
                /* 服务器断开则存入数据库 */
                PARSE_LOG_WARN("Scoket [%d] get disconnected\n", sockfd);
                close(sockfd);
                break;
            }
            else if (rv > 0)
            {
                PARSE_LOG_INFO("Read %d bytes data from Server : %s\n", rv, buf);
            }

        }

    }


    while (1)
    {
DB:         /* 存入数据库 */    
        if ( sqlite_insert(data) < 0)
        {   
            PARSE_LOG_ERROR("Failed to save data into database\n");
            return -3; 
        }   


        /* 尝试重新连接服务器 */
        if ( (sockfd=socket_init(hostname,port)) >= 0)
        {
            PARSE_LOG_INFO("socket reconnect successfully.\n");
            goto Report;
        }

        current_timer = time(NULL);
        sleep_timer = current_timer-report_timer;
        if ( sleep_timer < set_time )
        {
            sleep( set_time-sleep_timer );
        }

        /* 重新连接失败则继续存入数据库 */
        /* 获取上报温度 */
        if ( get_temperature(&data.temp) < 0 )
        {
            PARSE_LOG_ERROR("get temperature failure.\n");
            return -2;
        }
        /* 获取上报产品序列号 */
        if ( get_sn(data.sn) < 0)
        {
            PARSE_LOG_ERROR("get sn failure.\n");
            return -2;
        }
        /* 获取上报时间 */
        report_timer = time(NULL);
        get_time(data.report_time);


    }


    return 0;
}


