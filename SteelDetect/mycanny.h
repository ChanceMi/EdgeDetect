#include <opencv2/opencv.hpp>
#include <vector>

float gauss(float x, float sigma);
float dGauss (float x, float sigma);
float meanGauss (float x, float sigma);


typedef cv::Mat IMAGE;
typedef std::vector<std::vector<float>> f2D;
//�������
void seperable_convolution (IMAGE& im, float *gau, int width,
	f2D& smx, f2D& smy);
void dxy_seperable_convolution (f2D& im, int nr, int nc, float *gau,
	int width, f2D& sm, int which);


//�����ֵ����
void nonmax_suppress (f2D& dx, f2D& dy, int nr, int nc,
	IMAGE& mag, IMAGE& ori);

void canny (float s, IMAGE& im, IMAGE& mag, IMAGE& ori);


//Ѱ�ұ߽����
int range (IMAGE& im, int i, int j);
int trace (int i, int j, int low, IMAGE& im,IMAGE& mag, IMAGE& ori);
//ͳ��ֱ��ͼ���ж���ֵ  �ͺ���ֵ��Hysteresis thresholding��[3] ��Ҫ������ֵ��������ֵ�����ֵ
void estimate_thresh (IMAGE& mag, int *low, int *hi);
void hysteresis (int high, int low, IMAGE& im, IMAGE& mag, IMAGE& oriim);





