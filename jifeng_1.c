#include<stdio.h>
#include<mpi.h>


double f(double x){
    x = x*x;
    return x;
}

float Trap(//计算局部的定积分值
      float left_endpt,//左边界
      float right_endpt,//右边界
      int    trap_count,//分块数
      float base_len){//每小块    
   
    double estimate, x;
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
	float a = 0.0, b = 1.0; int n = 1024;
	float h, local_a, local_b, local_n;
	float integral, total;
	int source, dest = 0, tag = 0;
	MPI_Status status;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	h = (b - a)/n;
	local_n = n/p;

	local_a = a + my_rank*local_n*h;
	local_b = local_a + local_n*h;
	integral = Trap(local_a, local_b, local_n, h);

	if(my_rank == 0)
	{
		total = integral;
		for(source = 1; source < p; ++source){
		MPI_Recv(&integral, 1, MPI_FLOAT, source,
					   	tag, MPI_COMM_WORLD, &status);
		total = total + integral;
		}
	}else{
		MPI_Send(&integral, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
	}
	if(my_rank == 0){
		printf("With n=%d trapezoids, our eatimate\n", n);
		printf("of the integral from %f to %f= %f\n", a, b, total);
	}
	MPI_Finalize();
	return 0;
}
