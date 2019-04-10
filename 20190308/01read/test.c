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
	stu st[3];
	fd=open("test.txt",O_RDWR | O_CREAT,0664);
	if(-1==fd)
	{
		perror("open");
	}
	//写入文件
	write(fd,s,sizeof(s));
	lseek(fd,0,SEEK_SET);
	//读取文件
	read(fd,st,sizeof(st));
	for(i=0;i<3;++i)
	{
		printf("%d %s %5.2f\n",st[i].id,st[i].name,st[i].score);
	}
	close(fd);
	return 0;
}

