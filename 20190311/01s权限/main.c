#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        printf("argc error\n");
    }
    printf(" pid=%d\nppid=%d\n uid=%d\n gid=%d\neuid=%d\negid=%d\n",
           getpid(),getppid(),getuid(),getgid(),geteuid(),getegid());
    int fd=open(argv[1],O_RDWR);
    write(fd,"hello",5);
    close(fd);
    return 0;
}
