#ifndef QVDISPLAYWIDGET_H
#define QVDISPLAYWIDGET_H

#include <opencv2/core/core.hpp>
#include <QtGui>

using namespace cv;

class QVDisplayWidget : public QGraphicsView
{
    Q_OBJECT

private:
    bool _mouseTracking;
    QGraphicsPixmapItem *_pixmap;
    QPoint _pointerPos;
    QGraphicsScene *_scene;
    QLabel *_textLabel;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

public:
    explicit QVDisplayWidget(QWidget *parent = 0);
    ~QVDisplayWidget();

    void displayImage(Mat src);
    void displayText(int x, int y, QString text);

    int getMouseXPos();
    int getMouseYPos();

    bool isMouseTrackingEnabled();

};

#endif // QVDISPLAYWIDGET_H
