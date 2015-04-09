#include "EdgeDetection.h"
#include "GlobalApi.h"
//#include <opencv2/opencv.hpp>
#include <math.h>
const int MINNUM = 255;

EdgeDetection::EdgeDetection(void)
{
	// TODO: 在此处添加构造代码
	//m_regiontype = ELLIPSE_REGION_TYPE;
	//m_drawflag = false;
	//m_befirstmoveflag = true;
	//m_beplayflag = false;
	//wavelet=new double[1024*1024];

	// 为小波变换设置的参数
	// 临时存放小波变换系数内存
	m_pDbImage = NULL;
	m_pDbImageR = NULL;
	m_pDbImageG = NULL;
	m_pDbImageB = NULL;
	
	// 设置当前层数
	m_nDWTCurDepth = 0;

	// 设置小波基紧支集长度
	m_nSupp = 1;

	//设置小波变换的默认层数
	Step = 1; 

	OriImage=NULL;
	pImage=NULL;
}


EdgeDetection::~EdgeDetection(void)
{
	//delete []wavelet;
}


bool EdgeDetection::DIBDWTStep(IplImage *pImage,int Step,int nInv)
{
	// 循环变量
	int i, j;

	// 获取图象的长度和宽度
	int nWidth	= pImage->width;
	int nHeight = pImage->height;
	int widthStep=pImage->widthStep;

	// 获取变换的最大层数
	int nMaxWLevel = Log2(nWidth);
	int nMaxHLevel = Log2(nHeight);
	int nMaxLevel;

	if (nWidth == 1<<nMaxWLevel && nHeight == 1<<nMaxHLevel)
		nMaxLevel = MIN(nMaxWLevel, nMaxHLevel);
	// 临时变量
	double	*pDbTemp;
	uchar	*pBits;

	// 如果小波变换的存储内存还没有分配，则分配此内存
	if(!m_pDbImage){			
		m_pDbImage = new double[nWidth*nHeight];
		if (!m_pDbImage)	return false;

		// 将图象数据放入m_pDbImage中 
		for (j=0; j<nHeight; j++)
		{		
			for (i=0; i<nWidth; i++)
			{
				double temp = (pImage->imageData + pImage->widthStep*j)[i];
				if (temp < 0) temp += 256;
				m_pDbImage[j*nWidth+i]=temp;
			}		
		}
	}

	// 进行小波变换（或反变换）
	for(Step;Step > 0;Step --)
	{
		if (!DWTStep_2D(m_pDbImage, nMaxWLevel-m_nDWTCurDepth, nMaxHLevel-m_nDWTCurDepth,
							nMaxWLevel, nMaxHLevel, nInv, 1, m_nSupp))
			return false;

		// 如果是反变换，则当前层数减1
		if (nInv)
			m_nDWTCurDepth --;
		// 否则加1
		else
			m_nDWTCurDepth ++;
	}

	

	// 然后，将数据拷贝回原图中，并进行相应的数据转换
	int lfw = nWidth>>m_nDWTCurDepth, lfh = nHeight>>m_nDWTCurDepth;
	for (j=0; j<nHeight; j++)
	{		
		for (i=0; i<nWidth; i++)
		{
			//(pImage->imageData + pImage->widthStep*j)[i]=m_pDbImage[j*nWidth+i];
			if (j<lfh && i<lfw)
			{
				/*if(!nInv) //小波增强
					m_pDbImage[j*nWidth+i]*=1.2;*/
				(pImage->imageData + pImage->widthStep*j)[i] = FloatToChar(m_pDbImage[j*nWidth+i]);
			}
			else
			{
				/*if(!nInv) //小波增强
					m_pDbImage[j*nWidth+i]*=0.8;*/
				(pImage->imageData + pImage->widthStep*j)[i] = FloatToChar(m_pDbImage[j*nWidth+i]) ^ 0x80;	
			}
		}		
	}

	if(nInv)
	{
		delete	m_pDbImage;
		m_pDbImage = NULL;
	}
	// 返回
	return true;

}


//硬阈值滤波
void  EdgeDetection::PassHard()
{
	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//IplImage *pImage = pDoc->GetImage();

	// 图象的高度和宽度	
	int nWidth	= pImage->width;
	int nHeight = pImage->height;
	int widthStep=pImage->widthStep;

	//小波类型及变换层数
	Step=3;
	m_nSupp=2;
	//阈值
	double Q=0,t=0;
	// 循环变量
	int i, j,k,p;
	//临时变量
	double	*pDbTemp;

	int	W = nWidth >> Step;
	int H = nHeight >> Step;
	int N = nWidth * nHeight;			//图像象素数

	// 获取变换的最大层数
	int nMaxWLevel = Log2(nWidth);
	int nMaxHLevel = Log2(nHeight);
	int nMaxLevel;

	if (nWidth == 1<<nMaxWLevel && nHeight == 1<<nMaxHLevel)
		nMaxLevel = min(nMaxWLevel, nMaxHLevel);

	// 进行小波变换
	int rsl;
	rsl = DIBDWTStep(pImage,1,0);
	
	for(i = nHeight / 2;i < nHeight;i ++)
	{
	//	pDbTemp = m_pDbImage + i*sizeImageSave.cx;						
		for(j = nWidth / 2;j < nWidth;j ++)
			Q += fabs(m_pDbImage[i]);
	}
	Q = Q / (N / 4);
	t = 3 * Q;

	rsl = DIBDWTStep(pImage,Step - 1,0);

	for(k = 0 ;k < Step; k ++)
	{
		if(H < nHeight)
		{
			//域值去除高高频和低高频、高低频信息
				
			for(p = 0;p < H; p ++)
			{
				pDbTemp = m_pDbImage + p*nWidth;						
					for (i = W; i < W * 2 ; i ++)
						if(fabs(pDbTemp[i]) < t)
							pDbTemp[i] = 0;
						
			}
				

				
			for(p = H;p < H * 2; p ++)
			{
				pDbTemp = m_pDbImage + p*nWidth;
				for (i = 0; i < W ; i ++)
					if(fabs(pDbTemp[i]) < t)
						pDbTemp[i] = 0;
											
			}
				
				
			for(p = H;p < H * 2; p ++)
			{
				pDbTemp = m_pDbImage + p*nWidth;
				for(i = W; i < W * 2 ; i ++)
					if(fabs(pDbTemp[i]) < t)
						pDbTemp[i] = 0;
			}
				

			H = H * 2;
			W = W * 2;
			
		}
	}
	// 进行小波反变换
	for(Step;Step >0;Step --)
	{
		if (!DWTStep_2D(m_pDbImage, nMaxWLevel-m_nDWTCurDepth, nMaxHLevel-m_nDWTCurDepth,
						nMaxWLevel, nMaxHLevel, 1, 1, m_nSupp))
			return;

		// 是反变换，则当前层数减1
		m_nDWTCurDepth --;
	}

	// 然后，将数据拷贝回原CDib中，并进行相应的数据转换
	int lfw = nWidth>>m_nDWTCurDepth, lfh = nHeight>>m_nDWTCurDepth;
	for (j=0; j<nHeight; j++)
	{
		pDbTemp = m_pDbImage + j*nWidth;
		//pBits = pDib->GetpBits() + (nHeight-1-j)*sizeImageSave.cx;
		for (i=0; i<nWidth; i++)
		{
			//(pImage->imageData + pImage->widthStep*j)[i]=m_pDbImage[j*nWidth+i];
			if (j<lfh && i<lfw)
				(pImage->imageData + pImage->widthStep*j)[i] = FloatToChar(m_pDbImage[j*nWidth+i]);
			else
				(pImage->imageData + pImage->widthStep*j)[i] = FloatToChar(m_pDbImage[j*nWidth+i]) ^ 0x80;					
		}
	}

	delete m_pDbImage;
	m_pDbImage = NULL;

	// 如果小波变换不成功，则直接返回
	if (!rsl)			
		return;

	//GetDocument()->SetModifiedFlag(TRUE);	
	//// 更新视图
	//GetDocument()->UpdateAllViews(NULL);

}


