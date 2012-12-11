#include "displaywidget.h"
#include "ui_displaywidget.h"

#include <QDebug>

DisplayWidget::DisplayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DisplayWidget)
{
    ui->setupUi(this);
    mousePosition = QPoint(-1, -1);
    setMouseTracking(true);
}

DisplayWidget::~DisplayWidget()
{
    delete ui;
}

int DisplayWidget::getMouseXPos() {
    return mousePosition.x();
}

int DisplayWidget::getMouseYPos() {
    return mousePosition.y();
}

void DisplayWidget::mousePressEvent(QMouseEvent *mEvent) {
    if (mEvent->button() == Qt::RightButton)
        mousePosition = QPoint(-1,-1);
    else
        mousePosition = mEvent->pos();
}
