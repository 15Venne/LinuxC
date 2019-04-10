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
    strcpy(p,"How are you");
    shmdt(p);
    return 0;
}
