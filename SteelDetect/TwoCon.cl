__kernel void edgeDetectKernel(	__global  	uchar * input,
								__global  	uchar  * output,
                                __global  	uint  * clSobelOpX,
                                __global  	uint  * clSobelOpY,
                                const     	uint 	width,
								const       uint    height
                                )
{
	int ix	 = get_global_id(0);
	int iy   = get_global_id(1);    
	uint sumX = 0;
	uint sumY = 0;
	int maskWidth = 3;
	
	for(int r=0;r<maskWidth;r++){
	   const int idxInt = (iy+r)*width + ix;
	   for(int c=0;c<maskWidth;c++){
	      sumX +=clSobelOpX[(r*maskWidth)+c]*input[idxInt+c];
		  sumY +=clSobelOpY[(r*maskWidth)+c]*input[idxInt+c];
	   }
	}

	output[iy*get_global_size(0)+ix] = abs(sumX)+abs(sumY);
}
