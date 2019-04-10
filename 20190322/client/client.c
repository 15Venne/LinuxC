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
    //time_t begin, now;//按时间打印进度条
    //begin=time(NULL);
    
    //以下接收文件内容-----协议三
    double temp;
    int i;
    while(1)
    {
        ret = recvPlus(socketFd, (char*)&dataLen, 4);
        if(-1==ret)
        {
            sleep(3);
            goto Loop;
        }
        if(dataLen>0)
        {
            ret = recvPlus(socketFd, buf, dataLen);
            if(-1==ret)
            {
                sleep(3);
                printf("\n");
                goto Loop;
            }
            downloadSize += dataLen;
            write(fd, buf, dataLen);
            //now = time(NULL);
            if(downloadSize - preFileSize >=fileSizepi)/* if(now - begin >= 1) */
            {
                temp = (double)downloadSize/fileSize*100;
                printf("\r");
                
                printf("\033[42;32m%*s\033[0m", (int)temp/2, " ");
                printf("%*s", (50-(int)temp/2), " ");
                printf("[%4.2f%s]", temp, "%");
                fflush(stdout);
                //begin = now;
                preFileSize = downloadSize;
            }
        }
        else
        {
            printf("\r\033[42;32m%*s\033[0m[100.00%s]\n", 50, " ", "%");
            printf("Completed\n");
            close(fd);
            break;
        }
    }
    close(socketFd);
}
