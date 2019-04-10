#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<sys/select.h>
#include<sys/time.h>
#include<unistd.h>
#define ip "192.168.5.215"
#define port "2015" 

int main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == socket_fd)
    {
        perror("socket");
    }
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(ip);
    serAddr.sin_port = htons(atoi(port));  

    int ret;
    ret = bind(socket_fd, (struct sockaddr*)&serAddr, sizeof(struct sockaddr));
    if(-1 == ret)
    {
        perror("bind");
    }

    listen(socket_fd, 10); //监听

    struct sockaddr_in cliAddr; //客户端地址
    memset(&cliAddr, 0, sizeof(cliAddr));
    int sockLen = sizeof(struct sockaddr);
    int new_fd;
    char buf[100]={0};
    fd_set rdset;
    new_fd = accept(socket_fd, (struct sockaddr*)&cliAddr, &sockLen);
    while(1)
    {
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO, &rdset);
        FD_SET(new_fd, &rdset);
        ret = select(new_fd+1, &rdset, NULL, NULL, NULL);
        if(ret>0)
        {
            if(FD_ISSET(new_fd, &rdset))
            {
                memset(buf, 0, sizeof(buf));
                ret = recv(new_fd, buf, sizeof(buf), 0); //断开连接后循环返回0
                if(-1 == ret)
                {
                    perror("recv");
                }
                else if(0 == ret)
                {
                    printf("客户端断开连接\n");
                    new_fd=accept(socket_fd, (struct sockaddr*)&cliAddr, &sockLen);  //没有这句会进入死循环

                }
                printf("\e[0;35mclient:%s\033[0m\n", buf);
            }
            if(FD_ISSET(STDIN_FILENO, &rdset))
            {
                memset(buf, 0, sizeof(buf));
                scanf("%s", buf);
                ret = send(new_fd, buf, strlen(buf), 0);
                if(-1 == ret)
                {
                    perror("send");
                }
            }
        }   
    }
    return 0;
}
