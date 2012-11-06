#ifndef CIRCLES_H
#define CIRCLES_H

#include <opencv2/imgproc/imgproc.hpp>

#include "processthread.h"

using namespace cv;
using namespace std;

class Circles : public ProcessThread
{
    Q_OBJECT

protected slots:
    int exec();
    void run();
    void stop();

public:
    Circles(QObject *parent = 0);
    ~Circles();

    bool hasToolBar();
    QToolBar *toolBar();
};

#endif // CIRCLES_H