//软阈值滤波
void  EdgeDetection::PassSoft()
{
	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//IplImage *pImage = pDoc->GetImage();

	// 图象的高度和宽度	
	int nWidth	= pImage->width;
	int nHeight = pImage->height;
	int widthStep=pImage->widthStep;

	//小波类型及变换层数
	Step=2;
	m_nSupp=2;
	//阈值
	double Q=0,t=0;
	// 循环变量
	int i, j,k,p;
	//临时变量
	double	*pDbTemp;

	int	W = nWidth >> Step;
	int H = nHeight >> Step;
	int N = nWidth * nHeight;			//图像象素数

	// 获取变换的最大层数
	int nMaxWLevel = Log2(nWidth);
	int nMaxHLevel = Log2(nHeight);
	int nMaxLevel;

	if (nWidth == 1<<nMaxWLevel && nHeight == 1<<nMaxHLevel)
		nMaxLevel = MIN(nMaxWLevel, nMaxHLevel);

	// 进行小波变换
	int rsl;
	rsl = DIBDWTStep(pImage,1,0);
	
	for(i = nHeight / 2;i < nHeight;i ++)
	{
	//	pDbTemp = m_pDbImage + i*sizeImageSave.cx;						
		for(j = nWidth / 2;j < nWidth;j ++)
			Q += fabs(m_pDbImage[i]);
	}
	Q = Q / (N / 4);
	t = 3 * Q;

	rsl = DIBDWTStep(pImage,Step - 1,0);

	for(k = 0 ;k < Step; k ++)
	{
		if(H < nHeight)
		{
			//域值去除高高频和低高频、高低频信息
				
			for(p = 0;p < H; p ++)
			{
				pDbTemp = m_pDbImage + p*nWidth;						
					for (i = W; i < W * 2 ; i ++)
					{
						if(fabs(pDbTemp[i]) < t)
							pDbTemp[i] = 0;
						else
							pDbTemp[i] = Sgn(pDbTemp[i]) * (fabs(pDbTemp[i]) - t);
					}
						
			}
					
			for(p = H;p < H * 2; p ++)
			{
				pDbTemp = m_pDbImage + p*nWidth;
				for (i = 0; i < W ; i ++)
				{
					if(fabs(pDbTemp[i]) < t)
						pDbTemp[i] = 0;
					else
						pDbTemp[i] = Sgn(pDbTemp[i]) * (fabs(pDbTemp[i]) - t);
				}
											
			}
				
				
			for(p = H;p < H * 2; p ++)
			{
				pDbTemp = m_pDbImage + p*nWidth;
				for(i = W; i < W * 2 ; i ++)
				{
					if(fabs(pDbTemp[i]) < t)
						pDbTemp[i] = 0;
					else
						pDbTemp[i] = Sgn(pDbTemp[i]) * (fabs(pDbTemp[i]) - t);
				}
			}
				

			H = H * 2;
			W = W * 2;
			
		}
	}
	// 进行小波反变换
	for(Step;Step >0;Step --)
	{
		if (!DWTStep_2D(m_pDbImage, nMaxWLevel-m_nDWTCurDepth, nMaxHLevel-m_nDWTCurDepth,
						nMaxWLevel, nMaxHLevel, 1, 1, m_nSupp))
			return;

		// 是反变换，则当前层数减1
		m_nDWTCurDepth --;
	}

	// 然后，将数据拷贝回原CDib中，并进行相应的数据转换
	int lfw = nWidth>>m_nDWTCurDepth, lfh = nHeight>>m_nDWTCurDepth;
	for (j=0; j<nHeight; j++)
	{
		pDbTemp = m_pDbImage + j*nWidth;
		//pBits = pDib->GetpBits() + (nHeight-1-j)*sizeImageSave.cx;
		for (i=0; i<nWidth; i++)
		{
			//(pImage->imageData + pImage->widthStep*j)[i]=m_pDbImage[j*nWidth+i];
			if (j<lfh && i<lfw)
				(pImage->imageData + pImage->widthStep*j)[i] = FloatToChar(m_pDbImage[j*nWidth+i]);
			else
				(pImage->imageData + pImage->widthStep*j)[i] = FloatToChar(m_pDbImage[j*nWidth+i]) ^ 0x80;					
		}
	}

	delete m_pDbImage;
	m_pDbImage = NULL;

	// 如果小波变换不成功，则直接返回
	if (!rsl)			
		return;

}


//收缩算法
void  EdgeDetection::PassShronk()
{
	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//IplImage *pImage = pDoc->GetImage();

	// 图象的高度和宽度	
	int nWidth	= pImage->width;
	int nHeight = pImage->height;
	int widthStep=pImage->widthStep;

	//小波类型及变换层数
	Step=3;
	m_nSupp=2;
	//阈值
	double Q=0,t=0;
	// 循环变量
	int i, j,k,p;
	//临时变量
	double	*pDbTemp;

	int	W = nWidth >> Step;
	int H = nHeight >> Step;
	int N = nWidth * nHeight;			//图像象素数

	// 获取变换的最大层数
	int nMaxWLevel = Log2(nWidth);
	int nMaxHLevel = Log2(nHeight);
	int nMaxLevel;

	if (nWidth == 1<<nMaxWLevel && nHeight == 1<<nMaxHLevel)
		nMaxLevel = MIN(nMaxWLevel, nMaxHLevel);

	// 进行小波变换
	int rsl;
	rsl = DIBDWTStep(pImage,1,0);
	
	for(i = nHeight / 2;i < nHeight;i ++)
	{
	//	pDbTemp = m_pDbImage + i*sizeImageSave.cx;						
		for(j = nWidth / 2;j < nWidth;j ++)
			Q += fabs(m_pDbImage[i]);
	}
	Q = Q / (N / 4);
	t = 3 * Q;

	rsl = DIBDWTStep(pImage,Step - 1,0);

	for(k = 0 ;k < Step; k ++)
	{
		if(H < nHeight)
		{
			//域值去除高高频和低高频、高低频信息
				
			for(p = 0;p < H; p ++)
			{
				pDbTemp = m_pDbImage + p*nWidth;						
					for (i = W; i < W * 2 ; i ++)
					{
						if(pDbTemp[i] > t)
								pDbTemp[i] = pDbTemp[i] - (t / (1 << k));
							else
								if(pDbTemp[i] < (t * (-1)))
									pDbTemp[i] = pDbTemp[i] + (t / (1 << k));
								else
									pDbTemp[i] = 0;
					}
						
			}
				

				
			for(p = H;p < H * 2; p ++)
			{
				pDbTemp = m_pDbImage + p*nWidth;
				for (i = 0; i < W ; i ++)
				{
					if(pDbTemp[i] > t)
								pDbTemp[i] = pDbTemp[i] - (t / (1 << k));
							else
								if(pDbTemp[i] < (t * (-1)))
									pDbTemp[i] = pDbTemp[i] + (t / (1 << k));
								else
									pDbTemp[i] = 0;
				}
											
			}
				
				
			for(p = H;p < H * 2; p ++)
			{
				pDbTemp = m_pDbImage + p*nWidth;
				for(i = W; i < W * 2 ; i ++)
				{
					if(pDbTemp[i] > t)
								pDbTemp[i] = pDbTemp[i] - (t / (1 << k));
							else
								if(pDbTemp[i] < (t * (-1)))
									pDbTemp[i] = pDbTemp[i] + (t / (1 << k));
								else
									pDbTemp[i] = 0;
				}
			}
				

			H = H * 2;
			W = W * 2;
			
		}
	}
	// 进行小波反变换
	for(Step;Step >0;Step --)
	{
		if (!DWTStep_2D(m_pDbImage, nMaxWLevel-m_nDWTCurDepth, nMaxHLevel-m_nDWTCurDepth,
						nMaxWLevel, nMaxHLevel, 1, 1, m_nSupp))
			return;

		// 是反变换，则当前层数减1
		m_nDWTCurDepth --;
	}

	// 然后，将数据拷贝回原CDib中，并进行相应的数据转换
	int lfw = nWidth>>m_nDWTCurDepth, lfh = nHeight>>m_nDWTCurDepth;
	for (j=0; j<nHeight; j++)
	{
		pDbTemp = m_pDbImage + j*nWidth;
		//pBits = pDib->GetpBits() + (nHeight-1-j)*sizeImageSave.cx;
		for (i=0; i<nWidth; i++)
		{
			//(pImage->imageData + pImage->widthStep*j)[i]=m_pDbImage[j*nWidth+i];
			if (j<lfh && i<lfw)
				(pImage->imageData + pImage->widthStep*j)[i] = FloatToChar(m_pDbImage[j*nWidth+i]);
			else
				(pImage->imageData + pImage->widthStep*j)[i] = FloatToChar(m_pDbImage[j*nWidth+i]) ^ 0x80;					
		}
	}

	delete m_pDbImage;
	m_pDbImage = NULL;

	// 如果小波变换不成功，则直接返回
	if (!rsl)			
		return;
	//cv show

}

//取符号
int	EdgeDetection::Sgn(double d)
{
	if(d > 0)
		return 1;
	else if(d == 0)
		return 0;
		else 
			return -1;
		
}


