#ifndef VIDEOWRITERDEMOWINDOW_H
#define VIDEOWRITERDEMOWINDOW_H

#include <QMainWindow>

#include "camerathread.h"
#include "mpgwriter.h"
#include "qvdisplaywidget.h"
#include "qvdriverselect.h"

namespace Ui {
class videoWriterDemoWindow;
}

class videoWriterDemoWindow : public QMainWindow
{
    Q_OBJECT
    
private:
    Ui::videoWriterDemoWindow *ui;
    CameraThread *capture3ad;
    QVDriverSelect *driverSelectDialog;
    QVDisplayWidget *imageWidget;
    MPGWriter *process3ad;

    bool once;

private slots:
    void acceptedDriverSelection();
    void showFrame();
    void _debugShow();

public:
    explicit videoWriterDemoWindow(QWidget *parent = 0);
    ~videoWriterDemoWindow();
    
};

#endif // VIDEOWRITERDEMOWINDOW_H
