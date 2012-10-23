#include "camerathread.h"

#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <QDebug>

#include <opencv/highgui.h>

CameraThread::CameraThread(QObject *parent) : QThread(parent) {
    /// Configuration Engine Instance and signals connection
    _configEngine = new ConfigurationEngine();
    connect(_configEngine, SIGNAL(availableConfiguration()), this, SLOT(configure()));

    /// Cleaning frame format;
    CLEAR(_fmt);

    /// Cleaning Capture Device Name and File Descriptor
    _deviceName = "";
    _fd = -1;

    /// Cleaning FPS counter
    _fps = 0;

    /// Buffer select() Timeout Init
    _tv.tv_sec = 2;
    _tv.tv_usec = 0;

    /// V4L Buffer Type init
    _type = V4L2_BUF_TYPE_VIDEO_CAPTURE;


    /// ToolBar Settings
    _startAction = new QAction(QIcon(":/icons/start.png"), "Start", this);
    _startAction->setToolTip("Start Capture Stream");
    _startAction->setShortcut(Qt::Key_Space);
    _startAction->setEnabled(false);
    _stopAction = new QAction(QIcon(":/icons/stop.png"), "Stop", this);
    _stopAction->setToolTip("Stop Capture Stream");
    _stopAction->setShortcut(Qt::Key_Space);
    _stopAction->setEnabled(false);
    _settingsAction = new QAction(QIcon(":/icons/settings.png"), "Stop", this);
    _settingsAction->setToolTip("Open Configuration Dialog");
    _settingsAction->setShortcut(Qt::CTRL+Qt::Key_T);
    _settingsAction->setEnabled(true);
    _threadToolBar = new QToolBar("Thread Commands");
    _threadToolBar->setObjectName("threadToolBar");
    _threadToolBar->addAction(_startAction);
    _threadToolBar->addAction(_stopAction);
    _threadToolBar->addAction(_settingsAction);

    /// ToolBar signals connections
    connect(_startAction, SIGNAL(triggered()), this, SLOT(start()));
    connect(_stopAction, SIGNAL(triggered()), this, SLOT(stop()));
    connect(_settingsAction, SIGNAL(triggered()), _configEngine, SLOT(configRequest()));
}

CameraThread::~CameraThread(){
    if (_configEngine != NULL)
        delete _configEngine;
}

float CameraThread::getFPS() {
    return _fps;
}

Mat CameraThread::getFrame() {
    if(_cvMatbuffer.isEmpty()) {
        qWarning() << "[CAMERA_THREAD] << getFrame() - No Available frame in capture buffer!";
        return Mat(getHeight(), getWidth(), CV_8UC3, Scalar(0,0,0));
    }
    return _cvMatbuffer.dequeue().clone();
}

int CameraThread::getHeight() {
    if (_fmt.fmt.pix.height > 0)
        return _fmt.fmt.pix.height;
    return _STD_FRM_HGHT;
}


int CameraThread::getWidth() {
    if (_fmt.fmt.pix.width > 0)
        return _fmt.fmt.pix.width;
    return _STD_FRM_WDTH;
}

bool CameraThread::isConfigurated() {
    return ((_fmt.fmt.pix.height*_fmt.fmt.pix.width) && !_deviceName.isEmpty());
}

QToolBar *CameraThread::toolBar() {
    return _threadToolBar;
}

void CameraThread::stop() {
    qDebug() << "[CAMERA_THREAD] - stop() - Stopping";
    _type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    Settings::qioctl(_fd, VIDIOC_STREAMOFF, &_type, "CameraThread::stop()");
    for (unsigned int i = 0; i < _n_buffers; ++i)
           munmap(_buffers[i].start, _buffers[i].length);
    close(_fd);
    terminate();
    _startAction->setEnabled(true);
    _stopAction->setEnabled(false);
    _settingsAction->setEnabled(true);
}

void CameraThread::configure() {
    CaptureDevice configuration = _configEngine->getConfiguration();
    _deviceName = configuration.deviceName;
    _fmt = configuration.configuration;
    if (!isConfigurated()){
        _configEngine->configRequest();
    }
    else {
        _startAction->setEnabled(true);
        emit configurated();
    }
}


void CameraThread::openCaptureDevice() {
    /// Open Capture Device
    _fd = open(_deviceName.toAscii().constData(), O_RDWR | O_NONBLOCK, 0);
    /// Check if is it open
    if (_fd < 0 )
        qFatal("[CAMERA_THREAD] - openCaptureDevice() - Unable to open device!");
    /// Get Caèture Device Frame Format configuration
    Settings::qioctl(_fd, VIDIOC_G_FMT, &_fmt, "CameraThread::openCaptureDevice()");

    /// Set V4L frame buffer data conversion
    _v4lconvert_data = v4lconvert_create(_fd);
    if (_v4lconvert_data == NULL)
        qWarning("[CAMERA_THREAD] - openCaptureDevice() - v4lconvert_create() returns error");
    if (v4lconvert_try_format(_v4lconvert_data, &_fmt, &_src_fmt) != 0)
        qWarning("[CAMERA_THREAD] - openCaptureDevice() - v4lconvert_try_format() returns error");
}

