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

void change_fd_status(int fd){
    int status=fcntl(fd,F_GETFL);
    status=status|O_NONBLOCK;
    fcntl(fd,F_SETFL,status);
}

int main(int argc,char* argv[])
{

    //设定reuse标志，用epoll的边沿触发模式实现即时聊天
    int reuse=1;
    int sfd,ret;
    sfd=socket(AF_INET,SOCK_STREAM,0);
    ret=setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    struct sockaddr_in ser;
    bzero(&ser,sizeof(ser));
    ser.sin_family=AF_INET;
    ser.sin_port=htons(atoi(argv[2]));
    ser.sin_addr.s_addr=inet_addr(argv[1]);


    ret=bind(sfd,(struct sockaddr*)&ser,sizeof(ser));
    if(-1 == ret)
    {
        perror("bind");
    }
    printf("bind ret=%d\n", ret);
    listen(sfd,10);

    int new_fd;
    struct sockaddr_in cli;
    bzero(&cli,sizeof(cli));
    socklen_t len=sizeof(cli);


    char buf[50]={0};
    int epfd=epoll_create(1);
    struct epoll_event event,evs[3];
    event.events=EPOLLIN;
    event.data.fd=0;
    ret=epoll_ctl(epfd,EPOLL_CTL_ADD,0,&event);
    printf("epoll_ctl 0 ret=%d\n", ret);
event.data.fd=sfd;
    ret=epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
    printf("epoll_ctl sfd ret=%d\n", ret);
     //即时聊天
    //监控描述符
    int i,readyFdNum;
    while(1)
    {
        memset(evs,0,sizeof(evs));
        readyFdNum=epoll_wait(epfd,evs,3,-1);
        printf("readyFdNum=%d\n", readyFdNum);
        for(i=0;i<readyFdNum;i++)
        {
            if(evs[i].data.fd==sfd)
            {new_fd=accept(sfd,(struct sockaddr*)&cli,&len);
                printf("accept new_fd=%d\n", new_fd);
                change_fd_status(new_fd);
                event.events=EPOLLIN|EPOLLET;
                event.data.fd=new_fd;
                ret=epoll_ctl(epfd,EPOLL_CTL_ADD,new_fd,&event);
                printf("epoll_ctl ret=%d\n", ret);
                printf("ip=%s,port=%d\n",inet_ntoa(cli.sin_addr),ntohs(cli.sin_port));



                           }
            if(evs[i].data.fd==new_fd)
            {
                while(1)
                {
                    bzero(buf,sizeof(buf));
                    ret=recv(new_fd,buf,sizeof(buf),0);
                    if(ret>0)
                    {
                        printf("%s",buf);
                    }else if(0==ret)
                    {
                        event.data.fd=new_fd;
                        epoll_ctl(epfd,EPOLL_CTL_DEL,new_fd,&event);
                        close(new_fd);
                        printf("byebye\n");
                        break;
                    }else{
                        printf("\n");
                        break;
                    }
                }
            }
            if(EPOLLIN==evs[i].events&&evs[i].data.fd==0)
            {
                bzero(buf,sizeof(buf));
                read(0,buf,sizeof(buf));
                send(new_fd,buf,strlen(buf)-1,0);
            }
        }
    }
    close(new_fd);
    close(sfd);
}
