#include "opencvcamera.h"

OpenCVCamera::OpenCVCamera() : CameraThread() {
    _camera = new VideoCapture();
    _enableVerboseMode = new QCheckBox("Enable Verbose");
#ifdef _DEBUG_CAPTURE_THREADS
    _threadToolBar->addSeparator();
    _threadToolBar->addWidget(_enableVerboseMode);
#endif //_DEBUG_CAPTURE_THREADS

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
#ifdef _DEBUG_CAPTURE_THREADS
    qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Frame size: " << getWidth() << "x" << getHeight();
    qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Started!";
#endif //_DEBUG_CAPTURE_THREADS
    while(1) {
        _cvMatbuffer.clear();
        myTimer.start();
        if (_camera->isOpened()) {
            if (_mutex.tryLock(2000)) {
                static Mat img;
                _camera->read(img);
                _cvMatbuffer.enqueue(img.clone());
#ifdef _DEBUG_CAPTURE_THREADS
                if (_enableVerboseMode->isChecked())
                    qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Enqueued frame; " << _cvMatbuffer.count();
#endif //_DEBUG_CAPTURE_THREADS
                _mutex.unlock();
                _fps = 1000.0/myTimer.elapsed();
                emit availableFrame();
#ifdef _DEBUG_CAPTURE_THREADS
            } else {
                qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Unable to lock Mutex: ";
#endif //_DEBUG_CAPTURE_THREADS
            }
#ifdef _DEBUG_CAPTURE_THREADS
        } else {
            qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - No camera device opened!";
#endif //_DEBUG_CAPTURE_THREADS
        }
        int elapsedTime = myTimer.elapsed() > 0? myTimer.elapsed() : 0;
        msleep( elapsedTime < 20 ? 20 - elapsedTime : 1 );
#ifdef _DEBUG_CAPTURE_THREADS
        if (_enableVerboseMode->isChecked())
            qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - exec() - Framerate: " << 1000.0 / ((float) elapsedTime);
#endif //_DEBUG_CAPTURE_THREADS
    }
    return 0;
}

void OpenCVCamera::run() {
    _startAction->setEnabled(false);
    _stopAction->setEnabled(true);
    _settingsAction->setEnabled(false);
    if (_camera->isOpened()) {
#ifdef _DEBUG_CAPTURE_THREADS
        qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Device already opened!";
#endif //_DEBUG_CAPTURE_THREADS
        exec();
    } else {
        if (_camera->open(_deviceName.toAscii().constData())){
            exec();
        } else {
#ifdef _DEBUG_CAPTURE_THREADS
            qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Unable to open device: " << _deviceName;
            if (_enableVerboseMode->isChecked())
                qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Trying to open first device";
#endif //_DEBUG_CAPTURE_THREADS
            if(_camera->open(0))  {
#ifdef _DEBUG_CAPTURE_THREADS
                qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - camera0 opened!";
#endif //_DEBUG_CAPTURE_THREADS
                exec();
#ifdef _DEBUG_CAPTURE_THREADS
            } else {
                qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - run() - Unable to open default Capture Device!";
#endif //_DEBUG_CAPTURE_THREADS
            }
        }
    }
}

void OpenCVCamera::stop() {
#ifdef _DEBUG_CAPTURE_THREADS
    qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Stopping...";
#endif //_DEBUG_CAPTURE_THREADS
    _startAction->setEnabled(true);
    _stopAction->setEnabled(false);
    _settingsAction->setEnabled(false);
    _camera->release();
    this->terminate();
    while(isRunning());
#ifdef _DEBUG_CAPTURE_THREADS
    if (!isRunning())
        qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Stopped";
    else
        qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Process still running!";
    if (_camera->isOpened())
        qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Unable to release Camera Device";
    else
        qDebug() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Camera device succesfully released";
#endif //_DEBUG_CAPTURE_THREADS
    if (!_mutex.tryLock()) {
#ifdef _DEBUG_CAPTURE_THREADS
        qWarning() << "[CAMERA_THREAD::OPEN_CV_CAMERA] - stop() - Trying to release locked Mutex!";
#endif //_DEBUG_CAPTURE_THREADS
    }
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
