#ifndef THERMOGRAPHY_H
#define THERMOGRAPHY_H

#include "Defs.h"
#include "processthread.h"

class Thermography : public ProcessThread
{
    Q_OBJECT

public:
    Thermography(QObject *parent = 0);
    ~Thermography();

    bool hasToolBar();
    QToolBar *toolBar();

private:
    QToolBar *_thermographyToolBar;
    QSlider *_blueSlider;
    QSpinBox *_blueSpin;

private slots:
    int exec();
    void run();
    void stop();
};

#endif // THERMOGRAPHY_H
