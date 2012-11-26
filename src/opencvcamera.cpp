#include "opencvcamera.h"

#include <QDebug>

OpenCVCamera::OpenCVCamera() : CameraThread() {
    _camera = new VideoCapture();
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
    qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Started!";
    while(1) {
        myTimer.start();
        if (_mutex.tryLock(2000)) {
            static Mat img;
            _camera->read(img);
            _cvMatbuffer.enqueue(img);
            //_cvMatbuffer.enqueue(img.clone());
            _mutex.unlock();
            _fps = 1000.0/myTimer.elapsed();
            emit availableFrame();
            } else {
                qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Unable to lock Mutex: ";
            }
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
    qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - Stopping...";
    _startAction->setEnabled(true);
    _stopAction->setEnabled(false);
    _settingsAction->setEnabled(false);
    this->terminate();
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
