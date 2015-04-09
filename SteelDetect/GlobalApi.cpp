// 文件DWT.cpp存放的是有关小波变换的函数
#include "math.h"
//#include <opencv2/opencv.hpp>
#include "GlobalApi.h"

/*************************************************************************
 *
 * \函数名称：
 *   DWT_1D()
 *
 * \输入参数:
 *   double * pDbSrc		- 指向源数据的指针
 *   int nMaxLevel		- 最大可分解的层数
 *   int nDWTSteps		- 需要分界的层数
 *   int nInv			- 是否为DWT，1表示为IDWT，0表示DWT
 *   int nStep			- 当前的计算层数
 *   int nSupp			- 小波基的紧支集的长度
 *
 * \返回值:
 *   BOOL			- 成功则返回TRUE，否则返回FALSE
 *
 * \说明:
 *   该函数用对存放在pDBSrc中的数据进行一维DWT或者IDWT。其中，nInv为表示进行
 *   DWT或者IDWT的标志。nStep为当前已经分界的层数。计算后数据仍存放在pDbSrc中
 *
 *************************************************************************
BOOL DWT_1D(double* pDbSrc, int nMaxLevel,
			int nDWTSteps, int nInv, int nStep, int nSupp )
{
	// 计算最小可分界的层数
	int MinLevel = nMaxLevel-nDWTSteps;

	// 判断是否为DWT
	if (!nInv)
	{	// DWT
		int n = nMaxLevel;

		while (n>MinLevel)
			// 调用DWTStep_1D进行第n层的DWT
			if (!DWTStep_1D(pDbSrc, n--, nInv, nStep, nSupp)) 
				return FALSE;
	}

	// nInv为1则进行IDWT
	else
	{	// IDWT
		int n = MinLevel;

		while (n<nMaxLevel)
			// 调用DWTStep_1D进行第n层的IDWT
			if (!DWTStep_1D(pDbSrc, n++, nInv, nStep, nSupp)) 
				return FALSE;
	}
	return TRUE;
}
*/
/*************************************************************************
 *
 * \函数名称：
 *   DWTStep_1D()
 *
 * \输入参数:
 *   double * pDbSrc		- 指向源数据的指针
 *   int nCurLevel		- 当前分界的层数				//改为int nCurlength
 *   int nInv			- 是否为DWT，1表示为IDWT，0表示DWT
 *   int nStep			- 当前的计算层数
 *   int nSupp			- 小波基的紧支集的长度
 *
 * \返回值:
 *   BOOL			- 成功则返回TRUE，否则返回FALSE
 *
 * \说明:
 *   该函数用对存放在pDBSrc中的数据进行一层的一维DWT或者IDWT。其中，nInv为表示进行
 *   DWT或者IDWT的标志。nCurLevel为当前需要进行分界的层数。nStep为已经分界的层数
 *   计算后数据仍存放在pDbSrc中
 *
 *************************************************************************
 */

