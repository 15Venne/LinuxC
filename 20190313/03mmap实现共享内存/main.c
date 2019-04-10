#include<stdio.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>

int main(int argc,char *argv[])
{
    if(argc!=2)
        printf("argc error");
    int fd;
    fd=open(argv[1],O_RDWR);
    char *p=mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    if((char*)-1==p)
    {
        perror("mmap");
    }
    if(fork())//父进程
    {
        strcpy(p,"hello");
        wait(NULL);
    }
    else //子进程
    {
        sleep(3);
        printf("%s\n",p);
        exit(0);
    }
    munmap(p,4096);
    return 0;
}
