#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#define N 262144

//((1<<20*100)/4)

int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        printf("argc error");
    }
    int fd;
    fd=open(argv[1],O_RDWR);
    if(-1==fd)
    {
        perror("open");
    }
    int arr[N]={0};
    int i,j;
    int *p=arr;
    srand(time(NULL));
    for(j=0;j<100;++j)
    {
        for(i=0;i < N;++i)
        {
            arr[i]=rand()%100;
        }    
        write(fd,p,sizeof(arr));
    }
    close(fd);
    return 0;
}
