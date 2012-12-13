#ifndef HISTOGRAMS_H
#define HISTOGRAMS_H

#include <opencv2/core/core.hpp>

using namespace cv;

class Histograms
{
public:
    Histograms();

    static Mat plotHistogram(Mat src);
};

#endif // HISTOGRAMS_H
