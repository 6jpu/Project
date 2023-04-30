/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket.c
 *    Description:  This socket file 
 *                 
 *        Version:  1.0.0(2023年04月24日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月24日 20时56分53秒"
 *                 
********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/tcp.h>
#include "logger.h"
#include "socket.h"


/*Initial socket work context structure. */
int socket_init(socket_t *sock, char *hostname, int port)
{
    struct  addrinfo     hints;    //定义一个结构体
    int                  get_back = -1;  //定义getaddrinfo函数返回值
    char                *hostip;
    struct  addrinfo    *res;     //定义函数返回的结构体链表的指针
    struct  addrinfo    *readIP;    //定义一个遍历链表的指针
    struct  sockaddr_in *addr;
	

    sock->fd = -1;
    sock->connected = 0;
   
	
	/* DNS */
    memset(&hints, 0, sizeof(hints));   //将存放信息的结构体清零
    hints.ai_flags = AI_PASSIVE;      //写入期望返回的结构体的相关信息
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;
    hints.ai_protocol = 0;
    
	get_back = getaddrinfo(hostname, NULL, &hints, &res); // 调用函数
    if (get_back != 0)   //如果函数调用失败
    {
        PARSE_LOG_ERROR("DNS faliure:%s\n", strerror(errno));
        return -1;
    }
    
	for (readIP=res; readIP!=NULL; readIP=readIP->ai_next)   //遍历链表每一个节点，查询关于存储返回的IP的信息
    {
        addr = (struct sockaddr_in *)readIP->ai_addr;  //返回IP
        PARSE_LOG_INFO("IP address: %s\n", inet_ntoa(addr->sin_addr));  //inet_ntoa函数将字符串类型IP地址转化为点分十进制
    }
    hostip = inet_ntoa(addr->sin_addr);
    freeaddrinfo(res);  //释放getaddrinfo函数调用动态获取的空间
	
    sock->host = hostip;
    sock->port = port;

	return 0;
}


/*  Description: Close the socket */
int socket_close(socket_t *sock)
{
    close(sock->fd);
    sock->fd = -1;

    return 0;
}


/*  Description: Connect to socket server if not connected */
int socket_connect(socket_t *sock)
{
    struct  sockaddr_in  servaddr;
    int                  rv = -1;

    sock->fd=socket(AF_INET, SOCK_STREAM, 0);
    if (sock->fd < 0)
    {
        PARSE_LOG_ERROR("Create socket failure : %s\n", strerror(errno));
        return -1;
    }
    PARSE_LOG_INFO("Create socket[%d] successfully!\n", sock->fd);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(sock->port);
    inet_aton(sock->host,&(servaddr.sin_addr));

    rv=connect(sock->fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (rv < 0)
    {
        PARSE_LOG_WARN("Connect Server[%s:%d] failure : %s\n",
                sock->host, sock->port, strerror(errno));
        socket_close(sock);
    }
    else
    {
    	printf("Connect to Server [%s:%d] successfully!\n", sock->host, sock->port);
    }

	return rv;
   	
}


/*  Description: Check and set socket connect status */
int socket_diag(socket_t *sock)
{
    struct tcp_info info;    
    int             len = sizeof(info);
    
    if (sock->fd <= 0)
    {
        return -1;
    }

    getsockopt(sock->fd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *) & len);
    if ((info.tcpi_state == 1)) 
    {
	    sock->connected = 1;
        PARSE_LOG_INFO("socket connected\n");
    } 
    else
    {
		sock->connected = 0;
        PARSE_LOG_INFO("socket disconnected\n");
    }

    return 0;

}

/*  Description: Writer data to socket server */
int socket_write(socket_t *sock, char *data, int bytes)
{
    int		leftbytes = bytes;
    int		len;
    int		rv;

    while ( leftbytes > 0 )
    {
        len = leftbytes>512 ? 512 : leftbytes;

        rv = write(sock->fd, data, len);
        if ( rv < 0 )
        {
            PARSE_LOG_WARN("Write data failure : %s\n",strerror(errno));
            return -1;
        }

        leftbytes -= rv;
        data += rv;
    }

    return 0;
}

