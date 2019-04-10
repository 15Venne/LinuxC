#include"head.h"

int main()
{
    int fd=open("file", O_CREAT|O_RDWR, 0666);
    if(-1==fd)
    {
        perror("open");
    }
    ftruncate(fd, 2);
    char *p=mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    read(STDIN_FILENO, p, 10);
    munmap(p, 4096);
    return 0;
}
