/********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  socket.h
 *    Description:  This socket haed file 
 *
 *        Version:  1.0.0(2023年04月24日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月24日 20时57分20秒"
 *                 
 ********************************************************************************/
#ifndef  _SOCKET_H_
#define  _SOCKET_H_


typedef struct socket_s
{
     int   fd;
     char *host;  /*  socket server host name or IP address */
     int   port;      /*  socket server listen port */
     int   connected; /*  socket connect status, 0:Disconnected,1:Connected */
} socket_t;

/*  Description: Initial socket work context structure. */
int socket_init(socket_t *sock, char *hostname, int port);

/*  Description: Connect to socket server if not connected */
int socket_connect(socket_t *sock);

/*  Description: Check and set socket connect status */
int socket_diag(socket_t *sock);

/*  Description: Writer data to socket server */
int socket_write(socket_t *sock, char *data, int bytes);

/*  Description: Close the socket */
int socket_close(socket_t *sock);

#endif
