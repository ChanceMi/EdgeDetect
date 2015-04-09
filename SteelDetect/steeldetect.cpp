#include "steeldetect.h"
#include <QMenuBar>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>
#include "Tool.h"
#include "canny2.h"
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ocl/ocl.hpp>


SteelDetect::SteelDetect(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
	
{
	setWindowTitle(tr("SteelDetect"));
	//setIcon(QIcon("images/icon.png"));
	ui.setupUi(this);
	createActions();
	createMenus();
	
	setFrameStyle();
	//imgPath ="E://CDX//01//01//08.jpg";
	edgeDetectPtr = new EdgeDetection;
	
}

SteelDetect::~SteelDetect()
{
	
}

void SteelDetect::setFrameStyle(){
	QWidget *outer = new QWidget;
	QHBoxLayout *hb = new QHBoxLayout;

	firstImg =new ImgWidget(NULL,this);
    secondImg =new ImgWidget(NULL,this);

	hb->addWidget(firstImg);
	hb->addWidget(secondImg);
	outer->setLayout(hb);
	setCentralWidget(outer);
}

void SteelDetect::createActions(){
	imgOpenAction = new QAction(tr("&Open Img"),this);
	//imgOpenAction->setIcon(QIcon("images/icon.png"));
	imgOpenAction->setShortcut(tr("Ctrl+I"));
	imgOpenAction->setStatusTip(tr("Open an image"));
	imgOpenAction->setEnabled(true);
	connect(imgOpenAction,SIGNAL(triggered()),this,SLOT(slotOpenImg()));

	videoOpenAction = new QAction(tr("&Open Video"),this);
	videoOpenAction->setShortcut(tr("Ctrl+V"));
	videoOpenAction->setStatusTip(tr("Open a video"));
	connect(videoOpenAction,SIGNAL(triggered()),this,SLOT(slotOpenVideo()));

	camOpenAction = new QAction(tr("&Open Cam"),this);
	camOpenAction->setShortcut(tr("Ctrl+C"));
	camOpenAction->setStatusTip(tr("Open a cam"));
	connect(camOpenAction,SIGNAL(triggered()),this,SLOT(slotOpenCam()));

	imgSaveAction = new QAction(tr("&Save Image"),this);
	imgSaveAction->setShortcut(tr("Ctrl+S"));
	imgSaveAction->setStatusTip(tr("Save an image"));
	connect(imgSaveAction,SIGNAL(triggered()),this,SLOT(slotSaveImg()));

	detectAction = new QAction(tr("&Detect"),this);
	detectAction->setStatusTip(tr("Begin detect"));
	connect(detectAction,SIGNAL(triggered()),this,SLOT(slotDetect()));

	detectGpuAction = new QAction(tr("&DetectWGpu"),this);
	detectGpuAction->setStatusTip(tr("Begin detect"));
	connect(detectGpuAction,SIGNAL(triggered()),this,SLOT(slotDetectGpu()));
}

void SteelDetect::slotOpenImg(){
	QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),
		".",tr("Image Files (*.png *.jpg *.bmp)"));
	qDebug()<<"filenames:"<<fileName.toAscii().data();
    imgPath = fileName.toStdString();
	//cv::Mat im = cv::imread(imgPath);
	image = cv::imread(fileName.toAscii().data());
	if(!image.data)
	{
		printf("No data!--Exiting the program \n");
	}
	//cv::namedWindow("org Img",CV_WINDOW_AUTOSIZE);
	//cv::imshow("org Img",image);
	firstImg->setImageMat(image);
	firstImg->update();
	
	
}

void SteelDetect::slotOpenCam(){
	
	CvCapture *capture;
	IplImage *frame;
	capture = cvCaptureFromCAM(0);



}

void SteelDetect::slotOpenVideo(){

	

	QString fileName = QFileDialog::getOpenFileName(this,tr("Open an Video"),
		".",tr("Video Files (*.avi)"));
	//CvCapture *capture = cvCreateFileCapture(fileName.toStdString().c_str());
	const char* filePath = (const char*) fileName.toStdString().c_str();

	const char* sss = fileName.toAscii().data();
	//qDebug()<<"filenames:"<<fileName.toAscii().data();
	//CvCapture *capture = cvCaptureFromFile(sss);
	//CvCapture *capture = cvCaptureFromFile("E://1.avi");
	//CvCapture *capture = cvCaptureFromCAM(0);
	//cv::gpu::VideoReader_GPU d_reader(sss);
	//cv::gpu::GpuMat d_frame;
	//cv::gpu::VideoWriter_GPU();
	//oclMat::
	
	//cv::gpu::Canny()

	capture= cvCaptureFromAVI(fileName.toStdString().c_str());

	if(capture){
		timer = new QTimer(this);
		timer->setInterval(30);
		connect(timer,SIGNAL(timeout()),this,SLOT(nextFrame()));
		timer->start();
	}	
}

