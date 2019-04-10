#include<netdb.h>
#include<stdio.h>
#include<sys/socket.h>

int main(int argc, char *argv[1])
{
    if(argc!=2)
    {
        printf("请输入域名\n");
        return -1;
    }
    struct hostent *p;
    p=gethostbyname(argv[1]);
    printf("主机名=%s\n", p->h_name);
    int i;
    printf("主机别名:\n");
    for(i=0;p->h_aliases[i]!=NULL;++i)
    {
        printf("%s\n", p->h_aliases[i]);
    }
    return 0;
}
