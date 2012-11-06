#ifndef CIRCLESMAINWINDOW_H
#define CIRCLESMAINWINDOW_H

#include <QMainWindow>

#include "camerathread.h"
#include "circles.h"

namespace Ui {
class CirclesMainWindow;
}

class CirclesMainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::CirclesMainWindow *ui;
    CameraThread *capture3ad;
    Circles *process3ad;

private slots:
    void processFrame();
    void showFrame();

public:
    explicit CirclesMainWindow(QWidget *parent = 0);
    ~CirclesMainWindow();
    
};

#endif // CIRCLESMAINWINDOW_H
