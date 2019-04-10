#include<stdio.h>
#include<stdlib.h>
int multi(int i,int j);
int main()
{
	int i=0,j=0,k;
	for(k=0;k<5;++k)
	{
		printf("i*j=%d\n",multi(i++,j++));
	}
	return 0;
}

