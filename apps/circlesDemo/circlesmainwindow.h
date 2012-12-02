#ifndef CIRCLESMAINWINDOW_H
#define CIRCLESMAINWINDOW_H

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QTime>

#include "opencvcamera.h"
#include "circles.h"


namespace Ui {
class CirclesMainWindow;
}

class CirclesMainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::CirclesMainWindow *ui;
    OpenCVCamera *capture3ad;
    Circles *process3ad;
    Mat src;
    QTime dialogTimer;
    QDialog *blurDialog, *cannyDialog;
    QHBoxLayout *blurLayout, *cannyLayout, *ellipseLayout;
    QLabel  *blurLabel, *cannyLabel, *ellipseLabel;
private slots:
    void processFrame();
    void showFrame();

public:
    explicit CirclesMainWindow(QWidget *parent = 0);
    ~CirclesMainWindow();
    
};

#endif // CIRCLESMAINWINDOW_H
