#include "Defs.h"
#include "mpgwriter.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStringList>

#include <v4lsettings.h>

MPGWriter::MPGWriter(QString sFileName) {
    _outStream = NULL;
    _videoFileName = sFileName;
    _FPS = _STD_FRM_RT;
    _frameSize = Size(_STD_FRM_WDTH, _STD_FRM_HGHT);

    _startRecordAction = new QAction(QIcon(":/icons/record.png"), "Start", this);
    _startRecordAction->setToolTip("Start Recording...");
    _startRecordAction->setEnabled(false);

    _stopRecordAction = new QAction(QIcon(":/icons/stop2.png"), "Stop", this);
    _stopRecordAction->setToolTip("Stop Recording!");
    _stopRecordAction->setEnabled(false);

    _mpgwriterToolBar = new QToolBar("Writer Commands");
    _mpgwriterToolBar->setObjectName("mpgwriterToolBar");
    _mpgwriterToolBar->addAction(_startRecordAction);
    _mpgwriterToolBar->addAction(_stopRecordAction);

    connect(_startRecordAction, SIGNAL(triggered()), this, SLOT(start()));
    connect(_stopRecordAction, SIGNAL(triggered()), this, SLOT(stop()));
}

bool MPGWriter::checkFrame(Mat frame) {
    if (frame.cols != _frameSize.width || frame.rows != _frameSize.height ) {
        _frameSize.width = frame.cols;
        _frameSize.height = frame.rows;
        QStringList name = _videoFileName.split('.');
        _videoFileName = name.at(0) + QString("-%1.mpg").arg(_count);
        return true;
    }
    return false;
}

int MPGWriter::exec() {
    QTime myTimer;
    double fps = _FPS;
    unsigned int count = 0;
    _outStream = new VideoWriter();
    if(!_outStream->open(_videoFileName.toStdString(), CV_FOURCC('P','I','M','1'), _FPS, _frameSize, true))
        qFatal("[MPG_WRITER] - exec() - Error opening stream (1)");
    myTimer.start();
    while(1) {
        if (_inBuffer.isEmpty()) {
            msleep(50);
        } else {
            if(_inBuffMtx.tryLock(2000)) {
                Mat src = _inBuffer.dequeue().clone();
                if (checkFrame(src)) {
                    qDebug() << "[MPG_WRITER] - exec() - Adjusting video frame size to:" << _frameSize.width << "x" << _frameSize.height;
                    delete _outStream;
                    _outStream = new VideoWriter();
                    if(!_outStream->open(_videoFileName.toStdString(), CV_FOURCC('P','I','M','1'), _FPS, _frameSize, true))
                        qFatal("[MPG_WRITER] - exec() - Error opening stream (2)");

                }
                _outBuffer << src;
                cvtColor(src, src, CV_RGB2BGR);
                _outStream->write(src);
                emit availableProcessedFrame();
                _inBuffMtx.unlock();
            } else {
                qWarning() << "[MPG_WRITER] - exec() - Unable to lock Mutex";
            }
        }
    }
    return 0;
}

bool MPGWriter::hasToolBar() {
    return true;
}

void MPGWriter::run() {
    qDebug() << "[BG_SUBTRACTOR] - run() - Starting";
    QDateTime myDateTime;
    _startRecordAction->setEnabled(false);
    _stopRecordAction->setEnabled(true);
    if (_videoFileName.isEmpty())
        _videoFileName = myDateTime.currentDateTime().toString("yyyymmdd-hhmm") + ".mpg";
    if (QFile(_videoFileName).exists())
        _videoFileName = myDateTime.currentDateTime().toString("yyyymmdd-hhmm") + ".mpg";
    if (_outStream != NULL)
        delete _outStream;
    _count = 0;
    exec();
}

void MPGWriter::stop() {
    _startRecordAction->setEnabled(true);
    _stopRecordAction->setEnabled(false);
    if (isRunning()) {
        terminate();
        qDebug() << "[MPG_WRITER] - stop() - Stopped";
    }
}

QToolBar *MPGWriter::toolBar() {
    return _mpgwriterToolBar;
}
