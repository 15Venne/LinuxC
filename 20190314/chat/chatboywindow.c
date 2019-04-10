#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define RWME 1
#define RWYOU 2
#define RWFALSE 0
#define RWQUIT 3

/* chatboswindow
 与chatboy共享内存打印chatboy写出与读到的信息
  */


typedef struct msg
{
    int rwflag;
    char buf[1024];
}msg;

void changeline(char *tmp)
{
    int cnt=6;
    char *pspace=tmp;
    while(*tmp!='\0')
    {
        while(cnt<50)
        {
            while(*tmp != ' ' && *tmp != '\0')
            {
                tmp++;
                cnt++;
            }
            if(cnt>=50)
            {
                printf("\n");
                cnt=0;
                break;
            }
            else
            {
                while(pspace!=tmp)
                {
                    printf("%c", *pspace);
                    pspace++;
                }
            }
            if(*tmp == '\0')
            {
                printf("%c",*tmp);
                break;
            }
            tmp++;
            cnt++;
        }
        if(*tmp == '\0')
        {
            break;
        }
        while(pspace!=tmp)
        {
            printf("%c", *pspace);
            pspace++;
            cnt++;
        }
        tmp++;
        cnt++;
    }
    printf("\n");
}

int main()
{
    system("reset");//清屏
    int shmid;
    int ret;
    shmid = shmget(1015, 4096, IPC_CREAT|0600);//获取共享内存id（创建共享内存
    if(-1 == shmid)
    {
        perror("shmget");
    }
    msg *p = shmat(shmid, NULL, 0);  //连接，得到结构体指针
    if((msg*)-1 == p)
    {
        perror("shmat");
    }
    memset(p, 0, sizeof(msg));
                                     //以下创建信号量
    /* int semid = semget(1015, 1, IPC_CREAT|0600); //创建信号量 */
    /* if(-1 == semid) */
    /* { */
        /* perror("semget"); */
    /* } */
    /* ret = semctl(semid, 0, SETVAL, 1);  //初始化信号量 */
    /* if(-1 == ret) */
    /* { */
        /* perror("semctl"); */
    /* } */
    /* struct sembuf sopp, sopv; */
    /* sopp.sem_num = 0; */
    /* sopp.sem_op = -1; */
    /* sopp.sem_flg = SEM_UNDO; */
    /* sopv.sem_num = 0; */
    /* sopv.sem_op = 1; */
    /* sopv.sem_flg = SEM_UNDO; */
    char *tmp;
    while(1)
    {
        //semop(semid, &sopp, 1); //加锁
        if(p->rwflag == RWME || p->rwflag == RWYOU)  //若标志为真，则共享内存上有内容
        {                           //则输出
            if(p->rwflag == RWME)
            {
                tmp=p->buf;
                printf("\e[1;34m");
                while(*tmp!='@')
                {
                    printf("%c",*tmp);
                    tmp++;
                }
                printf("%c\033[0m",*tmp);tmp++;
                changeline(tmp);
                //printf("%s\n",tmp);
            }
            else if(p->rwflag == RWYOU)
            {
                tmp=p->buf;
                printf("\e[1;31m");
                while(*tmp!='@')
                {
                    printf("%c",*tmp);
                    tmp++;
                }
                printf("%c\033[0m",*tmp);tmp++;
                printf("%s\n",tmp);
            }
            p->rwflag = RWFALSE;        //标志设回假
        }
        else if(p->rwflag == RWFALSE)  //若标志为假，continue
        {
            continue;
            
        }
        else if(p->rwflag == RWQUIT)                 //都不是则为结束聊天
        {
            break;
        }
        //semop(semid, &sopv, 1);//解锁
    }
    
    shmdt(p);//解除共享内存连接
    return 0;
}
