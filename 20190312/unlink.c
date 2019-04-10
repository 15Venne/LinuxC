#include<stdio.h>
#include<unistd.h>

int main(int artc,char *argv[])
{
    unlink(argv[1]); 
    return 0;
}
