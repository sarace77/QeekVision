#include "v4lsettings.h"
#include "ui_v4lsettings.h"

#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>

V4LSettings::V4LSettings(QWidget *parent) : QMainWindow(parent), ui(new Ui::V4LSettings) {
    ui->setupUi(this);
    CLEAR(_format);
    _devicesList = QDir(_DEV_DIR, _DEV_FILTER, QDir::Name, QDir::AllEntries|QDir::System).entryList();
    for (quint8 i = 0; i < _devicesList.count(); i++) {
        /* Open Device */
        int fd = open(_devicesList.at(i));

        /* Query Device for Names */
        CLEAR(_querycap);
        if (qioctl(fd, VIDIOC_QUERYCAP, &_querycap, "V4LSettings::V4LSettings()") == 0) {
            _devicesNames << QString((char *) _querycap.card);
        } else {
            _devicesNames << _devicesList.at(i);
        }
        ::close(fd);
    }
    QStringListModel * model = new QStringListModel(getCapabilitiesStringList(_querycap));
    ui->capabilities->setModel(model);
    ui->device->addItems(_devicesNames);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(signalAccepted()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(signalRejected()));
}

V4LSettings::~V4LSettings()
{
    delete ui;
}

struct v4l2_format V4LSettings::getV4L2Config() {
    return _format;
}

QString V4LSettings::getV4L2DeviceName() {
    if (_devicesList.count() && (ui->device->currentIndex() >= 0))
        return QString(_DEV_DIR) + QString("/") + _devicesList.at(ui->device->currentIndex());
    return QString("");
}

void V4LSettings::on_captureMode_currentIndexChanged(int value) {
    if (value > -1)
        tryVideoMode();
}

void V4LSettings::on_device_currentIndexChanged(int value) {
    /* Open Device */
    int fd = open(_devicesList.at(value));
    /* Query for Capablities */
    QString sInfo = QString("Device: /dev/") + _devicesList.at(value);
    CLEAR(_querycap);
    if (qioctl(fd, VIDIOC_QUERYCAP, &_querycap, "V4LSettings::on_device_currentIndexChanged()") == 0) {
        sInfo += " (" + QString((char *) _querycap.driver);
        QString sVersion = QString("%1").arg(_querycap.version);
        sInfo += " v" + sVersion.mid(0,2) + "." + sVersion.mid(2,2) + "." + sVersion.mid(4,2);
        sInfo += "), BusID: " + QString((char *) _querycap.bus_info);
    }
    /* Query for Supported Video Format */
    struct v4l2_fmtdesc supportedFmt;
    CLEAR(supportedFmt);
    supportedFmt.type = V4L2_CAP_VIDEO_CAPTURE;
    if (ui->pixelformat->count() > 0)
        ui->pixelformat->clear();
    while (qioctl(fd, VIDIOC_ENUM_FMT, &supportedFmt, "V4LSettings::on_device_currentIndexChanged()") >= 0) {
        ui->pixelformat->addItem(QString((char *) supportedFmt.description));
        _supported4CC << supportedFmt.pixelformat;
        supportedFmt.index++;
    }
    /* Close Device */
    ::close(fd);

    tryCaptureMode();
    on_pixelformat_currentIndexChanged(0);
    ui->statusbar->showMessage(sInfo, 30000);
}

void V4LSettings::on_pixelformat_currentIndexChanged(int value) {
    if (ui->pixelformat->count() > 0) {
        ui->frameSize->clear();
        supportedFrameRates.clear();
        /* Open Device */
        int fd = open(_devicesList.at(value));
        struct v4l2_frmsizeenum frmsize;
        CLEAR(frmsize);
        frmsize.pixel_format = _supported4CC.at(ui->pixelformat->currentIndex());
        while(qioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize, "V4LSettings::on_pixelformat_currentIndexChanged()") >= 0) {
            if(ui->frameSize->count() > 0) {
                QStringList tmp = ui->frameSize->itemText(ui->frameSize->count() - 1).split(QChar('x'));
                if (tmp.at(0).toDouble() < frmsize.discrete.width) {
                    ui->frameSize->addItem(QString("%1x%2").arg(frmsize.discrete.width).arg(frmsize.discrete.height));
                } else {
                    if (tmp.at(1).toDouble() < frmsize.discrete.height) {
                        ui->frameSize->addItem(QString("%1x%2").arg(frmsize.discrete.width).arg(frmsize.discrete.height));
                    } else {
                        ui->frameSize->insertItem(ui->frameSize->count() - 1,
                                              QString("%1x%2").arg(frmsize.discrete.width).arg(frmsize.discrete.height));
                    }
                }
            } else {
                ui->frameSize->addItem(QString("%1x%2").arg(frmsize.discrete.width).arg(frmsize.discrete.height));
            }
            frmsize.index++;
            supportedFrameRates << QString("%1x%2").arg(frmsize.discrete.width).arg(frmsize.discrete.height);
        }
        /* Close Device */
        ::close(fd);
    }
}

