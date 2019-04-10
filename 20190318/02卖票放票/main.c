#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

#define N 30;

typedef struct Data
{
    pthread_cond_t cond; //条件变量
    int tickets;            
    pthread_mutex_t mutex;//锁
}Data;

void *threadFuncA(void *p)
{ 
   int i=0;
   while(1)
   {
       pthread_mutex_lock(&((Data*)p)->mutex);
       if(((Data*)p)->tickets > 0)
       {
            printf("I'm threadFuncA,tickets=%d\n", ((Data*)p)->tickets);
            ((Data*)p)->tickets = ((Data*)p)->tickets - 1;
            i++;
            if(((Data*)p)->tickets == 0)
            {
                pthread_cond_signal(&((Data*)p)->cond);
            }
            pthread_mutex_unlock(&((Data*)p)->mutex);
       }
       else
       {
           printf("sold out\n");
           pthread_mutex_unlock(&((Data*)p)->mutex);
           //printf("windowA had sold %d tickets\n", i);
           break;
       }
       sleep(1);
   }
}

void *threadFuncB(void *p)
{
   int i=0;
   while(1)
   {
       pthread_mutex_lock(&((Data*)p)->mutex);
       if(((Data*)p)->tickets > 0)
       { 
            printf("I'm threadFuncB,tickets=%d\n", ((Data*)p)->tickets);
            ((Data*)p)->tickets = ((Data*)p)->tickets - 1;
            i++;
            if(((Data*)p)->tickets == 0)
            {
                pthread_cond_signal(&((Data*)p)->cond);
            }
            pthread_mutex_unlock(&((Data*)p)->mutex);
       }
       else
       {
           printf("sold out\n");
           pthread_mutex_unlock(&((Data*)p)->mutex);
           //printf("windowB had sold %d tickets\n", i);
           break;
       }
       sleep(1);
   }
}

void *threadsetTickets(void *p)
{
    Data *pChild=(Data*)p;
    pthread_mutex_lock(&pChild->mutex);
    pthread_cond_wait(&pChild->cond, &pChild->mutex);
    pChild->tickets = 20;
    pthread_mutex_unlock(&pChild->mutex);
}

int main()
{
    pthread_t pthreadA, pthreadB, pthreadsetTickets;
    Data data;
    data.tickets = N;
    pthread_mutex_init(&data.mutex, NULL);//初始化锁

    pthread_cond_init(&data.cond, NULL);//

    int ret = pthread_create(&pthreadA, NULL, threadFuncA, &data);//A子线程
    if(ret!=0)
    {
        printf("pthread_createA failed %d\n", ret);
        return -1;
    }
    ret = pthread_create(&pthreadB, NULL, threadFuncB, &data);  //B子线程
    if(ret!=0)
    {
        printf("pthread_createB failed %d\n", ret);
        return -1;
    }
    pthread_create(&pthreadsetTickets, NULL, threadsetTickets, &data);//放票线程
    ret = pthread_join(pthreadA, NULL);  //回收A子线程
    if(ret!=0)
    {
        printf("pthread_joinA failed %d\n", ret);
        return -1;
    }
    ret = pthread_join(pthreadB, NULL);   //回收B子线程
    if(ret!=0)
    {
        printf("pthread_joinB failed %d\n", ret);
    }
    pthread_join(pthreadsetTickets, NULL);
    return 0;
}
