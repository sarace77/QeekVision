#include "qvdisplaywidget.h"

#include "camerathread.h"

QVDisplayWidget::QVDisplayWidget(QWidget *parent) : QGraphicsView(parent) {
    _mouseTracking = false;
    _pointerPos = QPoint(-1, -1);
    _scene = new QGraphicsScene(this);
    _textLabel = new QLabel(this);
    setScene(_scene);
    _pixmap = NULL;
}

QVDisplayWidget::~QVDisplayWidget() {
}

void QVDisplayWidget::displayImage(Mat src) {
    setGeometry(0, 0, src.cols + 10, src.rows + 10);
    if (_pixmap != NULL) {
        _scene->removeItem(_pixmap);
        delete _pixmap;
    }
    _pixmap = new QGraphicsPixmapItem(QPixmap::fromImage(CameraThread::mat2qImage(src)));
    _scene->addItem(_pixmap);
    setMouseTracking(_mouseTracking);
}

void QVDisplayWidget::displayText(int x, int y, QString text) {
    _textLabel->setText(text);
    _textLabel->setGeometry(x, y, 200, 120);
}

int QVDisplayWidget::getMouseXPos() {
    return _pointerPos.x();
}

int QVDisplayWidget::getMouseYPos() {
    return _pointerPos.y();
}

bool QVDisplayWidget::isMouseTrackingEnabled() {
    return _mouseTracking || (_pointerPos.x() != -1 && _pointerPos.y() != -1);
}

void QVDisplayWidget::mousePressEvent(QMouseEvent *event) {
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

void QVDisplayWidget::mouseReleaseEvent(QMouseEvent *event) {
     switch(event->button()) {
     default:
         break;
     }
}

void QVDisplayWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    switch(event->button()) {
    default:
        break;
    }
}

void QVDisplayWidget::mouseMoveEvent(QMouseEvent *event) {
    if (_mouseTracking)
        _pointerPos = event->pos();
}
