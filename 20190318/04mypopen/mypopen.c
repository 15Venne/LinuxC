#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

//fork,execl,pipe,dup实现popen


//读取print进程的标准输出
int main()
{
    int fds[2];
    pipe(fds);
    if(fork())//父
    {
        close(fds[1]);
        char buf[128]={0};
        read(fds[0], buf, sizeof(buf));
        printf("i am parent.buf=%s\n", buf);
        wait(NULL);
        close(fds[0]);
    }
    else  //子进程，execl到print
    {
        close(fds[0]);
        dup2(fds[1],1);
        execl("./print", "print", NULL);        
    }
    return 0;
}
