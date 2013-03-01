#include "v4lcamera.h"

#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <opencv/highgui.h>

V4LCamera::V4LCamera() {
    /// Configuration Engine Instance and signals connection
    _configEngine = new V4LConfigurationEngine();
    connect(_configEngine, SIGNAL(availableConfiguration()), this, SLOT(configure()));

    /// Cleaning frame format;
    CLEAR(_fmt);

    /// Cleaning Capture Device Name and File Descriptor
    _deviceName = "";
    _fd = -1;


    /// Buffer select() Timeout Init
    _tv.tv_sec = 2;
    _tv.tv_usec = 0;

    /// V4L Buffer Type init
    _type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    _frameSize = new QComboBox();
    _frameSize->setMinimumWidth(100);
    _frameSize->setEnabled(false);
    _frameSizeLabel = new QLabel("Frame Size: ");
    _threadToolBar->addSeparator();
    _threadToolBar->addWidget(_frameSizeLabel);
    _threadToolBar->addWidget(_frameSize);

    connect(_settingsAction, SIGNAL(triggered()), _configEngine, SLOT(configRequest()));
}

V4LCamera::~V4LCamera() {
    if (_configEngine != NULL)
        delete _configEngine;
}

void V4LCamera::configure() {
    CaptureDevice configuration = _configEngine->getConfiguration();
    _deviceName = configuration.deviceName;
    _fmt = configuration.configuration;
    if (!isConfigurated()){
        _configEngine->configRequest();
    }
    else {
        _startAction->setEnabled(true);
        _frameSize->setEnabled(true);
        QStringList fSizeItems = _configEngine->getSupportedFrameSizes();
        QString selectedSize = QString("%1x%2").arg(_configEngine->getConfiguration().configuration.fmt.pix.width).arg(_configEngine->getConfiguration().configuration.fmt.pix.height);
        _frameSize->addItems(fSizeItems);
        _frameSize->setCurrentIndex(fSizeItems.indexOf(selectedSize));
        connect(_frameSize, SIGNAL(currentIndexChanged(QString)), _configEngine, SLOT(frameSizeChangeRequest(QString)));
        emit configurated();
    }
}

