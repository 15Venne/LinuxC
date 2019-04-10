#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<time.h>
#include<unistd.h>
#include<sys/mman.h>
#include<time.h>
#include<sys/time.h>

#define myport 2015
//默认端口2015

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

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("argc error\n");
        return -1;
    }
    int socketFd;
    struct sockaddr_in serAddr;
    int ret;
Loop:
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(myport);
    serAddr.sin_addr.s_addr = inet_addr(argv[1]);
    int reuse=1;
    setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));

    ret = connect(socketFd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    if(-1 == ret)
    {
        perror("connect");
        sleep(3);
        goto Loop;
    }
    int dataLen;
    char buf[1000]={0};
    //以下接收文件名-----协议一
    ret = recvPlus(socketFd, (char*)&dataLen, 4);
    if(-1==ret)
    {
        sleep(3);
        goto Loop;
    }
    ret = recvPlus(socketFd, buf, dataLen);       //buf即文件名
    if(-1==ret)
    {
        sleep(3);
        goto Loop;
    }
    //以下接收文件大小-----协议二
    off_t downloadSize = 0;
    off_t fileSize;
    off_t preFileSize = 0;//上一次文件大小
    ret = recvPlus(socketFd, (char*)&dataLen, 4);
    if(-1==ret)
    {
        sleep(3);
        goto Loop;
    }
    ret = recvPlus(socketFd, (char*)&fileSize, dataLen);
    if(-1==ret)
    {
        sleep(3);
        goto Loop;
    }
    printf("fileSize=%ld\n", fileSize);
    off_t fileSizepi = fileSize/100; //文件大小的百分之一
    int fd = open(buf, O_CREAT|O_RDWR, 0666);
    if(-1 == fd)
    {
        perror("open");
    }
    
    struct timeval begin, end;
    gettimeofday(&begin, NULL);

    int fds[2];
    pipe(fds);
    int recvcnt;
    while(fileSize > 65535) //接收文件-----协议三
    {
        recvcnt = splice(socketFd, NULL, fds[1], NULL, 65535, SPLICE_F_MORE | SPLICE_F_MOVE);
        if(recvcnt == 0)
        {
            printf("服务器断开连接 recvcnt=%d\n", recvcnt);
            sleep(3);
            goto Loop;
        }
        if(-1 == recvcnt)
        {
            perror("while1splice1");
        }
        ret = splice(fds[0], NULL, fd, NULL, recvcnt, SPLICE_F_MORE | SPLICE_F_MOVE);
        if(-1 == ret)
        {
            perror("while2splice2");
        }
        fileSize -= recvcnt;
    }
    /* printf("last=%ld\n",fileSize); */
    recvcnt = splice(socketFd, NULL, fds[1], NULL, fileSize, SPLICE_F_MORE | SPLICE_F_MOVE);
    if(-1 == recvcnt)
    {
        perror("splice1");
    }
    ret = splice(fds[0], NULL, fd, NULL, recvcnt, SPLICE_F_MORE | SPLICE_F_MOVE);
    if(-1 == ret)
    {
        perror("splice2");
    }
    fileSize = fileSize - recvcnt;
    if(fileSize!=0)
    {
        recvcnt = splice(socketFd, NULL, fds[1], NULL, fileSize, SPLICE_F_MORE | SPLICE_F_MOVE);
        if(-1 == recvcnt)
        {
            perror("splice1");
        }
        ret = splice(fds[0], NULL, fd, NULL, recvcnt, SPLICE_F_MORE | SPLICE_F_MOVE);
        if(-1 == ret)
        {
            perror("splice2");
        }

    }

    recvPlus(socketFd, (char*)&dataLen, 4); //结束符
    gettimeofday(&end, NULL);
    printf("use time=%ld\n", (end.tv_sec-begin.tv_sec)*1000000+end.tv_usec-begin.tv_usec);
    close(fd);
    close(socketFd);
}