int V4LSettings::open(QString sFileName) {
    QString filename = QString(_DEV_DIR) + QString("/") + sFileName;
    int __fd = ::open(filename.toAscii().constData(), O_RDWR | O_NONBLOCK, 0);
    if (__fd < 0) {
        qFatal("[V4L_SETTINGS] - settings() - Error opening device");
    }
    return __fd;
}

void V4LSettings::signalAccepted() {
    CLEAR(_format);
    if (_captureModes.count() > 0) {
        struct v4l2_format fmt;
        /* Open Device */
        int fd = open(_devicesList.at(ui->device->currentIndex()));
        _format.type = _captureModes.at(ui->captureMode->currentIndex());
        if (qioctl(fd, VIDIOC_G_FMT, &_format, "V4LSettings::signalAccepted()") == 0) {
            if (_colorSpace.count() && (ui->colorSpace->currentIndex() > -1) )
                _format.fmt.pix.colorspace= _colorSpace.at(ui->colorSpace->currentIndex());
            fmt = _format;
            if (qioctl(fd, VIDIOC_S_FMT, &fmt, "V4LSettings::signalAccepted()") != 0) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
                qWarning("[V4L_SETTINGS] - signalAccepted() - Unable to set selected colorspace");
#endif //_DEBUG_CONFIGURATION_OBJECTS
            }
            if (_field.count() && (ui->videoMode->currentIndex() > -1) )
                _format.fmt.pix.field = _field.at(ui->videoMode->currentIndex());
            fmt = _format;
            if (qioctl(fd, VIDIOC_S_FMT, &fmt, "V4LSettings::signalAccepted()") != 0) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
                qWarning("[V4L_SETTINGS] - signalAccepted() - Unable to set selected Video Mode");
#endif //_DEBUG_CONFIGURATION_OBJECTS
            }
            if (ui->frameSize->count()) {
                QStringList res = ui->frameSize->currentText().split(QChar('x'));
                _format.fmt.pix.height = res.at(1).toUInt();
                _format.fmt.pix.width = res.at(0).toUInt();
            }
            fmt = _format;
            if (qioctl(fd, VIDIOC_S_FMT, &fmt, "V4LSettings::signalAccepted()") != 0){
#ifdef _DEBUG_CONFIGURATION_OBJECTS
                qWarning("[V4L_SETTINGS] - signalAccepted() - Unable to set selected framesize");
#endif //_DEBUG_CONFIGURATION_OBJECTS
            }
            if(qioctl(fd, VIDIOC_G_FMT, &_format, "V4LSettings::signalAccepted()") != 0) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
                qWarning("[V4L_SETTINGS] - signalAccepted() - Error setting given config");
#endif //_DEBUG_CONFIGURATION_OBJECTS
            } else {
                emit accepted();
            }
        } else {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
            qWarning("[V4L_SETTINGS] - signalAccepted() - Unable to get configuration for the selected device");
#endif //_DEBUG_CONFIGURATION_OBJECTS
            emit rejected();
        }
        /* Close Device */
        ::close(fd);
    } else {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
        qWarning("[V4L_SETTINGS] - signalAccepted() - No valid capture mode found for the selected device");
#endif //_DEBUG_CONFIGURATION_OBJECTS
        emit rejected();
    }
}

void V4LSettings::signalRejected() {
    CLEAR(_format);
    emit rejected();
}

