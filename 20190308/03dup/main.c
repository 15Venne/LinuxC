#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		printf("argc error!");
		return -1;
	}
	int fd1,fd2,fd3,fd4;
	fd1=open(argv[1],O_RDWR);
	if(-1==fd1)
	{
		perror("open1");
		return -1;
	}
	printf("\n");
	close(1);
	fd2=dup(fd1);
	printf("fd1=%d\nfd2=%d\n",fd1,fd2);
	printf("Long may the sun shine!\n");
	fd3=open(argv[2],O_RDWR);
	if(-1==fd3)
	{
		perror("open3");
		return -1;
	}
	fd4=dup2(fd3,2);
	printf("fd3=%d\nfd4=%d\n",fd3,fd4);
	perror("open?");
	perror("?");
	close(fd1);
	close(fd2);
	close(fd3);
	close(fd4);
	return 0;
}
