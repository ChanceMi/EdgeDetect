#define  CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include "Tool.h"
#include <QDebug>


using namespace std;
//const char cl_kernel_filename[] = "kernel.cl";
const int num_pixels_per_work_item = 32;
//static int num_iterations = 1000;
const int MAX_DEVICES = 128;

cl_context context;
cl_command_queue commandQueue;
cl_program program ;
cl_device_id device;

cl_kernel kernel = 0;
cl_device_id *devices;
cl_mem   inputBuffer;
cl_mem	 intermediateBuffer;
cl_mem	 outputBuffer;
cl_mem	 sobelOpXBuffer;
cl_mem	 sobelOpYBuffer;

cl_uint width;
cl_uint height;

			// Input data is stored here.
cl_uint  *intermediate;			// Output data is stored here.
cl_uchar  *output;				// Output data is stored here.

//// Sobel Operators are stored here.
cl_uint clSobelOpX[9] = {-1, 0, 1,
	-2, 0, 2,
	-1, 0, 1};
cl_uint clSobelOpY[9] = { 1, 2, 1,
	0, 0, 0,
	-1,-2,-1};

cl_uchar *input;
cl_uint clSobelOpXX[3][3] = {{-1, 0, 1},
{-2, 0, 2},
{-1, 0, 1}};
cl_uint clSobelOpYY[3][3] = {{1, 2, 1},
{0, 0, 0},
{-1,-2,-1}};

int convertToString(const char *filename, std::string& s){
	size_t size;
	char* str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));

	if(f.is_open()){
		size_t fileSize;
		f.seekg(0,std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0,std::fstream::beg);
		str = new char[size+1];
		if(!str){
			f.close();
			return 0;
		}

		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
		s = str;
		delete[] str;
		return 0;
	}

	cout<<"Error: failed to open file \n:"<<filename<<endl;
	return -1;
}


cl_context createContext(){
   cl_int errNum;
   cl_uint numPlatforms;
   cl_platform_id firstPlatformId;
   cl_context context = NULL;

   /////////////////////
   /*
   errNum = clGetPlatformIDs(0, 0, &num_devices);
   std::vector<cl_platform_id> platforms(num_devices);
   errNum = clGetPlatformIDs(numPlatforms, &platforms[0], &numPlatforms);

   if(errNum != CL_SUCCESS || numPlatforms <= 0){
	   std::cerr << "Failed to find any OpenCl platforms."<<std::endl;
	   return;
   }
   //查询device信息
   errNum=clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_ALL,0,0,&num_devices);
   errNum=clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_ALL,MAX_DEVICES,devices,&num_devices);

   //checkStatus(errNum);
   printf("Found Devices:\t\t%d\n", num_devices);

   for (int i = 0; i < num_devices; i++) {
	   printf("\nDevice: %d\n\n", i);

	   clGetDeviceInfo(devices[i], CL_DEVICE_TYPE, sizeof(dev_type),
		   &dev_type, &ret_size);
	   printf("\tDevice Type:\t\t");

	   */
   ///////////////////////////

   errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
   if(errNum != CL_SUCCESS || numPlatforms <= 0){
	   std::cerr << "Failed to find any OpenCl platforms."<<std::endl;
	   return NULL;
   }

   cl_context_properties contextProperties[] ={
	   CL_CONTEXT_PLATFORM,
	   (cl_context_properties)firstPlatformId,
	   0
   };

   context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU,
	                                 NULL,NULL,&errNum);

   if(errNum != CL_SUCCESS){
	   std::cout <<"Could not create GPU context, trying cpu.."<< std::endl;
	   context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU,
		                                 NULL,NULL,&errNum);
	   if(errNum != CL_SUCCESS){
		   std::cerr<< "Failed to create an Opencl context."<<std::endl;
		   return NULL;
	   }
   }
   return context;
}

cl_command_queue createCommandQueue(cl_context context, cl_device_id *device){
	cl_int errNum;
	cl_device_id *devices;
	cl_command_queue commandQueue = NULL;
	size_t deviceBufferSize = -1;

	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0 ,NULL,&deviceBufferSize);
	if(errNum != CL_SUCCESS){
		std::cerr<< "Failed call to clGetContextInfo"<<std::endl;
		return NULL;
	}

	if(deviceBufferSize <= 0){
		std::cerr<<"No devices available";
		return NULL;
	}

	devices = new cl_device_id[deviceBufferSize /sizeof(cl_device_id)];
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize,devices,NULL);
	if(errNum != CL_SUCCESS){
		std::cerr <<"Failed to get device IDs";
		return NULL;
	}
	//commandQueue = clCreateCommandQueue(context, devices[0],0,NULL);
	commandQueue = clCreateCommandQueue(context,devices[0],CL_QUEUE_PROFILING_ENABLE,NULL);
	if(commandQueue == NULL){
		std::cerr << "Failed to create commandQueue for device 0";
		return NULL;
	}

	*device = devices[0];
	delete[] devices;
	return commandQueue;
}

cl_program createProgram(cl_context context, cl_device_id device, const char* fileName){
	cl_int errNum;
	cl_program program;

	std::ifstream kernelFile(fileName, std::ios::in);
	if(!kernelFile.is_open()){
		std::cerr<<"Failed to open file for reading:"<<fileName<<std::endl;
		return NULL;
	}
	std::ostringstream oss;
	oss<<kernelFile.rdbuf();

	std::string srcStdStr = oss.str();
	const char* srcStr = srcStdStr.c_str();
	program = clCreateProgramWithSource(context, 1, (const char**)&srcStr,
		                                NULL,NULL);
	if(program == NULL){
		std::cerr<<"Failed to create CL program from source."<<std::endl;
		return NULL;
	}

	errNum = clBuildProgram(program, 0, NULL,NULL,NULL,NULL);
	if(errNum != CL_SUCCESS){
		char buildLog[16384];
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
			                  sizeof(buildLog), buildLog, NULL);

		std::cerr<<"Error in kernel: "<<std::endl;
		std::cerr<<buildLog;
		clReleaseProgram(program);
		return NULL;
	}

	return program;
}

