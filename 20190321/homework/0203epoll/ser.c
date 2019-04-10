#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<fcntl.h>

void setFdnonblock(int fd)
{
    int status = fcntl(fd, F_GETFL);
    status|= O_NONBLOCK;
    fcntl(fd, F_SETFL, status);
}

int main(int argc, char *argv[])
{
    if(argc!=3)
    {
        printf("argc error\n");
        return -1;
    }
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == socketFd)
    {
        perror("socket");
    }
    struct sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(atoi(argv[2])); //端口转换
    serAddr.sin_addr.s_addr = inet_addr(argv[1]);//地址转换
    int ret;
    int reuse = 1;
    setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));//设置reuse标志
    
    ret = bind(socketFd, (struct sockaddr*)&serAddr, sizeof(struct sockaddr));
    if(-1 == ret)
    {
        perror("bind");
    }
    listen(socketFd, 10);
    int new_fd;

    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    int sockLen = sizeof(struct sockaddr);
    new_fd = accept(socketFd, (struct sockaddr*)&clientAddr, &sockLen);
    if(-1 == new_fd)
    {
        perror("accept");
    }
    printf("client ip=%s\n", inet_ntoa(clientAddr.sin_addr));

    setFdnonblock(new_fd); //设置非阻塞
    
    char buf[10]={0};
    int epfd = epoll_create(1); //创建epoll句柄
    struct epoll_event event, evs[2];
    //注册stdin
    event.events = EPOLLIN;
    event.data.fd = STDIN_FILENO;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event);
    if(-1 == ret)
    {
        perror("epoll_ctl");
    }
    //注册new_fd
    event.events = EPOLLIN|EPOLLET; //边缘触发模式
    event.data.fd = new_fd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, new_fd, &event);
    if(-1 == ret)
    {
        perror("epoll_ctl");
    }
    
    int rcvlowat=20; //设置缓存下限
    setsockopt(new_fd, SOL_SOCKET, SO_RCVLOWAT, &rcvlowat, sizeof(int));

    int i,j;
    while(1)
    {
        memset(evs, 0, sizeof(evs));
        j = epoll_wait(epfd, evs, 2, -1); //2为 maxevents
        for(i=0;i<j;++i)
        {
            if(new_fd == evs[i].data.fd && EPOLLIN == evs[i].events) //读端
            {
                while(1)
                {
                    memset(buf, 0, sizeof(buf));
                    ret = recv(new_fd, buf, sizeof(buf)-1, 0);
                    if(-1 == ret)
                    {
                        printf("\n");//非阻塞读取完毕,输出\n
                        break;
                    }
                    else if(0 == ret) //结束标识
                    {
                        printf("bye\n");
                        close(new_fd);
                        close(socketFd);
                        goto end;
                    }
                    else
                    {
                        printf("%s", buf);
                    }
                }
            }
            if(STDIN_FILENO == evs[i].data.fd && EPOLLIN == evs[i].events) //写端
            {
                memset(buf, 0, sizeof(buf));
                ret = read(STDIN_FILENO, buf, sizeof(buf));
                if(0 == ret)
                {
                    printf("bye\n");
                    close(new_fd);
                    close(socketFd);
                    break;
                }
                else if(-1 == ret)
                {
                    perror("read");
                }
                else
                {
                    send(new_fd, buf, ret-1, 0);
                }
            }
        }
    }
end:
    return 0;
}









