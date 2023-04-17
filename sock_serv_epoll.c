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
#include <netinet/in.h>
#include <libgen.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <ctype.h>

#define  MAX_EVENTS     512
#define  ARRAY_SIZE(x)  (sizeof(x)/sizeof(x[0]))
#define  MSG            "Copy"

static inline void print_usage(char *progname)
{
        
        printf("usage:%s [OPTION]...\n",progname );
        printf("-b(--daemon): set program running on background\n");
        printf("-p(--port): sepcify Server listen port\n");
        printf("-h(--help): print this help information.\n");
        printf("\nExample: %s -b -p 8000\n", progname); 
        return ;
}
//将socket抽象成一个函数
int socket_server_init(char *listen_ip,int listen_port);
void set_socket_rlimit(void);


int main(int argc,char **argv)
{
        int                   listenfd,connfd;
        int                   serv_port = 0;
        char                 *progname = NULL;
        int                   daemon_run = 0;
        int                   ch;
        int                   i,j;
        int                   rv;
        int                   found;
        int                   max;
        char                  buf[1024];

        int                   epollfd;
        struct epoll_event    event;
        struct epoll_event    event_array[MAX_EVENTS];
        int                   events;  //发生了的事件

        struct option         long_options[] = {
                {"daemon",no_argument,NULL,'b'},  //后台运行
                {"port",required_argument,NULL,'p'},
                {"help",no_argument,NULL,'h'},
                {NULL,0,NULL,0}
        };

        progname = basename(argv[0]);
        //parser the command line parameters解析命令行参数
        while ((ch=getopt_long(argc, argv, "bp:h",long_options,NULL)) != -1)
        {
                switch(ch)
                {
                        case 'b':
                                daemon_run=1;
                                break;
                        case 'p':
                                serv_port=atoi(optarg);
                                break;
                        case 'h':
                                print_usage(argv[0]);
                                return EXIT_SUCCESS;
                        default:
                               break;
                }
        }
        if ( !serv_port )
        {
                print_usage(progname);
                return -1;
        }

        set_socket_rlimit();//解除Linux内核最大限制

        listenfd=socket_server_init(NULL, serv_port);
        if(listenfd < 0)
        {
                printf("ERROR:%s server listen on port %d failur\n",argv[0],serv_port);
                return -2;
        }
        printf("%s server start to listen on port %d\n",argv[0],serv_port);
        
        //后台运行程序
        if( daemon_run)
        {

              daemon(0, 0);
        }
       
        if( (epollfd = epoll_create(MAX_EVENTS)) < 0)
        {
            printf("epoll_create() failur:%s\n",strerror(errno));
            return -3;
          
        }
  
        event.events = EPOLLIN;
        event.data.fd = listenfd;
        
        if(epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event) < 0)
        {
            printf("epoll add listen socket failure :%s\n",strerror(errno));
            return -4;

        }

        for( ; ; )
        {
            /*program will block here*/
            events = epoll_wait(epollfd, event_array, MAX_EVENTS, -1);
            if(events <0)
            {
                printf("epoll failure :%s\n",strerror(errno));
                break;
            }
            else if(events == 0)
            {
                printf("epoll get timeout\n");
                continue;
             }

            for(i=0;i<events;i++)
            {
                if((event_array[i].events&EPOLLERR) || event_array[i].events&EPOLLHUP)
                {
                    printf("epoll_wait get error on fd[%d]:%s\n",event_array[i].data.fd,strerror(errno));
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, event_array[i].data.fd,NULL);
                    close(event_array[i].data.fd);
                   
                 }
            
                 /* listen socket get event means new client start connect now */
                 if( event_array[i].data.fd == listenfd)
                 {
                     if( (connfd=accept(listenfd,(struct sockaddr *)NULL,NULL)) < 0)
                     {
                          printf("accept new client failure :%s\n",strerror(errno));
                          continue;      
                     }

                     event.data.fd = connfd;
                     event.events = EPOLLIN;
                     if(epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event) < 0)
                     {
                         printf("epoll add client socket failure:%s\n",strerror(errno));
                         close(event_array[i].data.fd);
                         continue;
                     }
                     printf("epoll add new client socket[%d] successfully.\n",connfd);
                 }


                 else //已连接客户端
                 {
                        memset(buf,0,sizeof(buf));
                        if( (rv=read(event_array[i].data.fd, buf, sizeof(buf))) <= 0)
                         {
                             printf("socket[%d] read failure or getdisconnect and will be removed.\n",event_array[i].data.fd);
                             epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
                             close(event_array[i].data.fd);
                             continue;
                         }
                        else
                         {   
                             printf("socket [%d] read %d bytes data:%s \n",event_array[i].data.fd,rv,buf);
                             for(j= 0;j<rv;j++)
                             {
                                 buf[j] = toupper(buf[j]);

                             }
                             if( write(event_array[i].data.fd,MSG,rv) < 0)
                              {
                                  printf("socket[%d] write failure:%s\n",event_array[i].data.fd,strerror(errno));
                                  epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
                                  close(event_array[i].data.fd);
                              }

                         }
                 }

             }
        
        }

CleanUp:
    close(listenfd);
    return 0;
}

int socket_server_init(char *listen_ip,int listen_port)
{
    struct sockaddr_in     servaddr;
    int                    rv = 0;
    int                    on = 1;
    int                    listenfd;

    if( (listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("Use socket() to create a TCP socket failure: %s\n",strerror(errno));
        return -1;
     
     }

     //解决Address already in use（地址被占用）问题
    setsockopt(listenfd, SOL_SOCKET, SO_RCVTIMEO, &on, sizeof(on));


     memset(&servaddr, 0, sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_port = htons(listen_port);

     //监听所有IP
     if( !listen_ip )
     {
         servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

     }
     else
     {
         if( inet_pton(AF_INET,listen_ip,&servaddr.sin_addr) <=0)
         {
             printf("inet_pton() set listen IP address failure.\n");
             rv = -2;

             goto CleanUp;
         }
      }

 
      
        if( bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
        {
            printf("Use bind() to bind the TCP socket failure: %s\n",strerror(errno));
            rv = -3;
            goto CleanUp;

        }
        
       if(listen(listenfd,13) < 0)
        {
            printf("Use bind() to bind the TCP socket failure: %s\n",strerror(errno));
            rv = -4;
            goto CleanUp ;
         }
CleanUp:
       if(rv < 0)
          close(listenfd);
        else
          rv = listenfd;
 
        return rv;
 }

 void set_socket_rlimit(void)
 {
     struct rlimit limit = {0};

     getrlimit(RLIMIT_NOFILE, &limit);
     limit.rlim_cur = limit.rlim_max;
     setrlimit(RLIMIT_NOFILE, &limit);

     printf("set socket open fd max count to %ld\n",limit.rlim_max);

 }