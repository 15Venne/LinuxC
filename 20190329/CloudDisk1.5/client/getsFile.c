#include"head.h"

int getsrecv(int socketFd, char *filename)
{
    int dataLen, ret;
    char buf[200];
    //recv(socketFd, (char*)&dataLen, 4, 0);
    //recv(socketFd, buf, dataLen, 0); //文件名
    recv(socketFd, (char*)&dataLen, 4, 0); //接收服务器文件真假信息
    if(-1 == dataLen)
    {
        printf("file doesn't exist\n");
        return -1;
    }

    off_t fileSize;
    recv(socketFd, (char*)&dataLen, 4, 0);
    recv(socketFd, (char*)&fileSize, dataLen, 0); //文件大小
    int fd = open(filename, O_CREAT|O_RDWR, 0666);
    if(-1 == fd)
    {
        return -1;
    }
    ftruncate(fd, fileSize);
    char *p = mmap(NULL, fileSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    ret = recvPlus(socketFd, p, fileSize);
    if(-1==ret)
    {
        printf("lose connection...\n");
        return -1;
    }
    munmap(p,fileSize);
    //printf("mmap解除\n");
    recv(socketFd, (char*)&dataLen, 4, 0);
    printf("download completed\n");
    close(fd);
    return 0;
}
