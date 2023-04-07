/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  client.c
 *    Description:  This file socket client
 *                 
 *        Version:  1.0.0(2023年04月07日)
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


#define  Msg_str "Hi server!"


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
        char                  msg_str[1024];
        int                   sockfd = -1;
        int                   rv = -1;
        char                 *servip = NULL;
        int                   port = 0;
        char                  buf[1024];
        int                   ch;
		char                 *time;   //设置上报数据时间
        struct option         opts[] = {
                {"ipaddr",required_argument,NULL,'i'},
                {"port",required_argument,NULL,'p'},
                {"time",required_argument,NULL,'t'},
                {"help",no_argument,NULL,'h'},\
                {NULL,0,NULL,0}
        };

        while ((ch=getopt_long(argc, argv, "i:p:t:h",opts,NULL)) != -1)
        {
                switch(ch)
                {
                        case 'i':
                                servip=optarg;
                                break;
                        case 'p':
                                port=atoi(optarg);
                                break;
						case 't':
								time=optarg;
                        case 'h':
                                print_usage(argv[0]);
                                return 0;
                }
        }

        if (!servip || !port)
        {
                print_usage(argv[0]);
                return 0;
        }

        sockfd=socket_init(servip,port);

        rv=write(sockfd, Msg_str, strlen(Msg_str));
        if(rv < 0)
        {
          
                     printf("Write to Server by sockfd[%d] failure : %s\n",sockfd,strerror(errno));
                     return -3; 
        }

        while(1)
        {
                
                memset(buf, 0, sizeof(buf));
                rv=read(sockfd, buf, sizeof(buf));
         
                if(rv < 0)
                {
                
                        printf("Read from Server by sockfd[%d] failure : %s\n",sockfd                  
                                        ,strerror(errno));

                        break;
         
                }
                else if(rv == 0)
                {
                        printf("Scoket [%d] get disconnected\n",sockfd);
                        break;
                }
                else if(rv > 0)
                {
                        printf("Read %d bytes data from Server : %s\n",rv,buf);
                }

                printf("Send message to Server:");
                scanf("%s",msg_str);
                rv=write(sockfd, msg_str, strlen(msg_str));
                if(rv < 0)

                {

                        printf("Write to Server by sockfd[%d] failure : %s\n",sockfd
                                ,strerror(errno));
                        return -3;
        
                }
        }

        close(sockfd);

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
        if(sockfd < 0)
        {
                printf("Create socket failure : %s\n",strerror(errno));
                return -1;
        }
        printf("Create socket[%d] successfully!\n",sockfd);

        //DNS
        memset(&hints, 0, sizeof(hints));   //将存放信息的结构体清零
        hints.ai_flags = AI_PASSIVE;      //写入期望返回的结构体的相关信息
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_INET;
        hints.ai_protocol = 0;
        get_back = getaddrinfo(servip, NULL, &hints, &res); // 调用函数
        if(get_back != 0)   //如果函数调用失败
        {
                 printf("DNS faliure:%s\n",strerror(errno));
                 return -1;
        }
        for (readIP = res; readIP != NULL; readIP = readIP->ai_next)   //遍历链表每一个节点，查询关于存储返回的IP的信息
        {
                  addr = (struct sockaddr_in *)readIP->ai_addr;  //将返回的IP信息存储在addr指向的结构体中
                  printf("IP address: %s\n", inet_ntoa(addr->sin_addr));  //inet_ntoa函数将字符串类型IP地址转化为点分十进制
        }
        servip = inet_ntoa(addr->sin_addr);
        freeaddrinfo(res);  //释放getaddrinfo函数调用动态获取的空间

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);

        rv=connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
        if(rv < 0)
        {
                printf("Connect Server[%s:%d] failure : %s\n",
                                servip,  port, strerror(errno));
                return -2;

        }
        printf("Connect to Server [%s:%d] successfully!\n", servip,  port);

		return sockfd;
}
