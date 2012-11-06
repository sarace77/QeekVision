#include "circles.h"

#include <QDebug>
Circles::Circles(QObject *parent) : ProcessThread(parent) { }

Circles::~Circles() { }

bool Circles::hasToolBar() { return false; }
QToolBar *Circles::toolBar() { return NULL; }

int Circles::exec() {
    while(1) {
        if (_inBuffer.isEmpty()) {
            msleep(100);
        } else {
            if(_inBuffMtx.tryLock(2000)) {
                vector<Vec3f > circles;
                Mat srcFrame = _inBuffer.dequeue();
                Mat srcGray;
                cvtColor(srcFrame, srcGray, CV_BGR2GRAY);
                GaussianBlur(srcGray, srcGray, Size(9,9), 2, 2);
                HoughCircles(srcGray, circles, CV_HOUGH_GRADIENT, 1, srcGray.rows/8, 127, 63);
                for (size_t i = 0; i < circles.size(); i++) {
                    Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
                    int radius = cvRound(circles[i][2]);
                    circle(srcFrame, center, 3, Scalar(0,255,0), -1, 8, 0);
                    circle(srcFrame, center, radius, Scalar(255,0,0), 3, 8, 0);
                }
                _outBuffer.enqueue(srcFrame.clone());
                qDebug() << "Circles: " << circles.size();
                _inBuffMtx.unlock();
                emit availableProcessedFrame();
            } else {
                qWarning() << "[CIRCLES] - exec() - Unable to lock Mutex";
            }
        }
    }
    return 0;
}

void Circles::run() {
    exec();
}

void Circles::stop() {
    qDebug() << "[CIRCLES] - stop() - Stopping...";
    terminate();
}
