#include "mainwindow.h"
#include "ui_mainwindow.h"
using namespace cv;
QImage Mat2QImage(Mat image)
{
    QImage img;

    if (image.channels()==3) {
        cvtColor(image, image, CV_BGR2RGB);
        img = QImage((const unsigned char *)(image.data), image.cols, image.rows,
                image.cols*image.channels(), QImage::Format_RGB888);
    } else if (image.channels()==1) {
        img = QImage((const unsigned char *)(image.data), image.cols, image.rows,
                image.cols*image.channels(), QImage::Format_ARGB32);
    } else {
        img = QImage((const unsigned char *)(image.data), image.cols, image.rows,
                image.cols*image.channels(), QImage::Format_RGB888);
    }

    return img;
}
//构造函数，变量的初始化部分
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isToPredict=false;
    isGoOn=true;
    algorithmTimer = new QTimer(this);
    relateDetectROI=cvRect(0,0,0,0);
    //restoreInitFile();
    bool isModelLoad=m_predict.loadModel("model/inceptionweights-80x80-symbol.json","model/inceptionweights-80x80-0180.params","model/mean.bin");
    m_predict.isUsingMeanInputImage=false;
    if(false==isModelLoad)
    {
        QMessageBox::warning(this,QStringLiteral("警告"),QStringLiteral("CNN模型载入出错！"));
    }
    //将定时器超时信号与槽(功能函数)联系起来
   // connect( algorithmTimer, SIGNAL(timeout()), this, SLOT(onRun()));
   QObject::connect(algorithmTimer,&QTimer::timeout,this,&MainWindow::onRun);
}

MainWindow::~MainWindow()
{
   destroyAllWindows();
   delete algorithmTimer;
    delete ui;
}

void MainWindow::on_OpenFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("Open Iamge File") , "F:/datasets/DuWeiwei/test/", "Iamge (*.jpg *.png *.bmp)", 0);

    if (fileName.isNull())
    {
        return;
    }
   showImage = cv::imread(fileName.toStdString(),IMREAD_COLOR);

    /*************************/
    /****控件的初始化***/
    /*************************/


    algorithmTimer->start(33);
    ui->ShowImageLabel->setIsWorking(true);
}

void MainWindow::onRun()
{
    cv::Mat currentFrame;
    currentFrame.create(showImage.cols,showImage.rows,showImage.type());
    showImage.copyTo(currentFrame);

    rectangle(currentFrame,relateDetectROI,Scalar(0,255,0));
    //显示读取的图片
    QImage img = (Mat2QImage(currentFrame));

    if(!img.isNull())
    {
        QImage imgScaled = img.scaled(ui->ShowImageLabel->size(),Qt::IgnoreAspectRatio);
        ui->ShowImageLabel->setPixmap(QPixmap::fromImage(imgScaled));
        update();
    }
    if( true == isToPredict )
    {
        cv::Mat ROI(currentFrame,relateDetectROI);
        cv::Mat candidate(relateDetectROI.size(),CV_8UC3,cv::Scalar::all(0));
        ROI.copyTo(candidate);
        QImage img2= (Mat2QImage(candidate));

       if(!img2.isNull())
       {
           QImage imgScaled = img2.scaled(ui->CandidateImageLabel->size(),Qt::IgnoreAspectRatio);
           ui->CandidateImageLabel->setPixmap(QPixmap::fromImage(imgScaled));

           update();
       }

        double t = (double)getTickCount();

        int type=m_predict.predictImageClassfication(candidate);
        t = ((double)getTickCount() - t)/getTickFrequency();
        qDebug ("This function times passed in %f seconds: " ,t );
           if(type==0)
           {

               ui->StatusLabel->setText (QStringLiteral("门把手"));
           }
           else if(type==1)
           {
               ui->StatusLabel->setText (QStringLiteral("后视镜"));
           }
           else if(type==2)
           {
               ui->StatusLabel->setText (QStringLiteral("负样本"));
           }
           isToPredict=false;
           on_DeleteStoreRectsButton_clicked();
    }
    currentFrame.release();
}


