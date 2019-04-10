#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<errno.h>
#include<string.h>
int main()
{
    int shmid;
    shmid=shmget(1000,4096,IPC_CREAT|0600);
    if(-1==shmid)
    {
        perror("shmget");
        return -1;
    }
    printf("%d\n",shmid);
    char *p=shmat(shmid,NULL,0);
    puts(p);
    shmdt(p);
    int ret;
    ret=shmctl(shmid,IPC_RMID,NULL);
    if(-1==ret)
    {
        perror("shmctl");
    }
    return 0;
}
