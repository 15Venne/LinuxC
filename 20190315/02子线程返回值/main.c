#include<stdio.h>
#include<pthread.h>

void *threadFunc(void *p)
{
    printf("%ld\n", (long)p);
    pthread_exit((void*)2);
}

int main()
{
    pthread_t pthread;
    int ret;
    ret=pthread_create(&pthread, NULL, threadFunc, (void*)1);
    if(ret!=0)
    {
        printf("pthread_create failed %d\n", ret);
    }
    long num;
    ret=pthread_join(pthread,(void**)&num);
    printf("child return=%ld\n",num);
    return 0;
}
