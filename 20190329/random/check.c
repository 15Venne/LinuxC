#define _XOPEN_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<shadow.h>
#include<errno.h>

void get_salt(char *salt, char *passwd)
{
    int i,j;   //i记录密码字符串下标，j记录$出现次数
    for(i=0,j=0;passwd[i]&&j!=3;++i)
    {
        if(passwd[i]=='$')
        {
            ++j;
        }
    }
    strncpy(salt, passwd, i-1);
}
int main(int argc, char *argv[])
{
    struct spwd *sp;
    char *passwd;
    char salt[512]={0};
    passwd=getpass("请输入密码:");
    printf("密码:%s\n", passwd);
    //char *miwen="$6$Mpbf30xJ$MpsXw5LrzMFeo";
    //get_salt(salt, miwen);
    printf("请输入盐值:");
    scanf("%s", salt);
    printf("salt=%s\n", salt);
    printf("密文:%s\n", crypt(passwd, salt));
    //以下密码验证
    /* if(0 == strcmp("MpsXw5LrzMFeo", crypt(passwd, salt))) */
    /* { */
    /*     printf("验证通过\n"); */
    /* } */
    /* else */
    /* { */
    /*     printf("验证失败\n"); */
    /* } */
    return 0;

}
