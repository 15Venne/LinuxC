#include"head.h"

int tranFile(char *filename, int new_fd)
{
    train data;
    memset(&data, 0, sizeof(data));
    int ret;
    int fd = open(filename, O_RDONLY);
    send(new_fd, (char*)&fd, 4, 0); //告诉客户端此文件存不存在
    if(-1 == fd)
    {
        return -1;
    }
    
    struct stat filebuf;
    fstat(fd, &filebuf);
    data.dataLen = sizeof(filebuf.st_size);
    memcpy(data.buf, &filebuf.st_size, sizeof(filebuf.st_size));

    send(new_fd, (char*)&data, data.dataLen+4, 0); //发送文件大小

    char *p=mmap(NULL, filebuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if((char*)-1==p)
    {
       printf("mmap failed\n");
       return -1;
    }
    ret = sendPlus(new_fd, p, filebuf.st_size);
    if(-1 == ret)
    {
        return -1;
    }
    data.dataLen=0;
    send(new_fd, (char*)&data.dataLen, 4, 0);
    return 0;

}
