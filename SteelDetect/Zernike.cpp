#include <math.h>
#include <opencv2/opencv.hpp>
    /*1.高斯滤波对图像去噪。
	2.由原始灰度图求出纵横2个梯度图，以及综合梯度图（求梯度的算子很多）。
	3.结合3个 梯度图来进行非极大抑制（此步一过，检测的图像边缘已经很细了）
	4.进行边缘连接（强边缘到 弱边缘的连接，所有可能连接的点都出不了非极大抑制后的范围，在非极大抑制的结果中去除假边缘保留真边缘，同时又能让保留的 边缘尽量连贯真实）
	5.对边缘进行细化
	*/
/* Scale floating point magnitudes and angles to 8 bits */
const double PI_D=360;
const int count = 5;//5*5 算子
/** *  zer_pol() computes the zernike basis function *   
 V(n,m,x,y). 
 * @return res[1] is the dcomplex for V(n,m,x,y)*/

/*计算一行的像素个数 
imwidth:图像宽度 
deep:图像深度（8位灰度图为1，24位彩色图为3） 
*/  
//widthstep是行字节数
/*
图像详细参数
nSize:   112
imageSize: 36
nChannels: 3
height:   3
width:   3
widthstep: 12
		   979797979797979797000
		   979797979797979797000
		   979797979797979797000
		   */
//suppose only gray

//get the value of x ,y
//psrcBmp is the original point of img
//uchar *psrcBmp = img->imageData;
//CvScalar store 4 double val[4]
double getPixel(const uchar *srcImg, const int widthStep, const int x, const int y, int channel = 1)  
{  
	//return (srcImg+x*widthStep)[y]; 
	return (double)srcImg[x*widthStep+y];
}  

//获取标准矩(只支持8位灰度图)  
void GetStdMoment(uchar *psrcBmp ,int widthStep,int nsrcBmpWidth,int nsrcBmpHeight,double *m)  
{  
	// 如果p q 为0则M等于所有像素点的和
    // m0 m1 m2 
	for ( int p = 0 ; p < 2 ; p++ )  
		for ( int q = 0 ; q < 2 ; q++ )  
		{  
			if( p == 1 && q == 1)  
				break;  
			for ( int y = 0 ; y < nsrcBmpHeight ; y++ )  
				for ( int x = 0 ; x < nsrcBmpWidth ; x++ ){ 
					m[p*2+q] += (pow( (double)x , p ) * pow( (double)y , q ) * getPixel(psrcBmp , nsrcBmpWidth , x ,y, 1)); 
				}
		}  
}  
//阶乘  
double Factorial( int n )  
{  
	if( n < 0 )  
		return -1;  

	double m = 1;  
	for(int i = 2 ; i <= n ; i++)  
	{  
		m *= i;  
	}  
	return m;  
}  

//阶乘数，计算好方便用，提高速度  小于10 直接用下面的，大于10 用上面的。
double factorials[11] = {1 , 1 , 2 , 6 , 24 , 120 , 720 , 5040 , 40320 , 362880 , 39916800};  

//把图像映射到单位圆，获取像素极坐标半径
//x0,y0
double GetRadii(int nsrcBmpWidth,int nsrcBmpHeight,int x0,int y0,int x,int y)  
{
	double lefttop = sqrt(((double)0 - x0)*(0 - x0) + (0 - y0)*(0 - y0));  
	double righttop = sqrt(((double)nsrcBmpWidth - 1 - x0)*(nsrcBmpWidth - 1 - x0) + (0 - y0)*(0 - y0));  
	double leftbottom = sqrt(((double)0 - x0)*(0 - x0) + (nsrcBmpHeight - 1 - y0)*(nsrcBmpHeight - 1 - y0));  
	double rightbottom = sqrt(((double)nsrcBmpWidth - 1 - x0)*(nsrcBmpWidth - 1 - x0) + (nsrcBmpHeight - 1 - y0)*(nsrcBmpHeight - 1 - y0));  

	//maxRadii 为最大半径
	double maxRadii = lefttop;  
	maxRadii < righttop ? righttop : maxRadii;  
	maxRadii < leftbottom ? leftbottom : maxRadii;  
	maxRadii < rightbottom ? rightbottom : maxRadii;  

	double Radii = sqrt(((double)x - x0)*(x - x0) + (y - y0)*(y - y0))/maxRadii;  
	if(Radii > 1)  
	{  
		Radii = 1;  
	}  
	return Radii;  
}  

