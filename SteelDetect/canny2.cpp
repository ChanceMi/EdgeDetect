#include "canny2.h"
#include <stack>
//#include <algorithm>
#include <QDebug>

cv::Mat clArrayToCvImage(uchar* output, int width, int height){

	cv::Mat dstImg =cv::Mat(height,width,CV_8UC1);
	//IplImage* resultImg = cvCreateImage(size,IPL_DEPTH_8U,1);
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			int index = (y*width)+x;
			dstImg.ptr<uchar>(y)[x]=output[index];
			//qDebug("%d/t",imageArray[index]);
		}
	}
	cv::imshow("canny",dstImg);
	return dstImg;
}
/*************************************************************************
*
* /�������ƣ�
* Canny()
*
* /�������:
* unsigned char *pUnchImage- ͼ������
* int nWidth - ͼ�����ݿ��
* int nHeight - ͼ�����ݸ߶�
* double sigma - ��˹�˲��ı�׼����
* double dRatioLow - ����ֵ�͸���ֵ֮��ı���//0.5
* double dRatioHigh - ����ֵռͼ�����������ı���//0.79
* unsigned char *pUnchEdge - canny���Ӽ����ķָ�ͼ
*
* /����ֵ:
* ��
*
* /˵��:
* canny�ָ����ӣ�����Ľ��������pUnchEdge�У��߼�1(255)��ʾ�õ�Ϊ
* �߽�㣬�߼�0(0)��ʾ�õ�Ϊ�Ǳ߽�㡣�ú����Ĳ���sigma��dRatioLow
* dRatioHigh������Ҫָ���ġ���Щ������Ӱ��ָ��߽����Ŀ�Ķ���
*************************************************************************
*/
void Canny(unsigned char *pUnchImage, int nWidth, int nHeight, double sigma,
	double dRatioLow, double dRatioHigh, unsigned char *pUnchEdge)
{
	// ������˹�˲����ͼ������
	unsigned char * pUnchSmooth ;

	// ָ��x��������ָ��
	int * pnGradX ;
	// ָ��y��������ָ��
	int * pnGradY ;
	// �ݶȵķ���
	int * pnGradMag ;

	pUnchSmooth = new unsigned char[nWidth*nHeight] ;
	pnGradX = new int [nWidth*nHeight] ;
	pnGradY = new int [nWidth*nHeight] ;
	pnGradMag = new int [nWidth*nHeight] ;

	// ��ԭͼ������˲�
	GaussianSmooth(pUnchImage, nWidth, nHeight, sigma, pUnchSmooth) ;

	// ���㷽����
	DirGrad(pUnchSmooth, nWidth, nHeight, pnGradX, pnGradY) ;

	// �����ݶȵķ���
	GradMagnitude(pnGradX, pnGradY, nWidth, nHeight, pnGradMag) ;

	// Ӧ��non-maximum ����
	NonmaxSuppress(pnGradMag, pnGradX, pnGradY, nWidth, nHeight, pUnchEdge) ;

	// Ӧ��Hysteresis���ҵ����еı߽�
	Hysteresis(pnGradMag, nWidth, nHeight, dRatioLow, dRatioHigh, pUnchEdge);

	// �ͷ��ڴ�
	delete pnGradX ;
	pnGradX = NULL ;
	delete pnGradY ;
	pnGradY = NULL ;
	delete pnGradMag ;
	pnGradMag = NULL ;
	delete pUnchSmooth ;
	pUnchSmooth = NULL ;
}


