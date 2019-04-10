#include"head.h"

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("argc error\n");
        return -1;
    }
    int ret, i;
    int socketFd;
    tcpInit(&socketFd, argv[1], myport);

    int epfd = epoll_create(1);
    struct epoll_event event, *evs;
    evs = (struct epoll_event*)calloc(childNum+1, sizeof(struct epoll_event));

    //以下注册socketFd
    event.events = EPOLLIN;
    event.data.fd = socketFd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, socketFd, &event);
    if(-1==ret) { perror("epoll_ctl"); }
    
    ProcessData *p=(ProcessData*)calloc(childNum, sizeof(ProcessData));
    makeChild(p, childNum); //建子进程
    //以下注册子进程fd
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

    int j, new_fd, readyNum;
    char buf[30]={0};
    while(1)
    {
        readyNum = epoll_wait(epfd, evs, childNum+1, -1);
        if(-1 == readyNum) { perror("epoll_wait"); }
        for(i=0;i<readyNum;++i)  //遍历evs[]
        {
            if(evs[i].data.fd == socketFd) //如果有客户端连接
            {
                new_fd = accept(socketFd, NULL, NULL);
                printf("client connect success, new_fd =%d\n", new_fd);
                for(j=0;j<childNum;++j)
                {
                    if(0 == p[j].busy)
                    {
                        sendFd(p[j].fd, new_fd); //把连接上的new_fd传给空闲的子进程
                        p[j].busy=1;
                        printf("%d is busy.\n", p[j].pid);
                        break;
                    }
                }
                close(new_fd);
            }       
            for(j=0;j<childNum;++j)
            {
                if(evs[i].data.fd==p[j].fd)
                {
                    p[j].busy=0;
                    read(p[j].fd, &ret, 1);//接子进程发过来的信号
                    printf("%d misson completed\n", p[j].pid);
                    break;
                }
            }
        }
    }
}
