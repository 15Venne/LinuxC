#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<stdio.h>
#include<unistd.h>

int main()
{
    int semid;
    int arr[2]={15,0};
    semid=semget(1000,2,IPC_CREAT|0600);
    if(-1==semid)
    {
        perror("semget");
    }
    int ret;
    ret=semctl(semid,0,SETALL,arr);
    if(-1==ret)
    {
        perror("semctl");
    }
    struct sembuf sopp,sopv;
    if(fork())//父进程，生产者
    {
        sopp.sem_num=0;  // arr[0]为仓库，p操作是对arr[0]做
        sopp.sem_op=-1;
        sopp.sem_flg=SEM_UNDO;
        sopv.sem_num=1;  // arr[1]为产品，v操作是对arr[1]做
        sopv.sem_op=1;
        sopv.sem_flg=SEM_UNDO;
        while(1)
        {
            printf("我是生产者，当前仓库=%d,产品=%d\n",semctl(semid,0,GETVAL),semctl(semid,1,GETVAL));
            semop(semid,&sopp,1);//生产者进行p操作，即对仓库-1；
            printf("再生产！！\n");
            semop(semid,&sopv,1);//生产者进行v操作，即对产品+1；
            printf("再生产后，当前仓库=%d,产品=%d\n",semctl(semid,0,GETVAL),semctl(semid,1,GETVAL));
            sleep(1);
        }
    }
    else //子进程，消费者
    {  
        sopp.sem_num=1;  // arr[1]为产品，消费者的p操作是对arr[1]做
        sopp.sem_op=-1;
        sopp.sem_flg=SEM_UNDO;
        sopv.sem_num=0;  // arr[0]为仓库，消费者的v操作是对arr[0]做
        sopv.sem_op=1;
        sopv.sem_flg=SEM_UNDO;
        while(1)
        {
            printf("我是消费者，当前仓库=%d,产品=%d\n",semctl(semid,0,GETVAL),semctl(semid,1,GETVAL));
            semop(semid,&sopp,1);//消费者进行p操作，即对产品-1；
            printf("消费！！\n");
            semop(semid,&sopv,1);//消费者进行v操作，即对仓库+1；
            printf("消费后，当前仓库=%d,产品=%d\n",semctl(semid,0,GETVAL),semctl(semid,1,GETVAL));
            sleep(2);
        }
    }
    return 0;
}
