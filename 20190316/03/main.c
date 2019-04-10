#include<stdio.h>
#include<sys/ipc.h>
#include<sys/mman.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<unistd.h>
#include<string.h>

#define T 1
#define F 0;



typedef struct Data
{
    int turn;
    int interested[4];
    char buf[4000];
}Data;

void threadprint()
{

}

void enter_region(int process, Data *p)
{
    p->interested[process] = T;
    p->turn = process;
    while(p->turn == process && p->interested[0]==T);
}

void leave_region(int process,Data *p)
{
    p->interested[process] = F;
}

void *threadFuncA(void *p)
{
    Data *pA = (Data *)p;
    char buf;
}

void *threadFuncB(void *p)
{
    Data *pB = (Data*)p;
    char buf;
}

void *threadFuncC(void *p)
{
    Data *pC = (Data *)p;
    char buf;
}

int main()
{
    int fd;
    fd=open("file", fd, O_RDWR|O_CREAT, 0666);
    if(-1==fd)
    {
        perror("open");
    }
    lseek(fd, 4096, SEEK_SET);
    Data *p=mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if((Data*)-1==p)
    {
        perror("shmat");
    }
    memset(p, 0, sizeof(Data));
    pthread_t pthA, pthB, pthC;
    int ret;
    
    ret=pthread_create(&pthA, NULL, threadFuncA, p);
    ret=pthread_create(&pthB, NULL, threadFuncB, p);
    ret=pthread_create(&pthC, NULL, threadFuncC, p);
    /*******************************************************************************/
    int i;
    char buf[1024]={0};
    for(i=0;i<50;++i)
    {
        scanf("%s", buf);
        p->interested[0] = T;
        write(fd, p->buf, strlen(p->buf));
        
        memset(buf, 0, sizeof(buf));
    }


    /*******************************************************************************/
    pthread_join(pthA, NULL);
    pthread_join(pthB, NULL);
    pthread_join(pthC, NULL);
    
    munmap(p, 4096);
}
