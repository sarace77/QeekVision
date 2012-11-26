#include "opencvcamera.h"

#include <QDebug>

OpenCVCamera::OpenCVCamera() : CameraThread() {
    _camera = new VideoCapture();

    _enableVerboseMode = new QCheckBox("Enable Verbose");
    _threadToolBar->addSeparator();
    _threadToolBar->addWidget(_enableVerboseMode);
    connect(_settingsAction, SIGNAL(triggered()), this, SLOT(configure()));
    connect(&_watchdog, SIGNAL(timeout()), this, SLOT(timeout()));
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

void OpenCVCamera::timeout() {
    qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - timeout() - WatchDog timeout!";
    if (_enableVerboseMode->isChecked())
        qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - timeout() - Trying to stop!";
    stop();
    if (_enableVerboseMode->isChecked())
        qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - timeout() - Trying to start again!";
    start();
}

int OpenCVCamera::exec() {
    if (_enableVerboseMode->isChecked()) {
        qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Started!";
        qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Frame size: " << getWidth() << "x" << getHeight();
    }
    while(1) {
        _watchdog.start(5000);
        myTimer.start();
        if (_mutex.tryLock(2000)) {
            _watchdog.start(5000);
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
        if (_enableVerboseMode->isChecked())
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
                if (_enableVerboseMode->isChecked())
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
    if (!_mutex.tryLock())
        qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Trying to release locked Mutex!";
    _mutex.unlock();
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
