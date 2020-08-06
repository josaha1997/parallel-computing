#include<mpi.h>
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
int main(int argc,char *argv[])
{
	int rank,size,i;
	float h,f=0.0,x,rsum,a=0.0,b=1.0,start,end,localh,localn=6;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	/*if(size%3!=0)
	{
		fprintf(stderr,"Invalid number of processers(should be multiple of 3)\n");
		MPI_Abort(MPI_COMM_WORLD,911);
	}*/
	//printf("Rank:%d Size:%d ",rank,size);
	h=(b-a)/size;
	//printf("h=%f",h);
	start=a+rank*h;
	end=start+h;
	localh=(end-start)/localn;
	printf("localh=%f",localh);
	for(i=0;i<=localn;++i)
	{	
		x=start+i*localh;	
		if(i==0)
		{
			f=f+4+sin(sqrt(start)*2)+sin(sqrt(end)*2);
		//printf("f0+f3=%f",f);
		}
		else if(i%3==0)
		{
			f=2*(2+sin(sqrt(x)*2));
		//printf("f=%f",f);
		}
		else
		{
			f=3*(2+sin(sqrt(x)*2));
		//printf("f%d=%f",rank,f);
		}
		//f=(3.0/8.0)*localh*(4+sin(sqrt(start)*2)+sin(sqrt(end)*2)+3*(4+sin(sqrt(start+localh)*2)+sin(sqrt(start+2*localh)*2)));
	}
	
	f=(3.0/8.0)*localh*f;
	printf("F=%f",f);
	//printf("x=%f",x);
	/*if(rank==0)
	{
		f=4+sin(sqrt(x)*2)+sin(sqrt(b)*2);
		//printf("f0+f3=%f",f);
	}
	else if(rank%3==0)
	{
		f=2*(2+sin(sqrt(x)*2));
		//printf("f=%f",f);
	}
	else
	{
		f=3*(2+sin(sqrt(x)*2));
		//printf("f%d=%f",rank,f);
	}*/
	
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
	MPI_Reduce(&f,&rsum,1,MPI_FLOAT,MPI_SUM,0,MPI_COMM_WORLD);	//Collective call
	if(rank==0)
	{
		//printf("Rsum %f h=%f",rsum,h);
		//float simpson=(3.0/8.0)*h*rsum;
		printf("Integration:%f",rsum);
	}
	MPI_Finalize();
}
