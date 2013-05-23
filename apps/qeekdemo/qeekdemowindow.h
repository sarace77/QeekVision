#ifndef QEEKDEMOWINDOW_H
#define QEEKDEMOWINDOW_H

#include <QtCore>
#include <QtGui>

#include "camerathread.h"
#include "camerawrapper.h"
#include "qvdisplaywidget.h"
#include "qvdriverselect.h"
#include "qvglwidget.h"

namespace Ui {
class QeekDemoWindow;
}

class QeekDemoWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::QeekDemoWindow *ui;
    CameraThread *capture3ad;
    CameraWrapper *device;
    QVDisplayWidget  *imageWidget_1;
    QVGLWidget  *imageWidget_2;
    QVDriverSelect *driverSelectDialog;
    QToolBar *widgetSelectorToolBar;
    QRadioButton *openGlButton, *graphicSceneButton;

    Mat currentFrame;

private slots:
    void deviceChanged();
    void on_actionSaveFrame_triggered();
    void showFrame();

public:
    QeekDemoWindow(QWidget *parent = 0);
    ~QeekDemoWindow();
    
};

#endif // QEEKDEMOWINDOW_H
