#ifndef THERMOGRAPHY_H
#define THERMOGRAPHY_H

#include <opencv/cv.h>

#include <QMutex>
#include <QQueue>
#include <QThread>

using namespace cv;

class Thermography : public QThread
{
    Q_OBJECT

private:
    QQueue<Mat > _inBuffer;
    QQueue<Mat > _outBuffer;
    QMutex _inBuffMtx;
protected slots:
    int exec();
    void run();

public:
    Thermography(QObject *parent = 0);
    
public slots:
    Mat dequeue();
    void enqueue(Mat frm);
    void stop();

signals:
    void availableThermography();
};

#endif // THERMOGRAPHY_H
