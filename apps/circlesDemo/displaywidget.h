#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QMouseEvent>
#include <QPoint>
#include <QWidget>

namespace Ui {
class DisplayWidget;
}

class DisplayWidget : public QWidget
{
    Q_OBJECT

private:
    QPoint mousePosition;

public:
    explicit DisplayWidget(QWidget *parent = 0);
    ~DisplayWidget();
    int getMouseXPos();
    int getMouseYPos();

private:
    Ui::DisplayWidget *ui;
    void mousePressEvent(QMouseEvent * mEvent);
};

#endif // DISPLAYWIDGET_H