int V4LCamera::exec() {
    char header [50];
#ifdef _DEBUG_CAPTURE_THREADS
    qDebug() << "[CAMERA_THREAD::V4L_CAMERA] - exec() - Device Name" << _deviceName;
    qDebug() << "[CAMERA_THREAD::V4L_CAMERA] - exec() - frame size" << _fmt.fmt.pix.width << "x" << _fmt.fmt.pix.height;
#endif //_DEBUG_CAPTURE_THREADS

    sprintf(header,"P6\n%d %d 255\n",_fmt.fmt.pix.width,_fmt.fmt.pix.height);
    unsigned char *dst_buf;
#ifdef _DEBUG_CAPTURE_THREADS
    qDebug() << "[CAMERA_THREAD::V4L_CAMERA] - exec() - Started";
#endif //_DEBUG_CAPTURE_THREADS
    while(true){
        myTimer.start();
        do {
            FD_ZERO(&_fds);
            FD_SET(_fd, &_fds);
            _r = select(_fd + 1, &_fds, NULL, NULL, &_tv);
        } while ((_r == -1 && (errno = EINTR)));
        if (_r == -1)
            qFatal("[CAMERA_THREAD::V4L_CAMERA] - exec() - select() returns error!");
        CLEAR(_buf);
        _buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _buf.memory = V4L2_MEMORY_MMAP;
        V4LSettings::qioctl(_fd, VIDIOC_DQBUF, &_buf, "V4LCamera::exec()");

        dst_buf = (unsigned char*)malloc(_fmt.fmt.pix.sizeimage);
        if (v4lconvert_convert(_v4lconvert_data, &_src_fmt, &_fmt,
                                (unsigned char*)_buffers[_buf.index].start, _buf.bytesused,
                                dst_buf, _fmt.fmt.pix.sizeimage) < 0) {
#ifdef _DEBUG_CAPTURE_THREADS
            if (errno != EAGAIN)
                qWarning("[CAMERA_THREAD::V4L_CAMERA] - exec() - v4l_convert() returns error");
#endif //_DEBUG_CAPTURE_THREADS
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
        V4LSettings::qioctl(_fd, VIDIOC_QBUF, &_buf, "V4LCamera::exec()");
        _fps = 1000.0/myTimer.elapsed();
    }
    return 0;
}

int V4LCamera::getHeight() {
    if (_fmt.fmt.pix.height > 0)
        return _fmt.fmt.pix.height;
    return _STD_FRM_HGHT;
}


int V4LCamera::getWidth() {
    if (_fmt.fmt.pix.width > 0)
        return _fmt.fmt.pix.width;
    return _STD_FRM_WDTH;
}

bool V4LCamera::isConfigurated() {
    return ((_fmt.fmt.pix.height*_fmt.fmt.pix.width) && !_deviceName.isEmpty());
}

void V4LCamera::openCaptureDevice() {
    /// Open Capture Device
    _fd = open(_deviceName.toAscii().constData(), O_RDWR | O_NONBLOCK, 0);
    /// Check if is it open
    if (_fd < 0 )
        qFatal("[CAMERA_THREAD::V4L_CAMERA] - openCaptureDevice() - Unable to open device!");
    /// Get Capture Device Frame Format configuration
    V4LSettings::qioctl(_fd, VIDIOC_G_FMT, &_fmt, "V4LCamera::openCaptureDevice()");

    /// Set V4L frame buffer data conversion
    _v4lconvert_data = v4lconvert_create(_fd);
#ifdef _DEBUG_CAPTURE_THREADS
    if (_v4lconvert_data == NULL)
        qWarning("[CAMERA_THREAD::V4L_CAMERA] - openCaptureDevice() - v4lconvert_create() returns error");
    if (v4lconvert_try_format(_v4lconvert_data, &_fmt, &_src_fmt) != 0)
        qWarning("[CAMERA_THREAD::V4L_CAMERA] - openCaptureDevice() - v4lconvert_try_format() returns error");
#endif //_DEBUG_CAPTURE_THREADS
}

void V4LCamera::run() {
    /// Enabling/Disabling Toolbar Buttons
    _frameSize->setEnabled(false);
    _startAction->setEnabled(false);
    _stopAction->setEnabled(true);
    _settingsAction->setEnabled(false);

    if (!isConfigurated())
        configure();

    /// Opening Capture Device
    openCaptureDevice();

    /// Memory Mapping Init
    /// V4L Buffer Request Struct init
    CLEAR(_req);
    _req.count = _REQ_BUFF_COUNT;
    _req.type = _fmt.type;
    _req.memory = V4L2_MEMORY_MMAP;
    /// Setting Buffer Type
    V4LSettings::qioctl(_fd, VIDIOC_REQBUFS, &_req, "V4LCamera::run()");
    /// V4L Buffer Allocation
    _buffers = (Buffer *)calloc(_req.count, sizeof(*_buffers));
    for (_n_buffers = 0; _n_buffers < _req.count; ++_n_buffers) {
        CLEAR(_buf);
        _buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _buf.memory      = V4L2_MEMORY_MMAP;
        _buf.index       = _n_buffers;
        V4LSettings::qioctl(_fd, VIDIOC_QUERYBUF, &_buf, "V4LCamera::run()");
        _buffers[_n_buffers].length = _buf.length;
        _buffers[_n_buffers].start = mmap(NULL, _buf.length,
                                            PROT_READ | PROT_WRITE, MAP_SHARED,
                                            _fd, _buf.m.offset);
        if (MAP_FAILED == _buffers[_n_buffers].start) {
            qFatal("[CAMERA_THREAD::V4L_CAMERA] - run() - mmap() returns error!");
        }
    }

    /// Video Buffer Enable
    for (quint8 i = 0; i < _n_buffers; ++i) {
        CLEAR(_buf);
        _buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _buf.memory = V4L2_MEMORY_MMAP;
        _buf.index = i;
        V4LSettings::qioctl(_fd, VIDIOC_QBUF, &_buf, "V4LCamera::run()");
    }

    /// Enabling Video Capture
    V4LSettings::qioctl(_fd, VIDIOC_STREAMON, &_type, "V4LCamera::run()");

    /// Launching main loop
    exec();
}

void V4LCamera::stop() {
#ifdef _DEBUG_CAPTURE_THREADS
    qDebug() << "[CAMERA_THREAD::V4L_CAMERA] - stop() - Stopping";
#endif //_DEBUG_CAPTURE_THREADS
    _type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    V4LSettings::qioctl(_fd, VIDIOC_STREAMOFF, &_type, "V4LCamera::stop()");
    for (unsigned int i = 0; i < _n_buffers; ++i)
           munmap(_buffers[i].start, _buffers[i].length);
    close(_fd);
    terminate();
    _frameSize->setEnabled(true);
    _startAction->setEnabled(true);
    _stopAction->setEnabled(false);
    _settingsAction->setEnabled(true);
}
