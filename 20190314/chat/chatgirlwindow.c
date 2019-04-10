#include<stdio.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<string.h>
#include<stdlib.h>
#include<sys/msg.h>

/*通过消息列表接受chatgirl写出和收到的信息。
 
 */

#define RWME 1
#define RWYOU 2
#define RWFALSE 0
#define RWQUIT 3

typedef struct msgbuf
{
    long mtype;
    int flag;
    char message[1024];
}msgbuf;

int main()
{
    system("reset");//清屏
    int msgid;
    int ret;
    msgid = msgget(1015, IPC_CREAT|0600);//获取共享内存id（创建共享内存
    if(-1 == msgid)
    {
        perror("msgget");
    }
    msgbuf msg;
    memset(&msg, 0, sizeof(msg));
    msg.mtype=2;
    /****************************************************/
    char *tmp;
    while(1)
    {
        memset(&msg, 0, sizeof(msg));
        msgrcv(msgid, &msg, sizeof(msg.message), 0, 0);
        if(msg.flag == RWME || msg.flag == RWYOU)  //若标志为真，则共享内存上有内容
        {                           //则输出
            if(msg.flag == RWME)
            {
                tmp = msg.message;
                printf("\033[1;34m");
                while(*tmp!='@')
                {
                    printf("%c",*tmp);
                    tmp++;
                }
                printf("%c\033[0m",*tmp);tmp++;

                printf("%s\n",tmp);
            }
            else if(msg.flag == RWYOU)
            {
                tmp=msg.message;
                printf("\033[1;31m");
                while(*tmp!='@')
                {
                    printf("%c",*tmp);
                    tmp++;
                }
                printf("%c\033[0m",*tmp);tmp++;
                printf("%s\n",tmp);
            }
            msg.flag = RWFALSE;        //标志设回假
        }
        else if(msg.flag == RWFALSE)  //若标志为假，continue
        {
            continue;
            
        }
        else if(msg.flag == RWQUIT)                 //都不是则为结束聊天
        {
            break;
        }
    }
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
