 #pragma OPENCL EXTENSION cl_amd_printf : enable
 __kernel void gaussianFilter(int *a,int *b,int *c)
{
	int x=threadIdx.x + blockIdx.x*blockDim.x;
	int y==threadIdx.y + blockIdx.y*blockDim.y;
	int offset = x+y*blockDim.x*gridDim.x;


	while(tid<N){
	   c[td] = a[tid]+b[tid];
	   tid+=blockDim.x*gridDim.x;
	}
}