BOOL DWTStep_1D(double* data, int nCurlength,int nInv, int nStep,int m_nSupp)
{
	double s = sqrt(2.0); double* h = NULL;
	h = (double*)hCoef[m_nSupp-1]; // 获得小波基的指针
	int CurN = nCurlength; // 计算当前层数的长度

	if (nInv) 
	{
		CurN <<= 1;		
	}

	// 确认所选择的小波基和当前层数的长度有效
	if (m_nSupp<1 || CurN<2*m_nSupp)
		return FALSE;
	// 分配临时内存用于存放结果
	double *ptemp = new double[CurN];
	if (! ptemp) return FALSE;
	double s1, s2; int Index1, Index2;
	// 判断是进行DWT 还是IDWT
	if (! nInv)
	{// DWT
		Index1=0,Index2=2*m_nSupp-1;
		// 进行卷积，其中s1 为低频部分，s2 为高频部分的结果
		for (int i=0; i<CurN/2; i++)
		{
			s1 = s2 = 0; double t = -1;
			for (int j=0; j<2*m_nSupp; j++, t=-t)
			{
				int m = 0,n = 0;
				if(Index1 >= 0 && Index1 <= CurN-1) m = Index1; // 周期延拓
				else if(Index1 < 0)m = CurN + Index1;
				else m = Index1-CurN;
				if (Index2 >= 0 && Index2 <= CurN-1) n = Index2;
				else if(Index2 < 0)n = CurN + Index2;
				else n = Index2-CurN;
				s1 += h[j]*data[m * nStep];
				s2 += t*h[j]*data[n * nStep];
				Index1++; Index2--;
			}
			// 将结果存放在临时内存中
			ptemp[i] = s1/s,ptemp[i+CurN/2] = s2/s;
			Index1 -= 2*m_nSupp,Index2 += 2*m_nSupp;
			Index1 += 2,Index2 += 2;
		}
	}
	else{ // IDWT
		Index1 = CurN/2; Index2 = CurN/2-m_nSupp+1;
			int nHalf=CurN/2;
		for (int i=0; i<CurN/2; i++)
		{
			s1 = s2 = 0;
			int Index3 = 0;
			for (int j=0; j<m_nSupp; j++)
			{
				int m=0, n=0;
				if (Index1>=nHalf) m = Index1 - nHalf;
				else m = Index1;
				if (Index2>=nHalf) n = Index2 - nHalf;
				else n = Index2;
				s1 += h[Index3]*data[m * nStep]
				+h[Index3+1]*data[(n + CurN/2) * nStep];
				s2 += h[Index3+1]*data[m * nStep]
				-h[Index3]*data[(n + CurN/2) * nStep];
				Index3+=2,Index1--,Index2++;
			}
			// 将结果存入临时内存
			ptemp[2*i] = s1*s,ptemp[2*i+1] = s2*s;
			Index1 += m_nSupp,Index2 -= m_nSupp;
			Index1++,Index2++;
		}
	}
	// 将结果存入源图象中
	for (int i=0; i<CurN; i++)
		data[i*nStep] = ptemp[i];
	// 释放临时内存
	delete[] ptemp;
	return TRUE;
}