//Nobuyuki Ostu提出的基于矩的阈值选取
int EdgeDetection::moment_shresh_sel1(float *prob)
{
	int i,j,ming,maxg;
	float buf,ut=0,s,t1[256],t2[256];
	
	memset(t1,0,256*sizeof(float));
	memset(t2,0,256*sizeof(float));
	
	for(i=0;i<256;i++)
	{
		if(prob[i]!=0)
		{
			ming=i;
			break;
		} 
	}
	for(i=255;i>=0;i--){
		
		if(prob[i]!=0)
		{
			maxg=i;
			break;
		}
	}
	
	for(i=ming;i<=maxg;i++)
		ut+=i*prob[i];
	
	t1[ming]=prob[ming];
	t2[ming]=ming*prob[ming];
	for(i=(ming+1);i<=maxg;i++)
	{
		t1[i]=t1[i-1]+prob[i];
		t2[i]=t2[i-1]+i*prob[i];
	}
	for(i=ming;i<=maxg;i++)
	{
		if(t1[i]!=0)
		{
			ming=i;
			break;
		}
	}
	for(i=maxg;i>=ming;i--)
	{
		if(t1[i]<1)
		{
			maxg=i;
			break;
		}
	}
	s=(-999999);
	for(i=ming;i<=maxg;i++)
	{
		if((t1[i]!=0)&&(t1[i]!=1))
		{
			buf=ut*t1[i]-t2[i];
			buf=(buf*buf)/(t1[i]*(1-t1[i]));
		}
		else
			buf=(-999999);
		if(s<buf)
		{
			s=buf;
			j=i;
		}
	}		
	return j;
}


//最大熵和法
int EdgeDetection::MESM(float p[256])
{
	int i,t,T;
	float p1,p2,h1,h,fei,tempfei;
	fei=-65535.0;	
	for(t=0;t<255;t++){	//算熵和，并求出最大的熵和		
		p1=p2=h1=h=0.0;
		for(i=0;i<=t;i++)
			p1+=p[i];
		p2=1-p1;
		for(i=0;i<=t;i++)
			if(p1>MINNUM)
				if(fabs(p[i]/p1)>MINNUM)
					h1+=-1*(p[i]/p1)*(float)(log(p[i]/p1));
		for(i=0;i<=t;i++)
			if(fabs(p[i])>MINNUM)
				h+=-1*p[i]*(float)(log(p[i]));
		if(p1>MINNUM&&p2>MINNUM)
			tempfei=(float)(log(p1*p2))+h/p1+(h1-h)/p2;
		else
			tempfei=-65535.0;
		if(tempfei>fei) {
			fei=tempfei;
			T=t;
		}
	}
	return T;
}


//S. S. Reddi提出的基于矩的阈值选取
int EdgeDetection::moment_shresh_sel2(float *prob)
{
	int i,meand,meanb,maxp,minp;
	float temp1,total1,temp2,total2;
	
	minp=0;
	while(prob[minp]==0)
		minp++;
	maxp=255;
	while(prob[maxp]==0)
		maxp--;
	temp1=temp2=total1=total2=0;
	for(i=minp;i<=maxp;i++)
		total2+=i*prob[i];
	i=minp-1;
	do
	{
		i++;
		temp1+=prob[i];
		total1+=i*prob[i];
		temp2=1-temp1;
		if(temp2<=0)
		{
			i=128;
			break;
		}
		meand=(int)(total1/temp1);
		meanb=(int)((total2-total1)/temp2);
	}while((fabs(double(meanb+meand-2*i))>=1)&&(i<maxp));
	return i;
}


//快速标签算法
void EdgeDetection::SetLabels()
{
	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//IplImage *pImage = pDoc->GetImage();
	// 图象的高度和宽度	
	int nWidth	= pImage->width;
	int nHeight = pImage->height;

	gflag=new int*[nHeight];
	for(int h=0;h<nHeight;h++)
		gflag[h]=new int [nWidth];

	//初始化
	int i,j;
	countofflag=0;
	for(i=0;i<pImage->height;i++)
	{
		for(j=0;j<pImage->width;j++)
		{
			gflag[i][j]=0;
		}
	}

	for(i=0;i<pImage->height;i++)
	{
		for(j=0;j<pImage->width;j++)
		{			
			//I(i,j)为1
			double temp=(pImage->imageData + pImage->widthStep*i)[j];
			if(temp<0)
			{
				temp+=256;
			}

			if(temp==255)
			{
				int F[4];
				F[0]=(i>0&&j>0)?gflag[i-1][j-1]:0;
				F[1]=(i>0)?gflag[i-1][j]:0;
				F[2]=(i>0&&j<pImage->width-1)?gflag[i-1][j+1]:0;
				F[3]=(j>0)?gflag[i][j-1]:0;

				if(F[0]==0&&F[1]==0&&F[2]==0&&F[3]==0)
				{
					countofflag=countofflag+1;
					gflag[i][j]=countofflag;
				}
				else
				{
					int t[4],ct=0;//t[]中存放非零标签，ct表示非零标签的数量
					for(int k=0;k<4;k++)
					{
						if(F[k]>0)
						{
							t[ct++]=F[k];
						}
					}
			
					int Lmax=t[0],Lmin=t[0];
					for(int k=0;k<ct;k++)
					{
						if(t[k]>Lmax) Lmax=t[k];
						if(t[k]<Lmin) Lmin=t[k];
					}

					if(Lmax==Lmin)//表示t[]中的标签值全部相同
					{
						gflag[i][j]=Lmax;
					}
					else//表示t[]中的两种标签值不同相同（不可能有三种以上）
					{
						countofflag--;
						gflag[i][j]=Lmin;

						//将g中标过的所有标签中值为Lmax的修正为Lmin，值大于Lmax的减1
						//reflag(i,j,Lmax,Lmin);
						int flag=0;	
						for(int k1=0;flag!=1&&k1<=i;k1++)
						{
							for(int k2=0;k2<pImage->width;k2++)
							{
								if(k1==i&&k2>j) 
								{
									flag=1;
									break;
								}
								else
								{
									if(gflag[k1][k2]==Lmax) gflag[k1][k2]=Lmin;
									if(gflag[k1][k2]>Lmax) gflag[k1][k2]-=1;
								}
							}
						}
					}
				}
			}
		}
	}
}

/*
//缺陷区域定位，并画出最终的检测窗口
void EdgeDetection::location(IplImage *srcpImage)
{
	if(countofflag==0)
		return ;

	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//IplImage *srcpImage = pDoc->GetImage();

	// 图象的高度和宽度	
	int nWidth	= srcpImage->width;
	int nHeight = srcpImage->height;
	int widthStep=srcpImage->widthStep;
	//动态申请表明位置的数组
	m_rStart=new int[countofflag];
	m_rEnd=new int[countofflag];
	m_cStart=new int[countofflag];
	m_cEnd=new int[countofflag];
	//初始化数组
	for(int i=0;i<countofflag;i++)
	{
		m_rStart[i]=nHeight;
		m_rEnd[i]=0;
		m_cStart[i]=nWidth;
		m_cEnd[i]=0;
	}

	for(int i=0;i<nHeight;i++)
	{
		for(int j=0;j<nWidth;j++)
		{
			if(gflag[i][j]!=0)
			{
				
				int k=gflag[i][j];
				//更新位置信息
				if(i<m_rStart[k-1])
					m_rStart[k-1]=i;
				if(i>m_rEnd[k-1])
					m_rEnd[k-1]=i;
				if(j<m_cStart[k-1])
					m_cStart[k-1]=j;
				if(j>m_cEnd[k-1])
					m_cEnd[k-1]=j;
			}
		}
	}

	SubWindows ResultRect;
	int count=0;
	for(int i=0;i<countofflag;i++)
	{
		int s1=m_cEnd[i]-m_cStart[i];
		int s2=m_rEnd[i]-m_rStart[i];
		//if(s1>1&&s2>1)
		//{
			RectInfo resultrect;
			resultrect.x=m_cStart[i];
			resultrect.width=s1;
			resultrect.y=m_rStart[i];
			resultrect.height=s2;
			resultrect.passed=true;
			ResultRect.push_back(resultrect);
			count++;
		//}
	}
	CvScalar col=cvScalar(255,255,0);
	
	
	Regionalanalysis(ResultRect,nWidth,nHeight);
	for(SubWindows::iterator it=addwindows.begin();it!=addwindows.end();it++)
	{
		ResultRect.push_back(*it);
	}
	PruneSubWindows(ResultRect,10,prunedsubwindows);
	//prunedsubwindows=ResultRect;

	
	for(SubWindows::iterator it=prunedsubwindows.begin();it!=prunedsubwindows.end();it++)
	{
//		cvRectangle(srcpImage,cvPoint((int)it->x,(int)it->y),cvPoint((int)it->x+it->width,(int)it->y+it->height),col,1);
	}
	

	//GetDocument()->SetModifiedFlag(TRUE);	
	//// 更新视图
	//GetDocument()->UpdateAllViews(NULL);

}


//合并窗口
void EdgeDetection::PruneSubWindows(SubWindows const & good_windows,double dis_threshold,SubWindows &window_clusters)
{
	 if (good_windows.empty())
        return;
	 SubWindows windows = good_windows;
	 int curMerWinNum=windows.size();//本次循环结束后的待合并窗口
	 int perMerWinNum=0;//上一次循环结束后的待合并窗口

	 while(curMerWinNum!=perMerWinNum)
	 {
		 perMerWinNum=curMerWinNum;
		 window_clusters.clear();
		 for(SubWindows::iterator it1=windows.begin();it1!=windows.end();it1++)
		 {
			 if(it1->passed)
			 {
				//找到代合并的窗口
				for(SubWindows::iterator it2=windows.begin();it2!=windows.end();it2++)
				{
					if(it2!=it1&&it2->passed)
					{
						double dis=get_disoftworect(*it1,*it2);
						if (dis<dis_threshold)//小于阈值,则合并两个窗口
						{
							it1->passed=false;
							it2->passed=false;
							RectInfo cluster;
							int x1=min(it1->x,it2->x);//合并后的最左
							int x2=max(it1->x+it1->width,it2->x+it2->width);//合并后的最右
							int y1=min(it1->y,it2->y);//合并后的最上
							int y2=max(it1->y+it1->height,it2->y+it2->height);//合并后的最下

							cluster.x=x1;
							cluster.y=y1;
							cluster.width=x2-x1;
							cluster.height=y2-y1;
							cluster.passed=true;
							window_clusters.push_back(cluster);
							break;
						}
					}
				}
				if(it1->passed)//找了一圈都没有找到能合并的,则这个就相当于是不能再合并的
				{	
					it1->passed=false;
					window_clusters.push_back(*it1);
				}
			 }
		 }
		 curMerWinNum=window_clusters.size();
		 for(int i=0;i<curMerWinNum;i++)
		{
			window_clusters[i].passed=true;
		}

		 windows.clear();
		 windows=window_clusters;
	 }
}


//计算两个窗口的距离
double EdgeDetection::get_disoftworect(RectInfo const & w1, RectInfo const & w2)
{
	int x = max(w1.x, w2.x)-min(w1.x + w1.width, w2.x + w2.width);
	if(x<0)
		x=0;
	int y=max(w1.y, w2.y)-min(w1.y + w1.height, w2.y + w2.height);
	if(y<0)
		y=0;
	double dis=sqrt(double(x*x+y*y));
	return dis;
}

*/

