#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
typedef struct student
{
	int id;
	char name[20];
	float score;	
}stu;

int main()
{
	stu s[3]={{1001,"Jonathan",99.5},{1002,"Joseph",98.5},{1003,"DIO",100}};
	int fd;
	int i;
	int ret;
	char buf[1024]={0};
	char buf2[20]={0};
	fd=open("stu.txt",O_RDWR | O_CREAT,0664);
	if(-1==fd)
	{
		perror("open");
	}
	for(i=0;i<3;++i)
	{
		sprintf(buf,"%d %s %5.2f\n",s[i].id,s[i].name,s[i].score);
		write(fd,buf,strlen(buf));
	}
	lseek(fd,0,SEEK_SET);
	while(memset(buf2,0,sizeof(buf2)),(ret=read(fd,buf2,sizeof(buf2)))!=0)
	{
		printf("%s",buf2);
	}
	close(fd);
	return 0;
}