void V4LSettings::tryCaptureMode() {
    if (_devicesList .count()> 0) {
        /* Open Device */
        int fd = open(_devicesList.at(ui->device->currentIndex()));
        ui->captureMode->clear();
        _captureModes.clear();
        struct v4l2_format fmt;
        CLEAR(fmt);
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (qioctl(fd, VIDIOC_G_FMT, &fmt, "V4LSettings::tryCaptureMode()") != -1) {
            ui->captureMode->addItem("Single Plane Video Capture");
            _captureModes << V4L2_BUF_TYPE_VIDEO_CAPTURE;
        }
        CLEAR(fmt);
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, "V4LSettings::tryCaptureMode()") != -1) {
            ui->captureMode->addItem("Multi Plane Video Capture");
            _captureModes << V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        }
        /* Close Device */
        ::close(fd);
        tryVideoMode();
        tryColorSpace();
    }
}

void V4LSettings::tryColorSpace() {
    if (ui->captureMode->count()){
        /* Open Device */
        int fd = open(_devicesList.at(ui->device->currentIndex()));
        ui->colorSpace->clear();
        _colorSpace.clear();
        struct v4l2_format fmt;
        CLEAR(fmt);
        fmt.type = _captureModes.at(ui->captureMode->currentIndex());
        qioctl(fd, VIDIOC_G_FMT, &fmt, " V4LSettings::tryColorSpace()");
        fmt.fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE170M;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, " V4LSettings::tryColorSpace()") == 0) {
            ui->colorSpace->addItem("SMPTE170M");
            _colorSpace << V4L2_COLORSPACE_SMPTE170M;
        }
        fmt.fmt.pix.colorspace = V4L2_COLORSPACE_SMPTE240M;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, " V4LSettings::tryColorSpace()") == 0) {
            ui->colorSpace->addItem("SMPTE240M");
            _colorSpace << V4L2_COLORSPACE_SMPTE240M;
        }
        fmt.fmt.pix.colorspace = V4L2_COLORSPACE_REC709;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, " V4LSettings::tryColorSpace()") == 0) {
            ui->colorSpace->addItem("REC709");
            _colorSpace << V4L2_COLORSPACE_REC709;
        }
        fmt.fmt.pix.colorspace = V4L2_COLORSPACE_BT878;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, " V4LSettings::tryColorSpace()") == 0) {
            ui->colorSpace->addItem("BT878");
            _colorSpace << V4L2_COLORSPACE_BT878;
        }
        fmt.fmt.pix.colorspace = V4L2_COLORSPACE_470_SYSTEM_M;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, " V4LSettings::tryColorSpace()") == 0) {
            ui->colorSpace->addItem("470 System M");
            _colorSpace << V4L2_COLORSPACE_470_SYSTEM_M;
        }
        fmt.fmt.pix.colorspace = V4L2_COLORSPACE_470_SYSTEM_BG;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, " V4LSettings::tryColorSpace()") == 0) {
            ui->colorSpace->addItem("470 System BG");
            _colorSpace << V4L2_COLORSPACE_470_SYSTEM_BG;
        }
        fmt.fmt.pix.colorspace = V4L2_COLORSPACE_JPEG;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, " V4LSettings::tryColorSpace()") == 0) {
            ui->colorSpace->addItem("JPEG");
            _colorSpace << V4L2_COLORSPACE_JPEG;
        }
        fmt.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, " V4LSettings::tryColorSpace()") == 0) {
            ui->colorSpace->addItem("sRGB");
            _colorSpace << V4L2_COLORSPACE_SRGB;
        }
        qioctl(fd, VIDIOC_G_FMT, &fmt, " V4LSettings::tryColorSpace()");
        ui->colorSpace->setCurrentIndex(_colorSpace.indexOf((v4l2_colorspace) fmt.fmt.pix.colorspace));
        /* Close Device */
        ::close(fd);
    }

}

