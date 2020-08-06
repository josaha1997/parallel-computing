//Quick sort
#include<stdio.h>
#include<stdlib.h>
//void quick_sort(int[],int,int);
int partition(int[],int,int,int);

int main()
{
	int *array,size,i,pivot,pivotindex;
	printf("Enter the number of inputs:");
	scanf("%d",&size);
	array=(int*)malloc(size*sizeof(int));
	for(i=0;i<size;++i)
		scanf("%d",&array[i]);
	printf("Enter pivot:");
	scanf("%d",&pivot);
	for(i=0;i<size;++i)
	{
		if(array[i]==pivot)
		{
			pivotindex=i;
			break;
		}
		else
			pivotindex=-1;
	}
	if(pivotindex==-1)
	{
		printf("Pivot element not in the input array\n");
		exit(0);
	}
	partition(array,0,size-1,pivotindex);
	printf("Sorted Array\n");
	for(i=0;i<size;++i)
		printf("%d\t",array[i]);
	return 0;
}

/*void quick_sort(int array[],int low,int upper)
{
	int j;
	if(low<upper)
	{
		j=partition(array,low,upper);
		quick_sort(array,low,j-1);
		quick_sort(array,j+1,upper);
	}
}*/

int partition(int array[],int low,int upper,int pivotindex)
{
	int i,j,temp;
	i=low-1;
	j=upper+1;
	do
	{
		do
		{
			i++;
		}while(array[i]<=array[pivotindex] && i<=upper);
		do
		{
			j--;
		}while(array[pivotindex]<array[j]);
		if(i<j)
		{
			temp=array[i];
			array[i]=array[j];
			array[j]=temp;
		}
			
	}while(i<j);
	temp=array[pivotindex];
	array[pivotindex]=array[j];
	array[j]=temp;
	return j;
}
