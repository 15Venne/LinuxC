#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
void dirprint(char *path,int width)
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
		sprintf(buf,"%s/%s",path,p->d_name);
		printf("%*s%s\n",width," ",p->d_name);
		dirprint(buf,width+4);
	}
}

int main(int argc,char *argv[])
{
	if(argc!=2)
	{	
		printf("argc error!");
		return -1;
	}
	//puts(argv[1]);
    puts(getcwd(NULL,0));
	dirprint(argv[1],4);
	return 0;
}
