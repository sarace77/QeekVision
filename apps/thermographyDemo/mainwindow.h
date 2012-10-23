#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "camerathread.h"
#include "thermography.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
private:
    Ui::MainWindow *ui;
    CameraThread *capture3ad;
    Thermography *process3ad;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
    void processFrame();
    void showFrame();

};

#endif // MAINWINDOW_H
