#ifndef SETTINGS_H
#define SETTINGS_H

#include "Defs.h"

#include <QMainWindow>

#include <libv4l2.h>
#include <libv4lconvert.h>
#include <linux/videodev2.h>

#ifndef CLEAR
#define CLEAR(x) memset(&x, 0, sizeof(x))
#endif //CLEAR

namespace Ui {
class Settings;
}

class Settings : public QMainWindow
{
    Q_OBJECT
private:
    Ui::Settings *ui;

    QStringList _devicesList, _devicesNames;
    struct v4l2_capability _querycap;
    struct v4l2_format _format;

    QList<__u32 >           _supported4CC;
    QList<__u32 >           _captureModes;
    QList<v4l2_field >      _field;
    QList<v4l2_colorspace > _colorSpace;

    int open(QString sFileName);

    void tryCaptureMode();
    void tryColorSpace();
    void tryVideoMode();

private slots:

    void on_captureMode_currentIndexChanged(int value);
    void on_device_currentIndexChanged(int value);
    void on_pixelformat_currentIndexChanged(int value);

    void signalAccepted();
    void signalRejected();

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    struct v4l2_format getV4L2Config();
    QString getV4L2DeviceName();
    
    static QString decode4CC(__u32 code);
    static __u32 encode4CC(QString code);
    static QString decodeKernelVersion(__u32 code);
    static QStringList getCapabilitiesStringList(struct v4l2_capability cap);
    static QStringList getFormatStringList(struct v4l2_format fmt);
    static int qioctl(int fh, int request, void *arg, QString module);

signals:
    void accepted();
    void rejected();
};

#endif // SETTINGS_H
