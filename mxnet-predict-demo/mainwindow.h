#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QtCore>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QSettings>
#include <QDir>
#include <QDateTime>
#include <QPainter>
#include <QCloseEvent>
#include <QPen>
#include <QDebug>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "showlabel.h"

#include "image-classification.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_OpenFileButton_clicked();


    void on_QuitButton_clicked();

    void on_PopStoreRectsButton_clicked();

    void on_DeleteStoreRectsButton_clicked();

    void on_LockROIButton_clicked();

    void storeInitFile();


    void on_ImagePushButton_clicked();

private:
    Ui::MainWindow *ui;

public:
    QTimer *algorithmTimer;

    cv::Mat showImage;

    cv::VideoCapture imageCapture;

    bool isToPredict;

    cv::Mat backgroundFrame;

    CNN_predict m_predict;


    std::vector<cv::Mat> candidateFrames;



     cv::Rect relateDetectROI;

     void onRun();


private:

     bool isGoOn;
    void closeEvent();

    void restoreInitFile();
};

#endif // MAINWINDOW_H
