#include<stdio.h>


int main()
{
    FILE *fp=popen("./print","r");
    if(NULL==fp)
    {
        perror("popen");
    }
    char buf[128]={0};
    fread(buf,sizeof(char),sizeof(buf),fp);
    printf("%s\n",buf);
    return 0;
}