void GaussianSmooth(unsigned char *pUnchImg, int nWidth, int nHeight,
	double sigma, unsigned char * pUnchSmthdImg)
{
	// ѭ�����Ʊ���
	int y;
	int x;
	int i;
	// ��˹�˲��������鳤��
	int nWindowSize;
	// ���ڳ��ȵ�1/2
	int nHalfLen;
	// һά��˹�����˲���
	double *pdKernel ;
	// ��˹ϵ����ͼ�����ݵĵ��
	double dDotMul ;
	// ��˹�˲�ϵ�����ܺ�
	double dWeightSum ;
	// �м����
	double * pdTmp ;
	// �����ڴ�
	pdTmp = new double[nWidth*nHeight];
	// ����һά��˹�����˲���
	// MakeGauss(sigma, &dKernel, &nWindowSize);
	cv::imshow("beforegaussfilter",clArrayToCvImage(pUnchImg,nWidth,nHeight));
	MakeGauss(sigma, &pdKernel, &nWindowSize) ;
	// MakeGauss���ش��ڵĳ��ȣ����ô˱������㴰�ڵİ볤
	nHalfLen = nWindowSize / 2;
	// x��������˲�
	for(y=0; y<nHeight; y++){
		for(x=0; x<nWidth; x++){
			dDotMul = 0;
			dWeightSum = 0;
			
			for(i=(-nHalfLen); i<=nHalfLen; i++){
				// �ж��Ƿ���ͼ���ڲ�
				if( (i+x) >= 0 && (i+x) < nWidth ){
					int xs =pUnchImg[y*nWidth + (i+x)];
					dDotMul += double((double)xs * pdKernel[nHalfLen+i]);
					dWeightSum += pdKernel[nHalfLen+i];
				}
			}
			pdTmp[y*nWidth + x] = dDotMul/dWeightSum ;
			//qDebug("xx %f\t%f\n",dDotMul,dWeightSum);
		}
	}
	// y��������˲�
	for(x=0; x<nWidth; x++){
		for(y=0; y<nHeight; y++){
			dDotMul = 0;
			dWeightSum = 0;
			for(i=(-nHalfLen); i<=nHalfLen; i++){
				// �ж��Ƿ���ͼ���ڲ�
				if( (i+y) >= 0 && (i+y) < nHeight ){
					dDotMul += (double)pdTmp[(y+i)*nWidth + x] * pdKernel[nHalfLen+i];
					dWeightSum += pdKernel[nHalfLen+i];
					//qDebug("yy%f\t%f\n",dDotMul,dWeightSum);
				}
			}
			pUnchSmthdImg[y*nWidth + x] = (unsigned char)dDotMul/dWeightSum ;
			//qDebug("psmth%d\t",pUnchSmthdImg[y*nWidth+x]);
		}
	}

	// �ͷ��ڴ�

	cv::imshow("gaussfilter",clArrayToCvImage(pUnchSmthdImg,nWidth,nHeight));
	delete []pdKernel;
	pdKernel = NULL ;

	delete []pdTmp;
	pdTmp = NULL;
}

// ���㷽����
void DirGrad(unsigned char *pUnchSmthdImg, int nWidth, int nHeight,
	int *pnGradX , int *pnGradY)
{
	// ѭ�����Ʊ���
	int y ;
	int x ;
	// ����x����ķ��������ڱ߽�������˴�����ֹҪ���ʵ����س���
	for(y=0; y<nHeight; y++)
	{
		for(x=0; x<nWidth; x++)
		{
			pnGradX[y*nWidth+x] = (int) ( pUnchSmthdImg[y*nWidth+MIN(nWidth-1,x+1)]
			- pUnchSmthdImg[y*nWidth+MAX(0,x-1)] );
		}
	}

	// ����y����ķ��������ڱ߽�������˴�����ֹҪ���ʵ����س���
	for(x=0; x<nWidth; x++)
	{
		for(y=0; y<nHeight; y++)
		{
			pnGradY[y*nWidth+x] = (int) ( pUnchSmthdImg[MIN(nHeight-1,y+1)*nWidth + x]- pUnchSmthdImg[MAX(0,y-1)*nWidth+ x ] );
		}
	}
}


