#ifndef STEELDETECT_H
#define STEELDETECT_H

#include <QtGui/QMainWindow>
#include <opencv2/opencv.hpp>
#include <QAction>
#include "ui_steeldetect.h"
#include "imgwidget.h"
#include <string>
#include <QLabel>
#include <QTimer>
#include "EdgeDetection.h"

class SteelDetect : public QMainWindow
{
	Q_OBJECT

public:
	SteelDetect(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SteelDetect();

private:
	Ui::SteelDetectClass ui;
	void createMenus();
	void createActions();
	void createToolBars();

	void setFrameStyle();
	void edgeDetect();

	void oclModule();

public slots:
	void slotOpenVideo();
	void slotOpenImg();
	void slotSaveImg();
	void slotDetect();
	void slotOpenCam();
	void nextFrame();


	void slotDetectGpu();

private:
	

	ImgWidget *firstImg;
	ImgWidget *secondImg;
	QLabel *imageLabel;


	QMenu *fileMenu;
	QMenu *imgMenu;
	QMenu *helpMenu;
	//QMenu *addSubMenu;
	QMenu *detectMenu;

	QToolBar *imgTool;

	QAction *imgOpenAction;
	QAction *videoOpenAction;
	QAction *camOpenAction;
	QAction *imgSaveAction;
	QAction *detectAction;

	QAction *detectGpuAction;

	CvCapture *capture;
	QTimer *timer;
		 
private:
	cv::Mat image;
	std::string imgPath;
	IplImage *frame;


private :
	void edgeDetectGpu();
	EdgeDetection *edgeDetectPtr;
};

#endif // STEELDETECT_H
