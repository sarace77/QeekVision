#include "camerawrapper.h"

CameraWrapper::CameraWrapper(QObject *parent) :
    QObject(parent)
{
    cameraTypeMenu = new QMenu("C&amera Type");
    selectOpenCV = new QAction("Use &OpenCV Camera", this);
    selectV4L = new QAction("Use &V4L2 Camera", this);
    openCVCamera = new OpenCVCamera();
    v4lCamera = new V4LCamera();

    selectOpenCV->setCheckable(true);
    selectV4L->setCheckable(true);
    selectV4L->setChecked(true);

    cameraTypeMenu->addAction(selectOpenCV);
    cameraTypeMenu->addAction(selectV4L);
    selectedCamera = v4lCamera;

    connect(selectOpenCV, SIGNAL(toggled(bool)), this, SLOT(on_selectOpenCV_toggled(bool)));
    connect(selectV4L, SIGNAL(toggled(bool)), this, SLOT(on_selectV4L_toggled(bool)));
}

CameraWrapper::~CameraWrapper() {
    delete cameraTypeMenu;
    delete selectOpenCV;
    delete selectV4L;
    openCVCamera->deleteLater();
    v4lCamera->deleteLater();
}

QMenu *CameraWrapper::cameraMenu() {
    return cameraTypeMenu;
}

QMenu *CameraWrapper::captureMenu() {
    return selectedCamera->menu();
}

QToolBar *CameraWrapper::captureToolBar() {
    return selectedCamera->toolBar();
}

void CameraWrapper::on_selectOpenCV_toggled(bool value) {
    qDebug() << "eccoci";
    if (value) {
        selectedCamera = openCVCamera;
        emit cameraTypeChanged();
        selectV4L->setChecked(false);
    }
}

void CameraWrapper::on_selectV4L_toggled(bool value) {
    if (value) {
        selectedCamera = v4lCamera;
        emit cameraTypeChanged();
        selectOpenCV->setChecked(false);
    }
}
