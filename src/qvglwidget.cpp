#include "qvglwidget.h"
#include "camerathread.h"

#include <opencv2/highgui/highgui.hpp>

QVGLWidget::QVGLWidget(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {
    _mouseTracking = false;
    _pointerPos = QPoint(-1, -1);
    _textLabel = new QLabel(this);
    resize(QSize(1,1));
}

void QVGLWidget::displayImage(Mat src) {
    glData = QGLWidget::convertToGLFormat(CameraThread::mat2qImage(src));
    resizeGL(src.cols, src.rows);
    resize(glData.size());
    paintGL();
    setMouseTracking(_mouseTracking);
}

void QVGLWidget::displayText(int x, int y, QString text) {
    _textLabel->setText(text);
    _textLabel->setGeometry(x, y, 200, 120);
}

int QVGLWidget::getMouseXPos() {
    return _pointerPos.x();
}

int QVGLWidget::getMouseYPos() {
    return _pointerPos.y();
}

bool QVGLWidget::isMouseTrackingEnabled() {
    return _mouseTracking || (_pointerPos.x() != -1 && _pointerPos.y() != -1);
}


void QVGLWidget::initializeGL() {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 0, 0);
}

void QVGLWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_Escape:
        close();
        break;
    default:
        event->ignore();
        break;
    }
}

void QVGLWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    switch(event->button()) {
    default:
        break;
    }
}

void QVGLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (_mouseTracking)
        _pointerPos = event->pos();
}

void QVGLWidget::mousePressEvent(QMouseEvent *event) {
    switch(event->button()) {
    case Qt::NoButton:
        break;
    case Qt::LeftButton:
        _pointerPos = event->pos();
        break;
    case Qt::RightButton:
        _pointerPos = QPoint(-1,-1);
        _mouseTracking = false;
        break;
    case Qt::MiddleButton:
        _mouseTracking = !_mouseTracking;
        break;
    default:
        break;
    }
}

void QVGLWidget::mouseReleaseEvent(QMouseEvent *event) {
     switch(event->button()) {
     default:
         break;
     }
}

void QVGLWidget::paintGL() {
    glDrawPixels(glData.width(), glData.height(), GL_RGBA, GL_UNSIGNED_BYTE, glData.bits());
}

void QVGLWidget::resizeGL(int w, int h) {
    glViewport (0, 0, w, h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w,0,h,-1,1);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}
