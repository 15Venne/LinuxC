#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include<sys/mman.h>
#include<string.h>
int main(int argc,char *argv[])
{
	if(argc!=2)
	{
		printf("argc error!");
	}
	int fd;
	fd=open(argv[1],O_RDWR|O_CREAT,0664);
	if(-1==fd)
	{
		perror("open");
	}
	write(fd,"hello",5);
	char *p=mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	if((char*)-1==p)
	{
		perror("mmap");
	}
	printf("*p=%c\n",*p);
	printf("%s\n",p);
	strcpy(p,"world");
	printf("%s\n",p);
	int ret=munmap(p,4096);
	if(-1==ret)
	{
		perror("munmap");
	}
	return 0;
}
