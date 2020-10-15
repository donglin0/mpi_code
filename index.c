#include<stdio.h>
#include<mpi.h>
#define NELEMENTS 6

int main(argc, argv) int argc; char* argv[];
{
	int numtasks, rank, source = 0, dest, tag = 1, i;
	MPI_Request req;
	float a[16]=
	{
		1.0, 2.0, 3.0, 4.0,5.0, 6.0, 7.0, 8.0,9.0, 
		10.0, 11.0, 12.0,13.0, 14.0, 15.0, 16.0
	};
	float b[NELEMENTS];

	MPI_Status stat;
	MPI_Datatype indextype;
	int blocklengths[2], displacements[2];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	
	blocklengths[0] = 4;//每个块的元素数量
	blocklengths[1] = 2;
	displacements[0] = 5;//该块的位移
	displacements[1] = 12;

	//keypoint
	//新类型占等间隔的的四个float内存
	MPI_Type_indexed(2, blocklengths, displacements,  MPI_FLOAT, &indextype);
	MPI_Type_commit(&indextype);

	if(rank == 0){
		for(i = 0;i < numtasks; i++) { 
			dest = i;
			MPI_Isend(a, 1, indextype, dest, tag, MPI_COMM_WORLD, &req);
		}
	}
	MPI_Recv(&b, NELEMENTS, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &stat);

	printf("rank=%d Result:%3.1f %3.1f %3.1f %3.1f %3.1f %3.1f \n", rank, b[0],
					   	b[1], b[2], b[3], b[4], b[5]);
	MPI_Type_free(&indextype);
	MPI_Finalize();
}


