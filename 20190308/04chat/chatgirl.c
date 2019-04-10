#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/time.h>
#include<errno.h>
#include<string.h>

int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		printf("argc error!\n");
	}
	int fdr,fdw;
	fdw=open(argv[1],O_WRONLY);
	if(-1==fdr)
	{
		perror("open1");
	}
	fdr=open(argv[2],O_RDONLY);
	if(-1==fdw)
	{
		perror("open2");
	}
	char buf[200];
	int ret;
	fd_set rdset;
	while(1)
	{
		FD_ZERO(&rdset);
		FD_SET(STDIN_FILENO,&rdset);
		FD_SET(fdr,&rdset);
		ret=select(fdr+1,&rdset,NULL,NULL,NULL);
		if(ret>0)
		{	
			if(FD_ISSET(STDIN_FILENO,&rdset))
			{	
				memset(buf,0,sizeof(buf));
				//scanf("%s",buf);
				ret=read(STDIN_FILENO,buf,sizeof(buf));
				if(0==ret)
				{
					printf("i am a girl\n");
					close(fdr);
					close(fdw);
					break;
				}
				write(fdw,buf,strlen(buf));
			}
			if(FD_ISSET(fdr,&rdset))
			{
				memset(buf,0,sizeof(buf));
				ret=read(fdr,buf,sizeof(buf));
				printf("%s",buf);	
				if(0==ret)
				{
					printf("88\n");
					close(fdr);
					close(fdw);
					break;
				}	
			}
		}
	}
	return 0;
}
