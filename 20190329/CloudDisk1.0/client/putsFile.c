#include"head.h"


int putsrecv(int socketFd, char *filename)
{
    train data;
    memset(&data, 0, sizeof(data));
    int ret, fd;
    char md5sum[33]={0};
    fd = open(filename, O_RDONLY);
    send(socketFd, (char*)&fd, 4, 0);//告诉客户端文件是否存在
    if(-1 == fd)
    {
        printf("file doesn't exist\n");
        return -1;
    }
    
    data.dataLen=strlen(filename);
    strcpy(data.buf, filename);
    send(socketFd, (char*)&data, data.dataLen+4, 0);//发送文件名

    struct stat filebuf;
    fstat(fd, &filebuf);
    data.dataLen = sizeof(filebuf.st_size);
    memcpy(data.buf, &filebuf.st_size, sizeof(filebuf.st_size));
    //printf("fileSize=%ld\n", filebuf.st_size);
    send(socketFd, (char*)&data, data.dataLen+4, 0); //发送文件大小
    
    memset(&data, 0, sizeof(data));
    getMD5(data.buf, fd);
    data.dataLen=strlen(data.buf);
    send(socketFd, (char*)&data, data.dataLen+4, 0);//发送文件md5码

    char *p=mmap(NULL, filebuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if((char*)-1==p)
    {
        printf("mmap failed\n");
        return -1;
    }
    ret =sendPlus(socketFd, p, filebuf.st_size); //发送文件内容
    if(-1==ret)
    {
        return -1;
    }

    data.dataLen = 0;
    send(socketFd, (char*)&data, 4, 0);  //结束符
    printf("upload completed\n");
    close(fd);
    return 0;
   
}