void V4LSettings::tryVideoMode() {
    if (_captureModes.count()){
        /* Open Device */
        int fd = open(_devicesList.at(ui->device->currentIndex()));
        ui->videoMode->clear();
        _field.clear();
        struct v4l2_format fmt;
        CLEAR(fmt);
        fmt.type = _captureModes.at(ui->captureMode->currentIndex());
        qioctl(fd, VIDIOC_G_FMT, &fmt, "V4LSettings::tryVideoMode()");
        fmt.fmt.pix.field = V4L2_FIELD_ANY;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, "V4LSettings::tryVideoMode()") == 0) {
            ui->videoMode->addItem("Any");
            _field << V4L2_FIELD_ANY;
        }
        fmt.fmt.pix.field = V4L2_FIELD_NONE;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, "V4LSettings::tryVideoMode()") == 0) {
            ui->videoMode->addItem("None");
            _field << V4L2_FIELD_NONE;
        }
        fmt.fmt.pix.field = V4L2_FIELD_TOP;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, "V4LSettings::tryVideoMode()") == 0) {
            ui->videoMode->addItem("Top");
            _field << V4L2_FIELD_TOP;
        }
        fmt.fmt.pix.field = V4L2_FIELD_BOTTOM;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, "V4LSettings::tryVideoMode()") == 0) {
            ui->videoMode->addItem("Bottom");
            _field << V4L2_FIELD_BOTTOM;
        }
        fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, "V4LSettings::tryVideoMode()") == 0) {
            ui->videoMode->addItem("Interlaced");
            _field << V4L2_FIELD_INTERLACED;
        }
        fmt.fmt.pix.field = V4L2_FIELD_SEQ_TB;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, "V4LSettings::tryVideoMode()") == 0) {
            ui->videoMode->addItem("Seq TB");
            _field << V4L2_FIELD_SEQ_TB;
        }
        fmt.fmt.pix.field = V4L2_FIELD_SEQ_BT;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, "V4LSettings::tryVideoMode()") == 0) {
            ui->videoMode->addItem("Seq BT");
            _field << V4L2_FIELD_SEQ_BT;
        }
        fmt.fmt.pix.field = V4L2_FIELD_ALTERNATE;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, "V4LSettings::tryVideoMode()") == 0) {
            ui->videoMode->addItem("Alternate");
            _field << V4L2_FIELD_ALTERNATE;
        }
        fmt.fmt.pix.field = V4L2_FIELD_INTERLACED_TB;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, "V4LSettings::tryVideoMode()") == 0) {
            ui->videoMode->addItem("Interlaced TB");
            _field << V4L2_FIELD_INTERLACED_TB;
        }
        fmt.fmt.pix.field = V4L2_FIELD_INTERLACED_BT;
        if (qioctl(fd, VIDIOC_TRY_FMT, &fmt, "V4LSettings::tryVideoMode()") == 0) {
            ui->videoMode->addItem("Interlaced BT");
            _field << V4L2_FIELD_INTERLACED_BT;
        }
        qioctl(fd, VIDIOC_G_FMT, &fmt, "V4LSettings::tryVideoMode()");
        ui->videoMode->setCurrentIndex(_field.indexOf((v4l2_field) fmt.fmt.pix.field));
        /* Close Device */
        ::close(fd);
    }
}

QString V4LSettings::decode4CC(__u32 code) {
    return QString((char *)&code).mid(0,4);
}

quint32 V4LSettings::encode4CC(QString code) {
    __u32 res = 0;
    memcpy(&res, code.toAscii().data(), sizeof(__u32));
    return res;
}

QString V4LSettings::decodeKernelVersion(__u32 code) {
    QString result;
    result += QString("%1").arg((quint8) (code >> 16));
    result += QString(".%1").arg((quint8) (code >> 8));
    result += QString(".%1").arg((quint8) (code));
    return result;
}

