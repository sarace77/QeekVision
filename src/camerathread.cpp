#include "camerathread.h"

CameraThread::CameraThread(QObject *parent) : QThread(parent) {
    /// Cleaning FPS counter
    _fps = 0;

    /// ToolBar Settings
    _startAction = new QAction(QIcon(":/icons/start.png"), "Start", this);
    _startAction->setToolTip("Start Capture Stream");
    _startAction->setShortcut(Qt::Key_Space);
    _startAction->setEnabled(false);
    _stopAction = new QAction(QIcon(":/icons/stop.png"), "Stop", this);
    _stopAction->setToolTip("Stop Capture Stream");
    _stopAction->setShortcut(Qt::Key_Space);
    _stopAction->setEnabled(false);
    _settingsAction = new QAction(QIcon(":/icons/settings.png"), "Settings", this);
    _settingsAction->setToolTip("Open Configuration Dialog");
    _settingsAction->setShortcut(Qt::CTRL+Qt::Key_T);
    _settingsAction->setEnabled(true);
    _imageFormat = new QLabel("Image Format:");
    _bgr = new QRadioButton("BGR");
    _bgr->setChecked(true);
    _bgr->setEnabled(false);
    _rgb = new QRadioButton("RGB");
    _rgb->setEnabled(false);
    _threadToolBar = new QToolBar("Thread Commands");
    _threadToolBar->setObjectName("threadToolBar");
    _threadToolBar->addAction(_startAction);
    _threadToolBar->addAction(_stopAction);
    _threadToolBar->addAction(_settingsAction);
    _threadToolBar->addSeparator();
    _threadToolBar->addWidget(_imageFormat);
    _threadToolBar->addWidget(_bgr);
    _threadToolBar->addWidget(_rgb);

    /// Menu
    _cameraMenu = new QMenu("&Camera");
    QList<QAction *> _menuActions;
    _menuActions.append(_startAction);
    _menuActions.append(_stopAction);
    _cameraMenu->addActions(_menuActions);
    _cameraMenu->addSeparator();
    _menuActions.clear();
    _menuActions.append(_settingsAction);
    _cameraMenu->addActions(_menuActions);

    /// ToolBar signals connections
    connect(_startAction, SIGNAL(triggered()), this, SLOT(start()));
    connect(_stopAction, SIGNAL(triggered()), this, SLOT(stop()));
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
    if (_rgb->isChecked())
        cvtColor(frame, frame, CV_RGB2BGR);
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

QMenu *CameraThread::menu() {
    return _cameraMenu;
}

QToolBar *CameraThread::toolBar() {
    return _threadToolBar;
}


