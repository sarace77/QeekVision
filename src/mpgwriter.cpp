#include "Defs.h"
#include "mpgwriter.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>

#include <v4lsettings.h>

MPGWriter::MPGWriter(double fps, Size fSize, QString sFileName) {
    _videoFileName = sFileName;
    _FPS = fps;
    _frameSize = fSize;
    if (_FPS < 1 || _FPS > 50)
        _FPS = 30;
    if ((fSize.width = 0) || (fSize.height == 0))
        _frameSize = Size(_STD_FRM_WDTH, _STD_FRM_HGHT);
}

int MPGWriter::exec() {
    QDateTime myDateTime;
    if (_videoFileName.isEmpty())
        _videoFileName = myDateTime.currentDateTime().toString("yyyymmdd-hh.mm") + ".mpg";
    if (QFile(_videoFileName).exists())
        _videoFileName = myDateTime.currentDateTime().toString("yyyymmdd-hh.mm") + ".mpg";
    const string NAME = _videoFileName.toStdString();
    VideoWriter _outStream;
    if(!_outStream.open(NAME, CV_FOURCC('P','I','M','1'), _FPS, _frameSize, true))
        qFatal("[MPG_WRITER] - exec() - Error opening stream");
    while(1) {
        if (_inBuffer.isEmpty()) {
            msleep(100);
        } else {
            if(_inBuffMtx.tryLock(2000)) {
                Mat src = _inBuffer.dequeue().clone();
                _outBuffer << src;
                cvtColor(src, src, CV_RGB2BGR);
                _outStream << src;
                emit availableProcessedFrame();
                _inBuffMtx.unlock();
            } else {
                qWarning() << "[MPG_WRITER] - exec() - Unable to lock Mutex";
            }
        }
    }
    return 0;
}

bool MPGWriter::hasToolBar() { return false; }

void MPGWriter::run() {
    qDebug() << "[BG_SUBTRACTOR] - run() - Starting";
    exec();
}

void MPGWriter::stop() {
    terminate();
    qDebug() << "[MPG_WRITER] - stop() - Stopped";
}

QToolBar *MPGWriter::toolBar() {
    QToolBar *tB = new QToolBar();
    return tB;
}
