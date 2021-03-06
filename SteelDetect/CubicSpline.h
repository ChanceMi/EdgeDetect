#include <cstdlib>
//样条插值
struct SimStruct{
	float x;
	float y;
};

typedef float real_T;
typedef int int_T;

void mdlInitializeSizes(SimStruct *S);
void mdlInitializeSampleTimes(SimStruct *S);
void mdlOutputs(SimStruct *S, int_T tid);
void cubic_getval(real_T* y, const int_T* size, const real_T* map, const real_T x, const int_T step);
void TDMA(real_T* X, const int_T n, real_T* A, real_T* B, real_T* C, real_T* D);