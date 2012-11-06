#ifndef VIDEOWRITERDEMOWINDOW_H
#define VIDEOWRITERDEMOWINDOW_H

#include <QMainWindow>

#include "camerathread.h"
#include "mpgwriter.h"

namespace Ui {
class videoWriterDemoWindow;
}

class videoWriterDemoWindow : public QMainWindow
{
    Q_OBJECT
    
private:
    Ui::videoWriterDemoWindow *ui;
    CameraThread *capture3ad;
    MPGWriter *process3ad;
    bool once;

private slots:
    void showFrame();
    void _debugShow();

public:
    explicit videoWriterDemoWindow(QWidget *parent = 0);
    ~videoWriterDemoWindow();
    
};

#endif // VIDEOWRITERDEMOWINDOW_H
