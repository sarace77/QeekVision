#ifndef V4LCONFIGURATIONENGINE_H
#define V4LCONFIGURATIONENGINE_H

#include <linux/videodev2.h>
#include <QtCore>

#include "Defs.h"
#include "v4lsettings.h"

typedef struct _capture_device {
    QString deviceName;
    struct v4l2_format configuration;
} CaptureDevice;

enum CONFIG_DIALOG_STATUS {
    CONFIG_DIALOG_IDLE = 0,
    CONFIG_DIALOG_ACCEPTED = 1,
    CONFIG_DIALOG_REJECTED = 2,
    CONFIG_DIALOG_UNKONWN = 255
};

class V4LConfigurationEngine : public QObject
{
    Q_OBJECT
public:
    V4LConfigurationEngine(QObject *parent = 0);
    ~V4LConfigurationEngine();

    static struct v4l2_format encodeConfigurationStringList(QStringList sConfig, v4l2_format old_config);

    CONFIG_DIALOG_STATUS getDialogStatus();
    QStringList getSupportedFrameSizes();

    void resetConfiguration();

public slots:
    void configRequest(QStringList sConfig = QStringList());
    void frameSizeChangeRequest(QString sReq);
    CaptureDevice getConfiguration();

    void resetConfigDialogStatus();

signals:
    void availableConfiguration();

private:
    QString _deviceName;
    V4LSettings * _settingsDialog;
    CONFIG_DIALOG_STATUS _dialogStatus;
    struct v4l2_format _stored_configuration;

    void configurationQuery(struct v4l2_format config);

private slots:
    void settingsAccepted();
    void settingsRejected();

};

#endif // V4LCONFIGURATIONENGINE_H
