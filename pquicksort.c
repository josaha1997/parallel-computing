#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
void quick_sort(int [],int ,int );
int partition(int[],int,int,int,int,int);
int main(int argc,char *argv[])
{
	int rank,size,*array,asize=atoi(argv[1]),n,i,j,k=0,tag=1,*rarray,rsize,pivot,lsize,ssize,*larray,*marray,msize;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);

	if(rank==0)
	{
		//printf("Enter the number of inputs:");
		//scanf("%d",&asize);
		array=(int*)malloc(asize*sizeof(int));
		for(i=0;i<asize;++i)
		{
			array[i]=rand()/10000000;		
			//printf("Array[i]=%d",array[i]);
		}
		n=asize/size;
	}
	MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	rarray=(int *)malloc(n*sizeof(int));
	MPI_Scatter(array,n,MPI_INT,rarray,n,MPI_INT,0,MPI_COMM_WORLD);
	if(rank==0)
		pivot=rarray[0];
	MPI_Bcast(&pivot,1,MPI_INT,0,MPI_COMM_WORLD);
	//printf("\nPivot=%d",pivot);
	j=partition(rarray,0,n-1,pivot,rank,1);
	/*printf("(rank %d)->j=%d",rank,j);	
	for(i=0;i<n;++i)
		printf("\n(Rank %d)->%d\t",rank,rarray[i]);
		//printf("%d\t",rarray[i]);*/
	switch(rank)
	{
		case 2:
		case 3:ssize=j+1;
				MPI_Send(&ssize,1,MPI_INT,rank-2,0,MPI_COMM_WORLD);
				if(ssize>0)
					MPI_Send(rarray,ssize,MPI_INT,rank-2,tag,MPI_COMM_WORLD);
				MPI_Recv(&lsize,1,MPI_INT,rank-2,0,MPI_COMM_WORLD,&status);
				if(ssize>0 && lsize>0)
					msize=lsize+(n-ssize);
				else if(ssize==0 && lsize==0)
					msize=n;
				else if(ssize>0 && lsize==0)
					msize=n-ssize;
				else if(ssize==0 && lsize>0)
					msize=n+lsize;
				if(lsize>0)
				{
					larray=(int*)malloc(lsize*sizeof(int));
					MPI_Recv(larray,lsize,MPI_INT,rank-2,tag,MPI_COMM_WORLD,&status);
				}
				marray=(int*)malloc(msize*sizeof(int));
				for(i=0;i<lsize;++i)
					marray[k++]=larray[i];
				for(i=j+1;i<n;++i)
					marray[k++]=rarray[i];
				free(rarray);
				if(lsize>0)
					free(larray);					
				/*for(i=0;i<msize;++i)
					printf("\n(RRank %d)->%d\t",rank,marray[i]);*/
				break;
				
		case 0:
		case 1:ssize=n-(j+1);
				MPI_Recv(&lsize,1,MPI_INT,rank+2,0,MPI_COMM_WORLD,&status);
				if(ssize>0 && lsize>0)
					msize=lsize+(n-ssize);
				else if(ssize==0 && lsize==0)
					msize=n;
				else if(ssize>0 && lsize==0)
					msize=n-ssize;
				else if(ssize==0 && lsize>0)
					msize=n+lsize;
				if(lsize>0)
				{
					larray=(int*)malloc(lsize*sizeof(int));
					MPI_Recv(larray,lsize,MPI_INT,rank+2,tag,MPI_COMM_WORLD,&status);
				}
				MPI_Send(&ssize,1,MPI_INT,rank+2,0,MPI_COMM_WORLD);
				if(ssize>0)
					MPI_Send(&rarray[j+1],ssize,MPI_INT,rank+2,tag,MPI_COMM_WORLD);
				marray=(int*)malloc(msize*sizeof(int));
				for(i=0;i<lsize;++i)
					marray[k++]=larray[i];
				for(i=0;i<j+1;++i)
					marray[k++]=rarray[i];
				free(rarray);
				if(lsize>0)
					free(larray);
				/*for(i=0;i<msize;++i)
					printf("\n(RRank %d)->%d\t",rank,marray[i]);*/
				break;
	}
	pivot=marray[0];
	if(rank==0 || rank==2)
		MPI_Send(&pivot,1,MPI_INT,rank+1,2,MPI_COMM_WORLD);
	if(rank==1 || rank==3)
		MPI_Recv(&pivot,1,MPI_INT,rank-1,2,MPI_COMM_WORLD,&status);
	//printf("\nRANK(%d)->PIVOT=%d",rank,pivot);
	j=partition(marray,0,msize-1,pivot,rank,2);
	//printf("\n2(rank %d)->j=%d",rank,j);	
	/*for(i=0;i<msize;++i)
		printf("\n2(Rank %d)->%d\t",rank,marray[i]);*/
	k=0;	
	switch(rank)
	{
		case 1:
		case 3:ssize=j+1;
				MPI_Send(&ssize,1,MPI_INT,rank-1,0,MPI_COMM_WORLD);
				if(ssize>0)
					MPI_Send(marray,ssize,MPI_INT,rank-1,tag,MPI_COMM_WORLD);
				MPI_Recv(&lsize,1,MPI_INT,rank-1,0,MPI_COMM_WORLD,&status);
				if(ssize>0 && lsize>0)
					rsize=lsize+(msize-ssize);
				else if(ssize==0 && lsize==0)
					rsize=msize;
				else if(ssize>0 && lsize==0)
					rsize=msize-ssize;
				else if(ssize==0 && lsize>0)
					rsize=msize+lsize;
				/*if(rsize==0)
				{
					rsize=1;
					rarray=(int*)malloc(rsize*sizeof(int));
					rarray[0]=-1;
					free(marray);
					break;
				}*/
				if(lsize>0)
				{
					larray=(int*)malloc(lsize*sizeof(int));
					MPI_Recv(larray,lsize,MPI_INT,rank-1,tag,MPI_COMM_WORLD,&status);
				}
				rarray=(int*)malloc(rsize*sizeof(int));
				if(rarray==NULL)
					printf("NULL");
				for(i=0;i<lsize;++i)
					rarray[k++]=larray[i];
				for(i=j+1;i<msize;++i)
					rarray[k++]=marray[i];
				free(marray);
				if(lsize>0)
					free(larray);					
				/*for(i=0;i<rsize;++i)
					printf("\n(--RRank %d)->%d\t",rank,rarray[i]);*/
				break;
				
		case 0:
		case 2:ssize=msize-(j+1);
				MPI_Recv(&lsize,1,MPI_INT,rank+1,0,MPI_COMM_WORLD,&status);
				if(ssize>0 && lsize>0)
					rsize=lsize+(msize-ssize);
				else if(ssize==0 && lsize==0)
					rsize=msize;
				else if(ssize>0 && lsize==0)
					rsize=msize-ssize;
				else if(ssize==0 && lsize>0)
					rsize=msize+lsize;
				/*if(rsize==0)
				{
					rsize=1;
					rarray=(int*)malloc(rsize*sizeof(int));
					rarray[0]=-1;
					free(marray);
					break;
				}*/				
				if(lsize>0)
				{
					larray=(int*)malloc(lsize*sizeof(int));
					MPI_Recv(larray,lsize,MPI_INT,rank+1,tag,MPI_COMM_WORLD,&status);
				}
				MPI_Send(&ssize,1,MPI_INT,rank+1,0,MPI_COMM_WORLD);
				if(ssize>0)
					MPI_Send(&marray[j+1],ssize,MPI_INT,rank+1,tag,MPI_COMM_WORLD);
				rarray=(int*)malloc(rsize*sizeof(int));
				for(i=0;i<lsize;++i)
					rarray[k++]=larray[i];
				for(i=0;i<j+1;++i)
					rarray[k++]=marray[i];
				free(marray);				
				if(lsize>0)
					free(larray);
				/*for(i=0;i<rsize;++i)
					printf("\n(--RRank %d)->%d\t",rank,rarray[i]);*/
				break;
	}
	quick_sort(rarray,0,rsize-1);
	/*for(i=0;i<rsize;++i)
		printf("\nRank(%d)=%d",rank,rarray[i]);*/
	//printf("Rank=%d rsize=%d",rank,rsize);
	if(rank==0)
	{
		marray=(int*)malloc(size*sizeof(int));
		larray=(int*)malloc(size*sizeof(int));
		larray[0]=0;
	}
	MPI_Gather(&rsize,1,MPI_INT,marray,1,MPI_INT,0,MPI_COMM_WORLD);
	if(rank==0)
	{
		asize=0;
		for(i=0;i<size;++i)
			asize+=marray[i];
		free(array);
		array=(int*)malloc(asize*sizeof(int));	
	}
	printf("\t%d",larray[0]);
	for(i=1;i<size;++i)
	{
		larray[i]=larray[i-1]+marray[i-1];
		//printf("\t%d",larray[i]);
	}
	
	//printf("Rank %d",rank);
	MPI_Gatherv(rarray,rsize,MPI_INT,array,marray,larray,MPI_INT,0,MPI_COMM_WORLD);
	if(rank==0)
	{
		for(i=0;i<asize;++i)
			printf("\n(--RRank %d)->%d\t",rank,array[i]);
	}
	//printf("hello%d",rank);
	MPI_Finalize();
	return 0;
}
void quick_sort(int array[],int low,int upper)
{
	int j;
	if(low<upper)
	{
		j=partition(array,low,upper,0,-1,0);
		quick_sort(array,low,j-1);
		quick_sort(array,j+1,upper);
	}
}
int partition(int array[],int low,int upper,int pivot,int rank,int stage)
{
	int i,j,temp;
	if(rank==-1)
	{
		pivot=array[low];
		i=low;
	}
	else
		i=low-1;
	j=upper+1;
	do
	{
		do
		{
			i++;
		}while(array[i]<=pivot && i<=upper);
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
	if((stage==1 && rank==0) || (stage==2 && rank==0 )||(stage==2 && rank==2))
	{
		temp=array[0];
		array[0]=array[j];
		array[j]=temp;
	}
	else if(rank==-1)
	{
		array[low]=array[j];
		array[j]=pivot;
	}
	return j;
}
