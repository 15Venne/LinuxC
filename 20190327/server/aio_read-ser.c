#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h> //...            // 不支持过大的文件
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<aio.h>
#include<sys/mman.h>

#define myport 2015


typedef struct Quest
{
    struct aiocb rd;
    int flag;
    int fd;
}Quest,*pQuest;

int recvPlus(int fd, char *p, int lenth)
{
    int total=0;
    int ret;
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


void aio_completion_handler(__sigval_t sigval)
{
    struct aiocb *prd;
    int ret;
    prd = (struct aiocb*)sigval.sival_ptr;
    
    while(aio_error(prd)==EINPROGRESS);  
    if(aio_error(prd)==0)
    {
        printf("成功返回\n");
    
        recvPlus(prd->aio_fildes, (char*)&ret, 4);
        close(prd->aio_fildes);
        munmap((char*)prd->aio_buf, prd->aio_nbytes);
    }    
}

void getNameAndFileSize(int new_fd, char *name, off_t *fileSize)
{
    int dataLen;
    
    recvPlus(new_fd, (char*)&dataLen, 4);
    recvPlus(new_fd, name, dataLen);
    off_t fileSize1;
    recvPlus(new_fd, (char*)&dataLen, 4);
    recvPlus(new_fd, (char*)&fileSize1, dataLen);
    printf("fileSize=%ld\n", fileSize1);
    *fileSize=fileSize1;
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
    char name[30]={0};
    off_t fileSize;
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

        getNameAndFileSize(new_fd, name, &fileSize);
        quest[i].fd = open(name, O_CREAT|O_RDWR, 0666);
        if(-1 == quest[i].fd)
        {
            perror("open");
        }
        printf("quest[i].fd=%d\n", quest[i].fd);
        ftruncate(quest[i].fd, fileSize);

        quest[i].rd.aio_fildes = new_fd;
        quest[i].rd.aio_buf = mmap(NULL, fileSize, PROT_READ|PROT_WRITE, MAP_SHARED, quest[i].fd, 0);
        quest[i].rd.aio_nbytes = fileSize;
        quest[i].rd.aio_offset = 0;
        printf("aio_nbytes=%ld\n",quest[i].rd.aio_nbytes);
        //回调通知，sigevent
        quest[i].rd.aio_sigevent.sigev_notify = SIGEV_THREAD; //回调通知
        quest[i].rd.aio_sigevent.sigev_notify_function = aio_completion_handler;//回调函数
        quest[i].rd.aio_sigevent.sigev_notify_attributes = NULL;
        quest[i].rd.aio_sigevent.sigev_value.sival_ptr = &quest[i].rd;
        
        quest[i].flag=1;
        ret = aio_read(&quest[i].rd);
        if(ret < 0)
        {
            perror("aio_read");
        }
        printf("异步读开始\n");
    }
    return 0;
}
