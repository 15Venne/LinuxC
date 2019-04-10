#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>

int main(int argc,char *argv[])
{
    if(2!=argc)
    {
        printf("argc error\n");
    }
    mkfifo(argv[1],0666);
    return 0;
}
