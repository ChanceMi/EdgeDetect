#include "imgwidget.h"
#include "steeldetect.h"
#include <opencv2/core/core.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <opencv2/core/cuda_devptrs.hpp>


ImgWidget::ImgWidget(QWidget *parent,SteelDetect *parent_window)
	: QWidget(parent)
	,parent_window_(parent_window)
{

}

ImgWidget::~ImgWidget()
{

}

void ImgWidget::setImageMat(cv::Mat &src){
	src_img_mat=src.clone();
	src_img_q = Mat2QImage(src_img_mat);
	
}

void ImgWidget::setQImageFromUchar(uchar *qimg,int width,int height){
	src_img_q=QImage(qimg,width,height,QImage::Format_RGB888);
}

QImage  ImgWidget::Mat2QImage(const cv::Mat &mat){
	
	QImage img ;
	if(mat.channels()==3)
	{
		//cvt Mat BGR 2 QImage RGB
		cvtColor(mat,img_mat,CV_BGR2RGB);
	}else{
		cvtColor(mat,img_mat,CV_GRAY2RGB);
	}
	img =QImage((const unsigned char*)(img_mat.data),
		img_mat.cols,img_mat.rows,
		img_mat.cols*img_mat.channels(),
		QImage::Format_RGB888);
	return img;
}



void ImgWidget::paintEvent(QPaintEvent *e){
	QPainter painter(this);
		
	if(src_img_q.data_ptr() == NULL){
		return;
	}
	painter.drawPixmap(0,0,this->width(),this->height(),QPixmap::fromImage(src_img_q));
	
}