#include <opencv2/opencv.hpp>
#include <vector>
#include <math.h>

/*

1)��3x3��˹�˲��������˲�����������;

����2)���ÿһ�����أ���������������������΢�ֽ��ƣ��Եõ����ص��ݶȴ�С�ͷ���;

  ����3)���ݶȽ���"�Ǽ�������"(�Ǿֲ����ֵ��0);

	����4)���ݶ�ȡ������ֵ;

	  ����5)�Ա�Ե��������;
*/
void Canny(unsigned char *pUnchImage, int nWidth, int nHeight, double sigma,
	double dRatioLow, double dRatioHigh, unsigned char *pUnchEdge);

void GaussianSmooth(unsigned char *pUnchImg, int nWidth, int nHeight,
	double sigma, unsigned char * pUnchSmthdImg);

void DirGrad(unsigned char *pUnchSmthdImg, int nWidth, int nHeight,
	int *pnGradX , int *pnGradY);

void GradMagnitude(int *pnGradX, int *pnGradY, int nWidth, int nHeight, int *pnMag);

void NonmaxSuppress(int *pnMag, int *pnGradX, int *pnGradY, int nWidth,
	int nHeight, unsigned char *pUnchRst);

void Hysteresis(int *pnMag, int nWidth, int nHeight, double dRatioLow,
	double dRatioHigh, unsigned char *pUnchEdge);

void EstimateThreshold(int *pnMag, int nWidth, int nHeight, int *pnThdHigh,int *pnThdLow,
	unsigned char * pUnchEdge, double dRatioHigh, double dRationLow);

void MakeGauss(double sigma, double **pdKernel, int *pnWindowSize);

void TraceEdge (int y, int x, int nLowThd, unsigned char *pUnchEdge, int *pnMag, int nWidth);

void TraceEdge2(int y, int x, int nLowThd, unsigned char *pUnchEdge, int *pnMag, int nWidth);

cv::Mat clArrayToCvImage(uchar* output, int resultWidth, int resultHeight);