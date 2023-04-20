/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file socket client
 *                 
 *        Version:  2.0.0(2023年04月20日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月07日 17时00分57秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "ds18b20.h"
#include "get_time.h"
#include "sqlite.h"
#include "logger.h"


int socket_init(char *servip,int port);

void print_usage(char *proname)
{
        printf("%s usage :[servip] [port]\n",proname );
        printf("-i(--ipaddr): sepcify Server IP address\n");
        printf("-p(--port): sepcify Server port\n");
		printf("-t(--time): set data reporting time\n");
        printf("-h(--help): print this help information.\n");

        return ;

}

int main(int argc,char **argv)
{
        int                   model;             //model为0则未到上报时间，为1则已到上报时间
	    time_t                current_timer;     //当前时间戳
		time_t                report_timer;	     //采样时间戳
	    long                  sleep_timer;
		myData                data;
        char                  msg_str[1024];
        int                   sockfd = -1;
        int                   rv = -1;
        char                 *servip = NULL;
        int                   port = 0;
        char                  buf[1024];
        int                   ch;
    	char                 *report_time;     //上报数据时间
    	int                   set_time=3;      //设置上报时间间隔，默认为三秒
        struct option         opts[] = {
                {"ipaddr", required_argument, NULL, 'i'},
                {"port", required_argument, NULL, 'p'},
                {"time", required_argument, NULL, 't'},
                {"help", no_argument, NULL, 'h'},
                {NULL, 0, NULL, 0}
        };

        while ((ch=getopt_long(argc, argv, "i:p:t:h", opts, NULL)) != -1)
        {
                switch (ch)
                {
                        case 'i':
                                servip=optarg;
                                break;
                        case 'p':
                                port=atoi(optarg);
                                break;
						case 't':
								set_time=atoi(optarg);
								break;
                        case 'h':
                                print_usage(argv[0]);
                                return 0;
						default:
								print_usage(argv[0]);
                }
        }

        if ( !servip || !port )
        {
                print_usage(argv[0]);
                return 0;
        }

        if ( (sockfd=socket_init(servip,port)) < 0)
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
DB:			/* 存入数据库 */	
			if ( sqlite_insert(data) < 0)
			{   
				PARSE_LOG_ERROR("Failed to save data into database\n");
				return -3; 
			}   


			/* 尝试重新连接服务器 */
			if ( (sockfd=socket_init(servip,port)) >= 0)
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


int socket_init(char *servip,int port)
{
        struct  sockaddr_in  servaddr;
 		struct  sockaddr_in *addr;
        int                  sockfd = -1;
		int                  get_back = -1;  //定义getaddrinfo函数返回值
        int                  rv = -1;
		struct  addrinfo     hints;    //定义一个结构体
		struct  addrinfo    *res;     //定义函数返回的结构体链表的指针
		struct  addrinfo    *readIP;    //定义一个遍历链表的指针
         
        sockfd=socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
                PARSE_LOG_ERROR("Create socket failure : %s\n", strerror(errno));
                return -1;
        }
        PARSE_LOG_INFO("Create socket[%d] successfully!\n", sockfd);

        //DNS
        memset(&hints, 0, sizeof(hints));   //将存放信息的结构体清零
        hints.ai_flags = AI_PASSIVE;      //写入期望返回的结构体的相关信息
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_INET;
        hints.ai_protocol = 0;
        get_back = getaddrinfo(servip, NULL, &hints, &res); // 调用函数
        if (get_back != 0)   //如果函数调用失败
        {
                 PARSE_LOG_ERROR("DNS faliure:%s\n", strerror(errno));
                 return -1;
        }
        for (readIP=res; readIP!=NULL; readIP=readIP->ai_next)   //遍历链表每一个节点，查询关于存储返回的IP的信息
        {
                  addr = (struct sockaddr_in *)readIP->ai_addr;  //将返回的IP信息存储在addr指向的结构体中
                  PARSE_LOG_INFO("IP address: %s\n", inet_ntoa(addr->sin_addr));  //inet_ntoa函数将字符串类型IP地址转化为点分十进制
        }
        servip = inet_ntoa(addr->sin_addr);
        freeaddrinfo(res);  //释放getaddrinfo函数调用动态获取的空间

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);

        rv=connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
        if (rv < 0)
        {
                PARSE_LOG_ERROR("Connect Server[%s:%d] failure : %s\n",
                                servip, port, strerror(errno));
                return -2;

        }
        printf("Connect to Server [%s:%d] successfully!\n", servip,  port);

		return sockfd;
}
