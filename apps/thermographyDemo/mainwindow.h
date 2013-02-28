#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QtGui>
#include "camerathread.h"
#include "processthread.h"
#include "qvdisplaywidget.h"
#include "qvdriverselect.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
private:
    Ui::MainWindow *ui;

    CameraThread *capture3ad;

    ProcessThread *process3ad;

    QVDisplayWidget *imageWidget;
    QVDriverSelect *driverSelectDialog;
    QFileDialog *fDialog;

private slots:
    void acceptedDriverSelection();
    void on_actionOpen_triggered();
    void openFile();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
    void processFrame();
    void showFrame();

};

#endif // MAINWINDOW_H
