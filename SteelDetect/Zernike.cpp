#include <math.h>
#include <opencv2/opencv.hpp>
    /*1.��˹�˲���ͼ��ȥ�롣
	2.��ԭʼ�Ҷ�ͼ����ݺ�2���ݶ�ͼ���Լ��ۺ��ݶ�ͼ�����ݶȵ����Ӻࣩܶ��
	3.���3�� �ݶ�ͼ�����зǼ������ƣ��˲�һ��������ͼ���Ե�Ѿ���ϸ�ˣ�
	4.���б�Ե���ӣ�ǿ��Ե�� ����Ե�����ӣ����п������ӵĵ㶼�����˷Ǽ������ƺ�ķ�Χ���ڷǼ������ƵĽ����ȥ���ٱ�Ե�������Ե��ͬʱ�����ñ����� ��Ե����������ʵ��
	5.�Ա�Ե����ϸ��
	*/
/* Scale floating point magnitudes and angles to 8 bits */
const double PI_D=360;
const int count = 5;//5*5 ����
/** *  zer_pol() computes the zernike basis function *   
 V(n,m,x,y). 
 * @return res[1] is the dcomplex for V(n,m,x,y)*/

/*����һ�е����ظ��� 
imwidth:ͼ���� 
deep:ͼ����ȣ�8λ�Ҷ�ͼΪ1��24λ��ɫͼΪ3�� 
*/  
//widthstep�����ֽ���
/*
ͼ����ϸ����
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

//��ȡ��׼��(ֻ֧��8λ�Ҷ�ͼ)  
void GetStdMoment(uchar *psrcBmp ,int widthStep,int nsrcBmpWidth,int nsrcBmpHeight,double *m)  
{  
	// ���p q Ϊ0��M�����������ص�ĺ�
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
//�׳�  
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

//�׳���������÷����ã�����ٶ�  С��10 ֱ��������ģ�����10 ������ġ�
double factorials[11] = {1 , 1 , 2 , 6 , 24 , 120 , 720 , 5040 , 40320 , 362880 , 39916800};  

//��ͼ��ӳ�䵽��λԲ����ȡ���ؼ�����뾶
//x0,y0
double GetRadii(int nsrcBmpWidth,int nsrcBmpHeight,int x0,int y0,int x,int y)  
{
	double lefttop = sqrt(((double)0 - x0)*(0 - x0) + (0 - y0)*(0 - y0));  
	double righttop = sqrt(((double)nsrcBmpWidth - 1 - x0)*(nsrcBmpWidth - 1 - x0) + (0 - y0)*(0 - y0));  
	double leftbottom = sqrt(((double)0 - x0)*(0 - x0) + (nsrcBmpHeight - 1 - y0)*(nsrcBmpHeight - 1 - y0));  
	double rightbottom = sqrt(((double)nsrcBmpWidth - 1 - x0)*(nsrcBmpWidth - 1 - x0) + (nsrcBmpHeight - 1 - y0)*(nsrcBmpHeight - 1 - y0));  

	//maxRadii Ϊ���뾶
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

//��ͼ��ӳ�䵽��λԲ����ȡ���ؼ�����Ƕ�  
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
	if(o*y<0)    //��������  
		o=o+PI_D;  
	return o;  
}  

//Zernike�����  
//M00 m01 m10 m11 x=m10/m00 y=m01/m00

// 
void J_GetZernikeMoment(uchar *psrcBmp ,int widthStep,int nsrcBmpWidth,int nsrcBmpHeight,double *Ze)  
{  
	double R[count][count] = {0.0}; //ʵ�� 
	double V[count][count] = {0.0}; //�鲿 

	double M[4] = {0.0};  
	GetStdMoment(psrcBmp ,widthStep,nsrcBmpWidth ,nsrcBmpHeight ,M);  
	int x0 = (int)(M[2]/M[0]+0.5);  //Ϊ����������
	int y0 = (int)(M[1]/M[0]+0.5);
	for(int n = 0 ; n < count ; n++)  
	{  
		for (int m = 0 ; m < count ; m++)  
		{  
			//�Ż��㷨��ֻ�������½���  
			//z10 z11 z20 z22 z30 z31 z32 z33 z40 z41 z42 z43 z44,����ô�ࣿ
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
							//Rpq ��ʵֵ�������ʽ��Ҳ����������ʽ
							R[n][m] += pow( -1.0, s )  
								* ( n - s > 10 ? Factorial( n - s ) : factorials[ n - s ] )  
								* pow( GetRadii( nsrcBmpWidth, nsrcBmpHeight, x0, y0, x, y ), n - 2 * s )  
								/ ( ( s > 10 ? Factorial( s ) : factorials[ s ] )  
								* ( ( n + m ) / 2 - s > 10 ? Factorial( ( n + m ) / 2 - s ) : factorials[ ( n + m ) / 2 - s ] )  
								* ( ( n - m ) / 2 - s > 10 ? Factorial( ( n - m ) / 2 - s ) : factorials[ ( n - m ) / 2 - s ] ) );  
						}  
						Ze[ n * count + m ] += R[ n ][ m ] 
						* getPixel( psrcBmp, widthStep, x ,y,1)  
							* cos( m * GetAngle( nsrcBmpWidth, nsrcBmpHeight, x, y) );//ʵ��  

						V[n][m] += R[ n ][ m ]   
						* getPixel( psrcBmp, widthStep, x, y,1)  
							* sin( m * GetAngle( nsrcBmpWidth, nsrcBmpHeight, x, y ) );//�鲿  

						R[n][m] = 0.0;  
					}  
				} 
				//��n m�� count Ϊ5,����ζ�ţ�n+1)/pi *
				*(Ze+n*count + m) = sqrt( (*(Ze+n*count + m))*(*(Ze+n*count + m)) + V[n][m]*V[n][m] )*(n+1)/PI_D/M[0];  
			}  
		}  
	}  
}  



void Zernike3Moment(uchar *psrcBmp ,int widthStep,int nsrcBmpWidth,int nsrcBmpHeight){

}