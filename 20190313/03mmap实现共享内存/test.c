#include<stdio.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>

int main()
{
    char *p=mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
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
