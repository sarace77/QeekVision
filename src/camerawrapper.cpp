#include "camerawrapper.h"

CameraWrapper::CameraWrapper(QObject *parent) :
    QObject(parent)
{
    _captureMenu = new QMenu("C&apture Menu");
    _imageFormatMenu = new QMenu("Frame Forma&t Menu");
    _imageFormatSubMenu = new QMenu("Frame Sizes");
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

    _captureMenu->clear();
    _captureMenu->addActions(selectedCamera->actionList);
    _imageFormatMenu->clear();
    _imageFormatMenu->addActions(selectedCamera->formatList);

    updateResMenu();

    connect(selectOpenCV, SIGNAL(toggled(bool)), this, SLOT(on_selectOpenCV_toggled(bool)));
    connect(selectV4L, SIGNAL(toggled(bool)), this, SLOT(on_selectV4L_toggled(bool)));
    connect(v4lCamera, SIGNAL(started()), this, SLOT(disableMenu()));
    connect(v4lCamera, SIGNAL(terminated()), this, SLOT(enableMenu()));
    connect(openCVCamera, SIGNAL(started()), this, SLOT(disableMenu()));
    connect(openCVCamera, SIGNAL(terminated()), this, SLOT(enableMenu()));
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
    return _captureMenu;
}

QMenu *CameraWrapper::frameFormatMenu() {
    return _imageFormatMenu;
}

void CameraWrapper::disableMenu() {
    cameraTypeMenu->setEnabled(false);
    _imageFormatSubMenu->setEnabled(false);
}

void CameraWrapper::enableMenu() {
    cameraTypeMenu->setEnabled(true);
    _imageFormatSubMenu->setEnabled(true);
}

void CameraWrapper::on_resolution_changed(int value) {
    for (int i = 0; i < selectedCamera->frameSizeList.count(); i++)
        _frameSizeActionList.at(i)->setChecked(false);
    _frameSizeActionList.at(value)->setChecked(true);
    selectedCamera->frameSizeList.swap(value,0);
    updateResMenu();

}

void CameraWrapper::on_selectOpenCV_toggled(bool value) {
    if (value) {
        selectedCamera = openCVCamera;
        emit cameraTypeChanged();
        selectV4L->setChecked(false);
        _captureMenu->clear();
        _captureMenu->addActions(selectedCamera->actionList);
        _imageFormatMenu->clear();
        _imageFormatMenu->addActions(selectedCamera->formatList);
    }
    updateResMenu();
}

void CameraWrapper::on_selectV4L_toggled(bool value) {
    if (value) {
        selectedCamera = v4lCamera;
        emit cameraTypeChanged();
        selectOpenCV->setChecked(false);
        _captureMenu->clear();
        _captureMenu->addActions(selectedCamera->actionList);
        _imageFormatMenu->clear();
        _imageFormatMenu->addActions(selectedCamera->formatList);
    }
    updateResMenu();
}

void CameraWrapper::updateResMenu() {
    _frameSizeActionList.clear();
    if (!selectedCamera->frameSizeList.isEmpty()) {
        _resActionMapper = new QSignalMapper(this);
        for (int i = 0; i < selectedCamera->frameSizeList.count(); i++) {
           QAction *res = new QAction(selectedCamera->frameSizeList.at(i), selectedCamera);
            res->setCheckable(true);
            connect(res, SIGNAL(triggered()), _resActionMapper, SLOT(map()));
            _resActionMapper->setMapping(res, i);
            _frameSizeActionList.append(res);
        }
        _frameSizeActionList.at(0)->setChecked(true);
        connect(_resActionMapper, SIGNAL(mapped(int)), this, SLOT(on_resolution_changed(int)));
        _imageFormatSubMenu->clear();
        _imageFormatSubMenu->addActions(_frameSizeActionList);
        _imageFormatMenu->addMenu(_imageFormatSubMenu);
    }
}
