#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>


float f(float x){
    x = x*x;
    return x;
}

float Trap(//计算局部的定积分值
      float left_endpt,//左边界
      float right_endpt,//右边界
      int    trap_count,//分块数
      float base_len){//每小块    
   
    float estimate, x;
    int i;

    estimate = (f(left_endpt) + f(right_endpt))/2.0;//先算首尾
    for (i = 1;i < trap_count; ++i){
    	x = left_endpt + i*base_len;//分块左下标值
	estimate += f(x);//循环加上中间其他项的值
    }
    estimate = estimate*base_len;//根据公式乘上梯形高度h

    return estimate;
}

int main(int argc, char** argv)
{
	int my_rank, p;
	float endpoint[2];//left and right 
	int n = 1024;
	float h, local_a, local_b, local_n;
	float integral, total;
	int source, dest = 0, tag = 0;
	MPI_Status status;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	if(argc != 3){
		if(my_rank == 0)
			printf("Usage: mpirun -np <numprocs> trapezoid <left> <right>\n ");
		MPI_Finalize();
		exit(0);
	}
	if(my_rank == 0){
	endpoint[0] = atof(argv[1]);//left endpoint
	endpoint[1] = atof(argv[2]);//ringht endpoint
	}
	
	MPI_Bcast(endpoint, 2, MPI_FLOAT, 0, MPI_COMM_WORLD);

	h = (endpoint[1] - endpoint[0])/n;
	local_n = n/p;

	if(my_rank == 0) printf("a=%f, b=%f, Local number of trapezoid=%f\n", endpoint[0], endpoint[1], local_n);

	local_a = endpoint[0] + my_rank*local_n*h;
	local_b = local_a + local_n*h;
	integral = Trap(local_a, local_b, local_n, h);
	
	MPI_Reduce(&integral, &total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
	if(my_rank == 0){
		printf("With n=%d trapezoids, our eatimate\n", n);
		printf("of the integral from %f to %f= %f\n", endpoint[0], endpoint[1], total);
	}
	MPI_Finalize();
	return 0;
}
