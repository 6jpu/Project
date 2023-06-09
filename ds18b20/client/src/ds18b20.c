/*********************************************************************************
 *      Copyright:     (C) 2023 Kun_<1433729173@qq.com>
 *                     All rights reserved.
 *
 *       Filename:     ds18b20.c
 *       Description:  This file ds18b20 get temperature 
 *                 
 *       Version:      2.0.0(2023年04月30日)
 *       Author:       Kun_ <1433729173@qq.com>
 *       ChangeLog:    1, Release initial version on "2023年04月10日 17时25分12秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include "ds18b20.h"
#include "logger.h"


int get_temperature(float *temp)
{
	int            rv = 0;
	int            fd = -1;
	int            found = 0;
//	char           w1_path[64] = "/sys/bus/w1/devices/";
	char           w1_path[64] = "/home/luofangheng/git/Project/";
	char           chip_sn[32];
	char           buf[128];
	char          *ptr = NULL;
	DIR           *dirp = NULL;
	struct dirent *direntp = NULL;


	dirp  = opendir(w1_path);
	if ( !dirp )
	{
		PARSE_LOG_ERROR("open folder %s failure: %s\n", w1_path, strerror(errno));
		rv = -1;
		goto CleanUp;
	}
	while ( NULL != (direntp=readdir(dirp)))
	{
		if (strstr(direntp->d_name, "28-"))
		{
			strncpy(chip_sn, direntp->d_name, sizeof(chip_sn));
			found = 1;
		}
	}
	if ( !found )
	{
		PARSE_LOG_ERROR("Can not find ds18b20 chipset\n");
		rv = -2;
		goto CleanUp;
	}

	closedir(dirp);

	strncat(w1_path, chip_sn, sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path, "/w1_slave", sizeof(w1_path)-strlen(w1_path));

	fd=open(w1_path, O_RDONLY);
	if ( fd < 0)
	{
		PARSE_LOG_ERROR("open file failure: %s\n", strerror(errno));
		//perror("open file failure");
		rv = -3;
		goto CleanUp;
	}
	memset(buf, 0, sizeof(buf));	
	if ( read(fd, buf, sizeof(buf)) < 0)
	{
		PARSE_LOG_ERROR("read data from fd=%d failure: %s\n", fd, strerror(errno));
		rv = -4;
		goto CleanUp;
	}

	ptr=strstr(buf, "t=");
	if ( !ptr )
	{
		PARSE_LOG_ERROR("Can not find t= string");
		rv = -5;
		goto CleanUp; 
	}

	ptr +=2;

	*temp= atof(ptr)/1000;

CleanUp:
	close(fd);
	return rv;	

}

