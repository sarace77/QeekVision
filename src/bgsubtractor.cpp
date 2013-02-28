#include "bgsubtractor.h"

#ifdef _DEBUG_PROCESS_THREADS
#include <QDebug>
#endif //_DEBUG_PROCESS_THREADS

BGSubtractor::BGSubtractor(QObject *parent) : ProcessThread(parent) {
    _bg = NULL;
    _rng = new RNG(12345);
    _bgSubtractorToolBar = new QToolBar();
    _bgSubtractorToolBar->setObjectName("BGSubtractor Toolbar");

    _historySpinBox = new QSpinBox();
    _historySpinBox->setToolTip("BG Subtractor history frames");
    _historySpinBox->setMinimum(2);
    _historySpinBox->setMaximum(65535);
    _historySpinBox->setValue(3000);

    _thresholdSpinBox = new QSpinBox();
    _thresholdSpinBox->setToolTip("BG Subtractor history threshold");
    _thresholdSpinBox->setMinimum(1);
    _thresholdSpinBox->setMaximum(256);
    _thresholdSpinBox->setValue(32);

    _bgSubtractorToolBar->addWidget(_historySpinBox);
    _bgSubtractorToolBar->addWidget(_thresholdSpinBox);
    _bgSubtractorToolBar->setEnabled(false);
    connect(this, SIGNAL(started()), this, SLOT(initBG()));
}

BGSubtractor::~BGSubtractor() {
    if (_bg != NULL)
        delete _bg;
    delete _rng;
    delete _bgSubtractorToolBar;
}

bool BGSubtractor::compareBlobs(vector<Point> firstBlob, vector<Point> secondBlob) {
    unsigned int count = 0;
    for (unsigned int i = 0; i < secondBlob.size(); i++) {
        if (pointPolygonTest(firstBlob, secondBlob.at(i), false) >= 0) {
            count++;
        }
    }
    return (count == secondBlob.size());
}

int BGSubtractor::exec() {
#ifdef _DEBUG_PROCESS_THREADS
    qDebug() << "[BG_SUBTRACTOR] - exec() - Started!";
#endif //_DEBUG_PROCESS_THREADS
    _bgSubtractorToolBar->setEnabled(true);
    while(1) {
        if (_inBuffer.isEmpty()) {
            msleep(100);
        } else {
            if(_inBuffMtx.tryLock(2000)) {
                _srcFrame = _inBuffer.dequeue();
                _bg->operator ()(_srcFrame, _fgFrame);
                _bg->getBackgroundImage(_bgFrame);
                erode(_fgFrame, _fgFrame, Mat());
                dilate(_fgFrame, _fgFrame, Mat());

                vector<vector<Point > > contours;
                vector<Vec4i> hierarchy;
                findContours( _fgFrame, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

                vector<vector<Point > > contoursPoly(contours.size());
                for (unsigned int i = 0; i < contours.size(); i++)
                    approxPolyDP(Mat(contours[i]), contoursPoly[i], 3, true);

                QList<vector<Point > > contoursPolyList;
                for (unsigned int i = 0; i < contours.size(); i++) {
                    contoursPolyList << contoursPoly.at(i);
                }

                for (int i = 0; i < contoursPolyList.size(); i++) {
                    for (int j = 0; j < i; j++) {
                        if ( compareBlobs(contoursPolyList.at(j), contoursPolyList.at(i)))
                            contoursPolyList.removeAt(i--);
                    }
                }
                vector<vector <Point > > bigContoursPoly;
                vector<Rect > blob(contoursPolyList.size());
                for (int i = 0; i < contoursPolyList.size(); i++) {
                    bigContoursPoly.push_back(contoursPolyList.at(i));
                    blob[i] = boundingRect(Mat(bigContoursPoly[i]));
                }
                for (int i = 0; i < contoursPolyList.size(); i++) {
                    drawContours(_srcFrame, bigContoursPoly, i, Scalar(255,0,0), 2);
                    rectangle(_srcFrame, blob[i], Scalar(_rng->uniform(0, 255), _rng->uniform(0, 255), _rng->uniform(0, 255)), 2);
                }
                _outBuffer.enqueue(_srcFrame);
                emit availableProcessedFrame();
                _inBuffMtx.unlock();
#ifdef _DEBUG_PROCESS_THREADS
            } else {
                qWarning() << "[BG_SUBTRACTOR] - exec() - Unable to lock Mutex";
#endif //_DEBUG_PROCESS_THREADS
            }
        }
    }
    return 0;
}

bool BGSubtractor::hasToolBar() {
    return true;
}

void BGSubtractor::initBG() {
    if(_inBuffMtx.tryLock(2000)) {
        if (_bg != NULL)
            delete _bg;
        _bg = new BackgroundSubtractorMOG2(_historySpinBox->value(), _thresholdSpinBox->value(), true);
        _inBuffMtx.unlock();
#ifdef _DEBUG_PROCESS_THREADS
    } else {
        qWarning() << "[BG_SUBTRACTOR] - initBG() - Unable to lock Mutex";
#endif //_DEBUG_PROCESS_THREADS
    }
}

void BGSubtractor::run() {
#ifdef _DEBUG_PROCESS_THREADS
    qDebug() << "[BG_SUBTRACTOR] - run() - Starting";
#endif //_DEBUG_PROCESS_THREADS
    exec();
}

void BGSubtractor::stop() {
    terminate();
#ifdef _DEBUG_PROCESS_THREADS
    qDebug() << "[BG_SUBTRACTOR] - stop() - Terminated";
#endif //_DEBUG_PROCESS_THREADS
}

QToolBar *BGSubtractor::toolBar() {
    return _bgSubtractorToolBar;
}
