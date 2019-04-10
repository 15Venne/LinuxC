#include<stdio.h>
#include<time.h>
#define N 10
void quickSort(int *a,int left,int right)
{
	int i,j;
	i=left;j=right;
	if(i>=j)
		return;
	int temp=a[left];
	while(i<j)
	{
		while(i<j && a[j]>temp)
		{
			j--;
		}	
		a[i]=a[j];
		while(i<j && a[i]<temp)
		{
			i++;
		}
		a[j]=a[i];
	}
	a[j]=temp;
	quickSort(a,left,j-1);
	quickSort(a,j+1,right);
}


int main()
{
	int a[N]={15,17,66,4,98,14,19,59,69,33};
	int i;
	int *b;
	//srand=((unsigned)time(NULL));
	for(i=0;i<N;++i)
	{
		//a[i]=rand()%100;
		printf("%d ",a[i]);
	}
	printf("\n");
	quickSort(b,0,N-1);
	for(i=0;i<N;++i)
	{
		printf("%d ",a[i]);
	}
	printf("\n");
	return 0;
}
	
