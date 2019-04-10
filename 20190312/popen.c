#include<stdio.h>

//fork,execl,pipe,dup实现popen

int main(int argc,char *argv[])
{
    //if(argc!=2)
    //{
    //    printf("argc error\n");
    //}
    FILE *fp=popen("./print","r");
    if(NULL==fp)
    {
        perror("popen");
        return -1;
    }
    char buf[128]={0};
    fread(buf,sizeof(char),sizeof(buf),fp);
    printf("%s\n",buf);
    return 0;
}
