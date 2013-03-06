#include "camerawrapper.h"

CameraWrapper::CameraWrapper(QObject *parent) :
    QObject(parent)
{

    _cameraMenu = new QMenu("&Camera");
    _captureMenu = new QMenu("C&apture");
    _frameMenu = new QMenu("Fra&me");
    _frameSizeMenu = new QMenu("Frame Si&ze");
    _mainToolBar = new QToolBar("Main Toolbar");
    _resActionMapper = NULL;
    _selectOpenCV =  new QAction("Use &OpenCV Camera", this);
    _selectV4L =  new QAction("Use &V4L2 Camera", this);

    _startAction = new QAction(QIcon(":/icons/start.png"), "Start", this);
    _stopAction = new QAction(QIcon(":/icons/stop.png"), "Stop", this);
    _settingsAction = new QAction(QIcon(":/icons/settings.png"), "Settings", this);
    _settingsAction->setToolTip("Open Configuration Dialog");

    _openCVCamera = new OpenCVCamera();
    _v4lCamera = new V4LCamera();

    _selectOpenCV->setCheckable(true);
    _selectV4L->setCheckable(true);
    _selectV4L->setChecked(true);
    _startAction->setToolTip("Start Capture Stream");
    _startAction->setShortcut(Qt::Key_Space);
    _startAction->setEnabled(true);
    _stopAction->setToolTip("Stop Capture Stream");
    _stopAction->setShortcut(Qt::Key_Space);
    _stopAction->setEnabled(false);
    _settingsAction->setShortcut(Qt::CTRL+Qt::Key_T);
    _settingsAction->setEnabled(true);

    _mainToolBar->addAction(_startAction);
    _mainToolBar->addAction(_stopAction);
    _mainToolBar->addSeparator();
    _mainToolBar->addAction(_settingsAction);

    _cameraMenu->addAction(_selectOpenCV);
    _cameraMenu->addAction(_selectV4L);
    selectedCamera = _v4lCamera;

    updatePointers();

    connect(_selectOpenCV, SIGNAL(triggered()), this, SLOT(selectOpenCVToggled()));
    connect(_selectV4L, SIGNAL(triggered()), this, SLOT(selectV4LToggled()));
    connect(_v4lCamera, SIGNAL(started()), this, SLOT(disableMenu()));
    connect(_v4lCamera, SIGNAL(terminated()), this, SLOT(enableMenu()));
    connect(_openCVCamera, SIGNAL(started()), this, SLOT(disableMenu()));
    connect(_openCVCamera, SIGNAL(terminated()), this, SLOT(enableMenu()));

}

CameraWrapper::~CameraWrapper() {
    delete _cameraMenu;
    delete _captureMenu;
    delete _frameMenu;
    delete _frameSizeMenu;
    delete _mainToolBar;
    delete _resActionMapper;
    delete _selectOpenCV;
    delete _selectV4L;
    delete _startAction;
    delete _stopAction;
    delete _settingsAction;
    delete _settingsAction;

    _openCVCamera->deleteLater();
    _v4lCamera->deleteLater();
}

QMenu *CameraWrapper::cameraMenu() {
    return _cameraMenu;
}

QMenu *CameraWrapper::captureMenu() {
    return _captureMenu;
}

QMenu *CameraWrapper::frameFormatMenu() {
    return _frameMenu;
}

void CameraWrapper::disableMenu() {
    _cameraMenu->setEnabled(false);
    _frameSizeMenu->setEnabled(false);
    _startAction->setEnabled(false);
    _stopAction->setEnabled(true);
    _settingsAction->setEnabled(false);
}

void CameraWrapper::enableMenu() {
    _cameraMenu->setEnabled(true);
    _frameSizeMenu->setEnabled(true);
    _startAction->setEnabled(true);
    _stopAction->setEnabled(false);
    _settingsAction->setEnabled(!_selectOpenCV->isChecked());
}

void CameraWrapper::resolutionChanged(int value) {
    for (int i = 0; i < selectedCamera->frameSizeList.count(); i++)
        _frameSizeActionList.at(i)->setChecked(false);
    _frameSizeActionList.at(value)->setChecked(true);
    selectedCamera->frameSizeList.swap(value,0);
    updateResMenu();
}

void CameraWrapper::selectOpenCVToggled() {
    _selectOpenCV->setChecked(true);
    _selectV4L->setChecked(false);
    disconnect(_startAction, SIGNAL(triggered()), selectedCamera, SLOT(start()));
    disconnect(_stopAction, SIGNAL(triggered()), selectedCamera, SLOT(stop()));
    disconnect(_settingsAction, SIGNAL(triggered()), selectedCamera, SLOT(configure()));
    selectedCamera = _openCVCamera;
    updatePointers();
    emit cameraTypeChanged();
}

void CameraWrapper::selectV4LToggled() {
    _selectOpenCV->setChecked(false);
    _selectV4L->setChecked(true);
    disconnect(_startAction, SIGNAL(triggered()), selectedCamera, SLOT(start()));
    disconnect(_stopAction, SIGNAL(triggered()), selectedCamera, SLOT(stop()));
    disconnect(_settingsAction, SIGNAL(triggered()), selectedCamera, SLOT(configure()));
    selectedCamera = _v4lCamera;
    updatePointers();
    emit cameraTypeChanged();
}

QToolBar *CameraWrapper::toolbar() {
    return _mainToolBar;
}

void CameraWrapper::updatePointers() {
    connect(_startAction, SIGNAL(triggered()), selectedCamera, SLOT(start()));
    connect(_stopAction, SIGNAL(triggered()), selectedCamera, SLOT(stop()));
    connect(_settingsAction, SIGNAL(triggered()), selectedCamera, SLOT(configure()));
    _captureMenu->clear();
    _captureMenu->addActions(selectedCamera->actionList);
    updateResMenu();
}

void CameraWrapper::updateResMenu() {
    _frameSizeMenu->clear();
    if (!selectedCamera->frameSizeList.isEmpty()) {
        _resActionMapper = new QSignalMapper(this);
        for (int i = 0; i < _frameSizeActionList.count(); i++)
            delete _frameSizeActionList.at(i);
        _frameSizeActionList.clear();
        for (int i = 0; i < selectedCamera->frameSizeList.count(); i++) {
           QAction *res = new QAction(selectedCamera->frameSizeList.at(i), selectedCamera);
            res->setCheckable(true);
            connect(res, SIGNAL(triggered()), _resActionMapper, SLOT(map()));
            _resActionMapper->setMapping(res, i);
            _frameSizeActionList.append(res);
        }
        _frameSizeActionList.at(0)->setChecked(true);
        _frameSizeMenu->addActions(_frameSizeActionList);
        connect(_resActionMapper, SIGNAL(mapped(int)), this, SLOT(resolutionChanged(int)));
    }
    _frameMenu->clear();
    _frameMenu->addMenu(_frameSizeMenu);
    _frameMenu->addSeparator();
    _frameMenu->addActions(selectedCamera->formatList);
}
