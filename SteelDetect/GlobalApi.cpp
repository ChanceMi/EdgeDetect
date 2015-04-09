// �ļ�DWT.cpp��ŵ����й�С���任�ĺ���
#include "math.h"
//#include <opencv2/opencv.hpp>
#include "GlobalApi.h"

/*************************************************************************
 *
 * \�������ƣ�
 *   DWT_1D()
 *
 * \�������:
 *   double * pDbSrc		- ָ��Դ���ݵ�ָ��
 *   int nMaxLevel		- ���ɷֽ�Ĳ���
 *   int nDWTSteps		- ��Ҫ�ֽ�Ĳ���
 *   int nInv			- �Ƿ�ΪDWT��1��ʾΪIDWT��0��ʾDWT
 *   int nStep			- ��ǰ�ļ������
 *   int nSupp			- С�����Ľ�֧���ĳ���
 *
 * \����ֵ:
 *   BOOL			- �ɹ��򷵻�TRUE�����򷵻�FALSE
 *
 * \˵��:
 *   �ú����öԴ����pDBSrc�е����ݽ���һάDWT����IDWT�����У�nInvΪ��ʾ����
 *   DWT����IDWT�ı�־��nStepΪ��ǰ�Ѿ��ֽ�Ĳ���������������Դ����pDbSrc��
 *
 *************************************************************************
BOOL DWT_1D(double* pDbSrc, int nMaxLevel,
			int nDWTSteps, int nInv, int nStep, int nSupp )
{
	// ������С�ɷֽ�Ĳ���
	int MinLevel = nMaxLevel-nDWTSteps;

	// �ж��Ƿ�ΪDWT
	if (!nInv)
	{	// DWT
		int n = nMaxLevel;

		while (n>MinLevel)
			// ����DWTStep_1D���е�n���DWT
			if (!DWTStep_1D(pDbSrc, n--, nInv, nStep, nSupp)) 
				return FALSE;
	}

	// nInvΪ1�����IDWT
	else
	{	// IDWT
		int n = MinLevel;

		while (n<nMaxLevel)
			// ����DWTStep_1D���е�n���IDWT
			if (!DWTStep_1D(pDbSrc, n++, nInv, nStep, nSupp)) 
				return FALSE;
	}
	return TRUE;
}
*/
/*************************************************************************
 *
 * \�������ƣ�
 *   DWTStep_1D()
 *
 * \�������:
 *   double * pDbSrc		- ָ��Դ���ݵ�ָ��
 *   int nCurLevel		- ��ǰ�ֽ�Ĳ���				//��Ϊint nCurlength
 *   int nInv			- �Ƿ�ΪDWT��1��ʾΪIDWT��0��ʾDWT
 *   int nStep			- ��ǰ�ļ������
 *   int nSupp			- С�����Ľ�֧���ĳ���
 *
 * \����ֵ:
 *   BOOL			- �ɹ��򷵻�TRUE�����򷵻�FALSE
 *
 * \˵��:
 *   �ú����öԴ����pDBSrc�е����ݽ���һ���һάDWT����IDWT�����У�nInvΪ��ʾ����
 *   DWT����IDWT�ı�־��nCurLevelΪ��ǰ��Ҫ���зֽ�Ĳ�����nStepΪ�Ѿ��ֽ�Ĳ���
 *   ����������Դ����pDbSrc��
 *
 *************************************************************************
 */

BOOL DWTStep_1D(double* data, int nCurlength,int nInv, int nStep,int m_nSupp)
{
	double s = sqrt(2.0); double* h = NULL;
	h = (double*)hCoef[m_nSupp-1]; // ���С������ָ��
	int CurN = nCurlength; // ���㵱ǰ�����ĳ���

	if (nInv) 
	{
		CurN <<= 1;		
	}

	// ȷ����ѡ���С�����͵�ǰ�����ĳ�����Ч
	if (m_nSupp<1 || CurN<2*m_nSupp)
		return FALSE;
	// ������ʱ�ڴ����ڴ�Ž��
	double *ptemp = new double[CurN];
	if (! ptemp) return FALSE;
	double s1, s2; int Index1, Index2;
	// �ж��ǽ���DWT ����IDWT
	if (! nInv)
	{// DWT
		Index1=0,Index2=2*m_nSupp-1;
		// ���о��������s1 Ϊ��Ƶ���֣�s2 Ϊ��Ƶ���ֵĽ��
		for (int i=0; i<CurN/2; i++)
		{
			s1 = s2 = 0; double t = -1;
			for (int j=0; j<2*m_nSupp; j++, t=-t)
			{
				int m = 0,n = 0;
				if(Index1 >= 0 && Index1 <= CurN-1) m = Index1; // ��������
				else if(Index1 < 0)m = CurN + Index1;
				else m = Index1-CurN;
				if (Index2 >= 0 && Index2 <= CurN-1) n = Index2;
				else if(Index2 < 0)n = CurN + Index2;
				else n = Index2-CurN;
				s1 += h[j]*data[m * nStep];
				s2 += t*h[j]*data[n * nStep];
				Index1++; Index2--;
			}
			// ������������ʱ�ڴ���
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
			// �����������ʱ�ڴ�
			ptemp[2*i] = s1*s,ptemp[2*i+1] = s2*s;
			Index1 += m_nSupp,Index2 -= m_nSupp;
			Index1++,Index2++;
		}
	}
	// ���������Դͼ����
	for (int i=0; i<CurN; i++)
		data[i*nStep] = ptemp[i];
	// �ͷ���ʱ�ڴ�
	delete[] ptemp;
	return TRUE;
}

