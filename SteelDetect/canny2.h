#include <opencv2/opencv.hpp>
#include <vector>
#include <math.h>

/*

1)用3x3高斯滤波器进行滤波，消除噪声;

　　2)针对每一个像素，计算横向与纵向两方向的微分近似，以得到像素的梯度大小和方向;

  　　3)对梯度进行"非极大抑制"(非局部最大值置0);

	　　4)对梯度取两次阈值;

	  　　5)对边缘进行连接;
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