//反色
void EdgeDetection::Fanse(IplImage *pImage)
{
	// TODO: 在此添加命令处理程序代码
	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//IplImage *pImage = pDoc->GetImage();
	
	// 图象的高度和宽度	
	int nWidth	= pImage->width;
	int nHeight = pImage->height;

	for(int y=0; y<nHeight; ++y)
	{
		for(int x=0; x<nWidth; ++x)
		{
			double temp = (pImage->imageData + pImage->widthStep*y)[x];

			if (temp < 0) temp += 256;
			
			//(pImage->imageData + pImage->widthStep*y)[x]=256-temp;
			if(temp>128)
			{
				(pImage->imageData + pImage->widthStep*y)[x]=0;
			}
			else
			{
				(pImage->imageData + pImage->widthStep*y)[x]=255;
			}
		}
	}
	//GetDocument()->SetModifiedFlag(TRUE);
	//	
	//// 更新视图
	//GetDocument()->UpdateAllViews(NULL);
}


//小波变换
void EdgeDetection::DWT()
{
	// TODO: 在此添加命令处理程序代码
	
	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//CDWT  cDwt;

	//IplImage *pImage = pDoc->GetImage();

	//// 图象的高度和宽度	
	//int nWidth	= pImage->width;
	//int nHeight = pImage->height;
	//int widthStep=pImage->widthStep;

	////wavelet=new double[nWidth*nHeight];

	////for(int j=0;j<nHeight;j++)
	////{
	////	for(int i=0;i<nWidth;i++)
	////	{
	////		wavelet[j*nWidth+i]=(pImage->imageData + pImage->widthStep*j)[i];
	////	}
	////}


	////int i,j;
	////int n=1;
	////for(int k=1;k<=n;k++)
	////{
	////	

	////	double *temp1=new double[nWidth*nHeight];
	////	double *temp2=new double[nWidth*nHeight];

	////	int Width_H=nWidth/pow(2.0,(double)k);
	////	int Height_H=nHeight/pow(2.0,(double)k);

	////	//完成行变换
	////	for(j = 0; j < nHeight/pow(2.0,(double)(k-1)); j ++)            
	////	{
	////		for(i = 0; i < Width_H; i ++)           
	////		{
	////		/*	if(j==78&&i==23)
	////			{
	////				int test=0;
	////			}*/
	////			int w = i *2;
	////			double temp =wavelet[j*nWidth+w];
	////			if (temp < 0) temp += 256;
	////			temp1[j*nWidth+i] =temp;	//偶

	////			temp = wavelet[j*nWidth+w+1];
	////			if (temp < 0) temp += 256;
	////			temp1[j*nWidth+Width_H+i] = temp;	//奇
	////		}
	////	}
	////	//通过图像的差分，完成小波变换
	////	for(j= 0; j<nHeight/pow(2.0,(double)(k-1)); j++)
	////	{
	////		for(i=0; i<Width_H; i++)
	////		{
	////			double temp_low;
	////			double temp_high;
	////			temp_low=(temp1[j*nWidth+Width_H+i]+temp1[j*nWidth+i])/2;
	////			temp_high=(temp1[j*nWidth+i]-temp1[j*nWidth+Width_H+i] )/2+128;
	////			
	////			

	////			temp1[j*nWidth+i]=temp_low;
	////			temp1[j*nWidth+Width_H+i] =temp_high;	
	////		}
	////	}

	////	
	////	
	////	//完成列变换
	////	for(i = 0; i < nWidth/pow(2.0,(double)k-1); i ++)            
	////	{
	////		for(j =  0; j < Height_H; j ++)           
	////		{
	////			
	////			int h= j*2 ;
	////			temp2[j*nWidth+i] = temp1[h*nWidth+i];		//even
	////			temp2[(Height_H + j)*nWidth+i] = temp1[(h+1)*nWidth+i];	//odd
	////		}
	////	}
	////	//通过图像的差分，完成小波变换
	////	for(i=0; i<nWidth/pow(2.0,(double)(k-1)); i++)
	////	{
	////		for(j =  0; j <  Height_H; j ++) 
	////		{
	////			//temp2[ (j+Height_H)*nWidth+i] = temp2[j*nWidth+i]-temp2[(Height_H + j)*nWidth+i]+128;

	////			double temp_low;
	////			double temp_high;
	////			temp_low=(temp2[j*nWidth+i]+temp2[(Height_H + j)*nWidth+i])/2;
	////			temp_high=(temp2[j*nWidth+i]-temp2[(Height_H + j)*nWidth+i])/2+128;

	////			temp2[j*nWidth+i]=temp_low;
	////			temp2[(Height_H + j)*nWidth+i] =temp_high;	
	////		}
	////	}
	////	//小波经过处理后，放入显示缓存中
	////	for(j = 0; j <nHeight/pow(2.0,(double)(k-1)); j ++)    
	////	{
	////		for(i=0; i<nWidth/pow(2.0,(double)(k-1)); i++)
	////		{
	////		/*	if(j==78&&i==174)
	////			{
	////				int test0=0;
	////			}
	////			if(j==206&&i==174)
	////			{
	////				int test0=0;
	////			}*/
	////			
	////			if( temp2[j*nWidth+i]<0)
	////				 temp2[j*nWidth+i]+=256;
	////			//int test0=temp2[j*nWidth+i];
	////			(pImage->imageData + pImage->widthStep*j)[i]  = temp2[j*nWidth+i];
	////			wavelet[j*nWidth+i]=temp2[j*nWidth+i];
	////		}
	////	}
	////	//删除临时的数据空间
	////	delete temp1;
	////	delete temp2;
	////}
	//// 设置滤波器组
	//CString WaveletFilterName;	
	//WaveletFilterName =_T("haar");	
	//cDwt.SetFilters(WaveletFilterName);

	//// 小波分解的级数
	//int nstep = 2;

	//int rsl = cDwt.ImageDWT(LPBYTE(pImage->imageData), wavelet, 
	//	                    nWidth, nHeight,
	//	                    0, nstep,
	//						cDwt.ExtendMode);
	//// 如果小波变换不成功，则直接返回
	//if (!rsl)			
	//	return;

	//

	//GetDocument()->SetModifiedFlag(TRUE);
	//	
	//// 更新视图
	//GetDocument()->UpdateAllViews(NULL);


	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//IplImage *pImage = pDoc->GetImage();

	// 图象的高度和宽度	

    if(pImage==NULL)
	{   
		
		pImage = cvCreateImage(cvSize(OriImage->width,OriImage->height),8,1);
		cvCopy(OriImage, pImage, NULL);
	}
	int nWidth	= pImage->width;
	int nHeight = pImage->height;
	int widthStep=pImage->widthStep;

	//wavelet=new double[nWidth*nHeight];
	Step=2;
	m_nSupp=2;

	// 进行小波变换
	int rsl;
	//if(colorNum != 8)
	//	rsl = DIBDWTStepColor(pDib,step,0);
	//else
		rsl= DIBDWTStep(pImage,Step,0);

	// 恢复光标形状
	//EndWaitCursor();

	// 如果小波变换不成功，则直接返回
	if (!rsl)			
		return;

	namedWindow( "DWT", 1 );//创建窗口
	
    imshow("DWT", Mat(pImage) );//显示图像

}


