#include"func.h"

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("argc error\n");
        return -1;
    }
    int ret;
    Factory Fa;
    factoryInit(&Fa, TNUM, QUECAPACITY);
    factoryStart(&Fa);
    int socketFd, new_fd;
    pNode_t pnew;
    ret = tcpInit(&socketFd, argv[1], MYPORT);
    while(1)
    {
        new_fd = accept(socketFd, NULL, NULL);
        printf("new_fd=%d\n", new_fd);
        pnew = (pNode_t)calloc(1, sizeof(Node_t));
        pnew->new_fd = new_fd;
        pthread_mutex_lock(&Fa.pQue->mutex);
        queInsert(Fa.pQue, pnew);
        pthread_mutex_unlock(&Fa.pQue->mutex);
        usleep(1000);
        pthread_cond_signal(&Fa.cond);
    }
    return 0;
}
