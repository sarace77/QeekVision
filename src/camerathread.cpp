#include "camerathread.h"

CameraThread::CameraThread(QObject *parent) : QThread(parent) {
    /// Cleaning FPS counter
    _fps = 0;

    /// ToolBar Settings
    _startAction = new QAction(QIcon(":/icons/start.png"), "Start", this);
    _startAction->setToolTip("Start Capture Stream");
    _startAction->setShortcut(Qt::Key_Space);
    _startAction->setEnabled(true);
    _stopAction = new QAction(QIcon(":/icons/stop.png"), "Stop", this);
    _stopAction->setToolTip("Stop Capture Stream");
    _stopAction->setShortcut(Qt::Key_Space);
    _stopAction->setEnabled(false);
    _settingsAction = new QAction(QIcon(":/icons/settings.png"), "Settings", this);
    _settingsAction->setToolTip("Open Configuration Dialog");
    _settingsAction->setShortcut(Qt::CTRL+Qt::Key_T);
    _settingsAction->setEnabled(true);
    _bgr = new QAction("BGR Image Format", this);
    _bgr->setCheckable(true);
    _gray = new QAction("Grayscale Image Format", this);
    _gray->setCheckable(true);
    _rgb = new QAction("RGB Image Format", this);
    _rgb->setCheckable(true);

    actionList.append(_startAction);
    actionList.append(_stopAction);
    actionList.append(_settingsAction);
    formatList.append(_bgr);
    formatList.append(_rgb);
    formatList.append(_gray);

    /// ToolBar signals connections
    connect(_startAction, SIGNAL(triggered()), this, SLOT(start()));
    connect(_stopAction, SIGNAL(triggered()), this, SLOT(stop()));

    connect(_bgr, SIGNAL(toggled(bool)), this, SLOT(convert2BGR(bool)));
    connect(_gray, SIGNAL(toggled(bool)), this, SLOT(convert2Gray(bool)));
    connect(_rgb, SIGNAL(toggled(bool)), this, SLOT(convert2RGB(bool)));
}

CameraThread::~CameraThread(){ }

float CameraThread::getFPS() {
    return _fps;
}

Mat CameraThread::getFrame() {
    _rgb->setEnabled(isRunning());
    _bgr->setEnabled(isRunning());
    if(_cvMatbuffer.isEmpty()) {
#ifdef _DEBUG_CAPTURE_THREADS
        qWarning() << "[CAMERA_THREAD] << getFrame() - No Available frame in capture buffer!";
#endif //_DEBUG_CAPTURE_THREADS
        return Mat(getHeight(), getWidth(), CV_8UC3, Scalar(0,0,0));
    }
    Mat frame = _cvMatbuffer.dequeue();
    if (_bgr->isChecked() && frame.channels() > 2)
        cvtColor(frame, frame, CV_BGR2RGB);
    if (_gray->isChecked() && frame.channels() > 2)
        cvtColor(frame, frame, CV_RGB2GRAY);
    return frame;
}

QImage CameraThread::mat2qImage(Mat src) {
    switch (src.type()) {
    case CV_8UC1:
        return QImage(src.data, src.cols, src.rows, QImage::Format_Indexed8);
        break;
    default:
        break;
    }
    return QImage(src.data, src.cols, src.rows, QImage::Format_RGB888);
}

void CameraThread::convert2BGR(bool value) {
    if(value) {
    _bgr->setChecked(value);
    _gray->setChecked(!value);
    _rgb->setChecked(!value);
    }
}

void CameraThread::convert2Gray(bool value) {
    if(value) {
    _bgr->setChecked(!value);
    _gray->setChecked(value);
    _rgb->setChecked(!value);
    }
}

void CameraThread::convert2RGB(bool value) {
    if(value) {
    _bgr->setChecked(!value);
    _gray->setChecked(!value);
    _rgb->setChecked(value);
    }
}
