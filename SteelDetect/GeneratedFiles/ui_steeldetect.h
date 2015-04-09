/********************************************************************************
** Form generated from reading UI file 'steeldetect.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STEELDETECT_H
#define UI_STEELDETECT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SteelDetectClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *SteelDetectClass)
    {
        if (SteelDetectClass->objectName().isEmpty())
            SteelDetectClass->setObjectName(QString::fromUtf8("SteelDetectClass"));
        SteelDetectClass->resize(600, 400);
        menuBar = new QMenuBar(SteelDetectClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        SteelDetectClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(SteelDetectClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        SteelDetectClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(SteelDetectClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        SteelDetectClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(SteelDetectClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        SteelDetectClass->setStatusBar(statusBar);

        retranslateUi(SteelDetectClass);

        QMetaObject::connectSlotsByName(SteelDetectClass);
    } // setupUi

    void retranslateUi(QMainWindow *SteelDetectClass)
    {
        SteelDetectClass->setWindowTitle(QApplication::translate("SteelDetectClass", "SteelDetect", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SteelDetectClass: public Ui_SteelDetectClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STEELDETECT_H
