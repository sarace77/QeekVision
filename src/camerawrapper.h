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
    QToolBar *toolbar();

    CameraThread *selectedCamera;

signals:
    void cameraTypeChanged();

public slots:

private:
    QMenu               *_cameraMenu, *_captureMenu, *_frameMenu, *_frameSizeMenu;
    QList<QAction* >    _frameSizeActionList;
    QToolBar            *_mainToolBar;
    QSignalMapper       *_resActionMapper;
    QAction             *_selectOpenCV, *_selectV4L, *_settingsAction, *_startAction, *_stopAction;

    OpenCVCamera        *_openCVCamera;
    V4LCamera           *_v4lCamera;

private slots:
    void disableMenu();
    void enableMenu();
    void selectOpenCVToggled();
    void selectV4LToggled();
    void resolutionChanged(int value);
    void updatePointers();
    void updateResMenu();
};

#endif // CAMERAWRAPPER_H