// �����ݶȵķ���
void GradMagnitude(int *pnGradX, int *pnGradY, int nWidth, int nHeight, int *pnMag)
{
	// ѭ�����Ʊ���
	int y ;
	int x ;
	// �м����
	double dSqtOne;
	double dSqtTwo;
	for(y=0; y<nHeight; y++){
		for(x=0; x<nWidth; x++){
			dSqtOne = pnGradX[y*nWidth + x] * pnGradX[y*nWidth + x];
			dSqtTwo = pnGradY[y*nWidth + x] * pnGradY[y*nWidth + x];
			pnMag[y*nWidth + x] = (int)(sqrt(dSqtOne + dSqtTwo) + 0.5);
		}
	}
}


void NonmaxSuppress(int *pnMag, int *pnGradX, int *pnGradY, int nWidth,
	int nHeight, unsigned char *pUnchRst)
{
	// ѭ�����Ʊ���
	int y ;
	int x ;
	int nPos;
	// x�����ݶȷ���
	int gx ;
	int gy ;
	// ��ʱ����
	int g1, g2, g3, g4 ;
	double weight ;
	double dTmp1 ;
	double dTmp2 ;
	double dTmp ;
	// ����ͼ���Ե����Ϊ�����ܵı߽��
	for(x=0; x<nWidth; x++){
		pUnchRst[x] = 0 ;
		pUnchRst[nHeight-1+x] = 0;
		//pUnchRst[(nHeight-1)*nWidth+x] = 0;?????????
	}
	for(y=0; y<nHeight; y++){
		pUnchRst[y*nWidth] = 0 ;
		pUnchRst[y*nWidth + nWidth-1] = 0;
	}

	for(y=1; y<nHeight-1; y++){
		for(x=1; x<nWidth-1; x++){
			nPos = y*nWidth + x ;
			// �����ǰ���ص��ݶȷ���Ϊ0�����Ǳ߽��
			if(pnMag[nPos] == 0 ){
				pUnchRst[nPos] = 0 ;
			}else{
				// ��ǰ���ص��ݶȷ���
				dTmp = pnMag[nPos] ;
				// x��y������
				gx = pnGradX[nPos] ;
				gy = pnGradY[nPos] ;
				// ���������y������x������˵�������ķ�����ӡ�������y������
				if (abs(gy) > abs(gx)){
					// �����ֵ�ı���
					weight = abs(gx)/abs(gy);
					g2 = pnMag[nPos-nWidth] ;
					g4 = pnMag[nPos+nWidth] ;
					// ���x��y��������ķ������ķ�����ͬ
					// C�ǵ�ǰ���أ���g1-g4��λ�ù�ϵΪ��
					// g1 g2
					// C
					// g4 g3
					if (gx*gy > 0){
						g1 = pnMag[nPos-nWidth-1] ;
						g3 = pnMag[nPos+nWidth+1] ;
					}
					// ���x��y��������ķ������ķ����෴
					// C�ǵ�ǰ���أ���g1-g4��λ�ù�ϵΪ��
					// g2 g1
					// C
					// g3 g4
					else{
						g1 = pnMag[nPos-nWidth+1] ;
						g3 = pnMag[nPos+nWidth-1] ;
					}
				}
				// ���������x������y������˵�������ķ�����ӡ�������x����
				// ����ж���������x������y������ȵ����
				else{
					// �����ֵ�ı���
					weight = abs(gy)/abs(gx);
					g2 = pnMag[nPos+1] ;
					g4 = pnMag[nPos-1] ;
					// ���x��y��������ķ������ķ�����ͬ
					// C�ǵ�ǰ���أ���g1-g4��λ�ù�ϵΪ��
					// g3
					// g4 C g2
					// g1
					if (gx*gy > 0){
						g1 = pnMag[nPos+nWidth+1] ;
						g3 = pnMag[nPos-nWidth-1] ;
					}
					// ���x��y��������ķ������ķ����෴
					// C�ǵ�ǰ���أ���g1-g4��λ�ù�ϵΪ��
					// g1
					// g4 C g2
					// g3
					else{
						g1 = pnMag[nPos-nWidth+1] ;
						g3 = pnMag[nPos+nWidth-1] ;
					}
				}

				// ��������g1-g4���ݶȽ��в�ֵ
				{
					dTmp1 = weight*g1 + (1-weight)*g2 ;
					dTmp2 = weight*g3 + (1-weight)*g4 ;
					// ��ǰ���ص��ݶ��Ǿֲ������ֵ
					// �õ�����Ǹ��߽��
					if(dTmp>=dTmp1 && dTmp>=dTmp2){
						pUnchRst[nPos] = 128 ;
					}else{
						// �������Ǳ߽��
						pUnchRst[nPos] = 0 ;
					}
				}
			} //else
		} // for

	}
} 


