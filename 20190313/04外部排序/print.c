#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        printf("argc error");
    }
    int fd=open(argv[1],O_RDWR);
    int arr[262144]={0};
    read(fd,arr,262144*4);
    int i;
    for(i=0;i<262144;++i)
    {
        printf("%d ",arr[i]);
        if(i%20==0)
        {
            printf("\n");
        }
    }
    close(fd);
    return 0;
}
