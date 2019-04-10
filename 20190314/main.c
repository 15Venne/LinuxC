#include<stdio.h>
#include<sys/types.h>
#include<sys/shm.h>

int main()
{
    shmctl(262144, IPC_RMID, NULL);
    return 0;
}
