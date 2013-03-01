#include "qvdisplaywidget.h"
#include "ui_qvdisplaywidget.h"

#include "camerathread.h"

#include <QDebug>

QVDisplayWidget::QVDisplayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QVDisplayWidget)
{
    ui->setupUi(this);
    _mouseTracking = false;
    _pointerPos = QPoint(-1, -1);
    _textLabel = new QLabel(this);

    _firsTime = true;
}

QVDisplayWidget::~QVDisplayWidget()
{
    delete ui;
}

void QVDisplayWidget::displayImage(Mat src) {
    if (_firsTime) {
        setGeometry(10, 10, src.cols, src.rows);
        ui->imageLabel->setGeometry(0, 0, src.cols, src.rows);
        _firsTime = false;
    }
    ui->imageLabel->setPixmap(QPixmap::fromImage(CameraThread::mat2qImage(src)));
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
