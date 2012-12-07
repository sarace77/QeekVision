#include "gigeqeekdemowindow.h"
#include "ui_gigeqeekdemowindow.h"

#include <QDebug>

GigeQeekDemoWindow::GigeQeekDemoWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GigeQeekDemoWindow)
{
    ui->setupUi(this);
    capture3ad = new GigECamera();
    addToolBar(capture3ad->toolBar());

}

GigeQeekDemoWindow::~GigeQeekDemoWindow()
{
    if (capture3ad->isRunning())
        capture3ad->terminate();
    capture3ad->deleteLater();
    delete ui;
}