void Hysteresis(int *pnMag, int nWidth, int nHeight, double dRatioLow,
	double dRatioHigh, unsigned char *pUnchEdge)
{
	// ѭ�����Ʊ���
	int y;
	int x;

	int nThdHigh ;
	int nThdLow ;

	int nPos;
	// ����TraceEdge��Ҫ�ĵ���ֵ���Լ�Hysteresis����ʹ�õĸ���ֵ
	EstimateThreshold(pnMag, nWidth, nHeight, &nThdHigh,
		&nThdLow, pUnchEdge,dRatioHigh, dRatioLow);

	// ���ѭ������Ѱ�Ҵ���nThdHigh�ĵ㣬��Щ�㱻���������߽�㣬Ȼ����
	// TraceEdge���������ٸõ��Ӧ�ı߽�
	for(y=0; y<nHeight; y++){
		for(x=0; x<nWidth; x++){
			nPos = y*nWidth + x ;
			// ����������ǿ��ܵı߽�㣬�����ݶȴ��ڸ���ֵ����������Ϊ
			// һ���߽�����
			if((pUnchEdge[nPos] == 128) && (pnMag[nPos] >= nThdHigh)){
				// ���øõ�Ϊ�߽��
				pUnchEdge[nPos] = 255;
				//TraceEdge(y, x, nThdLow, pUnchEdge, pnMag, nWidth);
				TraceEdge2(y, x, nThdLow, pUnchEdge, pnMag, nWidth);
			}
		}
	}

	// ��Щ��û�б�����Ϊ�߽��������Ѿ������ܳ�Ϊ�߽��
	for(y=0; y<nHeight; y++){
		for(x=0; x<nWidth; x++)
		{
			nPos = y*nWidth + x ;
			if(pUnchEdge[nPos] != 255)
			{
				// ����Ϊ�Ǳ߽��
				pUnchEdge[nPos] = 0 ;
			}
		}
	}
}


