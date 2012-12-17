#ifndef QVDISPLAYWIDGET_H
#define QVDISPLAYWIDGET_H

#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QWidget>

#include <opencv2/core/core.hpp>

using namespace cv;

namespace Ui {
class QVDisplayWidget;
}

class QVDisplayWidget : public QWidget
{
    Q_OBJECT

private:
    bool _mouseTracking;
    QPoint _pointerPos;
    QLabel *_textLabel;

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    bool _firsTime;

public:
    explicit QVDisplayWidget(QWidget *parent = 0);
    ~QVDisplayWidget();

    void displayImage(Mat src);
    void displayText(int x, int y, QString text);

    int getMouseXPos();
    int getMouseYPos();

    bool isMouseTrackingEnabled();

private:
    Ui::QVDisplayWidget *ui;
};

#endif // QVDISPLAYWIDGET_H
