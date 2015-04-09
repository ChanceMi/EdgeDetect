#pragma once
/*
��1��Discover and initialize the platforms

��������clGetPlatformIDs��������һ�λ�ȡ���õ�ƽ̨�������ڶ��λ�ȡһ�����õ�ƽ̨��
��2��Discover and initialize the devices
��������clGetDeviceIDs��������һ�λ�ȡ���õ��豸�������ڶ��λ�ȡһ�����õ��豸��

��3��Create  a context(����clCreateContext������

������context���ܻ�������豸device��

��4��Create a command queue(����clCreateCommandQueue������

һ���豸device��Ӧһ��command queue��

������conetxt������͵��豸��Ӧ��command queue���豸�Ϳ���ִ���������������

��5��Create device buffers(����clCreateBuffer������

Buffer�б���������ݶ��󣬾����豸ִ�г�����Ҫ�����ݱ��������С�

Buffer��������conetxt���������������Ĺ���Ķ���豸�ͻṲ��Buffer�е����ݡ�

��6��Write host data to device buffers(����clEnqueueWriteBuffer������

��7��Create and compile the program

����������󣬳������ʹ�����ĳ���Դ�ļ����߶����ƴ������ݡ�

��8��Create the kernel(����clCreateKernel������

������ĳ����������kernel���󣬱�ʾ�豸�������ڡ�
��9��Set the kernel arguments(����clSetKernelArg������

��10��Configure the work-item structure(����worksize��

������work-item����֯��ʽ��ά����group��ɵȣ�

��11��Enqueue the kernel for execution(����clEnqueueNDRangeKernel������

��kernel�����Լ� work-item����������������н���ִ�С�
��12��Read  the output buffer back to the host(����clEnqueueReadBuffer������

��13��Release OpenCL resources�����˽����������й��̣�

*/
#include <CL/cl.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ctime>
#include <opencv2/opencv.hpp>
#include <OpenVideo/OVDecode.h>
#include <OpenVideo/OVDecodeTypes.h>
#include <QImage>

using namespace  std;

struct my_uint4{cl_uint u32[4];};
struct my_uint2{cl_uint u32[2];};

const bool PROFILE = false;

const cl_uint ALLOC_TILE = 0;
const cl_uint ALLOC_HORZ = 1;
const cl_uint ALLOC_VERT =2;

const cl_uint   MASK_WIDTH = 3;        	/**< mask dimensions */
const cl_uint   MASK_HEIGHT = 3;       	/**< mask dimensions */

// problem size for 1D algorithm and width of problem size for 2D algorithm

/** convert the kernel file into a string */
int convertToString(const char *filename, std::string& s);

/**Getting platforms and choose an available one.*/
int getPlatform(cl_platform_id &platform);
/**Step 2:Query the platform and choose the first GPU device if has one.*/
cl_device_id *getCl_device_id(cl_platform_id &platform);

cl_mem loadImage(cl_context context,string fileName, int &width, int &height);

int main_func_image(string filename);


void cvDisplay(IplImage* image, char windowName[], int x=0,int y=0);

IplImage* clArrayToImage(cl_uint* output,int width,int height);
cv::Mat clArrayToMat(cl_uint* output,int width,int height);
void cvMatToCvImage(IplImage* cvImg,cv::Mat matImg);

void cvGenerateIntensityImage(my_uint4* clArrRaw,cl_uint* clArrIntensity,int height,int width);
void cvGenerateSobelImage(cl_uint* clArrIntensity,cl_uint* clArrSobel,int height,int width);

void clInitialize(void);
void clInitializeHost(IplImage* cvRawImg);

void clCleanup(void);
void clCleanupHost(void);

cv::Mat sobelChangeMainFunction(string &filename);

int video_main_function();

