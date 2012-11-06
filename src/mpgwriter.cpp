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
    _adjustedFPS = _FPS;
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
    connect(this, SIGNAL(receivedFrame()), this, SLOT(adjustFrameRate()));
}

void MPGWriter::adjustFrameRate() {
    if ( 0 != _count++ ) {
        float fps = 1000.0 / _timer.elapsed();
        if ((_adjustedFPS + 5) < fps || (_adjustedFPS - 5 ) > fps) {
            if (fps > 30)
                _adjustedFPS = 30;
            if (fps + 4.99 < 30 && fps >= 25)
                _adjustedFPS = 25;
            if (fps + 4.99 < 25 && fps >= 20)
                _adjustedFPS = 20;
            if (fps + 4.99 < 20 && fps >= 15)
                _adjustedFPS = 15;
            if (fps + 4.99 < 15 && fps >= 10)
                _adjustedFPS = 10;
            if (fps + 4.99 < 10 ) {
                qDebug() << fps;
                _adjustedFPS = 5;
            }
        }
    }
    _timer.start();
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

void MPGWriter::enqueue(Mat frm) {
    emit receivedFrame();
    if(_inBuffMtx.tryLock(2000)) {
        _inBuffer.enqueue(frm);
        _inBuffMtx.unlock();
    } else {
        qWarning() << "[PROCESS_THREAD] - enqueue() - Unable to lock Mutex";
        return;
    }
}


int MPGWriter::exec() {
    _outStream = new VideoWriter();
    if(!_outStream->open(_videoFileName.toStdString(), CV_FOURCC('P','I','M','1'), _FPS, _frameSize, true))
        qFatal("[MPG_WRITER] - exec() - Error opening stream (1)");
    while(1) {
        if (_inBuffer.isEmpty()) {
            msleep(100);
        } else {
            if(_inBuffMtx.tryLock(2000)) {
                Mat src = _inBuffer.dequeue().clone();
                if (checkFrame(src)) {
                    qDebug() << "[MPG_WRITER] - exec() - Adjusting video frame size to:" << _frameSize.width << "x" << _frameSize.height;
                    delete _outStream;
                    _outStream = new VideoWriter();
                    if(!_outStream->open(_videoFileName.toStdString(), CV_FOURCC('D','I','V','X'), _FPS, _frameSize, true))
                        qFatal("[MPG_WRITER] - exec() - Error opening stream (2)");
                }
                _outBuffer << src;
                cvtColor(src, src, CV_RGB2BGR);
//                qDebug() << "Frame ratio: " << ((int) _FPS/_adjustedFPS);
//                for (int i = 0; i < ((int) _FPS/_adjustedFPS) - 1; i++) {
//                    qDebug() << "Frame Repeated:" << i;
//                    _outStream->write(src);
//                }
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
