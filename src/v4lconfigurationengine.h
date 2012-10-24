#ifndef V4LCONFIGURATIONENGINE_H
#define V4LCONFIGURATIONENGINE_H

#include <QStringList>
#include <QObject>

#include <linux/videodev2.h>

#include "v4lsettings.h"

typedef struct _capture_device {
    QString deviceName;
    struct v4l2_format configuration;
} CaptureDevice;

class V4LConfigurationEngine : public QObject
{
    Q_OBJECT

private:
    QString _deviceName;
    V4LSettings * _settingsDialog;
    struct v4l2_format _stored_configuration;

    void configurationQuery(struct v4l2_format config);

private slots:
    void settingsAccepted();
    void settingsRejected();

public:
    V4LConfigurationEngine(QObject *parent = 0);
    ~V4LConfigurationEngine();

    static struct v4l2_format encodeConfigurationStringList(QStringList sConfig);

public slots:
    void configRequest(QStringList sConfig = QStringList());
    CaptureDevice getConfiguration();

signals:
    void availableConfiguration();

};

#endif // V4LCONFIGURATIONENGINE_H
