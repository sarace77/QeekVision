#ifndef CIRCLESMAINWINDOW_H
#define CIRCLESMAINWINDOW_H

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QTime>

#include "camerathread.h"
#include "circles.h"
#include "qvdisplaywidget.h"
#include "qvdriverselect.h"


namespace Ui {
class CirclesMainWindow;
}

class CirclesMainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::CirclesMainWindow *ui;

    /// QeekVision Objects
    CameraThread *capture3ad;
    QVDriverSelect *driverSelectDialog;
    Circles *process3ad;

    /// Internal members
    Mat src;

    /// GUI Objects
    QVDisplayWidget *blurWidget, *cannyWidget, *histogramWidget, *imageWidget;

private slots:
    void acceptedDriverSelection();
    void processFrame();
    void showFrame();

public:
    explicit CirclesMainWindow(QWidget *parent = 0);
    ~CirclesMainWindow();
    
};

#endif // CIRCLESMAINWINDOW_H