int CameraThread::exec() {
    char header [50];
    sprintf(header,"P6\n%d %d 255\n",_fmt.fmt.pix.width,_fmt.fmt.pix.height);
    unsigned char *dst_buf;
    qDebug() << "[CAMERA_THREAD] - exec() - Started";
    while(true){
        myTimer.start();
        do {
            FD_ZERO(&_fds);
            FD_SET(_fd, &_fds);
            _r = select(_fd + 1, &_fds, NULL, NULL, &_tv);
        } while ((_r == -1 && (errno = EINTR)));
        if (_r == -1) {
            qFatal("[CAMERA_THREAD] - exec() - select() returns error!");
        }
        CLEAR(_buf);
        _buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _buf.memory = V4L2_MEMORY_MMAP;
        Settings::qioctl(_fd, VIDIOC_DQBUF, &_buf, "CameraThread::exec()");

        dst_buf = (unsigned char*)malloc(_fmt.fmt.pix.sizeimage);
        if (v4lconvert_convert(_v4lconvert_data, &_src_fmt, &_fmt,
                                (unsigned char*)_buffers[_buf.index].start, _buf.bytesused,
                                dst_buf, _fmt.fmt.pix.sizeimage) < 0) {
                if (errno != EAGAIN)
                        qWarning("[CAMERA_THREAD] - exec() - v4l_convert() returns error");
        }
        unsigned char* asil=(unsigned char*)malloc(_fmt.fmt.pix.sizeimage+qstrlen(header));
        memmove(asil, dst_buf, _fmt.fmt.pix.sizeimage);
        memmove(asil+qstrlen(header), asil, _fmt.fmt.pix.sizeimage);
        memcpy(asil,header,qstrlen(header));

        Mat *qq = new Mat(_fmt.fmt.pix.height, _fmt.fmt.pix.width, CV_8UC2, asil);
        if(!qq->empty()) {
            Mat newFrame = qq->clone();
            switch(_fmt.fmt.pix.height) {
            case 720:
            case 800:
            case 960:
                cvtColor(newFrame, newFrame, CV_YUV2RGB_YUYV);
            break;
        case 1024:
            cvtColor(newFrame, newFrame, CV_YUV2BGR_Y422);
            break;
        default:
            cvtColor(newFrame, newFrame, CV_YUV2RGB_Y422);
            break;
        }
            _cvMatbuffer.enqueue(newFrame);
            emit availableFrame();
        }
        free(asil);
        free(dst_buf);
        delete qq;
        Settings::qioctl(_fd, VIDIOC_QBUF, &_buf, "CameraThread::exec()");
        _fps = 1000.0/myTimer.elapsed();
    }
    return 0;
}

void CameraThread::run() {
    /// Enabling/Disabling Toolbar Buttons
    _startAction->setEnabled(false);
    _stopAction->setEnabled(true);
    _settingsAction->setEnabled(false);

    /// Opening Capture Device
    openCaptureDevice();


    /// Memory Mapping Init
    /// V4L Buffer Request Struct init
    CLEAR(_req);
    _req.count = _REQ_BUFF_COUNT;
    _req.type = _fmt.type;
    _req.memory = V4L2_MEMORY_MMAP;
    /// Setting Buffer Type
    Settings::qioctl(_fd, VIDIOC_REQBUFS, &_req, "CameraThread::run()");
    /// V4L Buffer Allocation
    _buffers = (Buffer *)calloc(_req.count, sizeof(*_buffers));
    for (_n_buffers = 0; _n_buffers < _req.count; ++_n_buffers) {
        CLEAR(_buf);
        _buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _buf.memory      = V4L2_MEMORY_MMAP;
        _buf.index       = _n_buffers;
        Settings::qioctl(_fd, VIDIOC_QUERYBUF, &_buf, "CameraThread::run()");
        _buffers[_n_buffers].length = _buf.length;
        _buffers[_n_buffers].start = mmap(NULL, _buf.length,
                                            PROT_READ | PROT_WRITE, MAP_SHARED,
                                            _fd, _buf.m.offset);
        if (MAP_FAILED == _buffers[_n_buffers].start) {
            qFatal("[CAMERA_THREAD] - run() - mmap() returns error!");
        }
    }

    /// Video Buffer Enable
    for (quint8 i = 0; i < _n_buffers; ++i) {
        CLEAR(_buf);
        _buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _buf.memory = V4L2_MEMORY_MMAP;
        _buf.index = i;
        Settings::qioctl(_fd, VIDIOC_QBUF, &_buf, "CameraThread::run()");
    }

    /// Enabling Video Capture
    Settings::qioctl(_fd, VIDIOC_STREAMON, &_type, "CameraThread::run()");

    /// Launching main loop
    exec();
}

QImage CameraThread::mat2qImage(Mat src) {
    return QImage(src.data, src.cols, src.rows, QImage::Format_RGB888);
}
