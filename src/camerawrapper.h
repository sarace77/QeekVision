#ifndef CAMERAWRAPPER_H
#define CAMERAWRAPPER_H

#include "opencvcamera.h"
#include "v4lcamera.h"

class CameraWrapper : public QObject
{
    Q_OBJECT
public:
    explicit CameraWrapper(QObject *parent = 0);
    ~CameraWrapper();

    QMenu *cameraMenu();
    QMenu *captureMenu();
    QMenu *frameFormatMenu();

    CameraThread *selectedCamera;

signals:
    void cameraTypeChanged();

public slots:

private:
    QAction *selectOpenCV, *selectV4L;
    QMenu *cameraTypeMenu, *_captureMenu, *_imageFormatMenu, *_imageFormatSubMenu;
    OpenCVCamera *openCVCamera;
    V4LCamera *v4lCamera;
    QList<QAction* > _frameSizeActionList;
    QSignalMapper *_resActionMapper;

    void updateResMenu();

private slots:
    void disableMenu();
    void enableMenu();
    void on_selectOpenCV_toggled(bool value);
    void on_selectV4L_toggled(bool value);
    void on_resolution_changed(int value);
};

#endif // CAMERAWRAPPER_H