void cleanUp(cl_context context, cl_command_queue commandQueue,
	         cl_program program, cl_kernel kernel, cl_mem imageObjects[2],
			 cl_sampler sampler){
	 for(int i=0; i<2;i++){
		 if(imageObjects[i] != 0){
			 if(imageObjects[i] !=0){
				 clReleaseMemObject(imageObjects[i]);
			 }
		 }
	 }
	 if(commandQueue !=0){
		 clReleaseCommandQueue(commandQueue);
	 }
	 if (kernel != 0)
		 clReleaseKernel(kernel);

	 if (program != 0)
		 clReleaseProgram(program);

	 if (sampler != 0)
		 clReleaseSampler(sampler);

	 if (context != 0)
		 clReleaseContext(context);

}

/**Getting platforms and choose an available one.*/
int getPlatform(cl_platform_id &platform){
	platform = NULL;//the chosen platform

	cl_uint numPlatforms;
	cl_int status = clGetPlatformIDs(0,NULL,&numPlatforms);
	if(status != CL_SUCCESS){
		cout<<"Error: Getting platforms !"<<endl;
		return -1;
	}

	if(numPlatforms >0){
		cl_platform_id* platforms = (cl_platform_id* )malloc(numPlatforms * sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms,platforms, NULL);
		platform = platforms[0];
		free(platforms);
		return -1;//
	}else {
		return -1;
	}

}

//查询设备集
cl_device_id *getCl_device_id(cl_platform_id &platform){
	cl_uint numDevices = 0;
	cl_device_id *devices= NULL;
	cl_int status = clGetDeviceIDs(platform,CL_DEVICE_TYPE_GPU,0,NULL,&numDevices);
	if(numDevices >0){
		devices = (cl_device_id *)malloc(numDevices*sizeof(cl_device_id));
		status = clGetDeviceIDs(platform,CL_DEVICE_TYPE_GPU, numDevices, devices ,NULL);
	}
	return devices;
}

//将图像加载到buffer中
cl_mem loadImage(cl_context context,string fileName, int &width, int &height){
	cv::Mat image = cv::imread(fileName); 
	width = image.cols;
	height = image.rows;

	cv::Mat dstImg(height,width,CV_8UC1);
	cvtColor(image,dstImg,CV_BGR2BGRA);
	
	unsigned char *buffer = new unsigned char[width*height];
	//memcpy(buffer,dstImg.data,width*height*3);
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			//CvScalar colorValue = cvGet2D(&image,y,x);
			int index = (y*width)+x;
			buffer[index]=image.ptr<uchar>(y)[x];
			//qDebug("%d/t",imageArray[index]);
		}
	}
	// create opencl image;
	cl_image_format clImageFormat;
	clImageFormat.image_channel_order = CL_R;
	clImageFormat.image_channel_data_type = CL_UNSIGNED_INT8;

	cl_int errNum;
	cl_mem clImage = NULL;
	
	clImage = clCreateImage2D(context,
		                      CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR,
							  &clImageFormat,
							  width,
							  height,
							  0,
							  buffer,
							  &errNum);
    
	if(errNum != CL_SUCCESS){
		std::cerr<< "Error creating CL image object"<< std::endl;
		return 0;
	}

	/////////
	cv::Mat test = cv::Mat(height,width,CV_8UC1,buffer);
	cv::imshow("test",test);

	return clImage;
}

//save an image using 
bool saveImage(string &fileName, unsigned char *buffer, int width, int height){
	//cv::Mat resultImg = cv::Mat(width,height,CV_8UC4,buffer);
	//cv::imshow("resultImg",resultImg);
	//QImage image(buffer,width,height,QImage::Format_ARGB32);
	return true;
	//how to convert buffer to file
}

//分组与



