#include "gigecamera.h"

#include <QDebug>
#include <arpa/inet.h>

GigECamera::GigECamera()
{
    settingsDialog = new GigESettings();
    connect(_settingsAction, SIGNAL(triggered()), this, SLOT(configure()));
    connect(settingsDialog, SIGNAL(cameraChanged()), this, SLOT(viewCameraNetInfo()));
    numCameras = 0;
    PvResult = ePvErrSuccess;
}
GigECamera::~GigECamera() {
    PvUnInitialize();
    if(isRunning())
        stop();
    delete settingsDialog;
}

void GigECamera::configure() {
    PvInitialize();
    msleep(1000);
    numCameras = PvCameraList(cameraList, MAX_CAMERAS, NULL);
    qDebug() << "[GIGE_CAMERA] - configure() - Num of detected cameras: " << numCameras;

    QStringList camList;

    if (numCameras < 1) {
        qWarning() << "[GIGE_CAMERA] - configure() - no camera detected!";
    } else {
        for (int i = 0; i < numCameras; i++) {
            camList.append(QString(cameraList[i].DisplayName));
        }
        settingsDialog->addCameraList(camList);
        viewCameraNetInfo();
        settingsDialog->show();
    }
}

int GigECamera::exec() {
    qDebug() << "[GIGE_CAMERA] - exec() - Started";
    return 0;
}

int GigECamera::getHeight() {
    return 0;
}

int GigECamera::getWidth() {
    return 0;
}

bool GigECamera::isConfigurated() {
    return true;
}

void GigECamera::printPvError() {
    QString sError = "No error";
    switch((int) PvResult) {
    case ePvErrSuccess:
        break;
    case ePvErrCameraFault:
        sError = "Unexpected camera fault";
        break;
    case ePvErrInternalFault:
        sError = "Unexpected fault in PvAPI or driver";
        break;
    case ePvErrBadHandle:
        sError = "Camera handle is bad";
        break;
    case ePvErrBadParameter:
        sError = "Function parameter is bad";
        break;
    case ePvErrBadSequence:
        sError = "Incorrect sequence of API calls";
        break;
    case ePvErrNotFound:
        sError = "Camera Open: Requested camera is not found";
        break;
    case ePvErrAccessDenied:
        sError = "Camera Open: Access Denied!";
        break;
    case ePvErrUnplugged:
        sError = "Unexpected Camera unplugged";
        break;
    case ePvErrInvalidSetup:
        sError = "Grab frame: Camera setup incorrect";
        break;
    case ePvErrResources:
        sError = "Required Resource or Network unavailble";
        break;
    case ePvErrBandwidth:
        sError = "ePvErrBandwidth";
        break;
    case ePvErrQueueFull:
        sError = "Frame Queue is full";
        break;
    case ePvErrBufferTooSmall:
        sError = "Frame buffer too small for acquiring frame";
        break;
    case ePvErrCancelled:
        sError = "Frame Queuing Canceled";
        break;
    case ePvErrDataLost:
        sError = "Data Frame corruption";
        break;
    case ePvErrDataMissing:
        sError = "Data Frame corruption (missing)";
        break;
    case ePvErrTimeout:
        sError = "Timeout";
        break;
    case ePvErrOutOfRange:
        sError = "Passed value is out of range";
        break;
    case ePvErrWrongType:
        sError = "Wrong Type";
        break;
    case ePvErrForbidden:
        sError = "Unable to write attribute at this time";
        break;
    case ePvErrUnavailable:
        sError = "Unsupported Camera attribute";
        break;
    case ePvErrFirewall:
        sError = "Streaming port blocked!";
        break;
    default:
        break;
    }
    qDebug() << "[GIGE_CAMERA] - printPvError() - Error:" << sError;
}

void GigECamera::run() {
    PvInitialize();
    msleep(1000);
    numCameras = PvCameraList(cameraList, MAX_CAMERAS, NULL);

    if (numCameras < 1) {
        qWarning() << "[GIGE_CAMERA] - run() - no camera detected!";
    } else {
        PvResult = PvCameraOpen(Camera.UID, ePvAccessMaster, &(Camera.Handle));
        if (PvResult == ePvErrSuccess) {
            PvAttrUint32Set(Camera.Handle,"ExposureValue", settingsDialog->getExposure());
            PvAttrUint32Set(Camera.Handle, "GainValue", settingsDialog->getGain());
            _settingsAction->setEnabled(false);
            _startAction->setEnabled(false);
            _stopAction->setEnabled(true);
            exec();
        } else {
            qWarning() << "[GIGE_CAMERA] - run() - Error opening" << QString(cameraList[settingsDialog->getSelectedCamera()].DisplayName);
            printPvError();
        }

    }
}

void GigECamera::stop() {
    _settingsAction->setEnabled(true);
    _startAction->setEnabled(true);
    _stopAction->setEnabled(false);
    PvResult = PvCameraClose(Camera.Handle);
    msleep(1000);
    PvUnInitialize();
    msleep(1000);
    terminate();
}

void GigECamera::viewCameraNetInfo() {
    Camera.UID = cameraList[settingsDialog->getSelectedCamera()].UniqueId;
    QStringList sAddress;
    if (!PvCameraInfo(Camera.UID,&camInfo) && !PvCameraIpSettingsGet(Camera.UID,&ipSettings)) {
        struct in_addr addr;
        addr.s_addr = ipSettings.CurrentIpAddress;
        sAddress << QString("Camera address: \t%1").arg(inet_ntoa(addr));
        addr.s_addr = ipSettings.CurrentIpSubnet;
        sAddress << QString("Camera subnet: \t%1").arg(inet_ntoa(addr));
        addr.s_addr = ipSettings.CurrentIpGateway;
        sAddress << QString("Camera gateway: \t%1").arg(inet_ntoa(addr));
        settingsDialog->setCameraIPData(sAddress);
        PvResult = PvCameraOpen(Camera.UID, ePvAccessMaster, &(Camera.Handle));
        if (PvResult == ePvErrSuccess) {
            tPvAttrListPtr  listPtr;
            unsigned long   listLength;
            if (PvAttrList(Camera.Handle, &listPtr, &listLength) == ePvErrSuccess) {
                for (int i = 0; i < listLength; i++) {
                    const char* attributeName = listPtr[i];
                    qDebug() << "Supported Attribute: " << QString(attributeName);
                }
            }
            tPvUint32 value = 0;
            PvAttrUint32Get(Camera.Handle,"ExposureValue", &value);
            settingsDialog->setExposure((int) value);
            PvAttrUint32Get(Camera.Handle, "GainValue", &value);
            settingsDialog->setGain(value);
            PvResult = PvCameraClose(Camera.Handle);
            msleep(1000);
            _settingsAction->setEnabled(true);
            _startAction->setEnabled(true);
            _stopAction->setEnabled(false);
            emit configurated();
            PvUnInitialize();
            msleep(1000);
        } else {
            qWarning() << "[GIGE_CAMERA] - run() - Error opening" << QString(cameraList[settingsDialog->getSelectedCamera()].DisplayName);
            printPvError();
        }

    } else {
           qWarning("[GIGE_CAMERA] - run() - could not retrieve camera IP settings.\n");
    }
}
