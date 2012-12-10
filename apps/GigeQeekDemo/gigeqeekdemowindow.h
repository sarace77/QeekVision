#ifndef GIGEQEEKDEMOWINDOW_H
#define GIGEQEEKDEMOWINDOW_H

#include <QMainWindow>

#include "gigecamera.h"

namespace Ui {
class GigeQeekDemoWindow;
}

class GigeQeekDemoWindow : public QMainWindow
{
    Q_OBJECT
    
private:
    GigECamera * capture3ad;

private slots:
    void showFrame();

public:
    explicit GigeQeekDemoWindow(QWidget *parent = 0);
    ~GigeQeekDemoWindow();
    
private:
    Ui::GigeQeekDemoWindow *ui;
};

#endif // GIGEQEEKDEMOWINDOW_H