QStringList V4LSettings::getCapabilitiesStringList(struct v4l2_capability cap) {
    QStringList result;
    QString sVersion = decodeKernelVersion(cap.version);
    result << "Device Name: " + QString((char *) cap.card);
    result << "Driver Name: " + QString((char *) cap.driver) + " (kernel " + sVersion + sVersion.mid(4,2) + ")";
    result << "Bus ID: " + QString((char *) cap.bus_info);
    result << "---";
    result << "Supported Capabilities";
    if (cap.device_caps | V4L2_CAP_VIDEO_CAPTURE)
        result << "V4L2_CAP_VIDEO_CAPTURE";
    if (cap.device_caps | V4L2_CAP_VIDEO_CAPTURE_MPLANE)
        result << "V4L2_CAP_VIDEO_CAPTURE_MPLANE";
    if (cap.device_caps | V4L2_CAP_VIDEO_OVERLAY)
        result << "V4L2_CAP_VIDEO_OVERLAY";
    if (cap.device_caps | V4L2_CAP_VIDEO_OUTPUT)
        result << "V4L2_CAP_VIDEO_OUTPUT";
    if (cap.device_caps | V4L2_CAP_VIDEO_OUTPUT_MPLANE)
        result << "V4L2_CAP_VIDEO_OUTPUT_MPLANE";
    if (cap.device_caps | V4L2_CAP_VIDEO_OUTPUT_OVERLAY)
        result << "V4L2_CAP_VIDEO_OUTPUT_OVERLAY";
    if (cap.device_caps | V4L2_CAP_VIDEO_M2M)
        result << "V4L2_CAP_VIDEO_M2M";
    if (cap.device_caps | V4L2_CAP_VIDEO_M2M_MPLANE)
        result << "V4L2_CAP_VIDEO_M2M_MPLANE";
    if (cap.device_caps | V4L2_CAP_SLICED_VBI_CAPTURE)
        result << "V4L2_CAP_SLICED_VBI_CAPTURE";
    if (cap.device_caps | V4L2_CAP_SLICED_VBI_OUTPUT)
        result << "V4L2_CAP_SLICED_VBI_OUTPUT";
    if (cap.device_caps | V4L2_CAP_VBI_CAPTURE)
        result << "V4L2_CAP_VBI_CAPTURE";
    if (cap.device_caps | V4L2_CAP_VBI_OUTPUT)
        result << "V4L2_CAP_VBI_OUTPUT";
    if (cap.device_caps | V4L2_CAP_ASYNCIO)
        result << "V4L2_CAP_ASYNCIO";
    if (cap.device_caps | V4L2_CAP_READWRITE)
        result << "V4L2_CAP_READWRITE";
    if (cap.device_caps | V4L2_CAP_STREAMING)
        result << "V4L2_CAP_STREAMING";
    if (cap.device_caps | V4L2_CAP_DEVICE_CAPS)
        result << "V4L2_CAP_DEVICE_CAPS";
    if (cap.device_caps | V4L2_CAP_TIMEPERFRAME)
        result << "V4L2_CAP_TIMEPERFRAME";
    if (cap.device_caps | V4L2_CAP_AUDIO)
        result << "V4L2_CAP_AUDIO";
    if (cap.device_caps | V4L2_CAP_MODULATOR)
        result << "V4L2_CAP_MODULATOR";
    if (cap.device_caps | V4L2_CAP_RADIO)
        result << "V4L2_CAP_RADIO";
    if (cap.device_caps | V4L2_CAP_TUNER)
        result << "V4L2_CAP_TUNER";
    if (cap.device_caps | V4L2_CAP_HW_FREQ_SEEK)
        result << "V4L2_CAP_HW_FREQ_SEEK";
    if (cap.device_caps | V4L2_CAP_RDS_CAPTURE)
        result << "V4L2_CAP_RDS_CAPTURE";
    if (cap.device_caps | V4L2_CAP_RDS_OUTPUT)
        result << "V4L2_CAP_RDS_OUTPUT";
    return result;
}

