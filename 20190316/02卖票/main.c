#include<stdio.h>
#include<pthread.h>

#define N 10000000;

typedef struct Data
{
    int tickets;
    pthread_mutex_t mutex;
}Data;

void *threadFuncA(void *p)
{ 
    //Data *pB=(Data*)p;
   // pthread_mutex_t *mutex =& pB->mutex;
   int i=0;
   while(1)
   {
       pthread_mutex_lock(&((Data*)p)->mutex);
       if(((Data*)p)->tickets > 0)
       {
            ((Data*)p)->tickets = ((Data*)p)->tickets - 1;
            i++;
            pthread_mutex_unlock(&((Data*)p)->mutex);
       }
       else
       {
           printf("sold out\n");
           pthread_mutex_unlock(&((Data*)p)->mutex);
           printf("windowA had sold %d tickets\n", i);
           break;
       }
   }
}

void *threadFuncB(void *p)
{
    //Data *pB=(Data*)p;
   // pthread_mutex_t *mutex =& pB->mutex;
   int i=0;
   while(1)
   {
       pthread_mutex_lock(&((Data*)p)->mutex);
       if(((Data*)p)->tickets > 0)
       {
            ((Data*)p)->tickets = ((Data*)p)->tickets - 1;
            i++;
            pthread_mutex_unlock(&((Data*)p)->mutex);
       }
       else
       {
           printf("sold out\n");
           pthread_mutex_unlock(&((Data*)p)->mutex);
           printf("windowB had sold %d tickets\n", i);
           break;
       }
   }
}

int main()
{
    pthread_t pthreadA, pthreadB;
    Data data;
    data.tickets = N;
    pthread_mutex_init(&data.mutex, NULL);
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
    return 0;
}
