#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define NRA 4 /*number of rows in matrix A */
#define NCA 4 /*number of columns in matrix A */
#define NCB 1 /*number of columns in matrix B */

#define MASTER 0 /*taskid of first task */
#define FROM_MASTER 1 /*setting a message type */
#define FROM_WORKER 2

int main(int argc, char* argv[])
{
int	numtasks,	/*number of tasks in partion */	    
    	taskid, 	/*a task identifier */
	numworkers, 	/*number of worker tasks */
	source,
	dest, 
	mtype, 		/*message type */
	rows, 		/*rows of matrix A sent to each worker */
	averow, extra, offset,  //除数、余数、偏移量 	
	/*used to determine rows sent to each worker */
	i, j, k, rc; 	/*misc */
	
	double a[NRA][NCA],	/*Matrix A to be mutiplied */
	       b[NCA][NCB],	/*Vector B */
	       c[NRA][NCB];	/*result Vector C */

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	if(numtasks < 2){
	    printf("Need at least two MPI tasks. Quitting...\n");
	    MPI_Abort(MPI_COMM_WORLD, rc);/*终止mpi程序*/
	}
	numworkers = numtasks - 1;/*除0进程外都是worker*/
	/***********************master task*****************/
	if(taskid == MASTER) {
	    printf("mpi_mm has started with %d tasks.\n", numtasks);
	    printf("Initializing arrays.\n");
	    for(i = 0; i < NRA; i++)
		for(j = 0; j < NCA; j++)
		    a[i][j] = i+j;
	    for(i = 0; i < NRA; i++)
		for(j = 0; j < NCB; j++)
		    b[i][j] = (i + 1)*(j + 1);
	    /*****显示矩阵和向量******/
	    for(i = 0; i < NRA; i++) {
	    	printf("\n");
		for(j = 0; j < NCA; j++)
		    printf("%6.2f", a[i][j]);
	    }
	    for(i = 0; i < NRA; i++) {
	    	printf("\n");
		for(j = 0; j < NCB; j++)
		    printf("%6.2f", b[i][j]);
	    }

	    /*Send matrix data to the worker tasks */
	    averow = NRA/numworkers;
	    extra = NRA%numworkers;
	    offset = 0;
	    mtype = FROM_MASTER;
	    for(dest = 1; dest <= numworkers; dest++)
	    {
	    	rows = (dest <= extra) ? averow + 1 : averow;
	    	printf("Send %d rows to task %d offset=%d\n", rows, dest, offset);
		MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
		MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
		MPI_Send(&a[offset][0], rows*NCA, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
		
		MPI_Send(&b, NCA*NCB, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
		offset += rows;
	    }
	    /*Receive result from worker tasks */
	    mtype = FROM_WORKER;
	    for(i = 1; i <= numworkers; i++) {
	    	source = i;
		MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
		MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
		MPI_Recv(&a[offset][0], rows*NCB, MPI_DOUBLE, source, mtype, MPI_COMM_WORLD, &status);
	    	printf("Received results from task %d\n", source);
	    }
	    /*print result*/
	    printf("Result Matrix:\n");
	    for(i = 0; i < NRA; i++)
	    {
	    	printf("\n");
		for(j = 0; j < NCB; j++)
			printf("%6.2f", c[i][j]);
	    }
	    printf("\n****************************************************\n");
	    printf("Done.\n");
	}
	
	if(taskid > MASTER) {
	    mtype = FROM_MASTER;
	    MPI_Recv(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
	    MPI_Recv(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD, &status);
	    MPI_Recv(&a, rows*NCA, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
	    MPI_Recv(&b, NCA*NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD, &status);
	    
	    for(k = 0; k < NCB; k++) /*NCB不为1时则为矩阵乘*/
		for(i = 0; i < rows; i++) {
		    c[i][k] = 0.0;
		    for(j = 0; j < NCA; j++)
			    c[i][k] = c[i][k] + a[i][j]*b[j][k];
		}
	
	    mtype = FROM_WORKER;
	    MPI_Send(&offset, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
	    MPI_Send(&rows, 1, MPI_INT, MASTER, mtype, MPI_COMM_WORLD);
	    MPI_Send(&c, rows*NCB, MPI_DOUBLE, MASTER, mtype, MPI_COMM_WORLD);
	}	
	MPI_Finalize();
}
