#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
int main(int argc,char *argv[])
{
	
	FILE *fptr;
	int size,rank,msize;
	int row,index=0,flag,localsize,i,j,k,n,*sendbuf,*displs;
	float norm=0,*localresult,*old,*new,sum=0.0,*vector,matrix[3][3],*rcvvector;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
 
	if(rank==0)
	{
		fptr=fopen("gaussjacobi.txt","r");
		if(fptr==NULL)
		{
			printf("Error opening file\n");
			exit(0);
		}
		fscanf(fptr,"%d", &row);
		printf("Row=%d",row);
		//msize=row*row;
		//matrix[row][row];
		//int (*matrix)[row] = malloc(row*sizeof(*matrix));
		/*matrix=(int**)malloc(row*sizeof(int*));
		for(i=0;i<row;++i)
			matrix[i]=(int*)malloc(row*sizeof(int));*/
		vector=(float*)malloc(row*sizeof(float));
		old=(float*)malloc(row*sizeof(float));
		for(i=0;i<row;++i)
		{
			for(j=0;j<row;++j)
			{//*(arr + i*c + j) = ++count; 
				if(fscanf(fptr,"%f",&matrix[i][j])){}
				printf("\tM=%f",matrix[i][j]);
			}
			printf("\n");
		}
		for(i=0;i<row;++i)
		{
			fscanf(fptr,"%f",&vector[i]);
			printf("\tV=%f",vector[i]);
		}
		for(i=0;i<row;++i)
		{
			fscanf(fptr,"%f",&old[i]);
			printf("\tO=%f",old[i]);
		}
		sendbuf=(int*)malloc(size*sizeof(int));
		displs=(int*)malloc(size*sizeof(int));
		displs[0]=0;
		fclose(fptr);
	}
	
	
	MPI_Bcast(&row,1,MPI_INT,0,MPI_COMM_WORLD);
	if(rank!=0)
	{
		old=(float*)malloc(row*sizeof(float));
		sendbuf=(int*)malloc(size*sizeof(int));
		displs=(int*)malloc(size*sizeof(int));
	}
	MPI_Bcast(old,row,MPI_FLOAT,0,MPI_COMM_WORLD);
	if(row%size==0)
		n=(row/size)*row;
	else
	{
		n=(row/size)*row;
		if(rank==size-1)
			n=((row/size)+(row%size))*row;
	}
	//printf("Rank %d=%d",rank,n);
	new=(float*)malloc(row*sizeof(float));
	localsize=n/row;
	float rcvmatrix[localsize][row];
	//rcvmatrix=(int *)malloc(n*sizeof(int));
	rcvvector=(float *)malloc(localsize*sizeof(float));
	//for(i=0;i<localsize;++i)
		//rcvmatrix[i]=(int*)malloc(row*sizeof(int));
	localresult=(float*)malloc(localsize*sizeof(float));
	for(i=0;i<localsize;++i)
		localresult[i]=0.0;
	
	MPI_Gather(&n,1,MPI_INT,sendbuf,1,MPI_INT,0,MPI_COMM_WORLD);
	if(rank==0)
	{
		for(i=1;i<size;++i)
		{
			//printf("S=%d",sendbuf[i-1]);
			displs[i]=displs[i-1]+sendbuf[i-1];
		}
	}			
	MPI_Scatterv(matrix,sendbuf,displs,MPI_FLOAT,rcvmatrix,n,MPI_FLOAT,0,MPI_COMM_WORLD);
	for(i=0;i<localsize;++i)
		for(j=0;j<row;++j)
			printf("\nRank (%d)=>%f",rank,rcvmatrix[i][j]);
	if(rank==0)
	{
		for(i=0;i<size;++i)
		{
			sendbuf[i]/=row;
			displs[i+1]=displs[i]+sendbuf[i];
		}
	}
	MPI_Scatterv(vector,sendbuf,displs,MPI_FLOAT,rcvvector,n,MPI_FLOAT,0,MPI_COMM_WORLD);
	MPI_Bcast(displs,size,MPI_INT,0,MPI_COMM_WORLD);	
	MPI_Bcast(sendbuf,size,MPI_INT,0,MPI_COMM_WORLD);
	do
	{
		sum=0.0;
		for(i=0;i<localsize;++i)
		{
			
				index=displs[rank]+i;
			//printf("\nIndex of Rank %d is %d",rank,index);
		
			for(j=0;j<row;++j)
			{
				if(j!=index)
					sum+=rcvmatrix[i][j]*old[j];	
				
			}
			//printf("\nrcvm=%d Rank(%d)",rcvmatrix[i][index],rank);
			localresult[i]=(float)((rcvvector[i]-sum)/rcvmatrix[i][index]);
			//printf("LF=%f",localresult[i]);
		} 
		MPI_Allgatherv(localresult,localsize,MPI_FLOAT,new,sendbuf,displs,MPI_FLOAT,MPI_COMM_WORLD);
		/*if(rank==0){
		for(i=0;i<localsize;++i)
			printf("\nLR=%f",new[i]);
		}*/
		
		if(rank==0)
		{
			norm=0.0;
			for(i=0;i<row;++i)
				norm+=pow((new[i]-old[i]),2);
			norm=sqrt(norm);
			printf("\nNORM=%lf ",norm);
			if(norm<=0.0000001)
			{
				flag=1;	
				for(i=0;i<row;++i)
					printf("NEW=%f",new[i]);
				//fclose(fptr);
			}
			else
			{
				flag=0;
				for(i=0;i<row;++i)
				{
					old[i]=new[i];
					new[i]=0.0;
				}
				
			}	
		}
		MPI_Bcast(&flag,1,MPI_INT,0,MPI_COMM_WORLD);
		if(flag==0)
			MPI_Bcast(old,row,MPI_FLOAT,0,MPI_COMM_WORLD);
		
	}while(flag==0);
/*	if(rank==0)
	{
		free(vector);
		free(sendbuf);
		free(displs);
	}	
	free(sendbuf)
	free(old);
	free(new);
	free(rcvmatrix);
	free(rcvvector);
	free(localresult);*/
	
        MPI_Finalize();
        return 0;
}
        	
        
        
