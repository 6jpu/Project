#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>


int get_temperature(float *temp)
{
	int            fd = -1;
	char           buf[128];
	char          *ptr = NULL;
	DIR           *dirp = NULL;
	struct dirent *direntp = NULL;
	//char           w1_path[64] = "/sys/bus/w1/devices/";
	char           w1_path[64] = "/home/luofangheng/git/Project/";
	char           chip_sn[32];
	int            found = 0;


	dirp  = opendir(w1_path);
	if( !dirp )
	{
		printf("open folder %s failure: %s\n",w1_path,strerror(errno));
		return -1;
	}
	while( NULL != (direntp=readdir(dirp)))
	{
		if(strstr(direntp->d_name,"28-"))
		{
			strncpy(chip_sn,direntp->d_name,sizeof(chip_sn));
			found = 1;
		}
	}
	if(!found)
	{
		printf("Can not find ds18b20 chipset\n");
		return -2;
	}

	closedir(dirp);

    strncat(w1_path, chip_sn, sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path, "/w1_slave", sizeof(w1_path)-strlen(w1_path));

	fd=open(w1_path,O_RDONLY);
	if( fd < 0)
	{
		printf("open file failure: %s\n",strerror(errno));
		//perror("open file failure");
		return -3;
	}
	memset(buf,0,sizeof(buf));	
	if( read(fd,buf,sizeof(buf)) < 0)
	{
		printf("read data from fd=%d failure: %s\n",fd,strerror(errno));
		return -4;
	}
//      printf("buf :%s\n",buf);

	ptr=strstr(buf,"t=");
	if(!ptr)
	{
		printf("Can not find t= string");
		return -5;

	}

	ptr +=2;
//	printf("ptr: %s\n",ptr);

	*temp= atof(ptr)/1000;
//	printf("temprature: %f\n",*temp);
        

	close(fd);

        return 0;	

}


int get_sn(char *sn)
{
    char          *ptr = NULL;
    DIR           *dirp = NULL;
    struct dirent *direntp = NULL;
    //char           w1_path[64] = "/sys/bus/w1/devices/";
    char           w1_path[64] = "/home/luofangheng/git/Project/";
    int            found = 0;
    char           chip_sn[32];

    dirp  = opendir(w1_path);
    if( !dirp )
    {   
        printf("open folder %s failure: %s\n",w1_path,strerror(errno));
        return -1; 
    }   
    while( NULL != (direntp=readdir(dirp)))
    {   
        if(strstr(direntp->d_name,"28-"))
        { 
			strncpy(chip_sn,direntp->d_name,sizeof(chip_sn));
			memset(sn,0,sizeof(sn));
			strncpy(sn,chip_sn,sizeof(chip_sn));
            found = 1;
        }   
    }   
    if(!found)
    {   
        printf("Can not find ds18b20 chipset\n");
        return -2; 
    }   

    closedir(dirp);
    
    return 0;   

}
