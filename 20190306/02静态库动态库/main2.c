#include<stdio.h>

int mul(int i,int j);
int main()
{
	int i=1,j=1,k;
	for(k=0;k<5;++k)
	{
		printf("i*j=%d\n",mul(i++,j++));
	}
	return 0;
}

