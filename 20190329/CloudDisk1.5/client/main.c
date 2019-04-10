#define _XOPEN_SOURCE
#include<unistd.h>
#include"head.h"

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("argc error\n");
        return -1;
    }
    int socketFd;
    int ret, ret1;
    ret = tcpConnect(&socketFd, argv[1], myport);
    if(-1 == ret)
    {
        perror("connect");
    }
    printf("服务器已连接\n");sleep(1);
    system("clear");
    char order[1000]={0};
    char buf[50]={0};
    char path[200]={0};
    int dataLen;
Loop:
    printf("user name:");
    scanf("%s", buf);
    dataLen=strlen(buf);
    send(socketFd, (char*)&dataLen, 4, 0);
    send(socketFd, buf, dataLen, 0);//给服务器发送用户名
    
    memset(buf, 0, sizeof(buf));
    recv(socketFd, (char*)&dataLen, 4, 0);
    recv(socketFd, buf, dataLen, 0); //接收盐值
    
    memset(path, 0, sizeof(path));
    memset(order, 0, sizeof(order));
    strcpy(path, getpass("password:"));
    strcpy(order, crypt(path, buf)); //密文
    
    dataLen=strlen(order);
    send(socketFd, (char*)&dataLen, 4, 0);
    send(socketFd, order, dataLen, 0); //给服务器发送密文

    
    recv(socketFd, (char*)&ret, 4, 0); //密文配对返回, 0则成功，非0失败
    //printf("密文配对返回ret=%d\n", ret);
    if(ret!=0) //配对不成功
    {
        printf("用户名密码错误.\n");
        goto Loop;
    }
    printf("登陆成功\n");

    memset(path, 0, sizeof(path));
    recv(socketFd, (char*)&dataLen, 4, 0);
    recv(socketFd, path, dataLen, 0); //接路径
    while(1)
    {
        printf("\e[1;35m%s/\033[0m:", path);
        fflush(stdout);
        memset(order, 0, sizeof(order));
        ret = read(STDIN_FILENO, order, sizeof(order));
        if(0 == ret)
        {
            printf("bye\n");
            close(socketFd);
            break;
        }
        else if(-1 == ret)
        {
            perror("read");
        }
        memset(buf, 0, sizeof(buf));
        ret1 = orderCheck(order, buf);
        //printf("命令检查返回：%d\n", ret1);
        //printf("buf=%s\n", buf);
        if(-1 == ret1)
        {
            printf("错误命令！\n");
            continue;
        }
        dataLen = ret-1;
        send(socketFd, (char*)&dataLen, 4, 0);
        send(socketFd, order, dataLen, 0); //向服务器发送命令请求
        
        if(LS_CMD == ret1)  //不同命令进入不同的响应
        {
            ret = lsrecv(socketFd);
        }
        else if(CD_CMD == ret1)
        {
            ret = cdrecv(socketFd);
        }
        else if(PUTS_CMD == ret1)
        {
            ret = putsrecv(socketFd, buf);
            if(-1 == ret)
            {
                printf("download failed\n");
            }
        }
        else if(GETS_CMD == ret1)
        {
            ret = getsrecv(socketFd, buf);
            if(-1==ret)
            {
                printf("download failed\n");
            }
        }
        else if(REMOVE_CMD == ret1)
        {
            ret = removerecv(socketFd);
            if(-1 == ret)
            {
                printf("upload failed\n");
            }
        }
        else if(PWD_CMD == ret1)
        {
            ret = pwdrecv(socketFd);
        }
        else if(MKDIR_CMD == ret1)
        {
            ret = mkdirrecv(socketFd);
        }
        memset(path, 0, sizeof(path));
        recv(socketFd, (char*)&dataLen, 4, 0);
        recv(socketFd, path, dataLen, 0);
    }
    return 0;
}
