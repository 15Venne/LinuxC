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
    char buf[50]={0};
    sendto(sockFd, "1", 1, 0, (struct sockaddr*)&serAddr, sizeof(struct sockaddr)); //随便发一点信息给服务器
    
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
                sendto(sockFd, buf, ret-1, 0, (struct sockaddr*)&serAddr, sizeof(struct sockaddr));
            }
            if(FD_ISSET(sockFd, &rdset))//读
            {
                memset(buf, 0, sizeof(buf));
                ret = recvfrom(sockFd, buf, sizeof(buf), 0, NULL, NULL);
                if(0 == ret)
                {
                    printf("bye\n");
                    break;
                }
                else if(-1 == ret)
                {
                    perror("recvfrom");
                }
                else
                {
                    printf("%s\n", buf);
                }
            }
        }
    }
    close(sockFd);
    return 0;
}
