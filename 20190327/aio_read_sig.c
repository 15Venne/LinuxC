#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h> //...
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<aio.h>

#define BUFFER_SIZE 10

void aio_completion_handler(__sigval_t sigval)
{
    struct aiocb *prd;
    int ret;
    prd = (struct aiocb*)sigval.sival_ptr;
    printf("hello\n");
    
    if(0 == aio_error(prd))  //判断请求是否成功
    {
        ret = aio_return(prd);
        printf("ret=%d\n", ret);
        ((char*)prd->aio_buf)[ret]='\0';
        printf("%s\n", (char*)prd->aio_buf);
    }
}

int main(int argc, char *argv[])
{
    int fd, ret;
    struct aiocb rd;
    memset(&rd, 0, sizeof(rd));

    fd = open("file", O_RDONLY);
    if(-1 == fd)
    {
        perror("open");
    }

    rd.aio_fildes = fd;
    rd.aio_buf = (char*)malloc(sizeof(BUFFER_SIZE + 1));
    rd.aio_nbytes = BUFFER_SIZE;
    //rd.aio_offset = 0;
    
    //回调通知，sigevent
    rd.aio_sigevent.sigev_notify = SIGEV_THREAD; //回调通知
    rd.aio_sigevent.sigev_notify_function = aio_completion_handler;//回调函数
    rd.aio_sigevent.sigev_notify_attributes = NULL;
    rd.aio_sigevent.sigev_value.sival_ptr = &rd;

    ret = aio_read(&rd);
    if(ret < 0)
    {
        perror("aio_read");
    }

    printf("异步读开始\n");
    sleep(1);
    printf("异步读结束\n");

    return 0;
}
