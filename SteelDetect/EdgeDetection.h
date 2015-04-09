#pragma once
#include <opencv2/opencv.hpp>
#include <math.h>
using namespace cv;
//#define MINNUM		1e-8			//最小小数
//#define MAXNUM		1e+8			//最大整数
//struct RectInfo
//{
//	int x;
//	int y;
//	int width;
//	int height;
//	bool passed;
//};
//typedef std::vector<RectInfo> SubWindows;

class EdgeDetection
{
public:
	EdgeDetection(void);
	~EdgeDetection(void);

public:
	void DWT();//小波变换
	void IDWT();//小波逆变换
	void WaveletSmooth();//小波平滑
	void Sobel();//sobel边缘检测
	void Sobel5x5();
	//SubWindows Segment1(IplImage *srcpImage);//缺陷分割
	//SubWindows Segment2(IplImage *srcpImage);//缺陷分割
	void getEdge(CvSeq *seq);//获取边缘点序列

public:
	IplImage *OriImage;//从三通道转换过来的原始灰度图像
	IplImage* pImage;//整个算法过程中进行操作的灰度图像（中间图像）

	double	*m_pDbImage,*m_pDbImageR,*m_pDbImageG,*m_pDbImageB;	//用于存放小波变换的中间结果
	int m_nSupp; //表示选择的小波紧支集的长度
	int m_nDWTCurDepth;	 //当前层数
	int Step; //小波变换的层数

	//标签总数
	int countofflag;
	//标签图
	int **gflag;
	//位置初始行号数组
	int *m_rStart;
	//位置结束行号数组
	int *m_rEnd;
	//位置初始列号数组
	int *m_cStart;
	//位置结束列号数组
	int *m_cEnd;

	float probability[256];	//直方图

	//SubWindows prunedsubwindows;//缺陷区域定位之后的缺陷窗口

	//SubWindows addwindows;//通过区域分析法得到的候选窗口
public:
	//Nobuyuki Ostu提出的基于矩的阈值选取
	int moment_shresh_sel1(float *prob);
	int moment_shresh_sel2(float *prob);
	//最大熵和法
	int MESM(float p[256]);

	bool DIBDWTStep(IplImage *pImage,int Step,int nInv);

	//硬阈值滤波
	void PassHard();
	//软阈值滤波
	void PassSoft();
	//收缩算法
	void PassShronk();
	//取符号
	int Sgn(double d);

	//快速标签算法
	void SetLabels();	
	//缺陷区域定位
	void location(IplImage *srcpImage);

	//合并窗口
	//void PruneSubWindows(SubWindows const & good_windows,double dis_threshold,SubWindows &window_clusters);
	//计算两个窗口的距离
	//double get_disoftworect(RectInfo const & w1, RectInfo const & w2);

	//void printWindows(IplImage* pImage,SubWindows const & window_clusters);

	//反色
	void Fanse(IplImage *pImage);

	void mat2gray(IplImage* pImage);

	//通过区域分析获得缺陷候选窗口
	//void Regionalanalysis(SubWindows &windows,int width,int height);
	void maskFilter(cv::Mat &pImage);
};
