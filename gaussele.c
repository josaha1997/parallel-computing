#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
int main(int argc,char *argv[])
{
	
	FILE *fptr;
	int size,rank,row;
	float *augA,*localA,*recvA,*upperT,pivot,*result;
	int i,j,k=0;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size); 
	if(rank==0)
	{
		fptr=fopen("gaussele.txt","r");
		if(fptr==NULL)
		{
			printf("Error opening file\n");
			exit(0);
		}
		fscanf(fptr,"%d", &row);
		printf("Row=%d",row);
		if(size!=row)
			MPI_Finalize();
		augA=(float*)malloc(row*(row+1)*sizeof(float));
		/*for(i=0;i<row;++i)
			augA[i]=(float*)malloc((row+1)*sizeof(float));*/
		upperT=(float*)malloc(row*(row+1)*sizeof(float));
		/*for(i=0;i<row;++i)
			upperT[i]=(float*)malloc((row+1)*sizeof(float));*/
		for(i=0;i<row*(row+1);++i)
		{
			/*for(j=0;j<row+1;++j)
			{ */
				if(fscanf(fptr,"%f",&augA[i])){}
					//printf("\tM=%f",augA[i]);
			//}
			//printf("\n");
		}
		fclose(fptr);
	}
	result=(float*)malloc(row*sizeof(float));	
	MPI_Bcast(&row,1,MPI_INT,0,MPI_COMM_WORLD);
	localA=(float*)malloc((row+1)*sizeof(float));	
	recvA=(float*)malloc((row+1)*sizeof(float));	
	MPI_Scatter(augA,row+1,MPI_FLOAT,localA,row+1,MPI_FLOAT,0,MPI_COMM_WORLD);
	for(i=0;i<row+1;++i)
		//printf("\nRank(%d)=>%f",rank,localA[i]);
	do
	{
		//printf("\nk=%d",k);
		if(rank==k)
		{
			pivot=localA[k];
			for(i=0;i<=row;++i)
				if(pivot!=0)
					localA[i]/=pivot;
			for(i=rank+1;i<size;++i)
				MPI_Send(localA,row+1,MPI_FLOAT,i,1,MPI_COMM_WORLD);
		}
		else if (rank>k)
		{
			MPI_Recv(recvA,row+1,MPI_FLOAT,k,1,MPI_COMM_WORLD,&status);
			pivot=localA[k];
			for(i=k;i<row+1;++i)
				localA[i]-=recvA[i]*pivot;
		}
		k++;
	}while(k<row);
	k=row-1;
	do
	{
		float sum=0.0;
		if(rank==k)
		{
			for(i=row-1;i>k;--i)
				sum+=localA[i]*result[i];
			//printf("Rank %d sum %f",rank,sum);
			result[k]=localA[row]-sum;
			//printf("\n%fL",localA[row]);
			for(i=0;i<k;++i)
				MPI_Send(result,row,MPI_FLOAT,i,1,MPI_COMM_WORLD);
		}
		else if(rank<k)
		{
			MPI_Recv(result,row,MPI_FLOAT,k,1,MPI_COMM_WORLD,&status);
		}
		k--;
	}while(k>=0);
	
	/*MPI_Gather(localA,row+1,MPI_FLOAT,upperT,row+1,MPI_FLOAT,0,MPI_COMM_WORLD);
	if(rank==0)
	{
		int count=1;
		for(i=0;i<row*(row+1);++i)
		{
				printf("\tU=>%f",upperT[i]);
			if(i==count*row+(count-1))
			{
				printf("\n");
				count++;
			}
		}
	}*/
	if(rank==0)
		for(i=0;i<row;++i)
			printf("\tR=%f",result[i]);
	free(augA);
	free(localA);
	free(recvA);
	free(result);
	free(upperT);
	MPI_Finalize();
	return 0;
}