/*************************************************************************
*
* /�������ƣ�
* EstimateThreshold()
*
* /�������:
* int *pnMag - �ݶȷ���ͼ
* int nWidth - ͼ�����ݿ��
* int nHeight - ͼ�����ݸ߶�
* int *pnThdHigh - ����ֵ
* int *pnThdLow - ����ֵ
* double dRatioLow - ����ֵ�͸���ֵ֮��ı���
* double dRatioHigh - ����ֵռͼ�����������ı���
* unsigned char *pUnchEdge - ����non-maximum����������
*
* /����ֵ:
* ��
*
* /˵��:
* ����non-maximum����������pUnchEdge��ͳ��pnMag��ֱ��ͼ��ȷ����ֵ��
* ��������ֻ��ͳ��pUnchEdge�п���Ϊ�߽�����Щ���ء�Ȼ������ֱ��ͼ��
* ����dRatioHigh���ø���ֵ���洢��pnThdHigh������dRationLow�͸���ֵ��
* ���õ���ֵ���洢��*pnThdLow��dRatioHigh��һ�ֱ����������ݶ�С��
* *pnThdHigh��������Ŀռ��������Ŀ�ı�����dRationLow����*pnThdHigh
* ��*pnThdLow�ı��������������canny�㷨��ԭ������߸�����һ�����䡣
*
*************************************************************************
*/
void EstimateThreshold(int *pnMag, int nWidth, int nHeight, int *pnThdHigh,int *pnThdLow,
	unsigned char * pUnchEdge, double dRatioHigh, double dRationLow)
{
	// ѭ�����Ʊ���
	int y;
	int x;
	int k;
	// ������Ĵ�С���ݶ�ֵ�ķ�Χ�йأ�������ñ�������㷨����ô�ݶȵķ�Χ���ᳬ��pow(2,10)
	int nHist[1024] ;
	// ���ܵı߽���Ŀ
	int nEdgeNb ;

	// ����ݶ�ֵ
	int nMaxMag ;

	int nHighCount ;

	nMaxMag = 0 ;

	// ��ʼ��
	for(k=0; k<1024; k++){
		nHist[k] = 0;
	}

	// ͳ��ֱ��ͼ��Ȼ������ֱ��ͼ������ֵ
	for(y=0; y<nHeight; y++){
		for(x=0; x<nWidth; x++){
			// ֻ��ͳ����Щ�����Ǳ߽�㣬���һ�û�д����������
			if(pUnchEdge[y*nWidth+x]==128){
				nHist[ pnMag[y*nWidth+x] ]++;
			}
		}
	}
	nEdgeNb = nHist[0] ;
	nMaxMag = 0 ;
	// ͳ�ƾ����������ֵ��ֹ(non-maximum suppression)�����ж�������
	for(k=1; k<1024; k++){
		if(nHist[k] != 0){
			// ����ݶ�ֵ
			nMaxMag = k;
		}

		// �ݶ�Ϊ0�ĵ��ǲ�����Ϊ�߽���
		// ����non-maximum suppression���ж�������
		nEdgeNb += nHist[k];
	}

	// �ݶȱȸ���ֵ*pnThdHighС�����ص�����Ŀ
	nHighCount = (int)(dRatioHigh * nEdgeNb +0.5);

	k = 1;
	nEdgeNb = nHist[1];

	// �������ֵ
	while( (k<(nMaxMag-1)) && (nEdgeNb < nHighCount) ){
		k++;
		nEdgeNb += nHist[k];
	}
	// ���ø���ֵ
	*pnThdHigh = k ;

	// ���õ���ֵ
	*pnThdLow = (int)((*pnThdHigh) * dRationLow+ 0.5);
}


/*************************************************************************
*
* /�������ƣ�
* MakeGauss()
*
* /�������:
* double sigma - ��˹�����ı�׼��
* double **pdKernel - ָ���˹���������ָ��
* int *pnWindowSize - ���ݵĳ���
*
* /����ֵ:
* ��
*
* /˵��:
* ���������������һ��һά�ĸ�˹�������������ݣ������ϸ�˹���ݵĳ���Ӧ
* �������޳��ģ�����Ϊ�˼���ļ򵥺��ٶȣ�ʵ�ʵĸ�˹����ֻ�������޳���
* pnWindowSize�������ݳ���
*
*********************************************************************/
void MakeGauss(double sigma, double **pdKernel, int *pnWindowSize)
{//sigma 0.6 1
	// ѭ�����Ʊ���
	int i ;
	// ��������ĵ�
	int nCenter;
	// �����ĳһ�㵽���ĵ�ľ���
	double dDis ;
	double PI = 3.14159;
	// �м����
	double dValue;
	double dSum ;
	dSum = 0 ;
	// ���鳤�ȣ����ݸ����۵�֪ʶ��ѡȡ[-3*sigma, 3*sigma]���ڵ����ݡ�
	// ��Щ���ݻḲ�Ǿ��󲿷ֵ��˲�ϵ��
	*pnWindowSize = 1 + 2 * ceil(3 * sigma);//7
	
	// ����
	nCenter = (*pnWindowSize) / 2;

	// �����ڴ�
	*pdKernel = new double[*pnWindowSize] ;

	for(i=0; i< (*pnWindowSize); i++){
		dDis = (double)(i - nCenter);
		double fz = exp(-(0.5)*dDis*dDis/(sigma*sigma));
		double fm = (sqrt(2 * PI) * sigma );
		dValue =  fz/fm;

		(*pdKernel)[i] = dValue ;
		dSum += dValue;
	}
	//qDebug("sum%f",dSum);
	//��һ��
	for(i=0; i<(*pnWindowSize) ; i++)
	{
		(*pdKernel)[i] /= dSum;
		//qDebug("kernel%f",(*pdKernel)[i]);
	}
}
/*********************************/

