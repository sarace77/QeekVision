#ifndef QEEKDEMOWINDOW_H
#define QEEKDEMOWINDOW_H

#include <QMainWindow>
#include "camerathread.h"

namespace Ui {
class QeekDemoWindow;
}

class QeekDemoWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::QeekDemoWindow *ui;
    CameraThread *capture3ad;

private slots:
    void showFrame();

public:
    QeekDemoWindow(QWidget *parent = 0);
    ~QeekDemoWindow();
    
};

#endif // QEEKDEMOWINDOW_H
