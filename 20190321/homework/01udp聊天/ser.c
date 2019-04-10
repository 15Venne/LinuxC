#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/select.h>
#include<sys/time.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
    if(argc!=3)
    {
        printf("argc error");
        return -1;
    }
    int sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serAddr.sin_port = htons(atoi(argv[2]));

    int ret;
    ret = bind(sockFd, (struct sockaddr*)&serAddr, sizeof(struct sockaddr));
    if(-1 == ret)
    {
        perror("bind");
    }
    struct sockaddr_in client;
    memset(&client, 0, sizeof(client));
    int cliLen = sizeof(client);
    char buf[50]={0};
    recvfrom(sockFd, buf, sizeof(buf), 0, (struct sockaddr*)&client, &cliLen); //随便收一点信息确认客户端来源
    printf("client ip:%s\n", inet_ntoa(client.sin_addr));
    
    fd_set rdset;
    while(1)
    {
        FD_ZERO(&rdset);
        FD_SET(STDIN_FILENO, &rdset);
        FD_SET(sockFd, &rdset);
        ret = select(sockFd+1, &rdset, NULL, NULL, NULL);
        if(ret > 0)
        {
            if(FD_ISSET(STDIN_FILENO, &rdset))//写
            {
                memset(buf, 0, sizeof(buf));
                ret = read(STDIN_FILENO, buf, sizeof(buf));
                if(0 == ret)
                {
                    printf("bye\n");
                    close(sockFd);
                    break;
                }
                sendto(sockFd, buf, ret-1, 0, (struct sockaddr*)&client, sizeof(struct sockaddr));
            }
            if(FD_ISSET(sockFd, &rdset))//读
            {
                memset(buf, 0, sizeof(buf));
                ret = recvfrom(sockFd, buf, sizeof(buf), 0, (struct sockaddr*)&client, &cliLen);
                if(0 == ret)
                {
                    printf("bye\n");
                    break;
                }
                printf("%s\n", buf);
            }
        }
    }
    close(sockFd);
    return 0;
}