BOOL DWTStep_2D(double* pDbSrc, int nCurW, int nCurH,int nMaxW, int nMaxH, int nInv, int nStep, int nSupp)
{
	// ����ͼ��ĳ��ȺͿ�ȣ�2���ݶ��룩
//	int W = 1<<nMaxWLevel, H = 1<<nMaxHLevel;
	int W = nMaxW, H = nMaxH;
	// ���㵱ǰ�ֽ��ͼ��ĳ��ȺͿ��
//	int CurW = 1<<nCurWLevel, CurH = 1<<nCurHLevel;
	int CurW = nCurW, CurH = nCurH;

	// �ж��ǽ���DWT����IDWT
	if (!nInv)
	{	// ���н���һάDWT
		for (int i=0; i<CurH; i++)
			if (!DWTStep_1D(pDbSrc+(int)i*W*nStep, nCurW, nInv, nStep, nSupp)) return FALSE;
		// ���н���һάDWT
		for (int i=0; i<CurW; i++)
			if (!DWTStep_1D(pDbSrc+i*nStep, nCurH, nInv, W*nStep, nSupp)) return FALSE;
	}
	// �������IDWT
	else
	{
		// ���㵱ǰ�任��ͼ��ĳ��ȺͿ��
		CurW <<= 1;
		CurH <<= 1;

		// ���н���IDWT
		for (int i=0; i<CurW; i++)
			if (!DWTStep_1D(pDbSrc+i*nStep, nCurH, nInv, W*nStep, nSupp)) return FALSE;
		// ���н���IDWT
		for (int i=0; i<CurH; i++)
			if (!DWTStep_1D(pDbSrc+(int)i*W*nStep, nCurW, nInv, nStep, nSupp)) return FALSE;
	}
	// ����
	return TRUE;
}
/*************************************************************************
 *
 * \�������ƣ�
 *   ImageDWT()
 *
 * \�������:
 *   CDib* pDibSrc		- ָ��Դ���ݵ�ָ��
 *   int nMaxWLevel		- X���������ɷֽ�Ĳ���
 *   int nMaxHLevel		- Y���������ɷֽ�Ĳ���
 *   int nDWTSteps		- ��Ҫ���б任�Ĳ���
 *   int nInv			- �Ƿ�ΪDWT��1��ʾΪIDWT��0��ʾDWT
 *   int nStep			- ��ǰ�ļ������
 *   int nSupp			- С�����Ľ�֧���ĳ���
 *
 * \����ֵ:
 *   BOOL			- �ɹ��򷵻�TRUE�����򷵻�FALSE
 *
 * \˵��:
 *   �ú����öԴ����pDBSrc�е����ݽ���һ��Ķ�άDWT����IDWT��
 *   ����������Դ����pDbSrc��
 *
 *************************************************************************
 *//*
BOOL ImageDWT(LPBYTE lpImage, int nMaxWLevel, int nMaxHLevel,
			int nDWTSteps, int nInv, int nStep, int nSupp)
{
	// �жϱ任�Ĳ����Լ���ǰ�����Ƿ���Ч
	if (nDWTSteps>nMaxWLevel || nDWTSteps>nMaxHLevel || nStep<=0)
		return FALSE;

	// ���X��Y�����ϵ������������2���ݶ��룩
	int W = 1<<nMaxWLevel, H = 1<<nMaxHLevel;

	// ���X��Y�����ϱ任ʱ��С��������
	int minW = W>>nDWTSteps, minH = H>>nDWTSteps;

	int i, j, index;
	
	// ������ʱ�ڴ��Ž��
	double*	pDbTemp = new double[W*H];
	if (!pDbTemp) return FALSE;

	// �ж��ǽ���DWT����IDWT��Ȼ�����ݴ�ŵ���ʱ�ڴ��У���Ҫע����ǣ���Ҫ���в���
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

	// ����DWT_2D����С���任
	if(!DWT_2D(pDbTemp, nMaxWLevel, nMaxHLevel, nDWTSteps, nInv, nStep, nSupp))
	{
		delete []pDbTemp;
		return FALSE;
	}

	// �����ݴ���ԭʼ���ڴ��У���Ҫע����ǣ��洢ʱ��Ҫ��������ת��
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

	// �ͷ��ڴ�
	delete []pDbTemp;

	// ����
	return TRUE;
}
*/
/*************************************************************************
 *
 * \�������ƣ�
 *   FloatToByte()
 *
 * \�������:
 *   double  f			- ����˫���ȱ���
 *
 * \����ֵ:
 *   uchar			- ���ر����ͱ���
 *
 * \˵��:
 *   �ú����������˫���ȱ���ת��Ϊuchar�͵ı���
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
 * \�������ƣ�
 *   FloatToChar()
 *
 * \�������:
 *   double  f			- ����˫���ȱ���
 *
 * \����ֵ:
 *   Char			- �����ַ�����
 *
 * \˵��:
 *   �ú����������˫���ȱ���ת��ΪChar�͵ı���
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
 * \�������ƣ�
 *   Log2()
 *
 * \�������:
 *   int  n			- �������ͱ���
 *
 * \����ֵ:
 *   int			- ������������Ķ���
 *
 * \˵��:
 *   �ú�����ȡ�����������2Ϊ�׵Ķ�������ת��Ϊ���������
 *
 *************************************************************************
 */
int Log2(int n)
{
	int rsl = 0;
	while (n >>= 1) rsl++;
	return rsl;
}