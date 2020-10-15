#include<stdio.h>
#include<string.h>
#include<mpi.h>

double f(double x){
    x = x*x;
    return x;
}

double Trap(//计算局部的定积分值
      double left_endpt,//左边界
      double right_endpt,//右边界
      int    trap_count,//分块数
      double base_len){//每小块    
   
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

int main(void) {
    int my_rank, comm_sz, n = 1024, local_n;//n,local_n为全局和局部计数量
    double a = 0.0, b = 3.0, h, local_a, local_b;//分别为全局起始点，梯形高度，局部起始点
    double local_int, total_int;//局部总和和全体总和
    int source;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    h = (b - a)/n;
    local_n = n/comm_sz;//每个进程需要计算多少块

    local_a = a + my_rank*local_n*h;
    local_b = local_a + local_n*h;
    local_int = Trap(local_a, local_b, local_n, h);
    if(my_rank != 0){
    	MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0,  MPI_COMM_WORLD);
    }else{
    	total_int = local_int;
	for (source = 1; source <= comm_sz-1; ++source){
	    MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	    total_int += local_int;
	}
    }

    if(my_rank == 0){
    	printf("With n = %d trapezoidss, our estimate\n", n);
	printf("of the integral from %f to %f  = %.15e\n", a, b, botal_int);
    }

    MPI_Finalize();
    return 0;
}/*main*/





