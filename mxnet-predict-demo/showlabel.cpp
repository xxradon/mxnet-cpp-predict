
//ShowLabel.cpp
#include "showlabel.h"



ShowLabel::ShowLabel (QWidget *parent)
    : QLabel(parent)
{
    setMouseTracking (true);
    startPoint_x=0;
    startPoint_y=0;
    width=0;
    height=0;
    clickBottonStatus=0;
    isWorking=false;
}

void ShowLabel::mouseDoubleClickEvent (QMouseEvent *peventLabel)
{
    if (Qt::LeftButton == peventLabel->button ())
    {
        //emit handleMouseEvent (MOUSE_DOUBLECLICK);
    }
    return;
}

void ShowLabel::paintEvent(QPaintEvent *event)
{

        QLabel::paintEvent(event);//先调用父类的paintEvent为了显示‘背景‘!!!
        if(isWorking)
       {
            QPainter painter(this);
            if( clickBottonStatus == 1)
            {
                painter.setPen(QPen(Qt::blue,2));
                painter.drawRect(startPoint_x,startPoint_y,width,height);
            }
            for(size_t i=0;i< storeRects.size();i++)
            {
              QRect qrect(storeRects[i].x,storeRects[i].y,storeRects[i].width,storeRects[i].height);
              painter.setPen(QPen(Qt::red,2));
              painter.drawRect(qrect);
              painter.setPen(QPen(Qt::green,2));
              painter.drawText(QPoint(qrect.topRight()),QString::number(i+1));

            }
       }
}

void ShowLabel::setStoreRects(char handleType)
{
    if(handleType == PopLast)
     {
      if(!storeRects.empty())
      {
       storeRects.pop_back();
      }
    }
    else if(handleType == ClearAll)
    {
        if(!storeRects.empty())
        {
         storeRects.clear();
        }
    }
}



void ShowLabel::setIsWorking(bool value)
{
    isWorking = value;
}

void ShowLabel::mouseMoveEvent (QMouseEvent *peventLabel)
{
    if (peventLabel->MouseMove == peventLabel->type () && isWorking)
    {
        //emit handleMouseEvent (MOUSE_MOVE);
        if( clickBottonStatus == 1)
        {
            width=peventLabel->x()-startPoint_x;
            height=peventLabel->y()-startPoint_y;
            update();
        }
    }

    return;
}

void ShowLabel::mousePressEvent (QMouseEvent *peventLabel)
{
   if (Qt::LeftButton == peventLabel->button ()&&isWorking)
    {

         startPoint_x = peventLabel->x();
         startPoint_y = peventLabel->y();
//         width=peventLabel->x()-startPoint_x;
//         height=peventLabel->y()-startPoint_y;
         clickBottonStatus = 1;

    }

}
//按键恢复
void ShowLabel::mouseReleaseEvent (QMouseEvent *peventLabel)
{

    if (Qt::LeftButton == peventLabel->button () &&isWorking)
    {
        clickBottonStatus = 2;
        cv::Rect rect(startPoint_x,startPoint_y,width,height);
        width=0;
        height=0;
        storeRects.push_back(rect);
       // emit handleMouseEvent (MOUSE_RELEASE);
    }
    return;
}

void ShowLabel::leaveEvent (QEvent *peventLabel)
{

    Q_UNUSED (peventLabel);
   // emit handleMouseEvent (MOUSE_LEAVE);
    return;
}