QStringList V4LSettings::getFormatStringList(struct v4l2_format fmt) {
    QStringList result;
    result << "Frame Size: " + QString("%1x%2").arg(fmt.fmt.pix.width).arg(fmt.fmt.pix.height);
    result << "Format: " + decode4CC(fmt.fmt.pix.pixelformat);
    switch(fmt.fmt.pix.colorspace) {
    case V4L2_COLORSPACE_SMPTE170M:
        result << "ColorSpace: " + QString("V4L2_COLORSPACE_SMPTE170M");
        break;
    case V4L2_COLORSPACE_SMPTE240M:
        result << "ColorSpace: " + QString("V4L2_COLORSPACE_SMPTE240M");
        break;
    case V4L2_COLORSPACE_REC709:
        result << "ColorSpace: " + QString("V4L2_COLORSPACE_REC709");
        break;
    case V4L2_COLORSPACE_BT878:
        result << "ColorSpace: " + QString("V4L2_COLORSPACE_BT878");
        break;
    case V4L2_COLORSPACE_470_SYSTEM_M:
        result << "ColorSpace: " + QString("V4L2_COLORSPACE_470_SYSTEM_M");
        break;
    case V4L2_COLORSPACE_470_SYSTEM_BG:
        result << "ColorSpace: " + QString("V4L2_COLORSPACE_470_SYSTEM_BG");
        break;
    case V4L2_COLORSPACE_JPEG:
        result << "ColorSpace: " + QString("V4L2_COLORSPACE_JPEG");
        break;
    case V4L2_COLORSPACE_SRGB:
        result << "ColorSpace: " + QString("V4L2_COLORSPACE_SRGB");
        break;
    default:
        break;
    }
    switch(fmt.fmt.pix.field) {
    case V4L2_FIELD_ANY:
        result << "Field: " + QString("V4L2_FIELD_ANY");
        break;
    case V4L2_FIELD_NONE:
        result << "Field: " + QString("V4L2_FIELD_NONE");
        break;
    case V4L2_FIELD_TOP:
        result << "Field: " + QString("V4L2_FIELD_TOP");
        break;
    case V4L2_FIELD_BOTTOM:
        result << "Field: " + QString("V4L2_FIELD_BOTTOM");
        break;
    case V4L2_FIELD_INTERLACED:
        result << "Field: " + QString("V4L2_FIELD_INTERLACED");
        break;
    case V4L2_FIELD_SEQ_TB:
        result << "Field: " + QString("V4L2_FIELD_SEQ_TB");
        break;
    case V4L2_FIELD_SEQ_BT:
        result << "Field: " + QString("V4L2_FIELD_SEQ_BT");
        break;
    case V4L2_FIELD_ALTERNATE:
        result << "Field: " + QString("V4L2_FIELD_ALTERNATE");
        break;
    case V4L2_FIELD_INTERLACED_TB:
        result << "Field: " + QString("V4L2_FIELD_INTERLACED_TB");
        break;
    case V4L2_FIELD_INTERLACED_BT:
        result << "Field: " + QString("V4L2_FIELD_INTERLACED_BT");
        break;
    default:
        break;
    }
    result << "Bytes for Line: " + QString("%1").arg(fmt.fmt.pix.bytesperline);
    result << "Image Size: " + QString("%1").arg(fmt.fmt.pix.sizeimage) + " bytes";
    return result;
}

int V4LSettings::qioctl(int fh, int request, void *arg, QString module) {
    module = module.toUpper();
    int res;
    do {
        res = ioctl(fh, request, arg);
    } while (res == -1 && ((errno == EINTR) || (errno == EAGAIN)));

    if (res == -1 ) {
        QString sRequest;
        switch(request) {
        case VIDIOC_DQBUF:
            sRequest =  "VIDIOC_DQBUF";
            break;
        case VIDIOC_ENUM_FMT:
            sRequest =  "VIDIOC_ENUM_FMT";
            break;
        case VIDIOC_ENUM_FRAMESIZES:
            sRequest =  "VIDIOC_ENUM_FRAMESIZES";
            break;
        case VIDIOC_G_FMT:
            sRequest =  "VIDIOC_G_FMT";
            break;
        case VIDIOC_QBUF:
            sRequest =  "VIDIOC_QBUF";
            break;
        case VIDIOC_QUERYBUF:
            sRequest =  "VIDIOC_QUERYBUF";
            break;
        case VIDIOC_QUERYCAP:
            sRequest =  "VIDIOC_QUERYCAP";
            break;
        case VIDIOC_REQBUFS:
            sRequest =  "VIDIOC_REQBUFS";
            break;
        case VIDIOC_S_FMT:
            sRequest =  "VIDIOC_S_FMT";
            break;
        case VIDIOC_STREAMOFF:
            sRequest =  "VIDIOC_STREAMOFF";
            break;
        case VIDIOC_TRY_FMT:
            sRequest =  "VIDIOC_TRY_FMT";
            break;
        default:
            sRequest = "[Unmapped Request]";
            break;
        }
        if (errno == EINVAL) {
            switch(request) {
            case VIDIOC_ENUM_FMT:
            case VIDIOC_ENUM_FRAMESIZES:
            case VIDIOC_TRY_FMT:
                break;
            default:
#ifdef _DEBUG_CONFIGURATION_OBJECTS
                qWarning() << "[" + sRequest + "] called by [" + module + "] returns error #" + \
                              QString("%1").arg(errno) + " (" + QString(strerror(errno)).toUpper() +")";
#endif //_DEBUG_CONFIGURATION_OBJECTS
                break;
            }
        }
    }
    return res;
}