//////////////////////////////////////////////////////////////////////////
//filter
int main_func_image(string filename){
	
	cl_mem imageObjects[2] = {0,0};
	cl_sampler sampler = 0;
	cl_int errNum;
	size_t numGroups;

	context = createContext();
	if (context == NULL)
	{
		std::cerr << "Failed to create OpenCL context." << std::endl;
		return 1;
	}

	commandQueue = createCommandQueue(context, &device);
	if (commandQueue == NULL)
	{
	    cleanUp(context, commandQueue, program, kernel, imageObjects, sampler);
		return 1;
	}

	//cl_bool imageSupport = CL_FALSE;
	
	
	int width, height;
	imageObjects[0] = loadImage(context, filename, width, height);
	if(imageObjects[0] == 0){
		std::cerr<<"Error loading: "<< filename<<std::endl;
		cleanUp(context,commandQueue,program, kernel, imageObjects, sampler);
		return 1;
	}

	cl_image_format clImageFormat;
	clImageFormat.image_channel_order = CL_R;
	clImageFormat.image_channel_data_type = CL_UNSIGNED_INT8;
	imageObjects[1] = clCreateImage2D(context,
		                              CL_MEM_WRITE_ONLY,
									  &clImageFormat,
									  width,
									  height,
									  0,
									  NULL,
									  &errNum);
	if(errNum !=CL_SUCCESS){
		std::cerr<<"Error creating CL output image object."<<std::endl;
		cleanUp(context,commandQueue,program,kernel,imageObjects, sampler);
		return 1;
	}

	sampler = clCreateSampler(context,
		                      CL_FALSE,//no-normalized coord
							  CL_ADDRESS_CLAMP_TO_EDGE,
							  CL_FILTER_NEAREST,//用最近的数
							  &errNum);
	if(errNum != CL_SUCCESS){
		std::cerr<<"Error create cl sample object."<<std::endl;
		cleanUp(context,commandQueue,program,kernel,imageObjects,sampler);
		return 1;
	}

	program = createProgram(context,device,"kernel.cl");
	if(program == NULL){
		cleanUp(context, commandQueue, program, kernel, imageObjects, sampler);
		return 1;
	}

	kernel = clCreateKernel(program, "gaussianFilter", NULL);
	if(kernel == NULL){
		std::cerr <<" Failed to create kernel"<< std::endl;
		cleanUp(context, commandQueue, program, kernel ,imageObjects, sampler);
		return 1;
	}
	errNum = clSetKernelArg(kernel, 0 , sizeof(cl_mem),&imageObjects[0]);
	errNum |=clSetKernelArg(kernel, 1, sizeof(cl_mem),&imageObjects[1]);
	errNum |= clSetKernelArg(kernel, 2, sizeof(cl_sampler), &sampler);
	errNum |= clSetKernelArg(kernel, 3, sizeof(cl_int), &width);
	errNum |= clSetKernelArg(kernel, 4, sizeof(cl_int), &height);
	if (errNum != CL_SUCCESS)
	{
		std::cerr << "Error setting kernel arguments." << std::endl;
		cleanUp(context, commandQueue, program, kernel, imageObjects, sampler);
		return 1;
	}

	/*
	char *ssBuffer = (char*) clEnqueueMapImage(commandQueue,imageObjects[1],
		                                      CL_TRUE,
											  CL_MAP_READ,origin,
											  region, &rowPitch,
											  NULL,0,NULL,NULL,&errNum);
     clEnqueueUnmapMemObject(commandQueue, imageObjects[1],
	                         buffer,0,NULL,NULL);

	*/

	size_t localWorkSize[2];
	size_t globalWorkSize[2];

	cl_int supportGroupSize = 0;
	clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(cl_int),
		&supportGroupSize,NULL);

	{
		//索引空间即为
		size_t gsize[2];
		int w;
		if(supportGroupSize <=256){
			gsize[0] = 16;//128
			gsize[1] = supportGroupSize/16;
		}else if(supportGroupSize <=1024){
			gsize[0] =supportGroupSize/16;
			gsize[1] = 16;
		}else {
			gsize[0] = supportGroupSize/32;
			gsize[1] = 32;
		}
		localWorkSize[0] = gsize[0];
		localWorkSize[1] = gsize[1];

		w=(width+num_pixels_per_work_item-1)/num_pixels_per_work_item;
		globalWorkSize[0] =((w+gsize[0]-1)/gsize[0]);
		globalWorkSize[1] =((height +gsize[1] -1)/gsize[1]);

		numGroups = globalWorkSize[0]*globalWorkSize[1];
		globalWorkSize[0] = globalWorkSize[0]*gsize[0];
		globalWorkSize[1] = globalWorkSize[1]*gsize[1];

	}

	// Queue the kernel up for execution
	cl_int status;
	cl_event events[2];
	errNum = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL,
		globalWorkSize, localWorkSize,
		0, NULL, &events[0]);
	if (errNum != CL_SUCCESS)
	{
		std::cerr << "Error queuing kernel for execution." << std::endl;
		cleanUp(context, commandQueue, program, kernel, imageObjects, sampler);
		return 1;
	}

	status =clFinish(commandQueue);
	status = clWaitForEvents(1,&events[0]);
	unsigned char* buffer = new unsigned char[width*height];
	size_t origin[3] = {0,0,0};
	size_t region[3] = {width, height, 1};

	//cl_int clEnqueueReadImage (	cl_command_queue command_queue,cl_mem image,cl_bool blocking_read,const size_t origin[3],
	//	const size_t region[3],size_t row_pitch,size_t slice_pitch,void *ptr,cl_uint num_events_in_wait_list,
	//	const cl_event *event_wait_list,cl_event *event)
	errNum = clEnqueueReadImage(commandQueue, imageObjects[1], CL_TRUE,
		                        origin, region, 0,0,buffer,
								0, NULL,NULL);
	if(errNum != CL_SUCCESS){
		std::cerr << "Error reading result buffer." << std::endl;
		cleanUp(context, commandQueue, program, kernel, imageObjects, sampler);
		return 1;
	}
	cv::Mat testResult = cv::Mat(height,width,CV_8UC1,buffer);
	cv::imshow("testResult",testResult);

	std::cout <<std::endl;
	std::cout <<"Executed program succesfully."<<std::endl;
	/*
	if(!saveImage(filename,buffer,width,height)){
		std::cerr<<"Error writing output image:"<<std::endl;
		cleanUp(context, commandQueue, program, kernel, imageObjects, sampler);
		delete [] buffer;
		return 1;
	}
	*/
	delete[] buffer;
	cleanUp(context, commandQueue, program, kernel, imageObjects, sampler);
	return 1;
};


//此只有一个通道
IplImage* clArrayToCvImage(cl_uchar* output, int resultWidth, int resultHeight){
	CvSize size;
	size.width = resultWidth;
	size.height = resultHeight;
	IplImage* resultImg = cvCreateImage(size,IPL_DEPTH_8U,1);
	for(int y=0;y<resultHeight;y++){
		for(int x=0;x<resultWidth;x++){
			//CvScalar colorSelect;
			int index = (y*resultWidth)+x;
			//colorSelect.val[0] = output[index];
			//cvSet2D(resultImg,y,x,colorSelect);
			resultImg->imageData[index]=output[index];
			//qDebug("%d\t",output[index]);
		}
	}

	//IplImage* rresultImg = cvCreateImage(size,IPL_DEPTH_8U,1);
	//cvCanny(resultImg,rresultImg,30,100);
	cvShowImage("IplImage sobel",resultImg);
	return resultImg;

}

cv::Mat clArrayToMat(cl_uchar* output,int width,int height){
	cv::Mat dstImg =cv::Mat(height,width,CV_8UC1);
	//IplImage* resultImg = cvCreateImage(size,IPL_DEPTH_8U,1);
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			int index = (y*width)+x;
			dstImg.ptr<uchar>(y)[x]=output[index];
			//qDebug("%d/t",imageArray[index]);
		}
	}
	cv::imshow("Tool",dstImg);
	return dstImg;
}
cv::Mat clArrayToMat(cl_uint* output,int width,int height){
	cv::Mat dstImg =cv::Mat(height,width,CV_8UC1);
	//IplImage* resultImg = cvCreateImage(size,IPL_DEPTH_8U,1);
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			int index = (y*width)+x;
			dstImg.ptr<uchar>(y)[x]=output[index];
			//qDebug("%d/t",imageArray[index]);
		}
	}
	cv::imshow("Tool",dstImg);
	return dstImg;
}

void MatToCvImage(IplImage* cvImg,cv::Mat &mat){
	//mat = cv::Mat(cvImg);//共享数据
	int width = cvImg->width;
	int height = cvImg->height;
	mat.create(height,width,CV_8UC4);
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			CvScalar colorSelect;
			colorSelect=cvGet2D(cvImg,y,x);
			cvSet2D(&mat,y,x,colorSelect);
		}
	}
}



