#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h> //...         //客户端重复上传大文件报段错误，文件描述符异常
#include<unistd.h>               //可能是因为某指针越界导致
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<aio.h>

#define BUFFER_SIZE 1024
#define myport 2015

/* int funcCnt; */

typedef struct Quest
{
    struct aiocb rd;
    int flag;
}Quest,*pQuest;

int recvPlus(int fd, char *p, off_t lenth)
{
    off_t total=0;
    off_t ret;
    while(total<lenth)
    {
        ret=recv(fd, p+total, lenth-total, 0);
        if(0==ret)
        {
            return -1;
        }
        total = total+ret;
    }
    return 0;
}

void aio_completion_handler1(__sigval_t sigval)
{return;}

void aio_completion_handler(__sigval_t sigval)
{
    /* if(funcCnt!=1) */
    /* { */
    /*     return; */
    /* } */
    /* funcCnt++; */
    struct aiocb *prd;
    int ret;
    prd = (struct aiocb*)sigval.sival_ptr;
    prd->aio_sigevent.sigev_notify_function = aio_completion_handler1;//回调函数
                                                        //此函数中又重新调用了aio_read,改变回调函数不再进入此函数
    int dataLen;
    char buf[1000]={0};
    
    dataLen = *(int*)prd->aio_buf;
    printf("文件名长度:%d\n", dataLen);
    memcpy(buf, (char*)prd->aio_buf+4, dataLen);
    printf("文件名:%s\n", buf);

    off_t fileSize;

    fileSize = *(off_t*)((char*)prd->aio_buf+4+dataLen+4);

    int cur=4+dataLen+4+8;
    
    printf("fileSize=%ld\n", fileSize);

    int fd = open(buf, O_CREAT|O_RDWR, 0666);
    if(-1 == fd)
    {
        perror("open");
    }

    int new_fd;
    if(fileSize<=BUFFER_SIZE-cur)
    {
        write(fd, (char*)prd->aio_buf+cur, fileSize);
        fileSize=0;
        free((char*)prd->aio_buf);
        memset(prd, 0, sizeof(struct aiocb));
        close(fd);
        close(prd->aio_fildes);
        printf("接收完毕\n");
    }
    else
    {
        while(fileSize > BUFFER_SIZE-cur)
        {
            //printf("while begin\n");
            ret = write(fd, (char*)prd->aio_buf+cur, BUFFER_SIZE-cur);
            cur=0;
            fileSize=fileSize-ret;
            
            new_fd = prd->aio_fildes;
            free((char*)prd->aio_buf);                           //经测试需要free然后重新malloc
            memset(prd, 0, sizeof(struct aiocb));                //原因不明
            prd->aio_fildes = new_fd;
            prd->aio_buf = (char*)malloc(sizeof(BUFFER_SIZE));
            prd->aio_nbytes = BUFFER_SIZE;
            prd->aio_offset = 0;

            //printf("while ok\n");
            ret = aio_read(prd);
            if(ret<0)
            {
                perror("aio_read");
            }
            while(aio_error(prd)==EINPROGRESS)
            {} 
        }
    
        write(fd, (char*)prd->aio_buf, fileSize);
        fileSize=0;
        free((char*)prd->aio_buf);
        memset(prd, 0, sizeof(struct aiocb));
        close(fd);
        close(prd->aio_fildes);
        printf("接收完毕\n");
    }
    recvPlus(prd->aio_fildes, (char*)&dataLen, 4); //结束符    //不会走到这一步

    /* if(0 == aio_error(prd))  //判断请求是否成功 */
    /* { */
    /*     ret = aio_return(prd); */
    /*     printf("ret=%d\n", ret); */
    /* } */
    
}

int tcpInit(int *sfd, char *ip, int port)
{
    *sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == *sfd)
    {
        perror("socket");
    }
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(port);
    serAddr.sin_addr.s_addr = inet_addr(ip);

    int ret, reuse=1;
    ret = setsockopt(*sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    if(-1 == ret)
    {
        perror("setsockopt");
    }
    ret = bind(*sfd, (struct sockaddr*)&serAddr, sizeof(struct sockaddr));
    if(-1 == ret)
    {
        perror("bind");
    }

    listen(*sfd, 10);
    return 0;
}


int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("argc error\n");
        return -1;
    }

    int fd, ret;
    Quest quest[2];//最多支持2个客户端
    int i;
    memset(quest, 0, sizeof(quest));
    
    int socketFd, new_fd;
    tcpInit(&socketFd, argv[1], myport);
    while(1)
    {
        new_fd = accept(socketFd, NULL, NULL);
        printf("accept成功, new_fd=%d\n", new_fd);
        for(i=0;i<2;++i)
        {
            if(quest[i].rd.aio_fildes==0)
            {
                quest[i].flag=0;
            }
        }
        for(i=0;i<2;++i)
        {
            if(quest[i].flag==0)
            {
                break;
            }
            if(i>1)
            {
                i=0;
            }
        }
        printf("i=%d\n", i);
        quest[i].rd.aio_fildes = new_fd;
        quest[i].rd.aio_buf = (char*)malloc(sizeof(BUFFER_SIZE+1));
        quest[i].rd.aio_nbytes = BUFFER_SIZE;
        quest[i].rd.aio_offset = 0;
        
        //回调通知，sigevent
        quest[i].rd.aio_sigevent.sigev_notify = SIGEV_THREAD; //回调通知
        quest[i].rd.aio_sigevent.sigev_notify_function = aio_completion_handler;//回调函数
        quest[i].rd.aio_sigevent.sigev_notify_attributes = NULL;
        quest[i].rd.aio_sigevent.sigev_value.sival_ptr = &quest[i].rd;
        
        quest[i].flag=1;
        /* funcCnt=1;//防止重复调用回调函数 */
        ret = aio_read(&quest[i].rd);
        if(ret < 0)
        {
            perror("aio_read");
        }
        printf("异步读开始\n");
    }
    return 0;
}
