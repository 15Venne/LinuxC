#include"head.h"

int recvFile(char *filename, int new_fd)
{
    int dataLen;
    char buf[200]={0};
    int ret;
    char md5sum[33]={0};
    char md5sumLocal[33]={0};
    recv(new_fd, (char*)&ret, 4, 0);//客户端通知
    if(ret == -1)
    {
        return -1;
    }
    recvPlus(new_fd, (char*)&dataLen, 4);
    recvPlus(new_fd, buf, dataLen); //接收文件名
    //printf("filename=%s\n", buf);
    off_t fileSize;
    off_t fileSizeLocal=0;
    recvPlus(new_fd, (char*)&dataLen, 4);
    recvPlus(new_fd, (char*)&fileSize, dataLen);//接收文件大小
    //printf("fileSize=%ld\n",fileSize);
    
    /* ret = is_sameFile_exist(filename, &fileSizeLocal);//判断有无同名文件 */
    if(-1==ret) {return -1;}
    int fd = open(buf, O_CREAT|O_RDWR, 0666);
    if(-1==fd) {return -1;}
    sprintf(buf, "%s%s", buf, ".tmp");
    int tmpFd = open(buf, O_CREAT|O_RDWR, 0666);
    if(-1==tmpFd) {return -1;}
    
    recv(new_fd, (char*)&dataLen, 4, 0);
    recv(new_fd, md5sum, dataLen, 0);  //接收文件md5码
    /* if(0==ret) //无同名文件 */
    /* { */
    /*     write(tmpFd, md5sum, 32); */
    /* } */
    /* else if(1==ret)//有同名文件 */
    /* { */
    /*     read(tmpFd, md5sumLocal, 32); */
    /*     if(strcpy(md5sum, md5sumLocal)!=0) //与之前的md5码不一样 */
    /*     { */

        /* } */
        /* else //与之前md5码一样，说明是上次上传中断 */
        /* { */

        /* } */

    /* } */
    //以下获取文件路径
    /* char *pwd=getcwd(NULL, 0); */
    /* ret=0; */
    /* while(pwd[ret]!='r') */
    /* { */
    /*     ret++; */
    /* } */
    /* pwd=pwd+ret+2; */
    //插入数据库
    //ret = insertTable_fileHouse(filename, pwd, md5sum, fileSize);
    /* if(-1==ret) */
    /* { */
    /*     printf("文件数据创建失败\n"); */
    /*     close(fd); */
    /*     return -1; */
    /* } */
    //ftruncate(fd, fileSize);
    //char *p=mmap(NULL, fileSize,  PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    //ret =recvPlus(new_fd, p, fileSize);         //接收文件内容
    //if(-1==ret)
    //{
    //    return -1;
    //}
    int fds[2];
    pipe(fds);
    int recvcnt;
    while(fileSize > 65535) //接收文件-----协议三
    {
        recvcnt = splice(new_fd, NULL, fds[1], NULL, 65535, SPLICE_F_MORE | SPLICE_F_MOVE);
        if(recvcnt == 0)
        {
            return -1;
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
    recvcnt = splice(new_fd, NULL, fds[1], NULL, fileSize, SPLICE_F_MORE | SPLICE_F_MOVE);
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
        recvcnt = splice(new_fd, NULL, fds[1], NULL, fileSize, SPLICE_F_MORE | SPLICE_F_MOVE);
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

    recvPlus(new_fd, (char*)&dataLen, 4); //结束符
    close(tmpFd);
    close(fd);
    unlink(buf);
    return 0;
}
