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
#include<sys/ipc.h>
#include<sys/sem.h>
#include<time.h>
#include<sys/msg.h>

typedef struct msgbuf
{
    long mtype;
    int flag;
    char message[1024];
}msgbuf;


int main(int argc,char *argv[])
{
	/* if(argc!=3) */
	/* { */
		/* printf("argc error!\n"); */
	/* } */
    system("reset");
	int fdr,fdw;
	fdw=open("1",O_WRONLY);
	if(-1==fdw)
	{
		perror("open1");
	}
	fdr=open("2",O_RDONLY);
	if(-1==fdr)
	{
		perror("open2");
	}
    /**********************************************************/
    int msgid;
    msgid=msgget(1015, IPC_CREAT|0600); //创建(获取消息队列)
    if(-1==msgid)
    {
        perror("msgid");
    }
    msgbuf msg;                     //消息结构体
    memset(&msg, 0, sizeof(msg));
    msg.mtype=2;
    /*********************************************/
	char buf[200]={0};
	int ret;
	fd_set rdset;
    time_t stat;
    int mypid;
    int yourpid;
    mypid = getpid();
    //获取对方的pid，告诉对方自己的pid
    read(fdr, buf, sizeof(buf));
    yourpid= atoi(buf);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "%d", mypid);
    write(fdw, buf, strlen(buf));
/******************************************************************************/
	while(1)
	{
		FD_ZERO(&rdset);
		FD_SET(STDIN_FILENO,&rdset);
		FD_SET(fdr,&rdset);
		ret=select(fdr+1,&rdset,NULL,NULL,NULL);
		if(ret>0)
		{	
        /***********************************************************************/
			if(FD_ISSET(STDIN_FILENO,&rdset)) //写端
			{	
				memset(buf,0,sizeof(buf));
				ret=read(STDIN_FILENO,buf,sizeof(buf));
				if(0==ret)
				{
					printf("i am a girl\n");
					close(fdr);
					close(fdw);
					break;
				}
                for(ret=0;ret<40;++ret)
                {
                    printf("\n");
                }
                memset(msg.message, 0, sizeof(msg.message));
                time(&stat);
                sprintf(msg.message, "罪歌    (pid%d) %s",mypid, ctime(&stat)+11);
                msg.message[strlen(msg.message)-9]='\0';
                //sprintf(msg.message, "%s\no___0@: %s", msg.message, buf);
                strcat(msg.message,"\no___0@: ");
                strcat(msg.message, buf);
				write(fdw,buf,strlen(buf));
                msg.flag=1;
                /******************************************/
                ret=msgsnd(msgid, &msg, sizeof(msg.message), 0); //往消息队列添加消息
                if(-1==ret)
                {
                    perror("msgsnd");
                }

			}
            /******************************************************************/
			if(FD_ISSET(fdr,&rdset))        //读端
			{
				memset(buf,0,sizeof(buf));
				ret=read(fdr,buf,sizeof(buf));
				//printf("%s",buf);	
				if(0==ret)
				{
					printf("88\n");
					close(fdr);
					close(fdw);
					break;
				} 
                memset(msg.message, 0, sizeof(msg.message));
                time(&stat);
                sprintf(msg.message, "田中太郎(pid%d) %s",yourpid, ctime(&stat)+11);
                msg.message[strlen(msg.message)-9]='\0';
                //sprintf(msg.message, "%s\n>///<@: %s", msg.message, buf);
                strcat(msg.message, "\n>///<@: ");
                strcat(msg.message, buf);
                msg.flag=2;
                /*******************************************************/
	            ret=msgsnd(msgid, &msg, sizeof(msg.message), 0); //往消息队列添加消息
                if(-1==ret)
                {
                    perror("msgsnd");
                }
			}
		}
	}
/************************************************************************************/
    msg.flag=3;
    msgsnd(msgid, &msg, sizeof(msg.message), 0);
	msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