//小波逆变换
void EdgeDetection::IDWT()
{
	// TODO: 在此添加命令处理程序代码
	//double aa;
	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//int i,j;
	//
	//IplImage *pImage = pDoc->GetImage();
	//int nWidth	= pImage->width;
	//int nHeight = pImage->height;
	//int widthStep=pImage->widthStep;


	//int n=1;
	//for(int k=n;k>=1;k--)
	//{
	//	

	//	double *temp1=new double[nWidth*nHeight];
	//	double *temp2=new double[nWidth*nHeight];

	//	int Width_H=nWidth/pow(2.0,(double)k);
	//	int Height_H=nHeight/pow(2.0,(double)k);

	//	for(i=0;i<nWidth/pow(2.0,(double)(k-1));i++)
	//	{
	//		for(j=0;j<Height_H;j++)
	//		{
	//			int h=2*j;

	//		/*	if(h==156&&i==46)
	//			{
	//				int test0=0;
	//			}
	//			if(h==156&&i==174)
	//			{
	//				int test0=0;
	//			}*/
	//							

	//			double temp_low=wavelet[j*nWidth+i];
	//			if(temp_low<0)
	//				temp_low+=256;
	//			double temp_high=wavelet[(j+Height_H)*nWidth+i];
	//			if(temp_high<0)
	//				temp_high+=256;

	//			temp1[h*nWidth+i]=temp_low+temp_high-128;
	//			temp1[(h+1)*nWidth+i]=temp_low-temp_high+128;
	//		}
	//	}

	//	for(j=0;j<nHeight/pow(2.0,(double)(k-1));j++)
	//	{
	//		for(i=0;i<Width_H;i++)
	//		{
	//			int w=2*i;
	//		
	//		
	//			temp2[j*nHeight+w]=temp1[j*nHeight+i]+temp1[j*nHeight+i+Width_H]-128;
	//			/*if((temp1[j*nHeight+i]-temp1[j*nHeight+i+Width_H]+128)<0)
	//			{
	//				double test1=temp1[j*nHeight+i];
	//				double test2=temp1[j*nHeight+i+Width_H];
	//				double test3=temp1[j*nHeight+i]-temp1[j*nHeight+i+Width_H]+128;

	//			}*/
	//			temp2[j*nHeight+w+1]=temp1[j*nHeight+i]-temp1[j*nHeight+i+Width_H]+128;
	//		
	//		}
	//	}
	//
	//	for(j=0;j<nHeight/pow(2.0,(double)(k-1));j++)
	//	{
	//		for(i=0;i<nWidth/pow(2.0,(double)(k-1));i++)
	//		{
	//			//aa=temp2[j*nHeight+i];
	//			if(temp2[j*nHeight+i]<0)
	//				temp2[j*nHeight+i]+=256;
	//			if(temp2[j*nHeight+i]>256)
	//				temp2[j*nHeight+i]=255;
	//			
	//			//aa=temp2[j*nHeight+i];
	//			(pImage->imageData + pImage->widthStep*j)[i]=temp2[j*nHeight+i];
	//			wavelet[j*nWidth+i]=temp2[j*nHeight+i];
	//		}
	//	}
	//}
	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//CDWT  cDwt;

	////获取图像指针
	//IplImage *pImage = pDoc->GetImage();
	//// 图象的高度和宽度	
	//int nWidth	= pImage->width;
	//int nHeight = pImage->height;
	//int widthStep=pImage->widthStep;

	//// 设置滤波器组
	//CString WaveletFilterName;	
	//WaveletFilterName =_T("haar");	
	//cDwt.SetFilters(WaveletFilterName);

	//// 小波分解的级数
	//int nstep = 2;

	//// 小波合成
	//int rsl = cDwt.ImageIDWT(wavelet, LPBYTE(pImage->imageData),
	//	                     nWidth, nHeight,
	//	                     2, nstep,
	//						 cDwt.ExtendMode);

	//// 如果小波变换不成功，则直接返回
	//if (!rsl)			
	//	return;

	//GetDocument()->SetModifiedFlag(TRUE);
	//	
	//// 更新视图
	//GetDocument()->UpdateAllViews(NULL);

	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//IplImage *pImage = pDoc->GetImage();
    if(pImage==NULL)
	{
		return;
	}

	// 图象的高度和宽度	
	int nWidth	= pImage->width;
	int nHeight = pImage->height;
	int widthStep=pImage->widthStep;

	// 进行小波变换
	int rsl;
	//if(colorNum != 8)
	//	rsl = DIBDWTStepColor(pDib,step,0);
	//else
		rsl= DIBDWTStep(pImage,Step,1);

	// 恢复光标形状
	//EndWaitCursor();

	// 如果小波变换不成功，则直接返回
	if (!rsl)			
		return;

	cvNamedWindow( "IDWT", 1 );//创建窗口
    cvShowImage( "IDWT", pImage );//显示图像
}


//图像增强
void EdgeDetection::mat2gray(IplImage* pImage)
{
	int y,x;
	double max=0;
	double min=255;
	int nHeight=pImage->height;
	int nWidth=pImage->width;
	//求出最大最小值
	for(y=0; y<nHeight; ++y)
	{
		for(x=0; x<nWidth; ++x)
		{
			double Temp=(pImage->imageData + pImage->widthStep*y)[x];
			if(Temp<0)
				Temp+=256;
			if(Temp<min)
				min=Temp;
			if(Temp>max)
				max=Temp;
		}
	}
	//变换到0~255区间
	for(y=0; y<nHeight; ++y)
	{
		for(x=0; x<nWidth; ++x)
		{
			double Temp=(pImage->imageData + pImage->widthStep*y)[x];
			if(Temp<0)
				Temp+=256;

			(pImage->imageData + pImage->widthStep*y)[x]=255*(Temp-min)/(max-min);
		}
	}

//	cvNamedWindow( "mat2gray", 1 );//创建窗口
//    cvShowImage( "mat2gray", pImage );//显示图像
}


//小波平滑
void EdgeDetection::WaveletSmooth()
{
	//获取图像指针
	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//IplImage *srcImage = pDoc->GetImage();
	//pImage = cvCreateImage(cvSize(srcImage->width,srcImage->height),8,1);
	//cvCvtColor(srcImage,pImage,CV_RGB2GRAY);
	if(pImage==NULL)
	{
		pImage = cvCreateImage(cvSize(OriImage->width,OriImage->height),8,1);
		cvCopy(OriImage, pImage, NULL);
	}

	//图像增强
	//mat2gray(pImage);

	int mode=1;
	switch(mode)
	{
	case 0:
	//硬阈值滤波
		PassHard();
		break;
	//软阈值滤波
	case 1:
		PassSoft();
		break;
	//收缩算法
	case 2:
		PassShronk();
		break;
	default:
		break;
	}

//	cvNamedWindow( "WaveletSmooth", 1 );//创建窗口
//    cvShowImage( "WaveletSmooth", pImage );//显示图像
}

