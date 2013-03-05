#include "v4lconfigurationengine.h"

V4LConfigurationEngine::V4LConfigurationEngine(QObject *parent) : QObject(parent) {
    _settingsDialog = new V4LSettings();
    settingsAccepted();
    connect(_settingsDialog, SIGNAL(accepted()), this, SLOT(settingsAccepted()));
    connect(_settingsDialog, SIGNAL(rejected()), this, SLOT(settingsRejected()));
}

V4LConfigurationEngine::~V4LConfigurationEngine() {
    delete _settingsDialog;
}

void V4LConfigurationEngine::configRequest(QStringList sConfig) {
    struct v4l2_format config_format;
    if ( sConfig.empty()) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - configRequest() - Empty Configuration ";
#endif //_DEBUG_CONFIGURATION_OBJECTS
        if (_deviceName.isEmpty()) {
            CLEAR(config_format);
        }
    } else {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
        qDebug() << "[V4L_CONFIGURATION_ENGINE] - configRequest() - Configuration received: " << sConfig;
#endif //_DEBUG_CONFIGURATION_OBJECTS
        _deviceName = sConfig.at(0);
        config_format = encodeConfigurationStringList(sConfig, _stored_configuration);
        configurationQuery(config_format);
    }
}

void V4LConfigurationEngine::configurationQuery(struct v4l2_format config) {
    if (_deviceName.isEmpty()) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
        qDebug() << "[V4L_CONFIGURATION_ENGINE] - configurationQuery() - Launching Settings Panel";
#endif //_DEBUG_CONFIGURATION_OBJECTS
        _settingsDialog->show();
        return;
    }
    QFile  captureDevice(_deviceName);
    if (!captureDevice.exists()) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - configurationQuery() - Invalid device " << captureDevice.fileName();
#endif //_DEBUG_CONFIGURATION_OBJECTS
        _deviceName.clear();
        _settingsDialog->show();
        return;
    }
    if (!captureDevice.open(QIODevice::ReadWrite)) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - configurationQuery() - Unable to open device " << captureDevice.fileName();
#endif //_DEBUG_CONFIGURATION_OBJECTS
        _deviceName.clear();
        _settingsDialog->show();
        return;
    }
    if (V4LSettings::qioctl(captureDevice.handle(), VIDIOC_TRY_FMT, &config, "V4LConfigurationEngine::configurationQuery()") != 0) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - configurationQuery() - Invalid Configuration ";
#endif //_DEBUG_CONFIGURATION_OBJECTS
        _settingsDialog->show();
        return;
    }
    if (V4LSettings::qioctl(captureDevice.handle(), VIDIOC_S_FMT, &config, "V4LConfigurationEngine::configurationQuery();") != 0) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - configurationQuery() - Unable to set selected framesize" \
                   << config.fmt.pix.width << "x" << config.fmt.pix.height;
#endif //_DEBUG_CONFIGURATION_OBJECTS
    }
    if (V4LSettings::qioctl(captureDevice.handle(), VIDIOC_G_FMT, &_stored_configuration, "V4LConfigurationEngine::configurationQuery();") != 0) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
        qWarning("[V4L_CONFIGURATION_ENGINE] - configurationQuery() - Unable to set selected framesize");
#endif //_DEBUG_CONFIGURATION_OBJECTS
    }
    captureDevice.close();
    emit availableConfiguration();
}

CaptureDevice V4LConfigurationEngine::getConfiguration() {
    CaptureDevice device;
    device.deviceName = _deviceName;
    device.configuration = _stored_configuration;
    return device;
}

QStringList V4LConfigurationEngine::getSupportedFrameSizes() {
    return _settingsDialog->supportedFrameRates;
}

void V4LConfigurationEngine::settingsAccepted() {
    _deviceName = _settingsDialog->getV4L2DeviceName();
    _stored_configuration = _settingsDialog->getV4L2Config();
    configurationQuery(_stored_configuration);
}

void V4LConfigurationEngine::settingsRejected() {
    configurationQuery(_stored_configuration);
}

struct v4l2_format V4LConfigurationEngine::encodeConfigurationStringList(QStringList sConfig, v4l2_format old_config) {
    QRegExp resRegExp("\\d+x\\d+");
    struct v4l2_format config, tmpConfig;
    CLEAR(config);
    if (sConfig.count() < 2){
#ifdef _DEBUG_CONFIGURATION_OBJECTS
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - encodeConfigurationStringList() - Too few arguments";
#endif //_DEBUG_CONFIGURATION_OBJECTS
        return old_config;
    }
    QFile  captureDevice(sConfig.at(0));
    if (!captureDevice.exists()) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - encodeConfigurationStringList() - Invalid device " << captureDevice.fileName();
#endif //_DEBUG_CONFIGURATION_OBJECTS
        return old_config;
    }
    if (!captureDevice.open(QIODevice::ReadWrite)) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - encodeConfigurationStringList() - Unable to open device" << captureDevice.fileName();
#endif //_DEBUG_CONFIGURATION_OBJECTS
        return old_config;
    }
//    config.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//    if (V4LSettings::qioctl(captureDevice.handle(), VIDIOC_G_FMT, &config, "V4LConfigurationEngine::encodeConfigurationStringList()") != 0) {
//        config.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
//        if (V4LSettings::qioctl(captureDevice.handle(), VIDIOC_G_FMT, &config, "V4LConfigurationEngine::encodeConfigurationStringList()") != 0) {
//            CLEAR(config);
//            return config;
//        }
//    }
    config = old_config;
    for (quint8 i = 1; i < sConfig.count(); i++) {
        if (resRegExp.exactMatch(sConfig.at(i))) {
            tmpConfig = config;
            tmpConfig.fmt.pix.width = sConfig.at(i).toLower().split(QChar('x')).at(0).toUInt();
            tmpConfig.fmt.pix.height = sConfig.at(i).toLower().split(QChar('x')).at(1).toUInt();
            if (V4LSettings::qioctl(captureDevice.handle(), VIDIOC_TRY_FMT, &tmpConfig, "V4LConfigurationEngine::encodeConfigurationStringList()") != 0){
#ifdef _DEBUG_CONFIGURATION_OBJECTS
                qWarning() << "[V4L_CONFIGURATION_ENGINE] - encodeConfigurationStringList() - Invalid or unsupported frame Size";
#endif //_DEBUG_CONFIGURATION_OBJECTS
             } else
                config = tmpConfig;
        }
    }
    captureDevice.close();
    return config;
}

void V4LConfigurationEngine::frameSizeChangeRequest(QString sReq) {
#ifdef _DEBUG_CONFIGURATION_OBJECTS
    qWarning() << "[V4L_CONFIGURATION_ENGINE] - frameSizeChangeRequest() - set " << sReq << " frame size";
#endif //_DEBUG_CONFIGURATION_OBJECTS
    QStringList sConfig;
    sConfig += _deviceName;
    sConfig += sReq;
    configRequest(sConfig);
}
