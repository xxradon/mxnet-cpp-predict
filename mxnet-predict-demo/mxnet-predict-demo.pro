#-------------------------------------------------
#
# Project created by QtCreator 2016-07-16T14:17:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mxnet-predict-demo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    image-classification.cpp \
    showlabel.cpp

HEADERS  += mainwindow.h \
    image-classification.h \
    showlabel.h

FORMS    += mainwindow.ui

INCLUDEPATH +=  D:/opencv3/build/include\
                D:/opencv3/build/include/opencv\
                D:/opencv3/build/include/opencv2

INCLUDEPATH += mxnet\include

win32:CONFIG(debug, debug|release): {
LIBS += D:/opencv3/build/x64/vc12/lib/opencv_world310d.lib
LIBS += D:\mxnet\build\lib\windows\Debug\libmxnet.lib

} else:win32:CONFIG(release, debug|release): {
LIBS += D:/opencv3/build/x64/vc12/lib/opencv_world310.lib
LIBS += D:\mxnet\build\lib\windows\Release\libmxnet.lib
}
