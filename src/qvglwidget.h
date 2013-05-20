#ifndef QVGLWIDGET_H
#define QVGLWIDGET_H

#include <QGLWidget>
#include <QtGui>

#include <qgl.h>

#include <opencv2/core/core.hpp>

using namespace cv;

class QVGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit QVGLWidget(QWidget *parent = 0);
    
    void displayImage(Mat src);
    void displayText(int x, int y, QString text);

    int getMouseXPos();
    int getMouseYPos();

    bool isMouseTrackingEnabled();

protected:
    void initializeGL();
    void keyPressEvent(QKeyEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintGL();
    void resizeGL(int w, int h);

protected:
    bool _mouseTracking;
    QPoint _pointerPos;
    QLabel *_textLabel;
    QImage data, glData;

signals:
    
public slots:
    
};

#endif // QVGLWIDGET_H
