#ifndef _GLOBAL_API
#define _GLOBAL_API

#include <windows.h>
#include <math.h>
using namespace std;
typedef unsigned char uchar;
// 小波变换函数原型
// Daubechies紧致正交小波基
// 不同支撑区间长度下的滤波器系数如下
const double hCoef[10][20] =
{
	{ .707106781187,  .707106781187},

	{ .482962913145,  .836516303738,  .224143868042, -.129409522551 },

	{ .332670552950,  .806891509311,  .459877502118, -.135011020010, -.085441273882,  .035226291882 },

	{ .230377813309,  .714846570553,  .630880767930, -.027983769417,
	 -.187034811719,  .030841381836,  .032883011667, -.010597401785 },

	 //coif1小波系数
	{ -0.0157   ,-0.0727    ,0.3849    ,0.8526    ,0.3379   ,-0.0727 },

	//sym4 小波系数
	{ -0.0758  ,-0.0296  ,0.4976  ,0.8037  ,0.2979  ,-0.0992  ,-0.0126  ,0.0322 },

	{ .077852054085,  .396539319482,  .729132090846,  .469782287405, -.143906003929,
	 -.224036184994,  .071309219267,  .080612609151, -.038029936935, -.016574541631,
	  .012550998556,  .000429577973, -.001801640704,  .000353713800 },

	{ .054415842243,  .312871590914,  .675630736297,  .585354683654, -.015829105256,
	 -.284015542962,  .000472484574,  .128747426620, -.017369301002, -.044088253931,
	  .013981027917,  .008746094047, -.004870352993, -.000391740373,  .000675449406,
	 -.000117476784 },

	{ .038077947364,  .243834674613,  .604823123690,  .657288078051,  .133197385825,
	 -.293273783279, -.096840783223,  .148540749338,  .030725681479, -.067632829061,
	  .000250947115,  .022361662124, -.004723204758, -.004281503682,  .001847646883,
	  .000230385764, -.000251963189,  .000039347320 },

	{ .026670057901,  .188176800078,  .527201188932,  .688459039454,  .281172343661,
	 -.249846424327, -.195946274377,  .127369340336,  .093057364604, -.071394147166,
	 -.029457536822,  .033212674059,  .003606553567, -.010733175483,  .001395351747,
	  .001992405295, -.000685856695, -.000116466855,  .000093588670, -.000013264203 }
};

//BOOL DWT_1D(double* pDbSrc, int nMaxLevel,int nDWTSteps, int nInv, int nStep, int nSupp);
//BOOL DWTStep_1D(double* pDbSrc, int nCurLevel,int nInv, int nStep, int nSupp);
//BOOL DWTStep_1D(double* pDbSrc, int nCurlength, int nInv, int nStep,int nSupp);
BOOL DWTStep_1D(double* data, int nCurlength, int nInv, int nStep,int m_nSupp);
//BOOL DWT_2D(double* pDbSrc, int nMaxWLevel, int nMaxHLevel, int nDWTSteps, int nInv, int nStep, int nSupp);
//BOOL DWTStep_2D(double* pDbSrc, int nCurWLevel, int nCurHLevel,	int nMaxWLevel, int nMaxHLevel, int nInv, int nStep, int nSupp);
BOOL DWTStep_2D(double* pDbSrc, int nCurW, int nCurH, int nMaxW, int nMaxH, int nInv, int nStep, int nSupp);
//BOOL ImageDWT(LPBYTE lpImage, int nMaxWLevel, int nMaxHLevel,int nDWTSteps, int nInv, int nStep, int nSupp);
int Log2(int n);
uchar FloatToByte(double f);
char FloatToChar(double f);


#endif