//cv的方法
void cvGenerateSobelImage(){
	cl_uint Gx;
	cl_uint Gy;
	cl_uchar *clArrSobel = (cl_uchar*)malloc(sizeof(cl_uchar)*width*height);
	//generate sobel image;
	for(int y=1;y<height-1;y++){
		for(int x=1;x<width-1;x++){
			int index = (y*width)+x;
			Gx = (input[index-width-1]*clSobelOpX[0])+
				 (input[index-width]* clSobelOpX[1])+
				 (input[index-width+1]*clSobelOpX[2])+
				 (input[index-1]*clSobelOpX[3])+
				 (input[index]*clSobelOpX[4])+
				 (input[index+1]*clSobelOpX[5])+
				 (input[index+width-1])*clSobelOpX[6]+
				 (input[index+width])*clSobelOpX[7]+
				 (input[index+width+1]*clSobelOpX[8]);

			Gx = abs((float)Gx);

			Gy = (input[index-width-1]*clSobelOpY[0])+
				(input[index-width]* clSobelOpY[1])+
				(input[index-width+1]*clSobelOpY[2])+
				(input[index-1]*clSobelOpY[3])+
				(input[index]*clSobelOpY[4])+
				(input[index+1]*clSobelOpY[5])+
				(input[index+width-1])*clSobelOpY[6]+
				(input[index+width])*clSobelOpY[7]+
				(input[index+width+1]*clSobelOpY[8]);

			Gy = abs((float)Gy);
			
			clArrSobel[index] = Gx+Gy;
			
			//qDebug("%d\t",clArrSobel[index]);
			
		}
		
	}
	clArrayToCvImage(clArrSobel,width,height);
	free(clArrSobel);

}

void checkStatus(cl_uint status){
	if(status == CL_INVALID_PROGRAM)
	{
		//if program is not a valid program object.
		std::cout<<"Error: Invalid program object. (clBuildProgram)\n";
		return;
	}
	if(status == CL_INVALID_VALUE)
	{
		// (device_list == NULL) && (num_devices > 0)
		// ||
		// (device_list != NULL) && (num_devices ==0)
		// ||
		// (pfn_notify == NULL) && (user_data != NULL)
		std::cout<<"Error: Invalid value - device_list==NULL:" << (devices==NULL) << " - (clBuildProgram)\n";
		return;
	}
	if(status == CL_INVALID_DEVICE)
	{
		// OpenCL devices listed in device_list are not in the list of
		// devices associated with program.
		std::cout<<"Error: Invalid device. (clBuildProgram)\n";
		return;
	}
	if(status == CL_INVALID_BINARY)
	{
		// if program is created with clCreateWithProgramBinary and
		// devices listed in device_list do not have a valid program binary loaded.
		std::cout<<"Error: Invalid binary. (clBuildProgram)\n";
		return;
	}
	if(status == CL_INVALID_BUILD_OPTIONS)
	{
		// if the build options specified by options are invalid
		std::cout<<"Error: Invalid build options. (clBuildProgram)\n";
		return;
	}
	if(status == CL_INVALID_OPERATION)
	{
		// if the build of a program executable for any of the devices
		// listed in device_list by a previous call to clBuildProgram for program has not
		// completed
		// ||
		// if there are kernel objects attached to program.
		std::cout<<"Error: Invalid operation. (clBuildProgram)\n";
		return;
	}
	if(status == CL_COMPILER_NOT_AVAILABLE)
	{
		// CL_COMPILER_NOT_AVAILABLE if program is created with
		// clCreateProgramWithSource and a compiler is not available i.e.
		// CL_DEVICE_COMPILER_AVAILABLE specified in table 4.3 is set to CL_FALSE.
		std::cout<<"Error: Compiler not available. (clBuildProgram)\n";
		return;
	}
	if(status == CL_BUILD_PROGRAM_FAILURE)
	{
		// if there is a failure to build the program executable.
		// This error will be returned if clBuildProgram does not return until the build has
		// completed.
		std::cout<<"Error: Build program failure. (clBuildProgram)\n";
		return;
	}
	if(status == CL_OUT_OF_HOST_MEMORY)
	{
		// if there is a failure to allocate resources required by the
		// OpenCL implementation on the host.
		std::cout<<"Error: Out of host memory. (clBuildProgram)\n";
		return;
	}
}



double clRunKernels(cl_uint alloc_type, cl_uint kernelCount){
	double runTime;
	cl_int status;
	cl_event events[2];

	size_t globalThreads[1];
	size_t localThreads[1];


	globalThreads[0] = kernelCount;
	localThreads[0] = 64;
	status = clSetKernelArg(kernel,0,sizeof(cl_mem),(void*)&inputBuffer);
	status |=clSetKernelArg(kernel, 1, sizeof(cl_mem),(void *)&intermediateBuffer);
	status |= clSetKernelArg(kernel, 2, sizeof(cl_sampler), (void *)&outputBuffer);
	status |= clSetKernelArg(kernel, 3, sizeof(cl_int), (void *)&sobelOpXBuffer);
	status |= clSetKernelArg(kernel, 4, sizeof(cl_int), (void *)&sobelOpYBuffer);

	cl_uint2 inputOutputDim = {width, height};
	status |= clSetKernelArg(kernel, 5, sizeof(cl_uint2), (void *)&inputOutputDim );
	status |= clSetKernelArg(kernel, 6, sizeof(cl_uint), (void *)&alloc_type);
	if(status != CL_SUCCESS){
		std::cout<<"Error: setting kernel argument";
		return -1;
	}

	cl_int supportGroupSize = 0;
	clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(cl_int),
		&supportGroupSize,NULL);//128
	clock_t start,finish;
	start = clock(); // clock 为毫秒级
	
	status = clEnqueueNDRangeKernel(commandQueue,kernel,1,NULL,globalThreads,localThreads,0,
		                           NULL,&events[0]);
	if(status != CL_SUCCESS) 
	{ 
		std::cout<<"Error: Enqueueing kernel onto command queue. (clEnqueueNDRangeKernel)\n";
		return -1;
	}
	//status = clFlush(commandQueue);
	status =clFinish(commandQueue);
	finish = clock();
	double cvRunTime = (finish - start)/CLOCKS_PER_SEC;//secs
	printf("\nOpenCV Runtime:\t%f\n\n", cvRunTime);

	
	//status = clWaitForEvents(1,&events[0]);
	if(status != CL_SUCCESS) 
	{ 
		std::cout<<"Error: Waiting for kernel run to finish. (clWaitForEvents 0)\n";
		return -1;
	}
	
	long long kernelStartTime;
	long long kernelEndTime;
	status = clGetEventProfilingInfo(events[0],CL_PROFILING_COMMAND_START,sizeof(long long),
		                             &kernelStartTime,NULL);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: clGetEventProfilingInfo failed (start)\n";
		return -1;
	}
	status = clGetEventProfilingInfo(events[0],CL_PROFILING_COMMAND_END,sizeof(long long),&kernelEndTime,NULL);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: clGetEventProfilingInfo failed (end)\n";
		return -1;
	}
	double totalTime = (double)(kernelEndTime - kernelStartTime)/1e9;//已转换为秒
	std::printf("\nTIME: %f\n", totalTime);

	clReleaseEvent(events[0]);
	status = clEnqueueReadBuffer(commandQueue,outputBuffer,CL_TRUE,0,
		width * height * sizeof(cl_uint),output,0,NULL,&events[1]);
	if(status != CL_SUCCESS) 
	{ 
		std::cout <<"Error: clEnqueueReadBuffer failed. (clEnqueueReadBuffer)\n";
	}
	status = clWaitForEvents(1, &events[1]);
	if(status != CL_SUCCESS) 
	{ 
		std::cout<<"Error: Waiting for read buffer call to finish. (clWaitForEvents)\n";
		return -1;
	}
	clReleaseEvent(events[1]);

	return totalTime;
}

