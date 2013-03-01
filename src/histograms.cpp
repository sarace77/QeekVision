#include "histograms.h"

#include <opencv2/imgproc/imgproc.hpp>

Histograms::Histograms()
{
}


Mat Histograms::plotHistogram(Mat src) {
    int numBins = 256;
    int histSize[] = {numBins};
    float range[] = {0, 255};
    const float *ranges[] = {range};
    MatND hist;
    int channels3[] = {0, 1, 2};
    int channels1[] = {0};

    switch(src.channels()) {
    case 3:
        calcHist(&src, 1, channels3, Mat(), hist, 1, histSize, ranges, true, false);
        break;
    default:
        calcHist(&src, 1, channels1, Mat(), hist, 1, histSize, ranges, true, false);
        break;
    }
    double maxVal=0;
    minMaxLoc(hist, 0, &maxVal, 0, 0);
    Mat histImg = Mat::zeros(256, 512, CV_8UC3);
    for (int i = 0; i < numBins; i++) {
        float binVal = hist.at<float>(i);
        int intensity = round(binVal*255/maxVal);
        rectangle(histImg, Point(2 * i, 255), Point(2 * (i + 1) - 1, 255 - intensity), Scalar(255,0,0), CV_FILLED);
    }

    return histImg;
}
