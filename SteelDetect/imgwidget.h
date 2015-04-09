#ifndef IMGWIDGET_H
#define IMGWIDGET_H

#include <QObject>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QWidget>
#include <QPainter>

class SteelDetect;
class ImgWidget : public QWidget
{
	Q_OBJECT

public:
	ImgWidget(QWidget *parent,SteelDetect *parent_window);
	~ImgWidget();

private:
	SteelDetect *parent_window_;
	IplImage *src_img_ipl;
	QImage src_img_q;
	cv::Mat src_img_mat;

	cv::Mat img_mat;

	

private:
	void paintEvent(QPaintEvent *e);
	QImage  Mat2QImage(const cv::Mat &mat);

public :
	void setImageMat(cv::Mat &src);
	void setQImageFromUchar(uchar *qimg,int width,int height);//都转为单通道
	

};

#endif // IMGWIDGET_H
