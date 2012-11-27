#ifndef CIRCLESMAINWINDOW_H
#define CIRCLESMAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QHBoxLayout>
#include <QLabel>

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
    QHBoxLayout *ellipseLayout;
    QLabel  *ellipseLabel;
private slots:
    void processFrame();
    void showFrame();

public:
    explicit CirclesMainWindow(QWidget *parent = 0);
    ~CirclesMainWindow();
    
};

#endif // CIRCLESMAINWINDOW_H