void clCleanup(void){
	cl_int status;
	status = clReleaseKernel(kernel);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: In clReleaseKernel \n";
		return; 
	}
	status = clReleaseProgram(program);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: In clReleaseProgram\n";
		return; 
	}
	status = clReleaseMemObject(inputBuffer);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: In clReleaseMemObject (inputBuffer)\n";
		return; 
	}
	status = clReleaseMemObject(intermediateBuffer);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: In clReleaseMemObject (intermediateBuffer)\n";
		return;
	}
	status = clReleaseMemObject(outputBuffer);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: In clReleaseMemObject (outputBuffer)\n";
		return; 
	}
	status = clReleaseMemObject(sobelOpXBuffer);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: In clReleaseMemObject (sobelOpXBuffer)\n";
		return;
	}
	status = clReleaseMemObject(sobelOpYBuffer);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: In clReleaseMemObject (sobelOpYBuffer)\n";
		return;
	}
	status = clReleaseCommandQueue(commandQueue);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: In clReleaseCommandQueue\n";
		return;
	}
	status = clReleaseContext(context);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: In clReleaseContext\n";
		return;
	}
}

void clCleanupHost(void){
	if(input != NULL)
	{
		free(input);
		input = NULL;
	}
	if(intermediate != NULL)
	{
		free(input);
		input = NULL;
	}
	if(output != NULL)
	{
		free(output);
		output = NULL;
	}
	if(clSobelOpX != NULL)
	{
		free(output);
		output = NULL;
	}
	if(clSobelOpY != NULL)
	{
		free(output);
		output = NULL;
	}
	if(devices != NULL)
	{
		free(devices);
		devices = NULL;
	}
}