void MainWindow::closeEvent()
{

 destroyAllWindows();
}

void MainWindow::on_QuitButton_clicked()
{
    storeInitFile();
    algorithmTimer->stop();
    destroyAllWindows();
    MainWindow::close();
}

void MainWindow::on_PopStoreRectsButton_clicked()
{
    ui->ShowImageLabel->setStoreRects(ShowLabel::PopLast);
}

void MainWindow::on_DeleteStoreRectsButton_clicked()
{
    ui->ShowImageLabel->setStoreRects(ShowLabel::ClearAll);
}


void MainWindow::on_LockROIButton_clicked()
{
    float a=(float)showImage.rows/ui->ShowImageLabel->size().height();
    float b=(float)showImage.cols/ui->ShowImageLabel->size().width();
    if(ui->ShowImageLabel->storeRects.size()>0)
    {
    relateDetectROI= cvRect(b*ui->ShowImageLabel->storeRects[0].x,a*ui->ShowImageLabel->storeRects[0].y,
            b*ui->ShowImageLabel->storeRects[0].width,
            a*ui->ShowImageLabel->storeRects[0].height);
    isToPredict=true;
    }
}

void MainWindow::storeInitFile ()
{

      QSettings *configIniWrite = new QSettings("configration.ini", QSettings::IniFormat);
      //向ini文件中写入内容,setValue函数的两个参数是键值对
      if(relateDetectROI.height!=0 && relateDetectROI.width!=0)
      {
      configIniWrite->setValue("/relateDetectROI/relateDetectROI.x", relateDetectROI.x);
      configIniWrite->setValue("/relateDetectROI/relateDetectROI.y", relateDetectROI.y);
      configIniWrite->setValue("/relateDetectROI/relateDetectROI.height", relateDetectROI.height);
      configIniWrite->setValue("/relateDetectROI/relateDetectROI.width", relateDetectROI.width);

      }
      //写入完成后删除指针
      delete configIniWrite;
      //删除所有指针类
    MainWindow::close();
}
void MainWindow::restoreInitFile ()
{
    QSettings *configIniRead = new QSettings("configration.ini", QSettings::IniFormat);
    relateDetectROI.x=configIniRead->value ("/relateDetectROI/relateDetectROI.x").toInt();
    relateDetectROI.y=configIniRead->value ("/relateDetectROI/relateDetectROI.y").toInt();
    relateDetectROI.width=configIniRead->value ("/relateDetectROI/relateDetectROI.width").toInt();
    relateDetectROI.height=configIniRead->value ("/relateDetectROI/relateDetectROI.height").toInt();

    delete configIniRead;
}



void MainWindow::on_ImagePushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("Open Iamge File") , "F:/datasets/followingTest/", "Iamge (*.jpg *.png *.bmp)", 0);

    if (fileName.isNull())
    {
        return;
    }
    cv::Mat loadImage = cv::imread(fileName.toStdString(),IMREAD_COLOR);
    double t = (double)getTickCount();

    int type=m_predict.predictImageClassfication(loadImage);
    t = ((double)getTickCount() - t)/getTickFrequency();
    qDebug ("This function times passed in %f seconds: " ,t );
       if(type==1)
       {

           ui->StatusLabel->setText (QStringLiteral("单人"));
       }
       else if(type==0)
       {
           ui->StatusLabel->setText (QStringLiteral("多人"));
       }
       update();
       QImage img = (Mat2QImage(loadImage));

       if(!img.isNull())
       {
           QImage imgScaled = img.scaled(ui->ShowImageLabel->size(),Qt::IgnoreAspectRatio);
           ui->ShowImageLabel->setPixmap(QPixmap::fromImage(imgScaled));
           update();
       }
}