//sobel边缘检测
void EdgeDetection::Sobel5x5()
{
	if(pImage==NULL)
	{
		pImage = cvCreateImage(cvSize(OriImage->width,OriImage->height),8,1);
		cvCopy(OriImage, pImage, NULL);
	}

	// 图象的高度和宽度	
	int nWidth	= pImage->width;
	int nHeight = pImage->height;
	int widthStep=pImage->widthStep;

	//创建一个和原图像大小相同的图像
	IplImage *NewImage = cvCreateImage(cvSize(nWidth,nHeight),8,1);
	cvCopy(pImage, NewImage, NULL);

	OriImage=cvCreateImage(cvSize(nWidth,nHeight),8,1);
	cvCopy(pImage, OriImage, NULL);

	int d,max;
	//static int s[8][9]={
	//	{-1,-2,-1,0,0,0,1,2,1},
	//	{0,-1,-2,1,0,-1,2,1,0},
	//	{1,0,-1,2,0,-2,1,0,-1},
	//	{2,1,0,1,0,-1,0,-1,-2},
	//	{1,2,1,0,0,0,-1,-2,-1},
	//	{0,1,2,-1,0,1,-2,-1,0},
	//	{-1,0,1,-2,0,2,-1,0,1},
	//	{-2,-1,0,-1,0,1,0,1,2}
	//};
	static int sobel5x5[4][25]={  //sobel算子模板5x5		
		{2,3,0,-3,-2,//x水平方向
		3,4,0,-4,-3,
		6,6,0,-6,-6,
		3,4,0,-4,-3,
		2,3,0,-3,-2
		},
		{2,3,6,3,2,//y垂直方向
		3,4,6,4,3,
		0,0,0,0,0,
		-3,-4,-6,-4,-3,
		-2,-3,-6,-3,-2
		},
		{0,-2,-3,-2,-6,//45度方向
		2,0,-4,-6,-2,
		3,4,0,-4,-3,
		2,6,4,0,-2,
		6,2,3,2,0
		},
		{-6,-2,-3,-2,0,//135度方向
		-2,-6,-4,0,2,
		-3,-4,0,4,2,
		-2,0,4,6,2,
		0,2,3,2,6
		}
	};
	double a=0.9;//边缘的控制因子a=0.7~0.9

	//边缘检测
	for(int j=2;j<nHeight-2;j++)
	{
		for(int i=2;i<nWidth-2;i++)
		{
 			max=0;
			for(int k=0;k<4;k++)//4个方向
			{
				d=0;
				for(int y1=0;y1<5;y1++)//5*5邻域内卷积
				{
					for(int x1=0;x1<5;x1++)
					{
						double temp=(pImage->imageData + pImage->widthStep*(j+y1-2))[i+x1-2];
						if(temp<0)
							temp+=256;
						d+=sobel5x5[k][x1+y1*5]*temp;//srcimg[x+x1-1+(y+y1-1)*m_dwLine];
					}
				}
				if (d>max) max=d;
			}
			if (max>255) max=255;
			(NewImage->imageData+NewImage->width*j)[i]=max;
		}
	}	
	
	//初始化
	int **maxa=new int*[nHeight-4];
	for(int i=0;i<nHeight-4;i++)
	{
		maxa[i]=new int[nWidth-4];
	}

	//边缘局部细化：求局部细化的阈值
	for(int j=2;j<nHeight-2;j++)
	{
		for(int i=2;i<nWidth-2;i++)
		{
 			maxa[j-2][i-2]=0;
			for(int y1=0;y1<3;y1++) //3*3邻域内
			{
				for(int x1=0;x1<3;x1++)
				{
					if(x1!=0||y1!=0)
					{
						double temp=(NewImage->imageData + NewImage->widthStep*(j+y1-1))[i+x1-1];
						if(temp<0)
							temp+=256;
						if (temp>maxa[j-2][i-2]) maxa[j-2][i-2]=temp;
					}
				}			
			}
		}
	}	

	//边缘局部细化：梯度图细化
	for(int j=2;j<nHeight-2;j++)
	{
		for(int i=2;i<nWidth-2;i++)
		{		
			double t=(NewImage->imageData + NewImage->widthStep*j)[i];
			if(t<0)	t+=256;
			if(t<a*maxa[j-2][i-2])
			{
				(NewImage->imageData + NewImage->widthStep*j)[i]=0;
			}
			else
			{
				(NewImage->imageData + NewImage->widthStep*j)[i]=maxa[j-2][i-2];
			}



			// 为图像的(nHeight-4)*(nWidth-4)外的边界点赋值
			if(j==2)
			{
				for(int k=0;k<j;k++)
				{
					(NewImage->imageData + NewImage->widthStep*k)[i]=t;
					if (i==2)
					{
						(NewImage->imageData + NewImage->widthStep*k)[i-2]=t;
						(NewImage->imageData + NewImage->widthStep*k)[i-1]=t;
					}
					if (i==nWidth-1-2)
					{
						(NewImage->imageData + NewImage->widthStep*k)[i+1]=t;
						(NewImage->imageData + NewImage->widthStep*k)[i+2]=t;
					}
				}
			}
			else if(j==nHeight-1-2)
			{
				for(int k=j+1;k<nHeight;k++)
				{
					(NewImage->imageData + NewImage->widthStep*k)[i]=t;
					if (i==2)
					{
						(NewImage->imageData + NewImage->widthStep*k)[i-1]=t;
						(NewImage->imageData + NewImage->widthStep*k)[i-2]=t;
					}
					if (i==nWidth-1-2)
					{
						(NewImage->imageData + NewImage->widthStep*k)[i+1]=t;
						(NewImage->imageData + NewImage->widthStep*k)[i+2]=t;
					}
				}
			}

			if(i==2)
			{
				for(int k=0;k<i;k++)
				{
					(NewImage->imageData + NewImage->widthStep*j)[k]=t;
				}
			}
			else if(i==nWidth-1-2)
			{
				for(int k=i+1;k<nWidth;k++)
				{
					(NewImage->imageData + NewImage->widthStep*j)[k]=t;
				}
			}
		}
	}	

	
	cvCopy(NewImage, pImage, NULL);
		
//	cvNamedWindow( "Sobel", 1 );//创建窗口
//    cvShowImage( "Sobel", pImage );//显示图像
}


//sobel边缘检测
void EdgeDetection::Sobel()
{
	// TODO: 在此添加命令处理程序代码
	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	if(pImage==NULL)
	{
		pImage = cvCreateImage(cvSize(OriImage->width,OriImage->height),8,1);
		cvCopy(OriImage, pImage, NULL);
	}

	// 图象的高度和宽度	
	int nWidth	= pImage->width;
	int nHeight = pImage->height;
	int widthStep=pImage->widthStep;

	//创建一个和原图像大小相同的图像
	IplImage *NewImage = cvCreateImage(cvSize(nWidth,nHeight),8,1);
	cvCopy(pImage, NewImage, NULL);

	OriImage=cvCreateImage(cvSize(nWidth,nHeight),8,1);
	cvCopy(pImage, OriImage, NULL);

	int d,max;
	static int s[8][9]={
		{-1,-2,-1,0,0,0,1,2,1},
		{0,-1,-2,1,0,-1,2,1,0},
		{1,0,-1,2,0,-2,1,0,-1},
		{2,1,0,1,0,-1,0,-1,-2},
		{1,2,1,0,0,0,-1,-2,-1},
		{0,1,2,-1,0,1,-2,-1,0},
		{-1,0,1,-2,0,2,-1,0,1},
		{-2,-1,0,-1,0,1,0,1,2}
	};

	for(int j=1;j<nHeight-1;j++)
	{
		for(int i=1;i<nWidth-1;i++)
		{
			////s1=I(j+1,i-1)+2*I(j+1,i)+I(j+1,i+1)-(I(j-1,i-1)+2*I(j-1,i)+I(j-1,i+1))
			//double s1= abs((pImage->imageData + pImage->widthStep*(j+1))[i-1])+abs(2*(pImage->imageData + pImage->widthStep*(j+1))[i])+abs((pImage->imageData + pImage->widthStep*(j+1))[i+1])
			//			-(abs((pImage->imageData + pImage->widthStep*(j-1))[i-1])+abs(2*(pImage->imageData + pImage->widthStep*(j-1))[i])+abs((pImage->imageData + pImage->widthStep*(j-1))[i+1]));
			////s2=I(j-1,i+1)+2*I(j,i+1)+I(j+1,i+1)-(I(j-1,i-1)+2*I(j,i-1)+I(j+1,i-1))
			//double s2=abs((pImage->imageData + pImage->widthStep*(j-1))[i+1])+abs(2*(pImage->imageData + pImage->widthStep*(j))[i+1])+abs((pImage->imageData + pImage->widthStep*(j+1))[i+1])
			//			-(abs((pImage->imageData + pImage->widthStep*(j-1))[i-1])+abs(2*(pImage->imageData + pImage->widthStep*(j))[i-1])+abs((pImage->imageData + pImage->widthStep*(j+1))[i-1]));
			//int g=(int)sqrt(s1*s1+s2*s2);

			//if(g>255)
			//	(NewImage->imageData+NewImage->width*j)[i]=255;
			//else
			//	(NewImage->imageData+NewImage->width*j)[i]=g;
 			max=0;
			for(int k=0;k<8;k++)
			{
				d=0;
				for(int y1=0;y1<3;y1++)
					for(int x1=0;x1<3;x1++)
					{
						double temp=(pImage->imageData + pImage->widthStep*(j+y1-1))[i+x1-1];
						if(temp<0)
							temp+=256;
						d+=s[k][x1+y1*3]*temp;//srcimg[x+x1-1+(y+y1-1)*m_dwLine];
					}
					if (d>max) max=d;
			}
			if (max>255) max=255;
			(NewImage->imageData+NewImage->width*j)[i]=max;
			//// 为图像的(nHeight-2)*(nWidth-2)外的边界点赋值
			//if(j==1)
			//{
			//	(NewImage->imageData + NewImage->widthStep*(j-1))[i]=max;
			//	if (i==1)
			//	{
			//		(NewImage->imageData + NewImage->widthStep*(j-1))[i-1]=max;
			//	}
			//	else if (i==nWidth-1-1)
			//	{
			//		(NewImage->imageData + NewImage->widthStep*(j-1))[i+1]=max;
			//	}				
			//}
			//else if(j==nHeight-1-1)
			//{

			//		(NewImage->imageData + NewImage->widthStep*(j+1))[i]=max;
			//		if (i==1)
			//		{
			//			(NewImage->imageData + NewImage->widthStep*(j+1))[i-1]=max;
			//		}
			//		else if (i==nWidth-1-1)
			//		{
			//			(NewImage->imageData + NewImage->widthStep*(j+1))[i+1]=max;
			//		}
			//	
			//}

			//if(i==1)
			//{			
			//	(NewImage->imageData + NewImage->widthStep*j)[i-1]=max;				
			//}
			//else if(i==nWidth-1-1)
			//{
			//	(NewImage->imageData + NewImage->widthStep*j)[i+1]=max;				
			//}
		}
	}
	//for(int j=1;j<nHeight-1;j++)
	//{
	//	for(int i=1;i<nWidth-1;i++)
	//	{
    //		max=0;
	//		for(int k=0;k<8;k++)
	//		{
	//			d=0;
	//			for(int y1=0;y1<3;y1++)
	//				for(int x1=0;x1<3;x1++)
	//				{
	//					double temp=(NewImage->imageData + NewImage->widthStep*(j+y1-1))[i+x1-1];
	//					if(temp<0)
	//						temp+=256;
	//					d+=s[k][x1+y1*3]*temp;//srcimg[x+x1-1+(y+y1-1)*m_dwLine];
	//				}
	//				if (d>max) max=d;
	//		}
	//		if (max>255) max=255;
	//		(NewImage2->imageData+NewImage2->width*j)[i]=(BYTE)max;
	//	}
	//}
	//

	//for(int j=0;j<nHeight;j++)
	//{
	//	for(int i=0;i<nWidth;i++)
	//	{
	//		double s1=(NewImage2->imageData+NewImage2->width*j)[i];

	//		double s2=(NewImage->imageData+NewImage->width*j)[i];
	//		double s3=s2-s1;
	//		 //过限处理
	//           s3 = s3>255?255:s3;   
	//           s3 = s3<0?0:s3;
	//		
	//		(pImage->imageData+pImage->width*j)[i]=s3;
	//	}
	//}
	
	
	cvCopy(NewImage, pImage, NULL);
		
	//cvNamedWindow( "Sobel", 1 );//创建窗口
   // cvShowImage( "Sobel", pImage );//显示图像

	//GetDocument()->SetModifiedFlag(TRUE);
	//	
	//// 更新视图
	//GetDocument()->UpdateAllViews(NULL);
}

