#include "CubicSpline.h"
#include "malloc.h"  //方便使用变量定义数组大小

 void mdlInitializeSizes(SimStruct *S)
{
    /*参数只有一个，是n乘2的定点数组[xi, yi]:
     * [ x1，y1;
     *   x2, y2;
     *   ..., ...;
     *   xn, yn;
    */
}

 void mdlInitializeSampleTimes(SimStruct *S)
{
   
}


void mdlOutputs(SimStruct *S, int_T tid)
{
    const real_T *map =NULL;  //获取定点数据
    const int_T *mapSize = NULL;  //定点数组维数
    const real_T *x = NULL;  //输入x
    real_T       *y = NULL; //输出y
    int_T step = 0;  //输入x在定点数中的位置
    int_T i;
    real_T yval;

    for (i = 0; i < mapSize[0]; i++)
    {
        if (x[0] >= map[i] && x[0] < map[i + 1])
        {
            step = i;
            break;
        }
    }
    
    cubic_getval(&yval, mapSize, map, x[0], step);
    y[0] = yval;
    
}

//自然边界的三次样条曲线函数
void cubic_getval(real_T* y, const int_T* size, const real_T* map, const real_T x, const int_T step)
{
    int_T n = size[0];
    
    //曲线系数
    real_T* ai = (real_T*)malloc(sizeof(real_T) * (n-1));
    real_T* bi = (real_T*)malloc(sizeof(real_T) * (n-1));
    real_T* ci = (real_T*)malloc(sizeof(real_T) * (n-1));
    real_T* di = (real_T*)malloc(sizeof(real_T) * (n-1));
    
    real_T* h = (real_T*)malloc(sizeof(real_T) * (n-1));  //x的??
    
    /* M矩阵的系数
     *[B0, C0, ...
     *[A1, B1, C1, ...
     *[0,  A2, B2, C2, ...
     *[0, ...             An-1, Bn-1]
     */
    real_T* A = (real_T*)malloc(sizeof(real_T) * (n-2));
    real_T* B = (real_T*)malloc(sizeof(real_T) * (n-2));
    real_T* C = (real_T*)malloc(sizeof(real_T) * (n-2));
    real_T* D = (real_T*)malloc(sizeof(real_T) * (n-2)); //等号右边的常数矩阵
    real_T* E = (real_T*)malloc(sizeof(real_T) * (n-2)); //M矩阵
    
    real_T* M = (real_T*)malloc(sizeof(real_T) * (n));  //包含端点的M矩阵
    
    int_T i;
    
    //计算x的步长
    for ( i = 0; i < n -1; i++)
    {
        h[i] = map[i + 1] - map[i];
    }
    
    //指定系数
    for( i = 0; i< n - 3; i++)
    {
        A[i] = h[i]; //忽略A[0]
        B[i] = 2 * (h[i] + h[i+1]);
        C[i] = h[i+1]; //忽略C(n-1)
    }

    
    //指定常数D
    for (i = 0; i<n - 3; i++)
    {
        D[i] = 6 * ((map[n + i + 2] - map[n + i + 1]) / h[i + 1] - (map[n + i + 1] - map[n + i]) / h[i]);
    }
    
    
    //求解三对角矩阵，结果赋值给E
    TDMA(E, n-3, A, B, C, D);
    
    M[0] = 0; //自然边界的首端M为0
    M[n-1] = 0;  //自然边界的末端M为0
    for(i=1; i<n-1; i++)
    {
        M[i] = E[i-1]; //其它的M值
    }
    
    //?算三次?条曲?的系数
    for( i = 0; i < n-1; i++)
    {
        ai[i] = map[n + i];
        bi[i] = (map[n + i + 1] - map[n + i]) / h[i] - (2 * h[i] * M[i] + h[i] * M[i + 1]) / 6;
        ci[i] = M[i] / 2;
        di[i] = (M[i + 1] - M[i]) / (6 * h[i]);
    }
    
    *y = ai[step] + bi[step]*(x - map[step]) + ci[step] * (x - map[step]) * (x - map[step]) + di[step] * (x - map[step]) * (x - map[step]) * (x - map[step]);
    
    free(h);
    free(A);
    free(B);
    free(C);
    free(D);
    free(E);
    free(M);
    free(ai);
    free(bi);
    free(ci);
    free(di);

}

void TDMA(real_T* X, const int_T n, real_T* A, real_T* B, real_T* C, real_T* D)
{
    int_T i;
    real_T tmp;

    //上三角矩阵
    C[0] = C[0] / B[0];
    D[0] = D[0] / B[0];

    for(i = 1; i<n; i++)
    {
        tmp = (B[i] - A[i] * C[i-1]);
        C[i] = C[i] / tmp;
        D[i] = (D[i] - A[i] * D[i-1]) / tmp;
    }

    //直接求出X的最后一个值
    X[n-1] = D[n-1];

    //逆向迭代， 求出X
    for(i = n-2; i>=0; i--)
    {
        X[i] = D[i] - C[i] * X[i+1];
    }
}
