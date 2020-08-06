//Quick sort
#include<stdio.h>
#include<stdlib.h>
void quick_sort(int[],int,int);
int partition(int[],int,int);

int main()
{
	int *array,size,i;
	printf("Enter the number of inputs:");
	scanf("%d",&size);
	array=(int*)malloc(size*sizeof(int));
	for(i=0;i<size;++i)
		scanf("%d",&array[i]);	quick_sort(array,0,size-1);
	printf("Sorted Array\n");
	for(i=0;i<size;++i)
		printf("%d\t",array[i]);
	return 0;
}

void quick_sort(int array[],int low,int upper)
{
	int j;
	if(low<upper)
	{
		j=partition(array,low,upper);
		quick_sort(array,low,j-1);
		quick_sort(array,j+1,upper);
	}
}

int partition(int array[],int low,int upper)
{
	int pivot,i,j,temp;
	pivot=array[low];
	i=low;
	j=upper+1;
	do
	{
		do
		{
			i++;
		}while(array[i]<pivot && i<=upper);
		do
		{
			j--;
		}while(pivot<array[j]);
		if(i<j)
		{
			temp=array[i];
			array[i]=array[j];
			array[j]=temp;
		}
			
	}while(i<j);
	array[low]=array[j];
	array[j]=pivot;
	return j;
}
