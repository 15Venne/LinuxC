#include"func.h"

#define childNum 5
#define myport 2015

//默认端口2015，5个子进程
int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("argc error\n");
        return -1;
    }
    ProcessData *p = (ProcessData*)calloc(childNum, sizeof(ProcessData));
    makeChild(p, childNum); //建子进程
    //以下epoll
    int epfd = epoll_create(1);
    struct epoll_event event, *evs;
    evs = (struct epoll_event*)calloc(childNum + 1, sizeof(struct epoll_event));
    
    int i, ret;
    //注册子进程们的fd
    for(i=0;i<childNum;++i)
    {
        event.events = EPOLLIN;
        event.data.fd = p[i].fd;
        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, p[i].fd, &event);
        if(-1 == ret)
        {
            perror("epoll_ctl");
        }
    }
    int socketFd;
    tcpInit(&socketFd, argv[1], myport); //tcp,socket, bind, listen
    //以下注册socketFd
    event.events = EPOLLIN;
    event.data.fd = socketFd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, socketFd, &event);
    if(-1==ret)
    {
        perror("epoll_ctl_socketfd");
    }
    int readyNum;
    int j, new_fd;

    while(1)
    {
        readyNum = epoll_wait(epfd, evs, childNum+1, -1);
        if(-1 == readyNum)
        {
            perror("epoll_wait");
        }
        for(i=0;i<readyNum;++i) //遍历evs[]
        {
            if(evs[i].data.fd == socketFd) //如果有客户端连接
            {
                new_fd = accept(socketFd, NULL, NULL);
                printf("client connect success, new_fd=%d\n", new_fd);
                for(j=0;j<childNum;++j) //寻找空闲的子进程
                {
                    if(0 == p[j].busy)
                    {
                        sendFd(p[j].fd, new_fd); //传递new_fd, 分派任务
                        p[j].busy = 1;
                        printf("%d is busy.\n", p[j].pid);
                        break;
                    }
                }
                close(new_fd);
            }//客户端任务处理完毕
            //以下接受子进程空闲信号
            for(j=0;j<childNum;++j)
            {
                if(evs[i].data.fd == p[j].fd) //如果有子进程发来消息
                {
                    p[j].busy = 0;
                    read(p[j].fd, &ret, 1); //随便拿点什么去接信号(读缓存区)
                    printf("%d is not busy.\n", p[j].pid);
                    break;
                }
            }
        }
    }
}