/*

//求出检测出来的所有边缘点在原图中对应像素的灰度均值T，将T作为分割阈值对原图像进行二值化，
//得到二值化结果，再快速标签并定位缺陷区域，进行缺陷分割
SubWindows EdgeDetection::Segment1(IplImage *srcpImage)
{
	// TODO: 在此添加命令处理程序代码
	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	
	//获取图像指针
	//IplImage *srcImage = pDoc->GetImage();
	pImage = cvCreateImage(cvSize(srcImage->width,srcImage->height),8,1);
	cvCvtColor(srcImage,pImage,CV_RGB2GRAY);
	// 图象的高度和宽度	
	int nWidth	= pImage->width;
	int nHeight = pImage->height;
	int widthStep=pImage->widthStep;
	double sum=0;
	int count=0;
	//计算直方图
	int histab[256];
	for(int i=0;i<256;i++)
		histab[i]=0;

	for(int j=1;j<nHeight-1;j++)
	{
		for(int i=1;i<nWidth-1;i++)
		{
			//if(i==1&&j==1)
			//{
			//	int a=0;
			//}
			double temp=(pImage->imageData+nWidth*j)[i];
			if(temp<0)
				temp+=256;

			histab[(int)temp]++;
		}
	}
	for(int i=0;i<256;i++)
			probability[i]=(float)(histab[i]/((nHeight-2)*(nWidth-2)*1.0));
	double edgethreadhold=moment_shresh_sel1(probability);//计算边缘二值化的分割阈值
	for(int j=0;j<nHeight;j++)//将sobel检测出来的边缘进行二值化，滤去值过小的边缘点
	{
		for(int i=0;i<nWidth;i++)
		{
			if(i<1||j<1||j==nHeight-1||i==nWidth-1)
			{
				(pImage->imageData+nWidth*j)[i]=0;
			}
			else
			{
				double temp=(pImage->imageData+nWidth*j)[i];
				if(temp<0)
					temp+=256;

				if(temp>edgethreadhold)
				{
					count++;
					double temp1=(OriImage->imageData+nWidth*j)[i];
					if(temp1<0)
						temp1+=256;
					sum+=temp1;
					(pImage->imageData+nWidth*j)[i]=255;
				}
				else
					(pImage->imageData+nWidth*j)[i]=0;
			}
		}
	}

//	cvNamedWindow( "Segment1_边缘图", 1 );//创建窗口
//    cvShowImage( "Segment1_边缘图", OriImage );//显示图像
	
	////创建一个和原图像大小相同的图像
	//IplImage *NewImage = cvCreateImage(cvSize(nWidth,nHeight),8,1);
	//cvCopy(pImage, NewImage, NULL);
	//int d,max;
	//static int s[8][9]={
	//	{-1,-2,-1,0,0,0,1,2,1},
	//	{0,-1,-2,1,0,-1,2,1,0},
	//	{1,0,-1,2,0,-2,1,0,-1},
	//	{2,1,0,1,0,-1,0,-1,-2},
	//	{1,2,1,0,0,0,-1,-2,-1},
	//	{0,1,2,-1,0,1,-2,-1,0},
	//	{-1,0,1,-2,0,2,-1,0,1},
	//	{-2,-1,0,-1,0,1,0,1,2}
	//};

	//for(int j=1;j<nHeight-1;j++)
	//{
	//	for(int i=1;i<nWidth-1;i++)
	//	{
 //			max=0;
	//		for(int k=0;k<8;k++)
	//		{
	//			d=0;
	//			for(int y1=0;y1<3;y1++)
	//				for(int x1=0;x1<3;x1++)
	//				{
	//					double temp=(pImage->imageData + pImage->widthStep*(j+y1-1))[i+x1-1];
	//					if(temp<0)
	//						temp+=256;
	//					d+=s[k][x1+y1*3]*temp;//srcimg[x+x1-1+(y+y1-1)*m_dwLine];
	//				}
	//				if (d>max) max=d;
	//		}
	//		if (max>255) max=255;
	//		(NewImage->imageData+NewImage->width*j)[i]=(BYTE)max;
	//	}
	//}

	//	for(int j=1;j<nHeight-1;j++)
	//{
	//	for(int i=1;i<nWidth-1;i++)
	//	{
	//		if(i==1&&j==1)
	//		{
	//			int a=0;
	//		}
	//		double temp=(NewImage->imageData+nWidth*j)[i];
	//		if(temp<0)
	//			temp+=256;

	//		histab[(int)temp]++;
	//	}
	//}
	//for(int i=0;i<256;i++)
	//		probability[i]=(float)(histab[i]/((nHeight-2)*(nWidth-2)*1.0));
	//edgethreadhold=moment_shresh_sel1(probability);
	//for(int j=0;j<nHeight;j++)
	//{
	//	for(int i=0;i<nWidth;i++)
	//	{
	//		if(i<1||j<1||j==nHeight-1||i==nWidth-1)
	//		{
	//			(NewImage->imageData+nWidth*j)[i]=0;
	//		}
	//		else
	//		{
	//			double temp=(NewImage->imageData+nWidth*j)[i];
	//			if(temp<0)
	//				temp+=256;

	//			if(temp>edgethreadhold)
	//			{
	//				/*count++;
	//				double temp1=(OriImage->imageData+nWidth*j)[i];
	//				if(temp1<0)
	//					temp1+=256;
	//				sum+=temp1;*/
	//				(NewImage->imageData+nWidth*j)[i]=255;
	//			}
	//			else
	//				(NewImage->imageData+nWidth*j)[i]=0;
	//		}
	//	}
	//}


	//for(int j=0;j<nHeight;j++)
	//{
	//	for(int i=0;i<nWidth;i++)
	//	{
	//		double s1=(pImage->imageData+nWidth*j)[i];
	//		if(s1<0)
	//			s1+=256;
	//		double s2=(NewImage->imageData+nWidth*j)[i];
	//			if(s2<0)
	//				s2+=256;
	//		double v=s1-s2;
	//		if(v<0)
	//			v=0;
	//		else
	//		{
	//			count++;
	//			double temp1=(OriImage->imageData+nWidth*j)[i];
	//			if(temp1<0)
	//				temp1+=256;
	//			sum+=temp1;
	//		}
	//		(pImage->imageData+nWidth*j)[i]=v;
	//		
	//	}
	//}*/

