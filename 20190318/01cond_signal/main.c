#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

typedef struct Data
{
    pthread_cond_t cond;
    pthread_mutex_t mutex;
}Data;

void *threadA(void *p)
{
    Data *pA=(Data*)p;
    pthread_mutex_lock(&pA->mutex);
    pthread_cond_wait(&pA->cond, &pA->mutex);
    printf("i am threadA at waiting\n");
    pthread_mutex_unlock(&pA->mutex);
    printf("i am threadA after unlock\n");
    pthread_exit(NULL);
}

int main()
{
    pthread_t pthreadA;
    Data data;
    pthread_mutex_init(&data.mutex, NULL);
    pthread_cond_init(&data.cond, NULL);

    pthread_create(&pthreadA, NULL, threadA, &data);
    usleep(1000);
    pthread_cond_signal(&data.cond);
    pthread_join(pthreadA, NULL);
    return 0;
}
