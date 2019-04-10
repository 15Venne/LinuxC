#include<time.h>
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

void *threadA(void *p)
{
    time_t statA;
    time(&statA);
    char *pA=ctime(&statA);
    printf("i am threadA %s\n", pA);
    sleep(3);
    printf("i am threadA after 3s %s\n", pA);
    pthread_exit(NULL);
}

int main()
{
    pthread_t pthA;
    pthread_create(&pthA, NULL, threadA, NULL );
    sleep(1);
    time_t stat;
    time(&stat);
    char *ptime=ctime(&stat);
    printf("i am main thread %s\n", ptime);
    pthread_join(pthA, NULL);
    return 0;
}