/*************************************************************************
*
* /�������ƣ�
* TraceEdge()
*
* /�������:
* int x - ��������x����
* int y - ��������y����
* int nLowThd - �ж�һ�����Ƿ�Ϊ�߽��ĵ���ֵ
* unsigned char *pUnchEdge - ��¼�߽��Ļ�����
* int *pnMag - �ݶȷ���ͼ
* int nWidth - ͼ�����ݿ��
*
* /����ֵ:
* ��
*
* /˵��:
* �ݹ����
* ��(x,y)������������б߽��ĸ��٣�����ֻ����pUnchEdge��û�д�����
* �����Ǳ߽�����Щ����(=128),����ֵΪ0�����õ㲻�����Ǳ߽�㣬����ֵ
* Ϊ255�����õ��Ѿ�������Ϊ�߽�㣬�����ٿ���
*
*
*************************************************************************
*/
void TraceEdge (int y, int x, int nLowThd, unsigned char *pUnchEdge, int *pnMag, int nWidth)
{
	// ��8�������ؽ��в�ѯ
	int xNb[8] = {1, 1, 0,-1,-1,-1, 0, 1} ;
	int yNb[8] = {0, 1, 1, 1,0 ,-1,-1,-1} ;

	int yy ;
	int xx ;

	int k ;

	for(k=0; k<8; k++){
		yy = y + yNb[k] ;
		xx = x + xNb[k] ;
		// ���������Ϊ���ܵı߽�㣬��û�д����
		// �����ݶȴ�����ֵ
		if(pUnchEdge[yy*nWidth+xx] == 128 && pnMag[yy*nWidth+xx]>=nLowThd){
			// �Ѹõ����ó�Ϊ�߽��
			pUnchEdge[yy*nWidth+xx] = 255 ;

			// �Ըõ�Ϊ���Ľ��и���
			TraceEdge(yy, xx, nLowThd, pUnchEdge, pnMag, nWidth);
		}
	}
}



typedef struct _imPoint
{
	 int x;
	 int y;
}TimPoint;

inline TimPoint imPoint( int x, int y )
{
	 TimPoint p;
	 p.x = x;
	 p.y = y;
	 return p;
}

//����,ʹ�öѵ�ȡ���ݻغ�ʽ
void TraceEdge2(int y, int x, int nLowThd, unsigned char *pUnchEdge, int *pnMag, int nWidth)
{
	// ��8�������ؽ��в�ѯ
	int xNb[8] = {1, 1, 0,-1,-1,-1, 0, 1} ;
	int yNb[8] = {0, 1, 1, 1,0 ,-1,-1,-1} ;
	std::stack<TimPoint> TraceStack;
	TimPoint curPt = imPoint(x,y);
	TraceStack.push(curPt);
	int yy,sy ;
	int xx,sx ;
	int k ;

	while(!TraceStack.empty()){
			 curPt = TraceStack.top();
			 TraceStack.pop();
			 sx = curPt.x;
			 sy = curPt.y;
			 for(k=0; k<8; k++) {
					yy = sy + yNb[k] ;
					xx = sx + xNb[k] ;
					// ���������Ϊ���ܵı߽�㣬��û�д����
					// �����ݶȴ�����ֵ
					if(pUnchEdge[yy*nWidth+xx] == 128 && pnMag[yy*nWidth+xx]>=nLowThd){
							// �Ѹõ����ó�Ϊ�߽��
							pUnchEdge[yy*nWidth+xx] = 255 ;
							// �Ըõ����ѵ�
							 TraceStack.push(imPoint(xx,yy));
					}
			}
	}
}