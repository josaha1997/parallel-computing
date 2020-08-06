#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
int main(int argc,char *argv[])
{
	
	MPI_Init(&argc,&argv);
	FILE *fptr;
	int source,dest,asize,i,j,ii,jj,k=0,blocksize,dispA,dispB;
	double **A,**B,**C,*An,*Bn,*Cn;
	int coords[2],rank,size,dims[2],period[]={1,1};
	MPI_Comm NEWDOMAIN;
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	if(sqrt(size))
		dims[0]=dims[1]=sqrt(size);
	else
	{
		printf("Number of processors should be a perfect square");
		exit(0);
	}
	MPI_Cart_create(MPI_COMM_WORLD,2,dims,period,0,&NEWDOMAIN);
	MPI_Cart_coords(NEWDOMAIN,rank,2,coords);
	
	/*MPI_Cart_shift(NEWDOMAIN,0,1,&source,&dest);
	printf("Rank %d Source %d Dest %d",rank,source,dest);
	printf("\nRank %d has coords %d %d",rank,coords[0],coords[1]);*/
	if(rank==0)
	{
		//printf("DIMS %d %d",dims[0],dims[1]);
		
   		//memset(An, 0, asize*asize*sizeof(float));
   		//memset(Bn, 0, asize*asize*sizeof(float));
		fptr=fopen("cannon.txt","r");
		if(fptr==NULL)
		{
			printf("Error opening file\n");
			exit(0);
		}
		fscanf(fptr,"%d", &asize);
		printf("Row=%d",asize);
		A=(double**)malloc(asize*sizeof(double*));
		B=(double**)malloc(asize*sizeof(double*));
		C=(double**)malloc(asize*sizeof(double*));
		An=(double*)malloc(asize*asize*sizeof(double));
		Bn=(double*)malloc(asize*asize*sizeof(double));
		Cn=(double*)malloc(asize*asize*sizeof(double));
		//double Cn[asize][asize];
		for(i=0;i<asize;++i)
		{
			A[i]=(double*)malloc(asize*sizeof(double));
			B[i]=(double*)malloc(asize*sizeof(double));
			C[i]=(double*)malloc(asize*sizeof(double));
		}
		for(i=0;i<asize;++i)
		{
			for(j=0;j<asize;++j)
				if(fscanf(fptr,"%lf",&A[i][j]))
					printf("\tA%lf",A[i][j]);				
			printf("\n");
		}
		for(i=0;i<asize;++i)
		{
			for(j=0;j<asize;++j)
				if(fscanf(fptr,"%lf",&B[i][j]))
					printf("\tB%lf",B[i][j]);				
			printf("\n");
		}
		float bsize=asize/sqrt(size);
		if (fabsf(roundf(bsize) -bsize ) <= 0.00001f) 
			blocksize=(int)bsize;
		
		else
		{
			printf("Block size is not integer");
			//exit(0);
		}
		
		k=0;
		//printf("Size %ld",sizeof(An));
		for(i=0;i<dims[0];++i)
		{
			for(j=0;j<dims[1];++j)
			{
				for(ii=0;ii<blocksize;++ii)
				{
					for(jj=0;jj<blocksize;++jj)
					{
						//printf("\nAA %lf ",B[i*blocksize+ii][j*blocksize+jj]);
						An[k]=A[i*blocksize+ii][j*blocksize+jj];
						Bn[k]=B[i*blocksize+ii][j*blocksize+jj];
						k++;
					}
				}
			}
		}
		//printf("k%d",k);
		/*for(i=0;i<asize*asize;++i)
		{
			printf("\ni%d",i);
			//An[i]=i;
			//Bn[i]=i;
			printf("\tA%lf B%lf",An[i],Bn[i]);
		}*/
	}
	MPI_Bcast(&blocksize,1,MPI_INT,0,NEWDOMAIN);
	double subA[blocksize][blocksize];
	double subB[blocksize][blocksize];
	double subC[blocksize][blocksize];
	/*subA=(double**)malloc(blocksize*sizeof(double*));
	subB=(double**)malloc(blocksize*sizeof(double*));
	subC=(double**)malloc(blocksize*sizeof(double*));
		
	for(i=0;i<blocksize;++i)
	{
		subA[i]=(double*)malloc(blocksize*sizeof(double));
		subB[i]=(double*)malloc(blocksize*sizeof(double));
		subC[i]=(double*)malloc(blocksize*sizeof(double));
	}*/
	int bsize=blocksize*blocksize;
	MPI_Scatter(An,blocksize*blocksize,MPI_DOUBLE,subA,blocksize*blocksize,MPI_DOUBLE,0,NEWDOMAIN);
	MPI_Scatter(Bn,blocksize*blocksize,MPI_DOUBLE,subB,blocksize*blocksize,MPI_DOUBLE,0,NEWDOMAIN);
	for(i=0;i<sqrt(size);++i)
	{
		if(i==0)
		{
			dispA=coords[0];
			dispB=coords[1];
		}
		else
			dispA=dispB=1;
		
		MPI_Cart_shift(NEWDOMAIN,1,dispA,&source,&dest);
		MPI_Sendrecv_replace(subA,bsize,MPI_DOUBLE,dest,0,source,0,NEWDOMAIN,&status);
		MPI_Cart_shift(NEWDOMAIN,0,dispB,&source,&dest);
		MPI_Sendrecv_replace(subB,bsize,MPI_DOUBLE,dest,1,source,1,NEWDOMAIN,&status);
		//printf("Block size %d",blocksize);
		for(ii=0;ii<blocksize;++ii)
			for(jj=0;jj<blocksize;++jj)
				for(j=0;j<blocksize;++j)
					subC[ii][jj]+=subA[ii][j]*subB[j][jj];
	}
	for(ii=0;ii<blocksize;++ii)
	{
		for(jj=0;jj<blocksize;++jj)
			printf("\tRank (%d) at [%d][%d]=%lf",rank,ii,jj,subC[ii][jj]);	
		printf("\n");
	}
	MPI_Gather(subC,bsize,MPI_DOUBLE,Cn,bsize,MPI_DOUBLE,0,NEWDOMAIN);
	k=0;
	if(rank==0)
	{
		k=0;
		for(i=0;i<asize;++i)
			for(j=0;j<asize;++j)
				C[i][j]=Cn[k++];
		k=0;
		for(i=0;i<dims[0];++i)
		{
			for(j=0;j<dims[1];++j)
			{
				for(ii=0;ii<blocksize;++ii)
				{
					for(jj=0;jj<blocksize;++jj)
					{
						Cn[k]=C[i*blocksize+ii][j*blocksize+jj];
						k++;
					}
				}
			}
		}
		k=0;
		for(i=0;i<asize;++i)
		{
			for(j=0;j<asize;++j)
			{
				C[i][j]=Cn[k++];
				printf("\nCC  (%d)(%d)%lf ",i,j,C[i][j]);
			}
		}
				
	}		
	MPI_Finalize();
	return 0;
}
