#include"func.h"

int sendPlus(int fd, char *p, int lenth)
{
    int total=0;
    int ret;
    while(total < lenth)
    {
        ret = send(fd, p+total, lenth-total, 0);
        if(-1 == ret)
        {
            return -1;
        }
        total += ret;
    }
    return 0;
}

void sigFunc(int signum)
{
    printf("客户端已断开\n");
}

typedef void (*sigFun_t)(int);

int tranFile(int new_fd)
{
    train data;
    int ret;
    //以下发送文件名-----协议一
    data.dataLen = strlen(FILENAME);
    strcpy(data.buf, FILENAME);
/**/ret = sendPlus(new_fd, (char*)&data, data.dataLen+4);
    if(-1 == ret)
    {
        return -1;
    }
    int fd = open(FILENAME, O_RDONLY);
    if(-1 == fd)
    {
        perror("open");
    }
    //以下发送文件大小-----协议二
    struct stat buf;
    fstat(fd, &buf); //由已打开的文件描述符获取文件状态，复制到buf中
    data.dataLen = sizeof(buf.st_size);  
    memcpy(data.buf, &buf.st_size, sizeof(buf.st_size));
    ret = sendPlus(new_fd, (char*)&data, data.dataLen+4); 
    if(-1 == ret)
    {
        close(fd);
        return -1;
    }

    sigFun_t sigret = signal(SIGPIPE, sigFunc);
    if(sigret == SIG_ERR)
    {
        perror("signal");
    }

    //以下发送文件内容-----协议三
    printf("发送文件中……\n");
    
    sendfile(new_fd, fd, NULL, buf.st_size); //sendfile
    
    data.dataLen=0;
    ret =  sendPlus(new_fd, (char*)&data, 4); //发送文件结束标志
    if(-1 == ret)
    {
       close(fd);
       return -1;
    }
    printf("发送完毕！\n");
    close(fd);
    return 0;
}

int nodeGet(pQueue pq, pNode_t *pn)
{
    if(0 == pq->size)
    {
        return -1;
    }
    else
    {
        *pn = pq->phead;
        pq->phead = pq->phead->pnext;
        pq->size--;
        if(0 == pq->size)
        {
            pq->ptail = NULL;
        }
        return 0;
    }
}

void queInsert(pQueue pq, pNode_t pnew) //尾插法
{
    if(NULL == pq->phead)
    {
        pq->phead = pnew;
        pq->ptail = pnew;
    }
    else
    {
        pq->ptail->pnext = pnew;
        pq->ptail = pnew;
    }
    pq->size++;
}

int tcpInit(int *sfd, char *ip, int port)
{
    *sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == *sfd)
    {
        perror("socket");
    }
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(port);
    serAddr.sin_addr.s_addr = inet_addr(ip);

    int ret, reuse=1;
    ret = setsockopt(*sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    if(-1 == ret)
    {
        perror("setsockopt");
    }
    ret = bind(*sfd, (struct sockaddr*)&serAddr, sizeof(struct sockaddr));
    if(-1 == ret)
    {
        perror("bind");
    }

    listen(*sfd, 10);
    return 0;
}

void *threadQuest(void *p)
{
    pFactory pf=(pFactory)p;
    pNode_t pcur;//用来接队列的头结点（任务）
    int ret;
    while(1)
    {
        pthread_mutex_lock(&pf->pQue->mutex);
        if(pf->pQue->size!=0)
        {
            pthread_cond_wait(&pf->cond, &pf->pQue->mutex);
        }
        ret = nodeGet(pf->pQue, &pcur);
        pthread_mutex_unlock(&pf->pQue->mutex);
        if(-1 == ret)
        {
            continue;
        }
        else
        {
            ret = tranFile(pcur->new_fd);
            close(pcur->new_fd);
            free(pcur);
        }
    }
}

void factoryStart(pFactory pf) //创建子线程
{
    int i;
    if(0 == pf->startFlags)
    {
        for(i = 0; i < pf->threadNum; ++i)
        {
            pthread_create(&pf->pthid[i], NULL, threadQuest, pf);
        }
        pf->startFlags = 1;
    }
}

void queInit(pQueue pq, int qCapacity)
{
    memset(pq, 0, sizeof(Queue));
    pq->qCapacity = qCapacity;
    pthread_mutex_init(&pq->mutex, NULL);
}

void factoryInit(pFactory pf, int threadNum, int qCapacity)
{
    memset(pf, 0, sizeof(Factory));
    pf->pthid=(pthread_t*)calloc(threadNum, sizeof(pthread_t));
    pf->threadNum = threadNum;
    pthread_cond_init(&pf->cond, NULL);
    pf->pQue = (pQueue)calloc(1, sizeof(Queue));
    queInit(pf->pQue, qCapacity);
}
