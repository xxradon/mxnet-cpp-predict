#ifndef SHOWLABLE
#define SHOWLABLE

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include "mainwindow.h"


class ShowLabel: public QLabel
{
    Q_OBJECT
public:
    enum HandleType {
            PopLast,
            ClearAll
    };
    explicit ShowLabel (QWidget *parent = 0);
    void setIsWorking(bool value);
    void setStoreRects(char handleType);
     std::vector<cv::Rect> storeRects;

signals:
    void clicksl(int,int);
protected:
    void mousePressEvent (QMouseEvent *peventLabel);
   virtual  void mouseReleaseEvent (QMouseEvent *peventLabel);
   virtual   void leaveEvent (QEvent *peventLabel);
   virtual  void mouseMoveEvent (QMouseEvent *peventLabel);
   virtual  void mouseDoubleClickEvent (QMouseEvent *peventLabel);
   void paintEvent(QPaintEvent *event);
   int startPoint_x ;
   int startPoint_y;
   int width ;
   int height;
   uchar clickBottonStatus;
   bool isWorking;

   cv::Rect relateROI;

};

#endif // SHOWLABLE

