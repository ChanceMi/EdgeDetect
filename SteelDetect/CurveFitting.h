//////////////////////////////////////////////////////////////////////////
/// @file CurveFitting.h
/// @brief 曲线拟合头文件
/// @version 1.0
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2012, HUST, All Rights Reserved
// Author: Liu YuanZheng
// Created: 2012/08/30
///
/// @file $FILE_BASE$.$FILE_EXT$
/// @brief $end$
/// @version 1.0
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>


//////////////////////////////////////////////////////////////////////////
/// @namespace std
/// @brief Windows标准命名空间
using namespace std;

//////////////////////////////////////////////////////////////////////////
/// @defgroup SampleAnalysis 采样分析
/// @brief 采样分析模块
/// @{

typedef vector<vector<double> > Vector2d ;

//////////////////////////////////////////////////////////////////////////
/// @class CurveFitting
/// @brief 该类用多项式进行曲线拟合，支持任意次多项式；并在此基础上可以计算曲线的曲率、平均曲率、拐点。
class CurveFitting
{

public:
	Vector2d m_polynomialParam;		///< 曲线拟合数据,为(a0,a1,a2,...,an | f(x)=a0+a1x+a2x^2+...anx^n)
	Vector2d m_infltPointVector;	///< 拐点组
	Vector2d m_curvInfo;			///< 曲率半径信息(分别为横坐标,曲率半径)
	Vector2d m_firstDer;            ///< 一阶导数

	CurveFitting(void);
	~CurveFitting(void);

	bool curveFittingbyLeastSquare(	int m_matrixCalParam,
									Vector2d &m_sampleData);				///< 用最小二乘法生成多项式拟合曲线
	Vector2d curveFittingbyGaussianKernel(Vector2d &m_sampleData);    ///< 用高斯核进行曲线拟合
	vector<int> getFirstDerivativebyGaussianKernel(Vector2d &m_sampleData);    ///<用高斯核获得极值点
	bool curveFittingbyConstrainedLeastSquare(	int m_matrixCalParam,
												Vector2d &m_sampleData, 
												Vector2d &m_constraints);	///< 用带有边界条件的最小二乘法生成多项式拟合曲线
	float averageCurvature(	int m_matrixCalParam,
							int startPoint,
							int endPoint,
							Vector2d &m_sampleData,
							vector <Vector2d> &m_curvatureInfo,
							vector <Vector2d> &m_infltPoint,
							vector <Vector2d> &m_firstD);	///< 计算拟合曲线的若干点平均曲率
	float pointCurvature(	int m_matrixCalParam,
							int x,
							Vector2d &m_sampleData);			///< 计算拟合曲线的某指定点曲率

	float getPointValue(int m_matrixCalParam,
						float x);                               ///< 获得拟合曲线某指定点的值
	float getPointFirstDerivative(int m_matrixCalParam,
								  float x);                     ///< 获得拟合曲线某指定点的一阶导值
	float getPointSecondDerivative(int m_matrixCalParam,
								   float x);                     ///< 获得拟合曲线某指定点的二阶导值
	Vector2d getDisFromPointtoStraight(Vector2d &sample,float &k,float &b);
	Vector2d getDisFromPointtoStraightCommon(Vector2d &m_sampleData,int factor);
	float cutCurveRegion(Vector2d sample);
private:

	bool getDataPowMatrix(	int m_matrixCalParam,
							Vector2d &dataPowMatrix,
							Vector2d &m_sampleData);			///< 求出生成拟合曲线时的一个公共矩阵(数据幂矩阵)
	bool median(Vector2d &m_sampleData,float &median_x,float &median_y);
	inline void getStraight(float x1,float y1,float x2,float y2,float &k,float &b);

	void callCL();
};

/// @}