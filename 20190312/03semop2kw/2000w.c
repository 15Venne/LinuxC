#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<unistd.h>
#include<wait.h>
#include<stdlib.h>
#include<time.h>
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
    int *p=shmat(shmid,NULL,0);
    if((int*)-1==p)
    {
        perror("shmat");
    }
    p[0]=0;
    int semid=semget(1000,1,IPC_CREAT|0600);//创建信号量集合
    int ret=semctl(semid,0,SETVAL,1);//初始化
    if(-1==ret)
    {
        perror("semctl");
    }
    struct sembuf sopp,sopv;
    sopp.sem_num=0;
    sopp.sem_op=-1;
    sopp.sem_flg=SEM_UNDO;
    sopv.sem_num=0;
    sopv.sem_op=1;
    sopv.sem_flg=SEM_UNDO;
    if(fork())
    {
        int i;int j=100000;
        for(i=0;i<10000000;++i)
        {
            semop(semid,&sopp,1);
            p[0]++;
            if(p[0]>j)
            {
                printf("已到%d\n",j);
                j=j+100000;
            }
            semop(semid,&sopv,1);
        }
        wait(NULL);
        printf("p[0]=%d\n",p[0]);
    }
    else
    {
        int i;
        for(i=0;i<10000000;++i)
        {
            semop(semid,&sopp,1);
            p[0]++;
            semop(semid,&sopv,1);
        }
        exit(0);
    }
    shmdt(p);
    shmctl(shmid,IPC_RMID,NULL);
    end=time(NULL);
    printf("经过了%lds\n",end-begin);
    return 0;
}
