#include"head.h"

void *threadFuncGetsPuts(void *p)
{
    tokenNode *pt=(tokenNode*)p;
    int PutsGetsFd;
    int ret;
    ret=tcpConnect(&PutsGetsFd, pt->ip, pt->port);
    if(-1==ret)
    {
        perror("tcpConnect");
    }
    printf("子线程已连上服务器\n");
    send(PutsGetsFd, pt->token, 9, 0); //协议，连接成功发送token，申请认证
    recv(PutsGetsFd, (char*)&ret, 4, 0);//接收认证结果
    if(ret!=1)
    {
        printf("子线程认证失败,下载上传功能失效\n");
        exit(0);
    }
    pthread_mutex_lock(&pt->mutex);
    pthread_cond_wait(&pt->cond, &pt->mutex);
    printf("子线程被唤醒\n");
    if(PUTS_CMD == pt->orderNum)
    {
        printf("申请上传\n");
        ret = putsrecv(PutsGetsFd, pt->buf);
    }
    else if(GETS_CMD == pt->orderNum)
    {
        printf("申请下载\n");
        ret = getsrecv(PutsGetsFd, pt->buf);
    }
}
