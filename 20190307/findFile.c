#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>

void dirprint(char *path,char *filename)
{
	DIR *dir;
	dir=opendir(path);
	if(NULL==dir)
	{
		return;
	}
	struct dirent *p;
	char buf[1024]={0};
	
	while((p=readdir(dir))!=NULL)
	{
		if(0==(strcmp(p->d_name,"."))||(0==strcmp(p->d_name,"..")))
		{
			continue;
		}
		if(0==(strcmp(p->d_name,filename)))
		{
			
			printf("%s/%s\n",path,filename);
		}
		sprintf(buf,"%s/%s",path,p->d_name);
		dirprint(buf,filename);
	}
}

int main(int argc,char *argv[])
{
	if(argc!=3)
	{	
		printf("argc error!");
		return -1;
	}
	
	chdir(argv[1]);
	dirprint(getcwd(NULL,0),argv[2]);
	return 0;
}