void clPrintInfo(){
	//int MAX_DEVICES = 10;
	size_t p_size;
	size_t arr_tsize[3];
	size_t ret_size;
	char param[100];
	cl_uint entries;
	cl_ulong long_entries;
	cl_bool bool_entries;
	cl_device_id devices[MAX_DEVICES];
	size_t num_devices;
	cl_device_local_mem_type mem_type;
	cl_device_type dev_type;
	cl_device_fp_config fp_conf;
	cl_device_exec_capabilities exec_cap;

	//cl_int status = 0;
	cl_int errNum;
	cl_uint numPlatforms;
	cl_platform_id firstPlatformId;

	errNum = clGetPlatformIDs(0, 0, &num_devices);
	std::vector<cl_platform_id> platforms(num_devices);
	errNum = clGetPlatformIDs(numPlatforms, &platforms[0], &numPlatforms);

	if(errNum != CL_SUCCESS || numPlatforms <= 0){
		std::cerr << "Failed to find any OpenCl platforms."<<std::endl;
		return;
	}
	//OPContextHandle clGetPlatformContextHandle(cl_context context);
	/*
	int ret_vedio;
	if(!(ret_vedio = OVDecodeGetDeviceInfo(&num_devices,0))){

	}
	*/
	//查询device信息
	errNum=clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_ALL,0,0,&num_devices);
	errNum=clGetDeviceIDs(platforms[0],CL_DEVICE_TYPE_ALL,MAX_DEVICES,devices,&num_devices);
	
	//checkStatus(errNum);
	printf("Found Devices:\t\t%d\n", num_devices);

	for (int i = 0; i < num_devices; i++) {
		printf("\nDevice: %d\n\n", i);

		clGetDeviceInfo(devices[i], CL_DEVICE_TYPE, sizeof(dev_type),
			&dev_type, &ret_size);
		printf("\tDevice Type:\t\t");
		if (dev_type & CL_DEVICE_TYPE_GPU)
			printf("CL_DEVICE_TYPE_GPU ");
		if (dev_type & CL_DEVICE_TYPE_CPU)//device[1] 为cpu
			printf("CL_DEVICE_TYPE_CPU ");
		if (dev_type & CL_DEVICE_TYPE_ACCELERATOR)
			printf("CL_DEVICE_TYPE_ACCELERATOR ");
		if (dev_type & CL_DEVICE_TYPE_DEFAULT)
			printf("CL_DEVICE_TYPE_DEFAULT ");
		printf("\n");

		clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(param), param,
			&ret_size);
		printf("\tName: \t\t\t%s\n", param);//vendor

		clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(param), param,
			&ret_size);
		printf("\tVendor: \t\t%s\n", param);//Advanced Micro Devices, Inc.

		clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR_ID, sizeof(cl_uint),
			&entries, &ret_size);
		printf("\tVendor ID:\t\t%d\n", entries);//4098

		clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, sizeof(param), param,
			&ret_size);
		printf("\tVersion:\t\t%s\n", param);//OpenCL 1.2 AMD-APP (1642.5)

		clGetDeviceInfo(devices[i], CL_DEVICE_PROFILE, sizeof(param), param,
			&ret_size);
		printf("\tProfile:\t\t%s\n", param);//FULL_PROFILE

		clGetDeviceInfo(devices[i], CL_DRIVER_VERSION, sizeof(param), param,
			&ret_size);
		printf("\tDriver: \t\t%s\n", param);//1642.5VM

		clGetDeviceInfo(devices[i], CL_DEVICE_EXTENSIONS, sizeof(param), param,
			&ret_size);
		printf("\tExtensions:\t\t%s\n", param);

		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, 3
			* sizeof(size_t), arr_tsize, &ret_size);
		printf("\tMax Work-Item Sizes:\t(%d,%d,%d)\n", arr_tsize[0],
			arr_tsize[1], arr_tsize[2]);//128 128 128

		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_GROUP_SIZE,
			sizeof(size_t), &p_size, &ret_size);
		printf("\tMax Work Group Size:\t%d\n", p_size);//128

		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS,
			sizeof(cl_uint), &entries, &ret_size);
		printf("\tMax Compute Units:\t%d\n", entries);//2

		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_CLOCK_FREQUENCY,
			sizeof(cl_uint), &entries, &ret_size);
		printf("\tMax Frequency (Mhz):\t%d\n", entries);//750Mhz

		clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,
			sizeof(cl_uint), &entries, &ret_size);
		printf("\tCache Line (bytes):\t%d\n", entries);//000??? cpu 64

		clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE,
			sizeof(cl_ulong), &long_entries, &ret_size);
		printf("\tGlobal Memory (MB):\t%llu\n", long_entries / 1024 / 1024);//512MB

		clGetDeviceInfo(devices[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong),
			&long_entries, &ret_size);
		printf("\tLocal Memory (MB):\t%llu\n", long_entries / 1024 / 1024);//0.03125MB

		clGetDeviceInfo(devices[i], CL_DEVICE_LOCAL_MEM_TYPE,
			sizeof(cl_device_local_mem_type), &mem_type, &ret_size);
		if (mem_type & CL_LOCAL)
			printf("\tLocal Memory Type:\tCL_LOCAL\n");//LOCAL
		else if (mem_type & CL_GLOBAL)
			printf("\tLocal Memory Type:\tCL_GLOBAL\n");
		else
			printf("\tLocal Memory Type:\tUNKNOWN\n");

		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE,
			sizeof(cl_ulong), &long_entries, &ret_size);
		printf("\tMax Mem Alloc (MB):\t%llu\n", long_entries / 1024 / 1024);//190M

		clGetDeviceInfo(devices[i], CL_DEVICE_MAX_PARAMETER_SIZE,
			sizeof(size_t), &p_size, &ret_size);
		printf("\tMax Param Size (MB):\t%d\n", p_size);//1024

		clGetDeviceInfo(devices[i], CL_DEVICE_MEM_BASE_ADDR_ALIGN,
			sizeof(cl_uint), &entries, &ret_size);
		printf("\tBase Mem Align (bits):\t%d\n", entries);//2048

		clGetDeviceInfo(devices[i], CL_DEVICE_ADDRESS_BITS, sizeof(cl_uint),
			&entries, &ret_size);
		printf("\tAddress Space (bits):\t%d\n", entries);//32

		clGetDeviceInfo(devices[i], CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool),
			&bool_entries, &ret_size);//1
		printf("\tImage Support:\t\t%d\n", bool_entries);

		clGetDeviceInfo(devices[i], CL_DEVICE_TYPE, sizeof(fp_conf), &fp_conf,
			&ret_size);
		printf("\tFloat Functionality:\t");
		if (fp_conf & CL_FP_DENORM)
			printf("DENORM support ");
		if (fp_conf & CL_FP_ROUND_TO_NEAREST)
			printf("Round to nearest support ");// CL_FP_ROUND_TO_NEAREST
		if (fp_conf & CL_FP_ROUND_TO_ZERO)
			printf("Round to zero support ");
		if (fp_conf & CL_FP_ROUND_TO_INF)
			printf("Round to +ve/-ve infinity support ");
		if (fp_conf & CL_FP_FMA)
			printf("IEEE754 fused-multiply-add support ");
		if (fp_conf & CL_FP_INF_NAN)
			printf("INF and NaN support ");
		printf("\n");

		clGetDeviceInfo(devices[i], CL_DEVICE_ERROR_CORRECTION_SUPPORT,
			sizeof(cl_bool), &bool_entries, &ret_size);
		printf("\tECC Support:\t\t%d\n", bool_entries);//0 不支持

		clGetDeviceInfo(devices[i], CL_DEVICE_EXECUTION_CAPABILITIES,
			sizeof(cl_device_exec_capabilities), &exec_cap, &ret_size);//1
		printf("\tExec Functionality:\t");
		if (exec_cap & CL_EXEC_KERNEL)
			printf("CL_EXEC_KERNEL ");//CL_EXEC_KERNEL
		if (exec_cap & CL_EXEC_NATIVE_KERNEL)
			printf("CL_EXEC_NATIVE_KERNEL ");
		printf("\n");

		clGetDeviceInfo(devices[i], CL_DEVICE_ENDIAN_LITTLE, sizeof(cl_bool),
			&bool_entries, &ret_size);
		printf("\tLittle Endian Device:\t%d\n", bool_entries);//1

		clGetDeviceInfo(devices[i], CL_DEVICE_PROFILING_TIMER_RESOLUTION,
			sizeof(size_t), &p_size, &ret_size);//1
		printf("\tProfiling Res (ns):\t%d\n", p_size);

		clGetDeviceInfo(devices[i], CL_DEVICE_AVAILABLE, sizeof(cl_bool),
			&bool_entries, &ret_size);//1
		printf("\tDevice Available:\t%d\n", bool_entries);

	}
}


cl_uchar* cvMatToClArray(cv::Mat raw){
	int width = raw.cols;
	int height = raw.rows;
	cl_uchar *imageArray = (cl_uchar*)malloc(width*height*sizeof(cl_uchar));

	cv::Mat dstImg(width,height,CV_8UC1);
	//IplImage* dstImg = cvCreateImage(cvGetSize(raw),raw->depth,1);

	cv::cvtColor(raw,dstImg,CV_BGR2GRAY);

	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			int index = (y*width)+x;
			imageArray[index] =(cl_uchar)(raw.ptr<uchar>(y)[x]);	
		}
	}
	return imageArray;

}

cl_uchar* cvImageToClArrayTwoCon(IplImage* raw){
	//int width = raw->width;
	//int height = raw->height;
	cl_uchar *imageArray = (cl_uchar*)malloc(width*height*sizeof(cl_uchar));

	//cv::Mat dstImg(width,height,CV_32FC1);
	IplImage* dstImg = cvCreateImage(cvGetSize(raw),raw->depth,1);
	
    cvCvtColor(raw,dstImg,CV_BGR2GRAY);

	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			int index = (y*width)+x;
			imageArray[index] =(cl_uchar)(cvGetReal2D(dstImg,y,x));	
			//qDebug("%d\t",imageArray[index]);
		}
	}
	cvShowImage("orig",dstImg);
	return imageArray;

}

