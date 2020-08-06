#include<mpi.h>
#include<stdio.h>
int main(int argc,char *argv[])
{
	int rank,size,N=100,n,start,end,i,localsum=0,tag=1,rsum;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	//printf("Rank:%d Size:%d ",rank,size);
	n=N/size;
	start=rank*n;
	if(size%2!=0 && rank==size-1)
		end=N;
	else
		end=start+n;
	printf("\nn=%d start=%d end=%d\n",n,start,end-1);
	localsum=0;
	for(i=start;i<end;i++)
		localsum+=i;
	printf("Local Sum %d:%d\n",rank,localsum);
	
	/*if(rank==0)
	{
		for(i=1;i<size;i++)
		{
			MPI_Recv(&rsum,1,MPI_INT,i,tag,MPI_COMM_WORLD,&status);
			localsum+=rsum;
		}
		printf("Final Sum:%d",localsum);
	}
	else
		MPI_Send(&localsum,1,MPI_INT,0,tag,MPI_COMM_WORLD);*/
	MPI_Reduce(&localsum,&rsum,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);	//Collective call
	if(rank==0)
		printf("Final Sum:%d",rsum);
	MPI_Finalize();
}