void SteelDetect::nextFrame(){
	IplImage *frame = NULL;
	long nframe = (long)cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);
	//对视频开起一个堆缓冲
	int frameH    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	int frameW    = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int fps       = (int) cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	int i=0;
	
		frame = cvQueryFrame(capture);
		if(!frame){
			timer->stop();
			return;
		}
		cv::Mat img(frame);
		firstImg->setImageMat(img);
		firstImg->update();

		cv::Mat dst = img;
		cv::Canny(img,dst,50,150,3);
		secondImg->setImageMat(dst);
		secondImg->update();
		//cv::imshow("vedio",img);
	
}

void SteelDetect::slotSaveImg(){

}

void SteelDetect::slotDetect(){
	if(image.data == NULL){
		return;
	}
	
	//cv::ocl::oclMat src(image);
	//src.upload(image);
	//cv::ocl::oclMat dstOclMat;
	//cv::ocl::cvtColor(src,dstOclMat,CV_BGR2GRAY);
	//cv::Mat dMat = (cv::Mat)dstOclMat;
	//dstOclMat.download(dMat);
	edgeDetect();
}

void SteelDetect::slotDetectGpu(){

	if(imgPath.empty()){
		//return;
	}
	//main_func_image(imgPath);
	//secondImg->setQImage(img);
	secondImg->setImageMat(sobelChangeMainFunction(imgPath));
	secondImg->update();
}

void SteelDetect::createToolBars(){

}

void SteelDetect::createMenus(){
	//menuBar = new QMenuBar(this);
	//menuBar->setGeometry(QRect(0,0,200,20));

	fileMenu =menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(videoOpenAction);
	fileMenu->addAction(camOpenAction);



	imgMenu = menuBar()->addMenu(tr("&Img"));
	imgMenu->addAction(imgOpenAction);
	imgMenu->addAction(imgSaveAction);

	detectMenu = menuBar()->addMenu(tr("&Detect"));
	detectMenu->addAction(detectAction);
	detectMenu->addAction(detectGpuAction);

	//statusBar()->hide();
	
}

uchar* cvImageToArray(cv::Mat &raw){
	int width = raw.cols;
	int height = raw.rows;
	uchar *imageArray = (uchar*)malloc(width*height*sizeof(uchar));

	cv::Mat dstImg =cv::Mat(height,width,CV_8UC1);
	
	cv::cvtColor(raw,dstImg,CV_BGR2GRAY);
	//cv::imshow("gray",dstImg);
	for(int y=0;y<height;y++){
		for(int x=0;x<width;x++){
			int index = (y*width)+x;
			imageArray[index] =dstImg.ptr<uchar>(y)[x];	
			
		}
	}
	return imageArray;
}



void SteelDetect::edgeDetect(){
	if(image.data == NULL){
		return ;
	}
	cv::Mat ddst = image;
	
	//cv::Canny(image,ddst,50,150,3);
	cv::imshow("opencv canny",ddst);
	uchar* src = cvImageToArray(image);
	
	

	int width = image.cols;
    int height = image.rows;
	//cv::Mat dstMat = clArrayToCvImage(src,width,height);
	uchar *dst = (uchar*)malloc(width*height*sizeof(uchar));
	Canny(src,width,height,1,0.7,0.95,dst);
	//edgeDetectPtr->maskFilter(dst);
	//secondImg->setImageMat(dst);
	cv::Mat dstMat = clArrayToCvImage(dst,width,height);
	secondImg->setImageMat(dstMat);
	secondImg->update();
}

void SteelDetect::edgeDetectGpu(){
	if(imgPath.empty()){
		return;
	}
	cv::Mat image = cv::imread(imgPath); 
	int width = image.cols;
	int height = image.rows;

	cv::Mat dstImg(width,height,CV_32FC1);

	if(image.channels() == 3){
		cv::cvtColor(image,dstImg,CV_BGR2GRAY);
	}

	unsigned char *sBuffer = new unsigned char[width*height];
	memcpy(sBuffer,image.data,width*height);

	char *buffer = new char[width*height];
	memcpy(buffer,dstImg.data,width*height);

}

void SteelDetect::oclModule(){
	using namespace cv::ocl;
	//getdev
}