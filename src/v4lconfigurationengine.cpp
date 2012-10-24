#include "v4lconfigurationengine.h"

#include <QDebug>
#include <QFile>
#include <QRegExp>

V4LConfigurationEngine::V4LConfigurationEngine(QObject *parent) : QObject(parent) {
    _settingsDialog = new V4LSettings();
    connect(_settingsDialog, SIGNAL(accepted()), this, SLOT(settingsAccepted()));
    connect(_settingsDialog, SIGNAL(rejected()), this, SLOT(settingsRejected()));
}

V4LConfigurationEngine::~V4LConfigurationEngine() {
    delete _settingsDialog;
}

void V4LConfigurationEngine::configRequest(QStringList sConfig) {
    struct v4l2_format config_format;
    if (!sConfig.empty()){
        _deviceName = sConfig.at(0);
        config_format = encodeConfigurationStringList(sConfig);
    }
    else {
        _deviceName.clear();
        CLEAR(config_format);
    }
    configurationQuery(config_format);
}

void V4LConfigurationEngine::configurationQuery(struct v4l2_format config) {
    if (_deviceName.isEmpty()) {
        qDebug() << "[V4L_CONFIGURATION_ENGINE] - configurationQuery() - Launching Settings Panel";
        _settingsDialog->show();
        return;
    }
    QFile  captureDevice(_deviceName);
    if (!captureDevice.exists()) {
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - configurationQuery() - Invalid device " << captureDevice.fileName();
        _deviceName.clear();
        _settingsDialog->show();
        return;
    }
    if (!captureDevice.open(QIODevice::ReadWrite)) {
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - configurationQuery() - Unable to open device " << captureDevice.fileName();
        _deviceName.clear();
        _settingsDialog->show();
        return;
    }
    if (V4LSettings::qioctl(captureDevice.handle(), VIDIOC_TRY_FMT, &config, "V4LConfigurationEngine::configurationQuery()") != 0) {
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - configurationQuery() - Invalid Configuration ";
        _settingsDialog->show();
        return;
    }
    captureDevice.close();
    _stored_configuration = config;
    emit availableConfiguration();
}

CaptureDevice V4LConfigurationEngine::getConfiguration() {
    CaptureDevice device;
    device.deviceName = _deviceName;
    device.configuration = _stored_configuration;
    return device;
}

void V4LConfigurationEngine::settingsAccepted() {
    _deviceName = _settingsDialog->getV4L2DeviceName();
    _stored_configuration = _settingsDialog->getV4L2Config();
    configurationQuery(_stored_configuration);
}

void V4LConfigurationEngine::settingsRejected() {
    configurationQuery(_stored_configuration);
}

struct v4l2_format V4LConfigurationEngine::encodeConfigurationStringList(QStringList sConfig) {
    QRegExp resRegExp("\\d+x\\d+");
    struct v4l2_format config, tmpConfig;
    CLEAR(config);
    if (sConfig.count() < 2){
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - encodeConfigurationStringList() - Too few arguments";
        return config;
    }
    QFile  captureDevice(sConfig.at(0));
    if (!captureDevice.exists()) {
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - encodeConfigurationStringList() - Invalid device " << captureDevice.fileName();
        return config;
    }
    if (!captureDevice.open(QIODevice::ReadWrite)) {
        qWarning() << "[V4L_CONFIGURATION_ENGINE] - encodeConfigurationStringList() - Unable to open device" << captureDevice.fileName();
        return config;
    }
    config.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (V4LSettings::qioctl(captureDevice.handle(), VIDIOC_G_FMT, &config, "V4LConfigurationEngine::encodeConfigurationStringList()") != 0) {
        config.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
        if (V4LSettings::qioctl(captureDevice.handle(), VIDIOC_G_FMT, &config, "V4LConfigurationEngine::encodeConfigurationStringList()") != 0) {
            CLEAR(config);
            return config;
        }
    }
    for (quint8 i = 1; i < sConfig.count(); i++) {
        if (resRegExp.exactMatch(sConfig.at(i))) {
            tmpConfig = config;
            tmpConfig.fmt.pix.width = sConfig.at(i).toLower().split(QChar('x')).at(0).toUInt();
            tmpConfig.fmt.pix.height = sConfig.at(i).toLower().split(QChar('x')).at(0).toUInt();
            if (V4LSettings::qioctl(captureDevice.handle(), VIDIOC_TRY_FMT, &tmpConfig, "V4LConfigurationEngine::encodeConfigurationStringList()") != 0)
                qWarning() << "[V4L_CONFIGURATION_ENGINE] - encodeConfigurationStringList() - Invalid or unsupported frame Size";
            else
                config = tmpConfig;
        }
    }
    captureDevice.close();
    return config;
}
