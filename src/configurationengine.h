#ifndef CONFIGURATIONENGINE_H
#define CONFIGURATIONENGINE_H

#include <QStringList>
#include <QObject>

#include <linux/videodev2.h>

#include "settings.h"

typedef struct _capture_device {
    QString deviceName;
    struct v4l2_format configuration;
} CaptureDevice;

class ConfigurationEngine : public QObject
{
    Q_OBJECT

private:
    QString _deviceName;
    Settings * _settingsDialog;
    struct v4l2_format _stored_configuration;

    void configurationQuery(struct v4l2_format config);

private slots:
    void settingsAccepted();
    void settingsRejected();

public:
    ConfigurationEngine(QObject *parent = 0);
    ~ConfigurationEngine();

    static struct v4l2_format encodeConfigurationStringList(QStringList sConfig);

public slots:
    void configRequest(QStringList sConfig = QStringList());
    CaptureDevice getConfiguration();

signals:
    void availableConfiguration();

};

#endif // CONFIGURATIONENGINE_H
