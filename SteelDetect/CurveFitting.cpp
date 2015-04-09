//////////////////////////////////////////////////////////////////////////
/// @file CurveFitting.cpp
/// @brief 曲线拟合程序文件
/// @version 1.0
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2012, HUST, All Rights Reserved
// Author: Liu YuanZheng
// Created: 2012/08/30
////////////////////////////////////////////////////////////////////////////
/// @file $FILE_BASE$.$FILE_EXT$
/// @brief $end$
/// @version 1.0
////////////////////////////////////////////////////////////////////////////

#include "CurveFitting.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////
/// @namespace Eigen
/// @brief Eigen矩阵运算命名空间

CurveFitting::CurveFitting(void)
{
}

CurveFitting::~CurveFitting(void)
{
}

/////////////////////////////////////////////////////////////////////
///	@param [out] m_vPolynomial 存放拟合曲线系数向量的容器地址
///	@param [in] m_sampleData 采样点数据矩阵地址
/// @note 用最小二乘法拟合曲线,算法如下:
///
///		A (=cMatrix) =	|	E1		Exi		Exi^2	Exi^3	...	Exi^n	|
///			(n+1)*(n+1)	|	Exi		Exi^2	Exi^3	Exi^4	...	Exi^n+1	|
///			m			|	Exi^2	Exi^3	Exi^4	Exi^5	...	Exi^n+2	|
///			E(x)		|	...		...		...		...		...	...		|
///			i=0			|	Exi^n	Exi^n+1	Exi^n+2	Exi^n+3	...	Exi^n+n	|
/// 
///		B (len = n+1) (=m_polynomialParam) =
///			|	Eyi	Eyixi	Eyixi^2	Eyixi^3	...	Eyixi^n	|.transpose
/// 
///		拟合结果f(x)保存在m_polynomialParam中,记为C,则有:
///			C = A.inverse * B
///			C =	|	a0	a1	a2	...	an	|
///				( f(x)=a0+a1x+a2x^2+...anx^n )
bool CurveFitting::curveFittingbyLeastSquare(	int m_matrixCalParam,
												Vector2d &m_sampleData)
{
	return true;
}
/////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
///	@param [in] m_sampleData 采样点数据矩阵地址
///	@param [in] m_constraints 边界条件矩阵地址
///	@param [out] m_vPolynomia 存放拟合曲线系数向量的容器地址
/// @note 用有边界条件的最小二乘法拟合曲线,算法如下:
///
///		A1	(=cMatrixA1)	=	2*	|	E1		Exi		Exi^2	Exi^3	...		Exi^n	|
///			(n+1)*(n+1)				|	Exi		Exi^2	Exi^3	Exi^4	...		Exi^n+1	|
///				m-1					|	Exi^2	Exi^3	Exi^4	Exi^5	...		Exi^n+2	|
///				E(x)				|	...		...		...		...		...		...		|
///				i=1					|	Exi^n	Exi^n+1	Exi^n+2	Exi^n+3	...		Exi^n+n	|
/// 
///		A2	(=cMatrixA2)	=		|	1		1		0			0			0				0				|
///			(n+1)*6					|	x0		xm		1			1			0				0				|
///									|	x0^2	xm^2	2x0			2xm			2				2				|
///									|	x0^3	xm^3	3x0^2		3xm^2		6x0				6xm				|
///									|	...		...		...			...			...				...				|
///									|	x0^n	xm^n	nx0^(n-1)	nxm^(n-1)	n(n-1)x0^(n-2)	n(n-1)xm^(n-2)	|
/// 
///		A3	(=cMatrixA3)	( len = 6*(n+1) )	=	A3.transpose
///		A4	(=cMatrixA4)	( len = 6*4 )		=	Zero
/// 
///		A	(=cMatrixA)	=	|	A1	A2	|
///							|	A3	A4	|
/// 
///		B	(len = n+1+6)	(=m_polynomialParam)	=
///			|	2Eyi	2Eyixi	2Eyixi^2	2Eyixi^3	...	2Eyixi^n	h1	h2	h3	h4	h5	h6	|.transpose
/// 
///		h1 = f(0),		h2 = f(m)
///		h3 = f'(0),		h4 = f'(m)
///		h5 = f''(0),	h6 = f''(m)
/// 
///		拟合结果f(x)保存在m_polynomialParam中,记为C,则有:
///			C = A.inverse * B\n
///			C = |	a0	a1	a2	...	an	λ1	λ2	λ3	λ4	λ5	λ6	| 
///				( f(x)=a0+a1x+a2x^2+...anx^n )
//bool CurveFitting::curveFittingbyConstrainedLeastSquare(	int m_matrixCalParam,
//															Vector2d &m_sampleData, 
//															Vector2d &m_constraints)

