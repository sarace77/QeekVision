#ifndef QEEKDEMOWINDOW_H
#define QEEKDEMOWINDOW_H

#include <QMainWindow>
#include "camerathread.h"
#include "qvdisplaywidget.h"
#include "qvdriverselect.h"

namespace Ui {
class QeekDemoWindow;
}

class QeekDemoWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::QeekDemoWindow *ui;
    CameraThread *capture3ad;
    QVDisplayWidget *imageWidget;
    QVDriverSelect *driverSelectDialog;

    Mat currentFrame;

private slots:
    void acceptedDriverSelection();
    void on_actionSaveFrame_triggered();
    void showFrame();

public:
    QeekDemoWindow(QWidget *parent = 0);
    ~QeekDemoWindow();
    
};

#endif // QEEKDEMOWINDOW_H
