#include<stdio.h>
#include<mpi.h>
#define NELEM 25

int main(argc, argv) int argc;  char*argv[];
{
	int numtasks, rank, source = 0, dest, tag = 1, i;

	typedef struct{
		float x,y,z;
		float velocity;
		int n,type;
	} Particle;

	Particle p[NELEM], particles[NELEM];
	MPI_Datatype particletype, oldtypes[2];
	int blockcounts[2];//第一块内存

	MPI_Aint offset[2], extent, lb;//offset每块内存的起始地址
	MPI_Status stat;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	
	offset[0] = 0;
	oldtypes[0] = MPI_FLOAT;
	blockcounts[0] = 4;
	
	MPI_Type_get_extent(MPI_FLOAT, &lb, &extent);//获取下限和范围
	offset[1] = 4*extent;
	oldtypes[1] = MPI_INT;
	blockcounts[1] = 2;

	MPI_Type_create_struct(2, blockcounts, offset, oldtypes, &particletype);
	MPI_Type_commit(&particletype);
	if(rank == 0){
		for(i = 0;i < NELEM; ++i){
			particles[i].x = i*1.0;
			particles[i].y = i*-1.0;
			particles[i].z = i*1.0;
			particles[i].velocity = 0.25;
			particles[i].n = i;
			particles[i].type = i%2; 
		}
		for(i = 0; i < numtasks; ++i){
			dest = i;
			MPI_Send(particles, NELEM, particletype, dest, tag, MPI_COMM_WORLD);
		}
	}
		MPI_Recv(p, NELEM, particletype, source, tag, MPI_COMM_WORLD, &stat);
		printf("rank=%d, result: %3.2f %3.2f %3.2f %3.2f %d %d\n", rank, p[3].x, p[3].y, p[3].z, p[3].velocity, p[3].n, p[3].type);
		MPI_Type_free(&particletype);
		MPI_Finalize();
}
