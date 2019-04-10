#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/sendfile.h>

#define myport 2015

typedef struct train
{
    int dataLen;
    char buf[1000];
}train;

int sendPlus(int fd, char *p, int lenth)
{
    int total=0;
    int ret;
    while(total < lenth)
    {
        ret = send(fd, p+total, lenth-total, 0);
        if(-1 == ret)
        {
            return -1;
        }
        total += ret;
    }
    return 0;
}


int main(int argc, char *argv[])
{
    if(argc!=3)
    {
        printf("argc error\n");
        return -1;
    }
    int socketFd;
    struct sockaddr_in serAddr;
    int ret;

    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(myport);
    serAddr.sin_addr.s_addr = inet_addr(argv[1]);
    int reuse=1;
    setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    ret = connect(socketFd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    if(-1==ret)
    {
        perror("connect");
    }
    printf("已连接，ret=%d\n", ret);
    train data;
    data.dataLen = strlen(argv[2]);
    strcpy(data.buf, argv[2]);
    sendPlus(socketFd, (char*)&data, data.dataLen+4);//发送文件名
    printf("文件名已发送\n");

    int fd = open(argv[2], O_RDONLY);
    if(-1==fd)
    {
        perror("open");
    }

    
    struct stat buf;
    fstat(fd, &buf);
    data.dataLen = sizeof(buf.st_size);
    memcpy(data.buf, &buf.st_size, sizeof(buf.st_size));
    sendPlus(socketFd, (char*)&data, data.dataLen+4); //发送文件大小
    printf("文件大小已发送，filesize=%ld\n", buf.st_size);

    printf("文件发送中……\n");
    ret = sendfile(socketFd, fd, NULL, buf.st_size); //发送文件
    /* char tmp[1000]={0}; */
    /* while(1) */
    /* { */
    /*     data.dataLen=read(fd, tmp, sizeof(tmp)); */
    /*     if(data.dataLen>0) */
    /*     { */
    /*         ret = sendPlus(socketFd, tmp, data.dataLen); */
    /*         if(-1==ret) */
    /*         { */
    /*             return -1; */
    /*         } */
    /*     } */
    /*     else */
    /*     { */
    /*         break; */
    /*     } */
    /* } */

    data.dataLen=0;
    sendPlus(socketFd, (char*)&data, 4); //结束符

    printf("文件上传完毕\n");   

    close(fd);
    close(socketFd);
    return 0;

}
