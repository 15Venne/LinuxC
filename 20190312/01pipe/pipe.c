#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int main()
{
    int fds[2];
    pipe(fds);
    if(0 == fork())
    {
        close(fds[0]);
        write(fds[1],"Hello",5);
        close(fds[1]);
        exit(0);
    }
    else
    {
        char buf[128]={0};
        close(fds[1]);
        read(fds[0],buf,sizeof(buf));
        printf("i am parent\n");
        puts(buf);
        close(fds[0]);
        wait(NULL);
        exit(0);
    }
    return 0;
}
