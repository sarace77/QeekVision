#include "opencvcamera.h"

#include <QDebug>

OpenCVCamera::OpenCVCamera() : CameraThread() {
    _camera = new VideoCapture();
    _enableVerboseMode = new QCheckBox("Enable Verbose");
    _threadToolBar->addSeparator();
    _threadToolBar->addWidget(_enableVerboseMode);

    connect(_settingsAction, SIGNAL(triggered()), this, SLOT(configure()));
}

OpenCVCamera::~OpenCVCamera() {
    if (isRunning())
        terminate();
    if(_camera->isOpened())
        _camera->release();
    delete _camera;
}

void OpenCVCamera::configure() {
    _settingsAction->setEnabled(false);
    _startAction->setEnabled(true);
    _stopAction->setEnabled(false);
}

int OpenCVCamera::exec() {
    qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Frame size: " << getWidth() << "x" << getHeight();
    qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Started!";
    while(1) {
        _cvMatbuffer.clear();
        myTimer.start();
        if (_camera->isOpened()) {
            if (_mutex.tryLock(2000)) {
                static Mat img;
                _camera->read(img);
                _cvMatbuffer.enqueue(img);
                if (_enableVerboseMode->isChecked())
                    qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Enqueued frame; " << _cvMatbuffer.count();
                //_cvMatbuffer.enqueue(img.clone());
                _mutex.unlock();
                _fps = 1000.0/myTimer.elapsed();
                emit availableFrame();
            } else {
                qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Unable to lock Mutex: ";
            }
        } else {
            qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - No camera device opened!";
        }
        int elapsedTime = myTimer.elapsed() > 0? myTimer.elapsed() : 0;
        msleep( elapsedTime < 20 ? 20 - elapsedTime : 1 );
        if (_enableVerboseMode->isChecked())
            qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Framerate: " << 1000.0 / ((float) elapsedTime);
    }
    return 0;
}

void OpenCVCamera::run() {
    _startAction->setEnabled(false);
    _stopAction->setEnabled(true);
    _settingsAction->setEnabled(false);
    if (_camera->isOpened()) {
        qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Device already opened!";
        exec();
    } else {
        if (_camera->open(_deviceName.toAscii().constData())){
            /// Enabling/Disabling Toolbar Buttons
            exec();
        } else {
            qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Unable to open device: " << _deviceName;
            if (_enableVerboseMode->isChecked())
                qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Trying to open first device";
            if(_camera->open(0))  {
                qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - camera0 opened!";
                exec();
            } else {
                qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Unable to open default Capture Device!";
            }
        }
    }
}

void OpenCVCamera::stop() {
    qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Stopping...";
    _startAction->setEnabled(true);
    _stopAction->setEnabled(false);
    _settingsAction->setEnabled(false);
    _camera->release();
    this->terminate();
    while(isRunning());
    if (!isRunning())
        qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Stopped";
    else
        qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Process still running!";
    if (_camera->isOpened())
        qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Unable to release Camera Device";
    else
        qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Camera device succesfully released";
    if (!_mutex.tryLock())
        qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Trying to release locked Mutex!";
    _cvMatbuffer.clear();
    _mutex.unlock();
}


int OpenCVCamera::getHeight() {
    if (_camera->isOpened()) {
        return (int) (_camera->get(CV_CAP_PROP_FRAME_HEIGHT));
    }
    return _STD_FRM_HGHT;
}

int OpenCVCamera::getWidth() {
    if (_camera->isOpened()) {
        return (int) (_camera->get(CV_CAP_PROP_FRAME_WIDTH));
    }
    return _STD_FRM_WDTH;
}

bool OpenCVCamera::isConfigurated() {
    return _camera->isOpened();
}