//}
//////////////////////////////////////////////////////////////////
bool CurveFitting::curveFittingbyConstrainedLeastSquare(	int m_matrixCalParam,
															Vector2d &m_sampleData, 
															Vector2d &m_constraints)
{
	
	return true;
}

//////////////////////////////////////////////////////////////////
///	@param [in] startPoint 欲分析的采样点集的开头
///	@param [in] endPoint 欲分析的采样点集的结尾
///	@param [in] m_sampleData 采样点数据矩阵地址
///	@param [out] m_curvatureInfo 存放曲率信息的容器地址
///	@param [out] m_infltPoint 存放拐点信息的容器地址
/// @return float 这些点的平均曲率
/// @note 算法：先分别计算单个点的曲率，具体算法如下：
///
///					| f''(xm) |
///		km = -----------------------------
///				( 1+ ( f'(xm) )^2 )^1.5
///		其中，f(m) = a0 + a1x + a2x^2 + ... + amx^m
///		系数存在m_polynomialParam中，记为C：
///			C	=	|	a0	a1	a2	...	am	[...]	| 
///		计算avg(km)并返回
/// 
///		同时,将拐点信息用容器存放
///		拐点有临界值,处于上限与下限之间的连续点都被算入一个拐点段
///		将拐点段信息完成后压入容器中
//////////////////////////////////////////////////////////////////
float CurveFitting::averageCurvature(	int m_matrixCalParam,
										int startPoint,
										int endPoint,
										Vector2d &m_sampleData,
										vector <Vector2d> &m_curvatureInfo,
										vector <Vector2d> &m_infltPoint,
										vector <Vector2d> &m_firstD)
{
	return 1.0;
}
/////////////////////////////////////

float CurveFitting::getPointValue(int m_matrixCalParam,float x)
{
	
	return 1;
}

float CurveFitting::getPointFirstDerivative(int m_matrixCalParam,float x)
{
	
	return 1;
}

float CurveFitting::getPointSecondDerivative(int m_matrixCalParam,float x)
{
	
	return 2;
}

inline void CurveFitting::getStraight(float x1,float y1,float x2,float y2,float &k,float &b)
{
	k=(y1-y2)/(x1-x2);
	b=y1-k*x1;
}

Vector2d CurveFitting::getDisFromPointtoStraightCommon(Vector2d &m_sampleData,int factor)
{
	Vector2d dis;
	return dis;
}



float CurveFitting::cutCurveRegion(Vector2d sample)
{
	
	return 1;
}

Vector2d CurveFitting::getDisFromPointtoStraight(Vector2d &sample,float &k,float &b)
{
	return sample;
}

bool CurveFitting::median(Vector2d &m_sampleData,float &median_x,float &median_y)
{
	
	return true;
}

vector<int> CurveFitting::getFirstDerivativebyGaussianKernel(Vector2d &m_sampleData)
{
	vector<int> fea;
	
	

	return fea;
}


Vector2d CurveFitting::curveFittingbyGaussianKernel(Vector2d &m_sampleData)
{
	
	return m_sampleData;
}

//////////////////////////////////////////////////////////////////
///	@param [in] x 欲分析的采样点在采样点集中的位置
///	@param [in] m_sampleData 采样点数据矩阵地址
/// @return float 该点曲率
/// @note 计算单点曲率
///
///		算法：
///					| f''(xm) |
///		km = -----------------------------
///				( 1+ ( f'(xm) )^2 )^1.5
///		其中，f(m) = a0 + a1x + a2x^2 + ... + amx^m
///		系数存在m_polynomialParam中，记为C：
///			C	=	|	a0	a1	a2	...	am	[...]	| 
float CurveFitting::pointCurvature(	int m_matrixCalParam,
									int x,
									Vector2d &m_sampleData)
{
	
	return 1;
}
/////////////////////////////////////

///////////////////////////////////////////////////////////////////////
///	@param [out] dataPowMatrix 未经过计算的数据幂矩阵地址
///	@param [in] m_sampleData 采样点数据矩阵地址
/// @return bool 计算成功
/// @note 计算公共矩阵,算法如下：
///		A1	(=cMatrixA1)	=	2*	|	E1		Exi		Exi^2	Exi^3	...		Exi^n	|
///			(n+1)*(n+1)				|	Exi		Exi^2	Exi^3	Exi^4	...		Exi^n+1	|
///			m-1						|	Exi^2	Exi^3	Exi^4	Exi^5	...		Exi^n+2	|
///			E(x)					|	...		...		...		...		...		...		|
///			i=1						|	Exi^n	Exi^n+1	Exi^n+2	Exi^n+3	...		Exi^n+n	|
bool CurveFitting::getDataPowMatrix(	int m_matrixCalParam,
										Vector2d &dataPowMatrix,
										Vector2d &m_sampleData)
{
	
	return true;
}
//////////////////////////////////////////////


void CurveFitting::callCL(){
	

}