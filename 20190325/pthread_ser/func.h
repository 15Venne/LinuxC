#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/sendfile.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>

#define FILENAME "file"
#define MYPORT 2015
#define TNUM 5
#define QUECAPACITY 10

typedef struct tag_Node  //任务结点
{
    int new_fd;
    struct tag_Node *pnext;
}Node_t,*pNode_t;

typedef  struct Queue //结点队列
{
    pNode_t phead;
    pNode_t ptail;
    int qCapacity;
    int size;
    pthread_mutex_t mutex;
}Queue,*pQueue;

typedef struct Factory
{
    pthread_t *pthid; //线程id的起始地址
    int threadNum;
    pthread_cond_t cond;
    pQueue pQue;
    short startFlags;
}Factory,*pFactory;

typedef struct train
{
    int dataLen;
    char buf[1000];
}train;

int sendPlus(int, char*, int); //tranFile中

int tranFile(int); //发送文件 ,子线程拿到任务结点后

int nodeGet(pQueue, pNode_t*); //子线程中，参数2二级指针，要拿到头结点

void *threadQuest(void*);//子线程函数

void queInsert(pQueue, pNode_t);//main函数 whie(1)中

int tcpInit(int*, char*, int);//main函数中

void factoryStart(pFactory);//main函数中,factory初始化后

void queInit(pQueue, int);//factoryInit中

void factoryInit(pFactory, int, int);//main函数中