BOOL DWTStep_2D(double* pDbSrc, int nCurW, int nCurH,int nMaxW, int nMaxH, int nInv, int nStep, int nSupp)
{
	// 计算图象的长度和宽度（2次幂对齐）
//	int W = 1<<nMaxWLevel, H = 1<<nMaxHLevel;
	int W = nMaxW, H = nMaxH;
	// 计算当前分解的图象的长度和宽度
//	int CurW = 1<<nCurWLevel, CurH = 1<<nCurHLevel;
	int CurW = nCurW, CurH = nCurH;

	// 判断是进行DWT还是IDWT
	if (!nInv)
	{	// 对行进行一维DWT
		for (int i=0; i<CurH; i++)
			if (!DWTStep_1D(pDbSrc+(int)i*W*nStep, nCurW, nInv, nStep, nSupp)) return FALSE;
		// 对列进行一维DWT
		for (int i=0; i<CurW; i++)
			if (!DWTStep_1D(pDbSrc+i*nStep, nCurH, nInv, W*nStep, nSupp)) return FALSE;
	}
	// 否则进行IDWT
	else
	{
		// 计算当前变换的图象的长度和宽度
		CurW <<= 1;
		CurH <<= 1;

		// 对列进行IDWT
		for (int i=0; i<CurW; i++)
			if (!DWTStep_1D(pDbSrc+i*nStep, nCurH, nInv, W*nStep, nSupp)) return FALSE;
		// 对行进行IDWT
		for (int i=0; i<CurH; i++)
			if (!DWTStep_1D(pDbSrc+(int)i*W*nStep, nCurW, nInv, nStep, nSupp)) return FALSE;
	}
	// 返回
	return TRUE;
}
/*************************************************************************
 *
 * \函数名称：
 *   ImageDWT()
 *
 * \输入参数:
 *   CDib* pDibSrc		- 指向源数据的指针
 *   int nMaxWLevel		- X方向上最大可分解的层数
 *   int nMaxHLevel		- Y方向上最大可分解的层数
 *   int nDWTSteps		- 需要进行变换的层数
 *   int nInv			- 是否为DWT，1表示为IDWT，0表示DWT
 *   int nStep			- 当前的计算层数
 *   int nSupp			- 小波基的紧支集的长度
 *
 * \返回值:
 *   BOOL			- 成功则返回TRUE，否则返回FALSE
 *
 * \说明:
 *   该函数用对存放在pDBSrc中的数据进行一层的二维DWT或者IDWT。
 *   计算后数据仍存放在pDbSrc中
 *
 *************************************************************************
 *//*
BOOL ImageDWT(LPBYTE lpImage, int nMaxWLevel, int nMaxHLevel,
			int nDWTSteps, int nInv, int nStep, int nSupp)
{
	// 判断变换的层数以及当前层数是否有效
	if (nDWTSteps>nMaxWLevel || nDWTSteps>nMaxHLevel || nStep<=0)
		return FALSE;

	// 获得X，Y方向上的最大象素数（2次幂对齐）
	int W = 1<<nMaxWLevel, H = 1<<nMaxHLevel;

	// 获得X，Y方向上变换时最小的象素数
	int minW = W>>nDWTSteps, minH = H>>nDWTSteps;

	int i, j, index;
	
	// 分配临时内存存放结果
	double*	pDbTemp = new double[W*H];
	if (!pDbTemp) return FALSE;

	// 判断是进行DWT还是IDWT，然后将数据存放到临时内存中，需要注意的是，需要进行采样
	if (!nInv)	// DWT
		for (index=0; index<W*H; index++) pDbTemp[index] = lpImage[index*nStep];
	else		// IDWT
	{
		index = 0;
		for (i=0; i<minH; i++)
		{
			for (j=0; j<minW; j++, index++)
				pDbTemp[index] = lpImage[index*nStep];
			for (; j<W; j++, index++)
				pDbTemp[index] = (char)lpImage[index*nStep];
		}
		for (; index<W*H; index++)
			pDbTemp[index] = (char)lpImage[index*nStep];
	}

	// 调用DWT_2D进行小波变换
	if(!DWT_2D(pDbTemp, nMaxWLevel, nMaxHLevel, nDWTSteps, nInv, nStep, nSupp))
	{
		delete []pDbTemp;
		return FALSE;
	}

	// 将数据存入原始的内存中，需要注意的是，存储时需要进行类型转换
	if (!nInv)	// DWT
	{
		index = 0;
		for (i=0; i<minH; i++)
		{
			for (j=0; j<minW; j++, index++)
				lpImage[index*nStep] = FloatToByte(pDbTemp[index]);
			for (; j<W; j++, index++)
				lpImage[index*nStep] = (uchar)FloatToChar(pDbTemp[index]);
			//	lpImage[index*nStep] = (uchar)FloatToByte(pDbTemp[index]);
		}
		for (; index<W*H; index++)
			lpImage[index*nStep] = (uchar)FloatToChar(pDbTemp[index]);
			//lpImage[index*nStep] = (uchar)FloatToByte(pDbTemp[index]);
	}
	else		// IDWT
		for (index=0; index<W*H; index++) 
			lpImage[index*nStep] = FloatToByte(pDbTemp[index]);

	// 释放内存
	delete []pDbTemp;

	// 返回
	return TRUE;
}
*/
/*************************************************************************
 *
 * \函数名称：
 *   FloatToByte()
 *
 * \输入参数:
 *   double  f			- 输入双精度变量
 *
 * \返回值:
 *   uchar			- 返回比特型变量
 *
 * \说明:
 *   该函数将输入的双精度变量转换为uchar型的变量
 *
 *************************************************************************
 */
uchar FloatToByte(double f)
{
	if (f<=0) return (uchar)0;
	else if(f>=255) return (uchar)255;
	else return (uchar)(f+0.5);
}

/*************************************************************************
 *
 * \函数名称：
 *   FloatToChar()
 *
 * \输入参数:
 *   double  f			- 输入双精度变量
 *
 * \返回值:
 *   Char			- 返回字符变量
 *
 * \说明:
 *   该函数将输入的双精度变量转换为Char型的变量
 *
 *************************************************************************
 */
char FloatToChar(double f)
{
	if (f>=0)
		if (f>=127.0)
			return (char)127;
		else return (char)(f+0.5);
	else
		if (f<=-128)
			return (char)-128;
		else return -(char)(-f+0.5);
}

/*************************************************************************
 *
 * \函数名称：
 *   Log2()
 *
 * \输入参数:
 *   int  n			- 输入整型变量
 *
 * \返回值:
 *   int			- 返回输入参数的对数
 *
 * \说明:
 *   该函数求取输入参数的以2为底的对数，并转换为整型输出。
 *
 *************************************************************************
 */
int Log2(int n)
{
	int rsl = 0;
	while (n >>= 1) rsl++;
	return rsl;
}