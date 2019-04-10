#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

void cleanup(void *p)
{
    printf("i am cleanup function\n");
    free(p);
}

void *threadFunc(void *p)
{
    p=malloc(15);
    pthread_cleanup_push(cleanup, p);
    char buf[50];
    read(0,buf,sizeof(buf));
    printf("buf=%s\n",buf);
    pthread_exit(NULL);
    pthread_cleanup_pop(0);
}

int main()
{
    pthread_t pthread;
    int ret;
    ret = pthread_create(&pthread, NULL, threadFunc, NULL);
    if(ret!=0)
    {
        printf("pthread_create failed %d\n", ret);
        return -1;
    }
    sleep(2);
    pthread_cancel(pthread);
    sleep(1);
    return 0;

}
