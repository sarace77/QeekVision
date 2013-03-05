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
    QToolBar *captureToolBar();

    CameraThread *selectedCamera;

signals:
    void cameraTypeChanged();

public slots:

private:
    QAction *selectOpenCV, *selectV4L;
    QMenu *cameraTypeMenu;
    OpenCVCamera *openCVCamera;
    V4LCamera *v4lCamera;

private slots:
    void on_selectOpenCV_toggled(bool);
    void on_selectV4L_toggled(bool);
};

#endif // CAMERAWRAPPER_H
