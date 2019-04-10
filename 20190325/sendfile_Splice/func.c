#include"func.h"

int sendFd(int pipeFd, int fd)
{
    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    struct iovec iov[2];
    iov[0].iov_base = "ha";
    iov[0].iov_len = 2;
    iov[1].iov_base = "lo";
    iov[1].iov_len = 2;
    msg.msg_iov = iov;
    msg.msg_iovlen = 2;
    int cmsgLen = CMSG_LEN(sizeof(int)); //变长结构体长度
    
    struct cmsghdr *cmsg = (struct cmsghdr*)calloc(1,  cmsgLen);
    cmsg->cmsg_len = cmsgLen;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    *(int*)CMSG_DATA(cmsg)=fd;

    msg.msg_control = cmsg; //设置好的cmsg结构体指针给msg.msg_control
    msg.msg_controllen = cmsgLen;

    int ret;
    ret = sendmsg(pipeFd, &msg, 0);
    if(-1 == ret)
    {
        perror("sendmsg");
    }
    return 0;
}

int recvFd(int pipeFd, int *fd)
{
    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    struct iovec iov[2];
    char buf1[5], buf2[5];
    iov[0].iov_base = buf1;
    iov[0].iov_len = 2;
    iov[1].iov_base = buf2;
    iov[1].iov_len = 2;
    msg.msg_iov = iov;
    msg.msg_iovlen = 2;
    int cmsgLen = CMSG_LEN(sizeof(int)); //变长结构体长度
    
    struct cmsghdr *cmsg = (struct cmsghdr*)calloc(1, cmsgLen);
    cmsg->cmsg_len = cmsgLen;
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

    msg.msg_control = cmsg; //设置好的cmsg结构体指针给msg.msg_control
    msg.msg_controllen = cmsgLen;

    int ret;
    ret = recvmsg(pipeFd, &msg, 0);
    if(-1 == ret)
    {
        perror("sendmsg");
    }
    *fd = *(int*)CMSG_DATA(cmsg);
    return 0;
}

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

void sigFunc(int signum)
{

}
typedef void(*sigFun_t)(int);

int tranFile(int new_fd)
{
    train data;
    int ret;
    //以下发送文件名-----协议一
    data.dataLen = strlen(FILENAME);
    strcpy(data.buf, FILENAME);
/**/ret = sendPlus(new_fd, (char*)&data, data.dataLen+4);
    if(-1 == ret)
    {
        return -1;
    }
    int fd = open(FILENAME, O_RDONLY);
    if(-1 == fd)
    {
        perror("open");
    }
    //以下发送文件大小-----协议二
    struct stat buf;
    fstat(fd, &buf); //由已打开的文件描述符获取文件状态，复制到buf中
    data.dataLen = sizeof(buf.st_size);  
    memcpy(data.buf, &buf.st_size, sizeof(buf.st_size));
    ret = sendPlus(new_fd, (char*)&data, data.dataLen+4); 
    if(-1 == ret)
    {   
        close(fd);
        return -1;
    }

    /* char *p = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd, 0); */
    /* if((char*)-1 == p) */
    /* { */
    /*     perror("mmap"); */
    /* } */

    //以下发送文件内容-----协议三
  // ret = sendPlus(new_fd, p, buf.st_size); //mmap
    
    sigFun_t sigret = signal(SIGPIPE, sigFunc);
    if(sigret == SIG_ERR)
    {
        perror("signal");
    }

    ret = sendfile(new_fd, fd, NULL, buf.st_size); //sendfile
    if(-1 == ret )
   {
       close(fd);
       return -1;
   }
    data.dataLen=0;
    ret =  sendPlus(new_fd, (char*)&data, 4); //发送文件结束标志
    if(-1 == ret)
    {
        close(fd);
       return -1;
    }
    return 0;
}
int makeChild(ProcessData *p, int num) //建子进程
{
    pid_t pid;
    int fds[2], i, ret;
    int new_fd;
    for(i = 0; i < num; ++i)
    {
        ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, fds);
        if(-1 == ret)
        {
            perror("socketpair");
        }
        pid=fork();
        if(0 == pid)//子进程，处理任务
        {
            close(fds[1]);
            while(1)
            {
                recvFd(fds[0], &new_fd);//通过fds[0]接收new_fd (任务)
/*@@@*/         tranFile(new_fd); //执行任务，发送文件
                close(new_fd);
                write(fds[0], &new_fd, 1);//随便发送点什么告诉父进程完成任务
            }
        }
        else
        {
            close(fds[0]);
            p[i].pid = pid;
            p[i].fd = fds[1];  //??可以直接赋值吗?
        }
    }
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
