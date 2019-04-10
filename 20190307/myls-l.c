#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<sys/types.h>
#include<dirent.h>
#include<string.h>
#include<time.h>
#include<grp.h>
#include<pwd.h>

typedef struct fileInfo
{
    char name[50];
    struct stat *p;
}fileInfo;

// 根据st_mode值转换成rwx权限
/*
例：   st_mode为 81b4
       二进制为
         8   1     b   4	
       1000 0001 1011 0100
权限     -     r w-rw -r--
      (4为d)
*/
char *chmodchange(int n)
{
	char *p;
	p=(char*)malloc(11); //drwxrwxrwx，一共10位
	char temp[15];
	sprintf(temp,"%x",n);
	if('4'==temp[0])
	{
		p[0]='d';
	}
	else
	{
		p[0]='-';
	}
	int i;
	int j=1;
	for(i=9;i>0;--i)
	{
		if(n&j)
		{
			if(i%3==0)
			{
				p[i]='x';
			}
			else if(i%3==2)
			{
				p[i]='w';
			}
			else
			{
				p[i]='r';
			}
		}
		else
		{
			p[i]='-';
		}
		j<<=1;
	}
	p[10]='\0';
	return p;
}
int cnt =0;
//建文件数组
void makeFilearr(fileInfo *fileArr,char *name,struct stat *p)
{
    strcpy(fileArr[cnt].name,name);
    fileArr[cnt].p=(struct stat*)malloc(sizeof(struct stat));
    *fileArr[cnt].p=*p;
    cnt++;
}

char changeAlp(char c)
{
    
    if(c >= 'A' && c <= 'Z')
        return c+32;
    else 
        return c;
}

//无视字母大小写的字符串排序
int mystrcmp(char *p1,char *p2)
{
    int i=0;
    while(*p1 != '\0' && *p2 != '\0')
    {
        if(changeAlp(p1[i])>changeAlp(p2[i]))
            return 1;
        else if(changeAlp(p1[i])<changeAlp(p2[i]))
            return -1;
        else
        {
            p1++;
            p2++;   
        }
    }
    if(p1[i]=='\0'&&p2[i]=='\0')
        return 0;
    if(p1[i]=='\0')
        return -1;
    if(p2[i]=='\0')
        return 1;
}

//快排的compare
int compare(const void *a,const void *b)
{
    fileInfo *p=(fileInfo*)a;
    fileInfo *q=(fileInfo*)b;
    if(mystrcmp(p->name,q->name)<0)
        return -1;
    else if(mystrcmp(p->name,q->name)>0)
        return 1;
    else 
        return 0;
}

//打印文件信息
int fileinfoPrint(struct stat *p)
{
	struct group *mygrp;
	struct passwd *myinfo;
	char tm[40]={0};
	myinfo=getpwuid(p->st_uid);         //用户名转名字
	mygrp=getgrgid(p->st_gid);          //组名转名字
    //最后修改文件的时间
	strcpy(tm,ctime(&p->st_mtime)+4);   //星期几截掉
	tm[strlen(tm)-9]='\0';               //年份，秒数截掉
	printf("%s %ld %s %s %5ld %s ",chmodchange(p->st_mode),p->st_nlink,myinfo->pw_name,mygrp->gr_name,p->st_size,tm); 
                                       //权限            硬链接       用户名          组名            文件大小  时间  
    if('d'==chmodchange(p->st_mode)[0])
        return 34;
    else if('x'==chmodchange(p->st_mode)[9])
        return 32;
    else 
        return 0;
}

int main(int argc,char *argv[])
{
	if(argc!=2)
	{
		printf("argc error!");
	}
	struct dirent *p;
	char buf[1024]={0};
	int ret;
	struct stat cur;
    fileInfo fileArr[100]={0}; //文件数组，排序用
    chdir(argv[1]);   //切换当前工作目录
    char *chpwd=getcwd(NULL,0); //获取当前绝对路径
    printf("%s\n",chpwd);   //打印当前绝对路径
	DIR *dir=opendir(chpwd); //获取DIR
	if(NULL==dir)
	{	
		perror("opendir");
	}
	while((p=readdir(dir))!=NULL)// 依次读取目录下文件
	{
        if('.' == p->d_name[0])  // 如果是隐藏文件，continue
        {
            continue;
        }
		sprintf(buf,"%s/%s",chpwd,p->d_name); //拼接文件路径
		ret=stat(buf,&cur);                   //获取文件stat信息
        if(-1==ret)                           //返回给cur结构体
		{
			return -1;
		}
        makeFilearr(fileArr,p->d_name,&cur);
	}
    //给文件数组排序
    qsort(fileArr, cnt, sizeof(fileInfo), compare);
    int color;
    for(ret=0;ret<cnt;++ret)
    {
        color=fileinfoPrint(fileArr[ret].p);
        if(color==34)
        {
            printf("\e[1;34m%s\033[0m\n",fileArr[ret].name); 
        }
        else if(color==32)
        {
            printf("\e[1;32m%s\033[0m\n",fileArr[ret].name); 
        }
        else
            printf("%s\n",fileArr[ret].name); 
    }
    closedir(dir);
	return 0;
}
