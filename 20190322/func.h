#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/uio.h>

#define FILENAME "file"


typedef struct ProcessData //子进程结点
{
    pid_t pid;
    int fd;
    short busy;
}ProcessData;

typedef struct train //文件小火车，总长1004
{
    int dataLen;
    char buf[1000];
}train;

int sendPlus(int, char*, int);

int sendFd(int, int);//传送描述符

int recvFd(int, int*);//接受描述符

int tranFile(int); //发送文件

int makeChild(ProcessData*, int);//创建子进程

int tcpInit(int*, char*, int);//封装socket,bind,listen
                              //参数1:socketFd,
                              //参数2:ip,
                              //参数3:端口


