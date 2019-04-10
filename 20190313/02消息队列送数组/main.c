#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<stdlib.h>
#include<string.h>
#include<wait.h>

typedef struct msgbuf
{
    long mtype;
    int arr[5];
}msgbuf;

int compare(const void *a,const void *b)
{
    int *q=(int*)a;
    int *p=(int*)b;
    if(*q>*p)
        return 1;
    else if(*q<*p)
        return -1;
    else
        return 0;
}

int main()
{
    int msgid;
    msgid=msgget(1000,IPC_CREAT|0600);  //创建消息队列
    if(-1==msgid)
    {
        perror("msgget");
    }
    msgbuf msg;
    memset(&msg,0,sizeof(msg));
    msg.mtype=2;
    if(fork())//父进程
    {
        int i,ret;
        msg.arr[0]=15;msg.arr[1]=20;msg.arr[2]=12;msg.arr[3]=42;msg.arr[4]=33;
        ret=msgsnd(msgid,&msg,sizeof(msg.arr),0);  //往消息队列添加消息
        if(-1==ret)
        {
            perror("msgsnd");
        }
        wait(NULL);                                //子进程结束后 
        msgrcv(msgid,&msg,sizeof(msg.arr),0,0);    //读取消息队列信息
        printf("i am parent.\n");
        for(i=0;i<5;++i)
        {
            printf("%d ",msg.arr[i]);
        }
        printf("\n");
 
    }
    else //子进程
    {
        int i,ret;
        msgrcv(msgid,&msg,sizeof(msg.arr),0,0);  //获取消息队列的信息
        qsort(msg.arr,5,sizeof(int),compare);    //排序
        //for(i=0;i<5;++i)
        //{
        //    printf("%d ",msg.arr[i]);
        //}
        //printf("\n");
        ret=msgsnd(msgid,&msg,sizeof(msg.arr),0); //传回给消息队列
        if(-1==ret)
        {
            perror("msgsnd");
        }
        exit(0);
    }
    msgctl(msgid,IPC_RMID,NULL);
    return 0;
}