/*
cvCopy(NewImage, pImage, NULL);
	double threadhold=sum/count;//所有边缘点在原图中对应像素的灰度均值
	int count1=0;//区域1像素数量
	int count2=0;//区域2像素数量
	for(int j=0;j<nHeight;j++)//将求出的灰度均值作为分割阈值对原图像进行二值化
	{
		for(int i=0;i<nWidth;i++)
		{
			double temp2=(OriImage->imageData+nWidth*j)[i];
			if(temp2<0)
				temp2+=256;
			if(temp2>threadhold)
			{
				(OriImage->imageData+nWidth*j)[i]=255;
				count1++;
			}
			else
			{
				(OriImage->imageData+nWidth*j)[i]=0;
				count2++;
			}
		}
	}
	cvCopy(OriImage, pImage, NULL);
	
	if(count1>count2)
	{
		Fanse(pImage);
	}

//	cvNamedWindow( "Segment1", 1 );//创建窗口
//    cvShowImage( "Segment1", pImage );//显示图像

	SetLabels();//快速标签算法
	location(srcpImage);//通过标签图进行缺陷区域的定位，并画出最后的检测窗口
	return prunedsubwindows;


	//GetDocument()->SetModifiedFlag(TRUE);	
	//// 更新视图
	//GetDocument()->UpdateAllViews(NULL);

}

*/

void  EdgeDetection::getEdge(CvSeq *seq)
{
	
	int nWidth	= pImage->width;
	int nHeight = pImage->height;
	int widthStep=pImage->widthStep;

	//计算直方图
	int histab[256];
	for(int i=0;i<256;i++)
		histab[i]=0;

	for(int j=1;j<nHeight-1;j++)
	{
		for(int i=1;i<nWidth-1;i++)
		{
			double temp=(pImage->imageData+nWidth*j)[i];
			if(temp<0)
				temp+=256;

			histab[(int)temp]++;
		}
	}
	for(int i=0;i<256;i++)
		probability[i]=(float)(histab[i]/((nHeight-2)*(nWidth-2)*1.0));
	double edgethreadhold=moment_shresh_sel1(probability);//计算边缘二值化的分割阈值
	for(int j=0;j<nHeight;j++)//将sobel检测出来的边缘进行二值化，滤去值过小的边缘点
	{
		for(int i=0;i<nWidth;i++)
		{
			if(i<1||j<1||j==nHeight-1||i==nWidth-1)
			{
				(pImage->imageData+nWidth*j)[i]=0;
			}
			else
			{
				double temp=(pImage->imageData+nWidth*j)[i];
				if(temp<0)
					temp+=256;

				if(temp>edgethreadhold)
				{
					(pImage->imageData+nWidth*j)[i]=255;
					
					CvPoint2D32f sample;
					sample.x=j;
					sample.y=i;
					cvSeqPush(seq,&sample);

				}
				else
					(pImage->imageData+nWidth*j)[i]=0;
			}
		}
	}

	cvNamedWindow( "Segment2_边缘图", 1 );//创建窗口
	cvShowImage( "Segment2_边缘图", pImage );//显示图像
	
}

/*

//将sobel检测出来的边缘图像直接进行快速标签并定位缺陷区域，进行缺陷分割
SubWindows EdgeDetection::Segment2(IplImage *srcpImage)
{
	// TODO: 在此添加命令处理程序代码
	//CVideoFrameDoc* pDoc = (CVideoFrameDoc*)GetDocument();
	//IplImage *srcpImage = pDoc->GetImage();
	////获取图像指针
	//IplImage *pImage = pDoc->GetImage();
	// 图象的高度和宽度	
	int nWidth	= pImage->width;
	int nHeight = pImage->height;
	int widthStep=pImage->widthStep;

	//计算直方图
	int histab[256];
	for(int i=0;i<256;i++)
		histab[i]=0;

	for(int j=1;j<nHeight-1;j++)
	{
		for(int i=1;i<nWidth-1;i++)
		{
			//if(i==1&&j==1)
			//{
			//	int a=0;
			//}
			double temp=(pImage->imageData+nWidth*j)[i];
			if(temp<0)
				temp+=256;

			histab[(int)temp]++;
		}
	}
	for(int i=0;i<256;i++)
			probability[i]=(float)(histab[i]/((nHeight-2)*(nWidth-2)*1.0));
	double edgethreadhold=moment_shresh_sel1(probability);//计算边缘二值化的分割阈值
	for(int j=0;j<nHeight;j++)//将sobel检测出来的边缘进行二值化，滤去值过小的边缘点
	{
		for(int i=0;i<nWidth;i++)
		{
			if(i<1||j<1||j==nHeight-1||i==nWidth-1)
			{
				(pImage->imageData+nWidth*j)[i]=0;
			}
			else
			{
				double temp=(pImage->imageData+nWidth*j)[i];
				if(temp<0)
					temp+=256;

				if(temp>edgethreadhold)
				{
					(pImage->imageData+nWidth*j)[i]=255;
				}
				else
					(pImage->imageData+nWidth*j)[i]=0;
			}
		}
	}

	//cvNamedWindow( "Segment2_边缘图", 1 );//创建窗口
    //cvShowImage( "Segment2_边缘图", pImage );//显示图像

	SetLabels();//将sobel算子检测出来的边缘进行快速标签，得到countofflag种不同的连通域
	location(srcpImage);//通过标签图进行缺陷区域的定位，并画出最后的检测窗口

	//double *edgethreadhold=new double[countofflag];
	for(SubWindows::iterator it=prunedsubwindows.begin();it!=prunedsubwindows.end();it++)
	{	//对于每个合并之后的窗口it，计算阈值并进行分割
		double sum=0;
		int count=0;
		for(int j=it->y;j<(int)it->y+it->height;j++)
		{
			for(int i=it->x;i<(int)it->x+it->width;i++)
			{
				double temp=(pImage->imageData+nWidth*j)[i];
				if(temp<0)
					temp+=256;

				if(temp==255)
				{
					count++;
					double temp1=(OriImage->imageData+nWidth*j)[i];
					if(temp1<0)
						temp1+=256;
					sum+=temp1;
				}	
			}
		}

		double threadhold=sum/count;
		int count1=0;//区域1像素数量
		int count2=0;//区域2像素数量
		for(int j=it->y;j<(int)it->y+it->height;j++)
		{
			for(int i=it->x;i<(int)it->x+it->width;i++)
			{
				double temp2=(OriImage->imageData+nWidth*j)[i];
				if(temp2<0)
					temp2+=256;
				if(temp2>threadhold)
				{
					(OriImage->imageData+nWidth*j)[i]=255;
					count1++;
				}
				else
				{
					(OriImage->imageData+nWidth*j)[i]=0;
					count2++;
				}
			}
		}

		if(count1>count2)
		{
			Fanse(OriImage);
		}
	}
	
//    cvNamedWindow( "Segment2", 1 );//创建窗口
//    cvShowImage( "Segment2", OriImage );//显示图像
	return prunedsubwindows;
	//cvCopy(OriImage, pImage, NULL);
}

//基于区域分析的缺陷检出
void EdgeDetection::Regionalanalysis(SubWindows & windows,int width,int height)
{
	int left_max=width-32; assert( left_max >= 0 );
	int top_max=height-32; assert( top_max >= 0 );
	for (int x=0;x<=left_max;x+=16)
	{
		for (int y=0;y<=top_max;y+=16)
		{
			RectInfo temprect;
			temprect.x=x;
			temprect.y=y;
			temprect.width=32;
			temprect.height=32;
			int count=0;
			for(SubWindows::iterator it=windows.begin();it!=windows.end();it++)
			{
				
				double dis=get_disoftworect(temprect,*it);
				if (dis<1)
				{
					count++;
				}
			}
			if(count>=6)
				addwindows.push_back(temprect);
		}
	}
}

*/


void EdgeDetection::maskFilter(cv::Mat &pImage){
	//自定义模版
	//7*9
	const int mask[7][9] ={
	{1,1,0,0,0,0,0,1,1},
	{1,1,0,0,0,0,0,1,1},
	{1,1,1,1,2,1,1,1,1},
	{1,1,1,2,3,2,1,1,1},
	{1,1,1,1,2,1,1,1,1},
	{1,1,0,0,0,0,0,1,1},
	{1,1,0,0,0,0,0,1,1}
	};

	int tempX = 7;
	int tempY = 9;

	
	cv::Mat newImg = pImage.clone();

	int width = pImage.cols;
	int height = pImage.rows;

	double sum=0.0;
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			int max = 0;
			for(int k=0;k<8;k++){
				int d=0;
				for(int y1=0;y1<3;y1++){
					for(int x1=0;x1<3;x1++){
						double temp =pImage.ptr(y)[x];
						if(temp<0){
							temp +=256;
						}
						d +=mask[k][x1+y1*3]*temp;
					}
				}
				if(d>max){
					max = d;
				}
			}
			if(max >255){
				max = 255;
			}
			newImg.ptr(x)[y] = max;			
		}
	}

	pImage = newImg.clone();
}