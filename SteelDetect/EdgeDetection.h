#pragma once
#include <opencv2/opencv.hpp>
#include <math.h>
using namespace cv;
//#define MINNUM		1e-8			//��СС��
//#define MAXNUM		1e+8			//�������
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
	void DWT();//С���任
	void IDWT();//С����任
	void WaveletSmooth();//С��ƽ��
	void Sobel();//sobel��Ե���
	void Sobel5x5();
	//SubWindows Segment1(IplImage *srcpImage);//ȱ�ݷָ�
	//SubWindows Segment2(IplImage *srcpImage);//ȱ�ݷָ�
	void getEdge(CvSeq *seq);//��ȡ��Ե������

public:
	IplImage *OriImage;//����ͨ��ת��������ԭʼ�Ҷ�ͼ��
	IplImage* pImage;//�����㷨�����н��в����ĻҶ�ͼ���м�ͼ��

	double	*m_pDbImage,*m_pDbImageR,*m_pDbImageG,*m_pDbImageB;	//���ڴ��С���任���м���
	int m_nSupp; //��ʾѡ���С����֧���ĳ���
	int m_nDWTCurDepth;	 //��ǰ����
	int Step; //С���任�Ĳ���

	//��ǩ����
	int countofflag;
	//��ǩͼ
	int **gflag;
	//λ�ó�ʼ�к�����
	int *m_rStart;
	//λ�ý����к�����
	int *m_rEnd;
	//λ�ó�ʼ�к�����
	int *m_cStart;
	//λ�ý����к�����
	int *m_cEnd;

	float probability[256];	//ֱ��ͼ

	//SubWindows prunedsubwindows;//ȱ������λ֮���ȱ�ݴ���

	//SubWindows addwindows;//ͨ������������õ��ĺ�ѡ����
public:
	//Nobuyuki Ostu����Ļ��ھص���ֵѡȡ
	int moment_shresh_sel1(float *prob);
	int moment_shresh_sel2(float *prob);
	//����غͷ�
	int MESM(float p[256]);

	bool DIBDWTStep(IplImage *pImage,int Step,int nInv);

	//Ӳ��ֵ�˲�
	void PassHard();
	//����ֵ�˲�
	void PassSoft();
	//�����㷨
	void PassShronk();
	//ȡ����
	int Sgn(double d);

	//���ٱ�ǩ�㷨
	void SetLabels();	
	//ȱ������λ
	void location(IplImage *srcpImage);

	//�ϲ�����
	//void PruneSubWindows(SubWindows const & good_windows,double dis_threshold,SubWindows &window_clusters);
	//�����������ڵľ���
	//double get_disoftworect(RectInfo const & w1, RectInfo const & w2);

	//void printWindows(IplImage* pImage,SubWindows const & window_clusters);

	//��ɫ
	void Fanse(IplImage *pImage);

	void mat2gray(IplImage* pImage);

	//ͨ������������ȱ�ݺ�ѡ����
	//void Regionalanalysis(SubWindows &windows,int width,int height);
	void maskFilter(cv::Mat &pImage);
};
