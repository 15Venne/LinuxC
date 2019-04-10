#include<stdio.h>

int add(int i,int j);

int main()
{
	int i=1,j=1;
	int k;
	for(k=0;k<5;++k)
	{
		printf("i+j=%d \n",add(i++,j++));
	}
	return 0;
}
