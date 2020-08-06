#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
int main(int argc,char *argv[])
{
	int rank,size,mrow=atoi(argv[1]),mcol=atoi(argv[2]),matrix[mrow][mcol],vector[mcol],i,j,k=0,n,*sendbuf,*displs,*rcvbuf,*localresult,*result,localsize;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	if(rank==0)
	{
		//vector=(int*)malloc(mcol*sizeof(int));
		/*matrix=(int**)calloc(mrow,sizeof(int *));
	 	for (i=0; i<mrow; ++i) 
			matrix[i] = (int *)calloc(mcol , sizeof(int));*/ 
		for(i=0;i<mrow;++i)
		{
			for(j=0;j<mcol;++j)
			{
				matrix[i][j]=i;
				printf("%d\t",matrix[i][j]);
				if(i==0)
				{
					vector[j]=j+1;
					printf("(V)%d\t",vector[j]);
				}
			}
			printf("\n");
		}
		sendbuf=(int*)malloc(size*sizeof(int));
		displs=(int*)malloc(size*sizeof(int));	
		result=(int*)malloc(mcol*sizeof(int));
		displs[0]=0;		
	}
	
	MPI_Bcast(vector,mcol,MPI_INT,0,MPI_COMM_WORLD);
	if(mrow%size==0)
		n=(mrow/size)*mcol;
	else
	{
		n=(mrow/size)*mcol;
		if(rank==size-1)
			n=((mrow/size)+(mrow%size))*mcol;
	}
	
	localsize=n/mcol;
	rcvbuf=(int *)malloc(n*sizeof(int));
	localresult=(int*)malloc(localsize*sizeof(int));
	for(i=0;i<localsize;++i)
		localresult[i]=0;
	
	MPI_Gather(&n,1,MPI_INT,sendbuf,1,MPI_INT,0,MPI_COMM_WORLD);
	if(rank==0)
	{
		for(i=1;i<size;++i)
			displs[i]=displs[i-1]+sendbuf[i-1];
	}
	
			
	MPI_Scatterv(matrix,sendbuf,displs,MPI_INT,rcvbuf,n,MPI_INT,0,MPI_COMM_WORLD);
	i=0,j=0,k=0;
	
	for(i=0;i<localsize;++i)
	{
		for(j=0;j<mcol;++j)
		{
			
			localresult[i]+=rcvbuf[k]*vector[j];
			k++;
			
		}
	}
	MPI_Gather(&localsize,1,MPI_INT,sendbuf,1,MPI_INT,0,MPI_COMM_WORLD);	
	if(rank==0)
	{
		for(i=1;i<size;++i)
			displs[i]=displs[i-1]+sendbuf[i-1];
	} 
	MPI_Gatherv(localresult,localsize,MPI_INT,result,sendbuf,displs,MPI_INT,0,MPI_COMM_WORLD);
	if(rank==0)
	{
		for(i=0;i<mcol;++i)
			printf("\n%d",result[i]);
	}
        MPI_Finalize();
        return 0;
}
        	
        
        
