#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

void cleanup(void *p)
{
    printf("cleanup\n");
    pthread_mutex_unlock((pthread_mutex_t*)p);
}

void *threadFunc(void *p)
{
    pthread_mutex_lock((pthread_mutex_t*)p);//第三次加锁
    printf("子进程加锁\n");
    pthread_cleanup_push(cleanup,p);
    pthread_mutex_unlock((pthread_mutex_t*)p);//解锁
    printf("子进程解锁\n");
    pthread_cleanup_pop(0);
    pthread_exit(NULL);
}

int main()
{
    pthread_mutex_t mutex;
    pthread_mutexattr_t mutexattr; //锁的属性
    pthread_mutexattr_init(&mutexattr);//初始化锁的属性
    pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);//设置
    int ret=pthread_mutex_init(&mutex, &mutexattr);//初始化锁
    if(ret!=0)
    {
        printf("pthread_mutex_init filed %d\n",ret);
        return -1;
    }
    pthread_mutex_lock(&mutex);//第一次加锁
    printf("第一次加锁\n");
    pthread_mutex_lock(&mutex);// 第二次加锁
    printf("第二次加锁\n");
    pthread_mutex_unlock(&mutex);
    pthread_mutex_unlock(&mutex);
    pthread_t pthread;
    pthread_create(&pthread, NULL, threadFunc, &mutex);//子线程
    sleep(1);
    pthread_join(pthread, NULL);
    pthread_mutex_lock(&mutex);
    printf("1\n");
    pthread_mutex_lock(&mutex);
    printf("2\n");
    pthread_mutex_lock(&mutex);
    printf("3\nover\n");
    return 0;
}
