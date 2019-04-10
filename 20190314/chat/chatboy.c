#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/time.h>
#include<errno.h>
#include<string.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<string.h>
#include<time.h>

#define RWME 1
#define RWYOU 2
#define RWFALSE 0
#define RWQUIT 3

typedef struct msg
{
    int rwflag;
    char buf[1024];
}msg;


int main(int argc,char *argv[])
{
	//if(argc!=3)
	//{
	//	printf("argc error!\n");
	//}
    system("reset");
	int fdr,fdw;
    mkfifo("1",0664);
    mkfifo("2",0664);
	fdr=open("1",O_RDONLY); //打开读写管道文件，先读后写
	if(-1==fdr)
	{
		perror("open1");
	}
	fdw=open("2",O_WRONLY);
	if(-1==fdw)
	{
		perror("open2");
	}
/**********************************************************************/
    int shmid;
    shmid=shmget(1015, 4096, IPC_CREAT|0600);//获取(创建)共享内存
    if(-1==shmid)
    {
        perror("shmget");
    }
    msg *p=shmat(shmid, NULL, 0);//连接共享内存，得到结构体指针
    if((msg*)-1==p)
    {
        perror("shmat");
    }
    memset(p, 0, sizeof(msg)); //初始化共享结构体
/*************************************************************************/
    /* int semid=semget(1015, 1, IPC_CREAT|0600);//创建(获取)信号量 */
    /* if(-1==semid) */
    /* { */
        /* perror("semget"); */
    /* } */
	int ret;
    /* ret =semctl(semid, 0, SETVAL, 1); //初始化信号量 */
    ///* if(-1==ret) */
    ///* { */
    //    /* perror("semctl"); */
    ///* } */
    ///* struct sembuf sopp, sopv; */
    ///* sopp.sem_num = 0; */
    ///* sopp.sem_op = -1; */
    ///* sopp.sem_flg = SEM_UNDO; */
    /* sopv.sem_num = 0; */
    /* sopv.sem_op = 1; */
    /* sopv.sem_flg = SEM_UNDO; */
    int mypid;
    int yourpid;
    mypid = getpid();
/**************************************************************************/
	char buf[200]={0};
	fd_set rdset;
    char message[300];
    time_t stat;
    //告诉对方自己的pid，获取对方的pid
    sprintf(buf, "%d", mypid);
    write(fdw, buf, strlen(buf));
    memset(buf, 0, sizeof(buf));
    read(fdr, buf, sizeof(buf));
    yourpid = atoi(buf);
/******************************************************************************/
    while(1)
	{	
		FD_ZERO(&rdset);
		FD_SET(STDIN_FILENO,&rdset);
		FD_SET(fdr,&rdset);
		ret=select(fdr+1,&rdset,NULL,NULL,NULL);  //小纸条
		if(ret>0)
		{
        /**************************************************************************/
			if(FD_ISSET(STDIN_FILENO,&rdset)) // 写端
			{		
				memset(buf,0,sizeof(buf));
				//scanf("%s",buf);
				ret=read(STDIN_FILENO,buf,sizeof(buf));
				if(0==ret)
				{
					printf("i am a boy\n");
					close(fdr);
					close(fdw);
                    p->rwflag = RWQUIT; //退出的时候告诉窗口退出
					break;
				}
                for(ret=0;ret<40;++ret)
                {
                    printf("\n");
                }
                //system("reset");        //输入内容后清屏,不知道为什么会卡
                memset(message, 0, sizeof(message));
                time(&stat);
                sprintf(message, "田中太郎(pid%d) %s", mypid, ctime(&stat)+11);
                message[strlen(message)-9]='\0';
                sprintf(message, "%s\no___0@: %s",message,buf);
				write(fdw,buf,strlen(buf));
                /*************************************/
             //   semop(semid, &sopp, 1);  //加锁
                if(RWFALSE == p->rwflag)  //向窗口发送自己写的消息
                {
                    strcpy(p->buf, message);
                    p->rwflag = RWME;     //告诉窗口已发送
                }
                //semop(semid, &sopv, 1);  //解锁
			    /**********************************************************************/
            }    
			if(FD_ISSET(fdr,&rdset))    //读端
			{	
				memset(buf,0,sizeof(buf));
				ret=read(fdr,buf,sizeof(buf));
				//printf("%s",buf);  
				if(0==ret)
				{
					printf("886\n");
					close(fdr);
					close(fdw);
                    p->rwflag = RWQUIT; //退出的时候告诉窗口退出
					break;
				}
                memset(message, 0, sizeof(message));
                time(&stat); 
                sprintf(message, "罪歌    (pid%d) %s",yourpid, ctime(&stat)+11);
                message[strlen(message)-9]='\0';
                sprintf(message, "%s\n>///<@: %s",message,buf);
				//write(fdw,buf,strlen(buf));
                /**************************************/
               // semop(semid, &sopp, 1);  //加锁
                if(RWFALSE == p->rwflag)   //向窗口发送收到的消息
                {
                    strcpy(p->buf, message);  
                    p->rwflag = RWYOU;        //告诉窗口收到消息
                }
               // semop(semid, &sopv, 1);  //解锁
                /****************************************/
			}
        /****************************************************************************/
		}
	}
/*********************************************************************************/
    shmdt(p);//解除共享内存连接
    shmctl(shmid, IPC_RMID, NULL);
    unlink("1");
    unlink("2");
	return 0;
}
