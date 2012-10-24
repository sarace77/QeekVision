#ifndef THERMOGRAPHY_H
#define THERMOGRAPHY_H

#include <opencv/cv.h>

#include <QMutex>
#include <QQueue>
#include <QThread>

#include "processthread.h"

using namespace cv;

class Thermography : public ProcessThread
{
    Q_OBJECT

protected slots:
    int exec();
    void run();

public:
    Thermography(QObject *parent = 0);
    
};

#endif // THERMOGRAPHY_H
