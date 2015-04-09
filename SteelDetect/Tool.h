#pragma once
/*
（1）Discover and initialize the platforms

调用两次clGetPlatformIDs函数，第一次获取可用的平台数量，第二次获取一个可用的平台。
（2）Discover and initialize the devices
调用两次clGetDeviceIDs函数，第一次获取可用的设备数量，第二次获取一个可用的设备。

（3）Create  a context(调用clCreateContext函数）

上下文context可能会管理多个设备device。

（4）Create a command queue(调用clCreateCommandQueue函数）

一个设备device对应一个command queue。

上下文conetxt将命令发送到设备对应的command queue，设备就可以执行命令队列里的命令。

（5）Create device buffers(调用clCreateBuffer函数）

Buffer中保存的是数据对象，就是设备执行程序需要的数据保存在其中。

Buffer由上下文conetxt创建，这样上下文管理的多个设备就会共享Buffer中的数据。

（6）Write host data to device buffers(调用clEnqueueWriteBuffer函数）

（7）Create and compile the program

创建程序对象，程序对象就代表你的程序源文件或者二进制代码数据。

（8）Create the kernel(调用clCreateKernel函数）

根据你的程序对象，生成kernel对象，表示设备程序的入口。
（9）Set the kernel arguments(调用clSetKernelArg函数）

（10）Configure the work-item structure(设置worksize）

　配置work-item的组织形式（维数，group组成等）

（11）Enqueue the kernel for execution(调用clEnqueueNDRangeKernel函数）

将kernel对象，以及 work-item参数放入命令队列中进行执行。
（12）Read  the output buffer back to the host(调用clEnqueueReadBuffer函数）

（13）Release OpenCL resources（至此结束整个运行过程）

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

