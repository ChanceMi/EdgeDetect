#include "steeldetect.h"
#include <QTextCodec>
#include <QtGui/QApplication>
#include <opencv2/opencv.hpp>
#include <QFileDialog>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("System"));  //ฯ๛ณýยาย๋ 
	SteelDetect w;
	w.show();
	return a.exec();
}