void clInitializeHost(cv::Mat &raw){
	if(input !=NULL){
		free(input);
		input = NULL;
	}
	input = cvMatToClArray(raw);
	if(output != NULL){
		free(output);
		output = NULL;
	}

	output = (cl_uchar*)malloc(width * height * sizeof(cl_uchar));
}
void clInitializeHostTwoCon(IplImage* cvRawImg){
	if(input !=NULL){
		free(input);
		input = NULL;
	}

	if(output != NULL){
		free(output);
		output = NULL;
	}

	input = cvImageToClArrayTwoCon(cvRawImg);
	if(input == NULL){
		std::cout<<"Error: Failed to allocate host memory. (input)\n";
		return;
	}

	output = (cl_uchar*)malloc(width * height * sizeof(cl_uchar));
	if(output==NULL)
	{
		std::cout<<"Error: Failed to allocate host memory. (output)\n";
		return;
	}  
}

void clInitializeTwoCon(void){
	cl_int status = 0;
	size_t deviceListSize =0;
	cl_uint numPlatforms;
	cl_platform_id firstPlatformId;


	//ovdecode_device_info *deviceInfo = new ovdecode_device_info[deviceListSize];

	//OPContextHandle ovdContext = clCreateContext(properties,1,&clDeviceID,0,0);


	cl_int errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
	if(errNum != CL_SUCCESS || numPlatforms <= 0){
		std::cerr << "Failed to find any OpenCl platforms."<<std::endl;
		return;
	}
	clGetDeviceIDs(firstPlatformId,CL_DEVICE_TYPE_GPU,1,&device,NULL);

	cl_context_properties contextProperties[] ={
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)firstPlatformId,
		0
	};

	context = clCreateContext(contextProperties,1,&device,NULL,NULL,NULL);
	//context = clCreateContextFromType(0,CL_DEVICE_TYPE_CPU,NULL,NULL,&status);
	/* First, get the size of device list data */
	/////////////////////////////////////////////////////////////////
	// Detect OpenCL devices
	/////////////////////////////////////////////////////////////////

	commandQueue = clCreateCommandQueue(context,device,CL_QUEUE_PROFILING_ENABLE,NULL);


	inputBuffer = clCreateBuffer(
		context,
		CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
		sizeof(cl_uchar) * width * height,
		input,
		&status);
	if(status == CL_INVALID_CONTEXT)
	{
		//context not valid
		std::cout<<"Error: clCreateBuffer - invalid context - (inputBuffer)\n";
		return;
	}
	if(status == CL_INVALID_VALUE)
	{
		//flags value not valid
		std::cout<<"Error: clCreateBuffer - invalid flags value - (inputBuffer)\n";
		return;
	}
	if(status == CL_INVALID_BUFFER_SIZE)
	{
		//size==0 or size>CL_DEVICE_MAX_MEM_ALLOC_SIZE
		std::cout<<"Error: clCreateBuffer - invalid buffer size - (inputBuffer)\n";
		return;
	}
	if(status == CL_INVALID_HOST_PTR)
	{
		//(host_ptr == NULL) && (CL_MEM_USE_HOST_PTR || CL_MEM_COPY_HOST_PTR in flags)
		//||
		//(host_ptr != NULL) && (CL_MEM_COPY_HOST_PTR || CL_MEM_USE_HOST_PTR _not_ in flags)
		bool isNull = (input==NULL);
		std::cout<<"Error: clCreateBuffer - invalid host pointer - (inputBuffer) - NULL==" << isNull << "\n";
		return;
	}
	if(status == CL_MEM_OBJECT_ALLOCATION_FAILURE)
	{
		//there is a failure to allocate memory for buffer object
		std::cout<<"Error: clCreateBuffer - mem object alloc failure - (inputBuffer)\n";
		return;
	}
	if(status == CL_OUT_OF_HOST_MEMORY)
	{
		//there is a failure to allocate resources required by the OpenCL implementation on the host
		std::cout<<"Error: clCreateBuffer - out of host mem - (inputBuffer)\n";
		return;
	}
	if(status != CL_SUCCESS)
	{ 
		std::cout<<"Error: clCreateBuffer (inputBuffer)\n";
		return;
	}

	outputBuffer = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
		sizeof(cl_uchar) * width * height,output,&status);
	if(status == CL_INVALID_CONTEXT)
	{
		//context not valid
		std::cout<<"Error: clCreateBuffer - invalid context\n";
	}
	if(status == CL_INVALID_VALUE)
	{
		//flags value not valid
		std::cout<<"Error: clCreateBuffer - invalid flags value\n";
	}
	if(status == CL_INVALID_BUFFER_SIZE)
	{
		//size==0 or size>CL_DEVICE_MAX_MEM_ALLOC_SIZE
		std::cout<<"Error: clCreateBuffer - invalid buffer size\n";
	}
	if(status == CL_INVALID_HOST_PTR)
	{
		//(host_ptr == NULL) && (CL_MEM_USE_HOST_PTR || CL_MEM_COPY_HOST_PTR in flags)
		//||
		//(host_ptr != NULL) && (CL_MEM_COPY_HOST_PTR || CL_MEM_USE_HOST_PTR _not_ in flags)
		bool isNull = (input==NULL);
		std::cout<<"Error: clCreateBuffer - invalid host pointer - NULL==" << isNull << "\n";
	}
	if(status == CL_MEM_OBJECT_ALLOCATION_FAILURE)
	{
		//there is a failure to allocate memory for buffer object
		std::cout<<"Error: clCreateBuffer - mem object alloc failure\n";
	}
	if(status == CL_OUT_OF_HOST_MEMORY)
	{
		//there is a failure to allocate resources required by the OpenCL implementation on the host
		std::cout<<"Error: clCreateBuffer - out of host mem\n";
	}
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: clCreateBuffer (outputBuffer)\n";
		return;
	}

	sobelOpXBuffer = clCreateBuffer(context,CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
		sizeof(cl_uint ) * MASK_HEIGHT * MASK_WIDTH,clSobelOpXX,&status);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: clCreateBuffer (sobelOpXBuffer)\n";
		return;
	}

	sobelOpYBuffer = clCreateBuffer(context,CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
		sizeof(cl_uint ) * MASK_HEIGHT * MASK_WIDTH,clSobelOpYY,&status);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: clCreateBuffer (sobelOpYBuffer)\n";
		return;
	}

	/* create a cl program executable for all the devices specified */
	program = createProgram(context,device,"TwoCon.cl");

	kernel = clCreateKernel(program,"edgeDetectKernel",&status);
	if(status != CL_SUCCESS){
		std::cout<<"Error: create kernel from program.";
		return;
	}

}



