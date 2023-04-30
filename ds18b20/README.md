# project

### 介绍

本项目主要是树莓派上通过一线协议(1-Wire)连接DS18B20，然后采用网络socket编程同时实现客户端和服务器端程序。其中客户端主要实现定时上报的功能，而服务器端则用来采集客户端上报的程序并永久存储到数据库中。

### 软件架构

<img width="363" alt="image" src="https://user-images.githubusercontent.com/130042891/235354330-d2acab1f-464d-40ae-b44a-64ea5dd81f90.png">


### 安装教程

##### 客户端：

`/* 安装 */    
~ds18b20/client$ make    
/* 卸载，清除可执行文件和目标文件 */    
~ds18b20/client$ make distclean`

##### 服务器：

`/* 安装 */    
~ds18b20/server$ make     
/* 卸载，清除可执行文件和目标文件 */   
~ds18b20/server$ make distclean`

### 使用说明

使用本程序之前需在ds18b20/client/cli和ds18b20/servert/ser下新建 Log 文件夹以存放日志信息

##### 客户端：

`~ds18b20/client$ cd cli    
~ds18b20/client/cli$ mkdir Log    
/* -h 指定服务器IP  -p 指定服务器端口  -t 指定采样时间间隔  */      
~ds18b20/client/cli$ ./clientapp -h 192.168.0.1 -p 6666 -t 30    
/* 或者使用默认IP 127.0.0.1  默认端口 6666  默认采样时间间隔 3s */      
~ds18b20/client/cli$ ./clientapp`


##### 服务器：

`~ds18b20/servert$ cd ser    
~ds18b20/servert/ser$ mkdir Log      
~ds18b20/servert/ser$ ./serverapp -p 6666`   

### 参与贡献

1.  Fork 本仓库
2.  新建 main 分支
3.  提交代码
4.  新建 Pull Request
