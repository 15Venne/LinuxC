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
    ret = connect(socket_fd, (struct sockaddr*)&serAddr, sizeof(serAddr));
    if(-1 == ret)
    {
        perror("connect");
    }

    char buf[100]={0};
    fd_set rdset;

    while(1)
    {
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO, &rdset);
        FD_SET(socket_fd, &rdset);
        ret = select(socket_fd+1, &rdset, NULL, NULL, NULL);
        if(ret > 0)
        {
            if(FD_ISSET(STDIN_FILENO, &rdset))
            {
                memset(buf, 0, sizeof(buf));
                ret = read(STDIN_FILENO, buf, sizeof(buf));
                if(ret == -1)
                {
                    perror("send");
                }
                else if(ret == 0)
                {
                    printf("bye\n");
                    break;
                }
                ret = send(socket_fd, buf, strlen(buf), 0);
            }
            if(FD_ISSET(socket_fd, &rdset))
            {
                memset(buf, 0, sizeof(buf));
                ret = recv(socket_fd, buf, sizeof(buf), 0);
                if(-1 == ret)
                {
                    perror("recv");
                }
                else if(0 == ret)
                {
                    printf("服务器连接中断\n");
                    break;
                }
                printf("\e[0;34mserver:%s\033[0m\n", buf);
            }
        }
    }
    return 0;
}