double clRunTwoConKernels(){
	double runTime;
	cl_int status;
	cl_event events[2];


	size_t localx,localy;
	if(width/8>4){
		localx = 16;
	}else if(width<8){
		localx = width;
	}else {
		localx = 8;
	}
	if(height/8 >4)  
		localy = 16;  
	else if(height<8)  
		localy = height;  
	else localy = 8;  

    //int gtx = (width/localx+1)*localx;
	//int gty = (height/localy+1)*localy;
	size_t globalThreads[]={width,height};//512
	size_t localThreads[]={8,localy};
	

	status = clSetKernelArg(kernel,0,sizeof(cl_mem),(void*)&inputBuffer);
	status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&outputBuffer);
	status |= clSetKernelArg(kernel, 2, sizeof(cl_int), (void *)&sobelOpXBuffer);
	status |= clSetKernelArg(kernel, 3, sizeof(cl_int), (void *)&sobelOpYBuffer);
	status |= clSetKernelArg(kernel, 4, sizeof(cl_uint), (void *)&width);
	status |= clSetKernelArg(kernel, 5, sizeof(cl_uint), (void *)&height);
	if(status != CL_SUCCESS){
		std::cout<<"Error: setting kernel argument";
		return -1;
	}

	status = clEnqueueNDRangeKernel(commandQueue,kernel,2,NULL,globalThreads,localThreads,0,
		NULL,&events[0]);
	if(status != CL_SUCCESS) 
	{ 
		std::cout<<"Error: Enqueueing kernel onto command queue. (clEnqueueNDRangeKernel)\n";
		return -1;
	}
	
	status =clFinish(commandQueue);
    //status = clWaitForEvents(1,&events[0]);
	if(status != CL_SUCCESS) 
	{ 
		std::cout<<"Error: Waiting for kernel run to finish. (clWaitForEvents 0)\n";
		return -1;
	}

	long long kernelStartTime;
	long long kernelEndTime;
	status = clGetEventProfilingInfo(events[0],CL_PROFILING_COMMAND_START,sizeof(long long),
		&kernelStartTime,NULL);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: clGetEventProfilingInfo failed (start)\n";
		return -1;
	}
	status = clGetEventProfilingInfo(events[0],CL_PROFILING_COMMAND_END,sizeof(long long),&kernelEndTime,NULL);
	if(status != CL_SUCCESS)
	{
		std::cout<<"Error: clGetEventProfilingInfo failed (end)\n";
		return -1;
	}
	double totalTime = (double)(kernelEndTime - kernelStartTime)/1e9;//已转换为秒
	std::printf("\nTIME: %f\n", totalTime);

	clReleaseEvent(events[0]);
	status = clEnqueueReadBuffer(commandQueue,outputBuffer,CL_TRUE,0,
		width * height * sizeof(cl_uint),output,0,NULL,&events[1]);
	if(status != CL_SUCCESS) 
	{ 
		std::cout <<"Error: clEnqueueReadBuffer failed. (clEnqueueReadBuffer)\n";
	}
	status = clWaitForEvents(1, &events[1]);
	if(status != CL_SUCCESS) 
	{ 
		std::cout<<"Error: Waiting for read buffer call to finish. (clWaitForEvents)\n";
		return -1;
	}
	clReleaseEvent(events[1]);
	return totalTime;
}

cv::Mat sobelChangeMainFunction(string &filename){
	IplImage* imgRaw = cvLoadImage(filename.c_str(),1);
	//cv::Mat imgRaw =cv::imread(filename);
	//int repetitions = 100;
	
	//width = imgRaw.cols;
	//height = imgRaw.rows;
	width = imgRaw->width;
	height = imgRaw->height;
	double cvRunTime = 0;
	clock_t start,finish;
	start = clock(); // clock 为毫秒级
	
	finish = clock();
	cvRunTime = (finish - start)/CLOCKS_PER_SEC;//secs
	
	clInitializeHostTwoCon(imgRaw);//将img 放到input
	clInitializeTwoCon();//init opencl resources
	cvGenerateSobelImage();
	double clRunTime = 0;
	
		// 纳秒级 转为秒级
	clRunTime=clRunTwoConKernels();
	qDebug("\nopencl Average RunTime:\t%f\n\n",clRunTime);
	//cv::Mat outputMat =clArrayToMat(output,width,height);
	//clArrayToCvImage(output,width,height);
	clCleanup();
	clCleanupHost();//have changed output not release

	///////////////////////////////////////////////////////////////////
	//cvReleaseMat(imgRaw);
	cv::Mat outputMat;
	return outputMat;
}

int histogram(uchar* image_data,int w,int h){
	unsigned int *ref_histogram_results;
	unsigned int *ptr;
	ref_histogram_results = (unsigned int *)malloc(256*sizeof(unsigned int));

	ptr = ref_histogram_results;
	return 1;
}


int video_main_function(){

	cl_device_id clDeviceID;
	cl_int errNum;
	cl_uint numPlatforms;
	cl_platform_id firstPlatformId;

	errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
	if(errNum != CL_SUCCESS || numPlatforms <= 0){
		std::cerr << "Failed to find any OpenCl platforms."<<std::endl;
		return NULL;
	}
	clGetDeviceIDs(firstPlatformId,CL_DEVICE_TYPE_GPU,1,&clDeviceID,NULL);
	cl_context_properties contextProperties[] ={
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)firstPlatformId,
		0
	};
	OPContextHandle ovdContext = clCreateContext(contextProperties,1,&clDeviceID,0,0,&errNum);
	

	ovdecode_format output_Format;
	ovdecode_profile profile;
	unsigned int output_Width;
	unsigned int output_Height;
	unsigned int ovDeviceID=0;
	ov_session session = OVDecodeCreateSession(ovdContext,ovDeviceID,profile,output_Format,output_Width,output_Height);


	/*
	OVresult res = OVAcquireObject(session,num_handle,decode_handle,1,&pre_event);
	res = OVDecodePicture(session,&picture_parameter,&pic_parameter_2,pic_parameter_2_size,
		                  &bitstream_data,slice_data_control,slice_data_control_size,
		                  output_surface,1,&acquire_event,&eventRunVideoProgram,0);
	ret = OVReleaseObject(session,num_handle,decode_handle,1,&decode_event,&release_event);

	*/
}

