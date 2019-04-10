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


void change_fd_status(int fd)
{
	int status=fcntl(fd,F_GETFL);
	status=status|O_NONBLOCK;
	fcntl(fd,F_SETFL,status);
}

int main(int argc,char** argv)
{
	args_check(argc,3);
	int sfd;
	//初始化socket
	sfd=socket(AF_INET,SOCK_STREAM,0);
	check_error(-1,sfd,"socket");
	//改变socket必须在bind之前
	int reuse=1;
	int ret;
	ret=setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
	check_error(-1,ret,"setsockopt");
	struct sockaddr_in ser;
	bzero(&ser,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));//端口号转换为网络字节序
	ser.sin_addr.s_addr=inet_addr(argv[1]);//点分10进制的IP地址转为网络字节序
	ret=bind(sfd,(struct sockaddr*)&ser,sizeof(ser));
	check_error(-1,ret,"bind");
	listen(sfd,10);
	int new_fd;
	struct sockaddr_in cli;
	bzero(&cli,sizeof(cli));
	int len=sizeof(cli);
	char buf[10]={0};
	int epfd=epoll_create(1);
	struct epoll_event event,evs[3];
	event.events=EPOLLIN;//是否可读
	event.data.fd=STDIN_FILENO;
	ret=epoll_ctl(epfd,EPOLL_CTL_ADD,STDIN_FILENO,&event);
	check_error(-1,ret,"epoll_ctl");
	event.data.fd=sfd;
	ret=epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
	check_error(-1,ret,"epoll_ctl1");
	int i;
	//多路复用
	while(1)
	{
		ret=epoll_wait(epfd,evs,3,-1);
		for(i=0;i<ret;i++)
		{
			if(evs[i].data.fd==sfd)//sfd可读
			{
				new_fd=accept(sfd,(struct sockaddr*)&cli,&len);
				change_fd_status(new_fd);
				event.events=EPOLLIN|EPOLLET;
				event.data.fd=new_fd;
				epoll_ctl(epfd,EPOLL_CTL_ADD,new_fd,&event);
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
					}else if(0==ret){
						//代表对端断开
						event.data.fd=new_fd;
						epoll_ctl(epfd,EPOLL_CTL_DEL,new_fd,&event);//解注册
						close(new_fd);
						printf("byebye\n");
						break;
					}else{
						printf("\n");
						break;
					}
				}
			}
			if(EPOLLIN==evs[i].events&&evs[i].data.fd==STDIN_FILENO)
			{
				bzero(buf,sizeof(buf));
				read(STDIN_FILENO,buf,sizeof(buf));
				send(new_fd,buf,strlen(buf)-1,0);
			}
		}
	}
	close(new_fd);
	close(sfd);
}
