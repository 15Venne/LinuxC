#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<unistd.h>
#include<wait.h>
#include<stdlib.h>
#include<time.h>
#define FALSE 0
#define TRUE 1
#define N 10000000
//
void enter_region(int me,int *p)
{
    int you;
    you=1-me;
    p[me]=1;//该我了
    p[3]=me;   //我的回合（准备完毕
    while(p[3]==me && p[you]==1);//等待对方回合结束
}

void leave_region(int me,int *p)
{
    p[me]=0;//结束回合
}

int main()
{
    time_t begin,end;
    begin=time(NULL);
    int shmid;
    shmid=shmget(1000,4096,IPC_CREAT|0600);
    if(-1==shmid)
    {
        perror("shmget");
    }
    int *p=(int*)shmat(shmid,NULL,0);
    if((int*)-1==p)
    {
        perror("shmat");
    }
    p[2]=0;
    p[0]=0;//父进程回合标识
    p[1]=0;//子进程回合标识
    p[3]=0;
    p[4]=20000001;
    p[5]=0;//父进程计数
    p[6]=0;//子进程计数
    p[7]=100000;//代替j
    if(fork())
    {//父进程，设p[0]
        //int i;
        //int j=100000;
        for(p[5]=0;p[5]< N;++p[5])
        {
            enter_region(0,p);
           // p[0]=TRUE;
           // p[3]=0;
           // while(p[3]==0 && p[1]==TRUE);
            if(p[4]==p[2])
            {
                printf("子进程结束后父进程i=%d\n",p[5]);
            }
            p[2]++;
            if(p[2]==p[7])
            {
                printf("已到%d ",p[7]);
                printf("父进程i=%d  子进程i=%d\n",p[5],p[6]);
                p[7]=p[7]+100000;
            }
            leave_region(0,p);
           // p[0]=FALSE;
        }
        p[4]=p[2];
        wait(NULL);
        printf("p[2]=%d 父进程加了%d次，子进程加了%d次\n",p[2],p[5],p[6]);
    }
    else
    {//子进程设p[1]
        //int i;
        for(p[6]=0;p[6]< N;++p[6])
        {
            enter_region(1,p);
            //p[1]=TRUE;
            //p[3]=1;
            //while(p[3]==1 && p[0]==TRUE);
            if(p[4]==p[2])
            {
                printf("父进程加完后子进程i=%d p[2]=%d\n",p[6],p[2]);
            }
            p[2]++;
            if(p[2]==p[7])
            {
                printf("已到%d ",p[7]);
                printf("父进程i=%d  子进程i=%d\n",p[5],p[6]);
                p[7]=p[7]+100000;
            }
 
            leave_region(1,p);
            //p[1]=FALSE;
        }
        p[4]=p[2];
        printf("i am child. p[2]=%d\n",p[2]);
        exit(0);
    }
    printf("%d\n",p[2]);
    shmdt(p);
    shmctl(shmid,IPC_RMID,NULL);
    end=time(NULL);
    printf("经过了%lds\n",end-begin);
    return 0;
}