//把图像映射到单位圆，获取像素极坐标角度  
double GetAngle(int nsrcBmpWidth,int nsrcBmpHeight,int x,int y)  
{  
	double o=0.0;  

	double dia = sqrt((double)nsrcBmpWidth*nsrcBmpWidth + nsrcBmpHeight*nsrcBmpHeight);  
	int x0 = nsrcBmpWidth / 2;  
	int y0 = nsrcBmpHeight / 2;  
	double x_unity = (x - x0)/(dia/2);   
	double y_unity = (y - y0)/(dia/2);  

	if( x_unity == 0 && y_unity >= 0 )  
		o=PI_D/2;  
	else if( x_unity ==0 && y_unity <0)  
		o=1.5*PI_D;  
	else  
		o=atan( y_unity / x_unity );  
	if(o*y<0)    //第三象限  
		o=o+PI_D;  
	return o;  
}  

//Zernike不变矩  
//M00 m01 m10 m11 x=m10/m00 y=m01/m00

// 
void J_GetZernikeMoment(uchar *psrcBmp ,int widthStep,int nsrcBmpWidth,int nsrcBmpHeight,double *Ze)  
{  
	double R[count][count] = {0.0}; //实部 
	double V[count][count] = {0.0}; //虚部 

	double M[4] = {0.0};  
	GetStdMoment(psrcBmp ,widthStep,nsrcBmpWidth ,nsrcBmpHeight ,M);  
	int x0 = (int)(M[2]/M[0]+0.5);  //为了四舍五入
	int y0 = (int)(M[1]/M[0]+0.5);
	for(int n = 0 ; n < count ; n++)  
	{  
		for (int m = 0 ; m < count ; m++)  
		{  
			//优化算法，只计算以下介数  
			//z10 z11 z20 z22 z30 z31 z32 z33 z40 z41 z42 z43 z44,求这么多？
			//
			if( (n == 1 && m == 0) ||  
				(n == 1 && m == 1) ||  
				(n == 2 && m == 0) ||  
				(n == 2 && m == 1) ||  
				(n == 2 && m == 2) ||  
				(n == 3 && m == 0) ||  
				(n == 3 && m == 1) ||  
				(n == 3 && m == 2) ||  
				(n == 3 && m == 3) ||  
				(n == 4 && m == 0) ||  
				(n == 4 && m == 1) ||  
				(n == 4 && m == 2) ||  
				(n == 4 && m == 3) ||  
				(n == 4 && m == 4))  

			{  
				for(int y = 0 ; y < nsrcBmpHeight ; y++)  
				{  
					for (int x = 0 ; x < nsrcBmpWidth ; x++)  
					{  
						for(int s = 0 ; (s <= (n - m)/2 ) && n >= m ; s++)  
						{  
							//Rpq 是实值径向多项式：也即正交多项式
							R[n][m] += pow( -1.0, s )  
								* ( n - s > 10 ? Factorial( n - s ) : factorials[ n - s ] )  
								* pow( GetRadii( nsrcBmpWidth, nsrcBmpHeight, x0, y0, x, y ), n - 2 * s )  
								/ ( ( s > 10 ? Factorial( s ) : factorials[ s ] )  
								* ( ( n + m ) / 2 - s > 10 ? Factorial( ( n + m ) / 2 - s ) : factorials[ ( n + m ) / 2 - s ] )  
								* ( ( n - m ) / 2 - s > 10 ? Factorial( ( n - m ) / 2 - s ) : factorials[ ( n - m ) / 2 - s ] ) );  
						}  
						Ze[ n * count + m ] += R[ n ][ m ] 
						* getPixel( psrcBmp, widthStep, x ,y,1)  
							* cos( m * GetAngle( nsrcBmpWidth, nsrcBmpHeight, x, y) );//实部  

						V[n][m] += R[ n ][ m ]   
						* getPixel( psrcBmp, widthStep, x, y,1)  
							* sin( m * GetAngle( nsrcBmpWidth, nsrcBmpHeight, x, y ) );//虚部  

						R[n][m] = 0.0;  
					}  
				} 
				//在n m里 count 为5,这意味着（n+1)/pi *
				*(Ze+n*count + m) = sqrt( (*(Ze+n*count + m))*(*(Ze+n*count + m)) + V[n][m]*V[n][m] )*(n+1)/PI_D/M[0];  
			}  
		}  
	}  
}  



void Zernike3Moment(uchar *psrcBmp ,int widthStep,int nsrcBmpWidth,int nsrcBmpHeight){

}