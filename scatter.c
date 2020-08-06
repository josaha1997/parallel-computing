#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
int main(int argc,char *argv[])
{
	int rank,size,N[100],n,start,end,i,localsum=0,tag=1,rsum,rN[25];
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	if(rank==0)
	{
		for(i=1;i<=100;i++)
		{
			N[i-1]=i;	
			//printf("N[i]=%d",N[i-1]);
		}
		
	}
	
	n=100/size;
	//rN=(int*)calloc(n,sizeof(int));
	MPI_Scatter(&N,n,MPI_INT,&rN,n,MPI_INT,0,MPI_COMM_WORLD);
	for(i=0;i<n;++i)
	{
		localsum+=rN[i];
		//printf("\nrN[%d]=%d",rank,rN[i]);
	}
	MPI_Reduce(&localsum,&rsum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);	
	if(rank==0)
		printf("Total:%d",rsum);
	MPI_Finalize();
}
