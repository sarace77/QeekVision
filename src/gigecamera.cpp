#include "gigecamera.h"

#include <QDebug>
#include <arpa/inet.h>
#include "Defs.h"

GigECamera::GigECamera()
{
    settingsDialog = new GigESettings();
    connect(_settingsAction, SIGNAL(triggered()), this, SLOT(configure()));
    connect(settingsDialog, SIGNAL(cameraChanged()), this, SLOT(viewCameraNetInfo()));
    numCameras = 0;
    PvResult = ePvErrSuccess;
    frameHeight = _STD_FRM_HGHT;
    frameWidth = _STD_FRM_WDTH;
    srcFrame = NULL;
    stopInvoked = false;
}

GigECamera::~GigECamera() {
    stop();
    delete settingsDialog;
    if (srcFrame != NULL)
        delete srcFrame;
}

void GigECamera::configure() {
    PvInitialize();
    msleep(300);
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
    qDebug() << "[GIGE_CAMERA] - exec() - Started!";
    while(1) {
        PvResult = PvCaptureWaitForFrameDone(Camera.Handle, &(Camera.Frame), 5000);
        if (PvResult != ePvErrSuccess && !stopInvoked) {
            qWarning() << "[GIGE_CAMERA] - exec() - Timeout for " << QString(cameraList[settingsDialog->getSelectedCamera()].DisplayName);
            printPvError();
            stop();
        }
        if (settingsDialog->getPixelFormat() == "Mono8") {
            _cvMatbuffer.enqueue(srcFrame->clone());
        } else {
            Mat rgbFrame =srcFrame->clone();
            cvtColor(rgbFrame, rgbFrame, CV_GRAY2RGB);
            _cvMatbuffer.enqueue(rgbFrame.clone());
        }
        PvResult = PvCaptureQueueFrame(Camera.Handle, &(Camera.Frame), NULL);
        if (PvResult != ePvErrSuccess && !stopInvoked) {
            qWarning() << "[GIGE_CAMERA] - exec() - Unable to get frame from" << QString(cameraList[settingsDialog->getSelectedCamera()].DisplayName);
            printPvError();
            stop();
        }
        PvAttrFloat32Get(Camera.Handle, "FrameRate", &_fps);
        emit availableFrame();
    }
    return 0;
}

int GigECamera::getHeight() {
    return frameHeight;
}

int GigECamera::getWidth() {
    return frameWidth;
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
    qDebug() << "[GIGE_CAMERA] - run() - Starting...";
    PvInitialize();
    msleep(500);
    numCameras = PvCameraList(cameraList, MAX_CAMERAS, NULL);
    if(numCameras > 0 && (cameraList[settingsDialog->getSelectedCamera()].PermittedAccess & ePvAccessMaster)) {
        PvResult = PvCameraOpen(Camera.UID, ePvAccessMaster, &(Camera.Handle));
        if (PvResult == ePvErrSuccess) {
            PvAttrUint32Set(Camera.Handle, "ExposureValue", settingsDialog->getExposure());
            PvAttrUint32Set(Camera.Handle, "GainValue", settingsDialog->getGain());
            PvAttrUint32Set(Camera.Handle, "PacketSize", settingsDialog->getPacketSize());
//            if (settingsDialog->getPixelFormat() == "Mono8")
//                PvAttrEnumSet(Camera.Handle, "PixelFormat", settingsDialog->getPixelFormat().toAscii().constData());
            char pixelFormat[256];
            PvAttrUint32Get(Camera.Handle, "TotalBytesPerFrame", &frameSize);
            PvAttrUint32Get(Camera.Handle, "Width", &frameWidth);
            PvAttrUint32Get(Camera.Handle, "Height", &frameHeight);
            PvAttrEnumGet(Camera.Handle, "PixelFormat", pixelFormat,256,NULL);
            Camera.Frame.ImageBufferSize = frameSize;
             if (strcmp(pixelFormat, "Mono8")==0) {
                srcFrame = new Mat(frameHeight, frameWidth, CV_8UC1);
             } else if (strcmp(pixelFormat, "Mono16")==0) {
                srcFrame = new Mat(frameHeight, frameWidth, CV_16UC1);
             } else if (strcmp(pixelFormat, "Bgr24")==0) {
                srcFrame = new Mat(frameHeight, frameWidth, CV_8UC3);
             }
             Camera.Frame.ImageBuffer = srcFrame->data;
             PvResult = PvCaptureStart(Camera.Handle);
             if (PvResult != ePvErrSuccess) {
                 qWarning() << "[GIGE_CAMERA] - run() - Unable to start Capture for " << QString(cameraList[settingsDialog->getSelectedCamera()].DisplayName);
                 printPvError();
                 PvCaptureQueueClear(Camera.Handle);
                 return;
             } else {
                 qDebug() << "[GIGE_CAMERA] - run() - Capture Started";
             }
             PvResult = PvAttrEnumSet(Camera.Handle, "FrameStartTriggerMode", "Freerun");
             if (PvResult != ePvErrSuccess) {
                 qWarning() << "[GIGE_CAMERA] - run() - Unable to set Freerun mode for " << QString(cameraList[settingsDialog->getSelectedCamera()].DisplayName);
                 printPvError();
                 return;
             } else {
                 qDebug() << "[GIGE_CAMERA] - run() - FreeRun mode set";
             }
             PvResult = PvAttrEnumSet(Camera.Handle, "AcquisitionMode", "Continuous");
             if (PvResult != ePvErrSuccess) {
                 qWarning() << "[GIGE_CAMERA] - run() - Unable to set Continous Capture mode for " << QString(cameraList[settingsDialog->getSelectedCamera()].DisplayName);
                 printPvError();
                 return;
             } else {
                 qDebug() << "[GIGE_CAMERA] - run() - Continous mode set";
             }
             PvResult = PvCommandRun(Camera.Handle, "AcquisitionStart");
             if (PvResult != ePvErrSuccess) {
                 qWarning() << "[GIGE_CAMERA] - run() - Unable to set run command AcquisitionStart for " << QString(cameraList[settingsDialog->getSelectedCamera()].DisplayName);
                 printPvError();
                 return;
             } else {
                 qDebug() << "[GIGE_CAMERA] - run() - AcquisitionStart success";
             }
             PvResult = PvCaptureQueueFrame(Camera.Handle, &(Camera.Frame), NULL);
             if (PvResult != ePvErrSuccess) {
                 qWarning() << "[GIGE_CAMERA] - run() - Unable to get frame from" << QString(cameraList[settingsDialog->getSelectedCamera()].DisplayName);
                 printPvError();
                 PvCaptureQueueClear(Camera.Handle);
                 stop();
             } else {
                 qDebug() << "[GIGE_CAMERA] - run() - Capture Queue Started";
             }
            _settingsAction->setEnabled(false);
            _startAction->setEnabled(false);
            _stopAction->setEnabled(true);
            exec();
        } else {
            qWarning() << "[GIGE_CAMERA] - run() - Error opening" << QString(cameraList[settingsDialog->getSelectedCamera()].DisplayName);
            printPvError();
        }
    } else {
        qWarning() << "[GIGE_CAMERA] - run() - no camera detected!";
    }
}

void GigECamera::stop() {
    stopInvoked = true;
    qDebug() << "[GIGE_CAMERA] - stop() - Stopping...";
    PvResult = PvCaptureQueueClear(Camera.Handle);
    if (PvResult != ePvErrSuccess) {
        qWarning() << "[GIGE_CAMERA] - stop() - Error Clearing Capture Queue!";
        printPvError();
    } else {
        qDebug() << "[GIGE_CAMERA] - stop() - Capture Queue sucessfully cleaned!";
    }
    PvResult = PvCommandRun(Camera.Handle, "AcquisitionStop");
    if (PvResult != ePvErrSuccess) {
        qWarning() << "[GIGE_CAMERA] - stop() - Error Stopping Acquisition";
        printPvError();
    } else {
        qDebug() << "[GIGE_CAMERA] - stop() - Acquisition sucessfully stopped!";
    }
    PvResult = PvCaptureEnd(Camera.Handle);
    if (PvResult != ePvErrSuccess) {
        qWarning() << "[GIGE_CAMERA] - stop() - Error Ending Capture";
        printPvError();
    } else {
        qDebug() << "[GIGE_CAMERA] - stop() - Capture sucessfully ended!";
    }
    msleep(2000);
    PvResult = PvCameraClose(Camera.Handle);
    if (PvResult != ePvErrSuccess) {
        qWarning() << "[GIGE_CAMERA] - stop() - Error closing Camera";
        printPvError();
    } else {
        qDebug() << "[GIGE_CAMERA] - stop() - Camera sucessfully closed!";
    }
    _settingsAction->setEnabled(true);
    _startAction->setEnabled(true);
    _stopAction->setEnabled(false);
    stopInvoked = false;
    terminate();
    qDebug() << "[GIGE_CAMERA] - stop() - Stopped!";
}

void GigECamera::viewCameraNetInfo() {
    Camera.UID = cameraList[settingsDialog->getSelectedCamera()].UniqueId;
    QStringList sAddress, sAttributes;
    if (!PvCameraInfo(Camera.UID,&camInfo) && !PvCameraIpSettingsGet(Camera.UID,&ipSettings)) {
        struct in_addr addr;
        addr.s_addr = ipSettings.CurrentIpAddress;
        sAddress << QString("%1").arg(inet_ntoa(addr));
        addr.s_addr = ipSettings.CurrentIpSubnet;
        sAddress << QString("%1").arg(inet_ntoa(addr));
        addr.s_addr = ipSettings.CurrentIpGateway;
        sAddress << QString("%1").arg(inet_ntoa(addr));
        PvResult = PvCameraOpen(Camera.UID, ePvAccessMaster, &(Camera.Handle));
        if (PvResult == ePvErrSuccess) {
            tPvAttrListPtr  listPtr;
            unsigned long   listLength;
            if (PvAttrList(Camera.Handle, &listPtr, &listLength) == ePvErrSuccess) {
                for (int i = 0; i < listLength; i++) {
                    const char* attributeName = listPtr[i];
                    tPvAttributeInfo aInfo;
                    tPvUint32 uValue = 0;
                    tPvFloat32 fValue = 0;
                    tPvInt64 iValue = 0;
                    tPvBoolean bValue = 0;
                    PvAttrInfo(Camera.Handle, attributeName, &aInfo);
                    switch(aInfo.Datatype) {
                    case ePvDatatypeUnknown:
                        sAttributes << QString("[UNKN]") + QString(attributeName);
                        break;
                    case ePvDatatypeCommand:
                        sAttributes << QString("[0CMD]") + QString(attributeName);
                        break;
                    case ePvDatatypeRaw:
                        sAttributes << QString("[RAW_]")  + QString(attributeName);
                        break;
                    case ePvDatatypeString:
                        char sValue[256];
                        memset(sValue,0, 256 * sizeof(char));
                        PvAttrStringGet(Camera.Handle, attributeName, sValue, 256, NULL);
                        sAttributes << QString("[STRN]")  + QString(attributeName) + QString(" = ") + QString(sValue);
                        break;
                    case ePvDatatypeEnum:
                        char eValue[256];
                        memset(sValue,0, 256 * sizeof(char));
                        PvAttrEnumGet(Camera.Handle, attributeName, eValue, 256, NULL);
                        sAttributes <<QString("[ENUM]") + QString(attributeName) + QString(" = ") + QString(eValue);
                        break;
                    case ePvDatatypeUint32:
                        PvAttrUint32Get(Camera.Handle, attributeName, &uValue);
                        sAttributes << QString("[UI32]") + QString(attributeName) + QString(" = ") + QString("%1").arg(uValue);
                        break;
                    case ePvDatatypeFloat32:
                        PvAttrFloat32Get(Camera.Handle, attributeName, &fValue);
                        sAttributes << QString("[FL32]") + QString(attributeName) + QString(" = ") + QString("%1").arg(uValue);
                        break;
                    case ePvDatatypeInt64:
                        PvAttrInt64Get(Camera.Handle, attributeName, &iValue);
                        sAttributes << QString("[IN64]") + QString(attributeName) + QString(" = ") + QString("%1").arg(iValue);
                        break;
                    case ePvDatatypeBoolean:
                        PvAttrBooleanGet(Camera.Handle, attributeName, &bValue);
                        if (bValue) {
                            sAttributes << QString("[BOOL]") + QString(attributeName) + QString(" = ") + QString("true");
                        } else {
                            sAttributes << QString("[BOOL]") + QString(attributeName) + QString(" = ") + QString("false");
                        }

                        break;
                    }
                }
            }
            sAttributes.sort();
            tPvUint32 value = 0;
            PvAttrUint32Get(Camera.Handle,"ExposureValue", &value);
            settingsDialog->setExposure((int) value);
            PvAttrUint32Get(Camera.Handle, "GainValue", &value);
            settingsDialog->setGain(value);
            char pixelFormat[256];
            PvAttrEnumGet(Camera.Handle, "PixelFormat", pixelFormat,256,NULL);
            settingsDialog->setPixelFormat(QString(pixelFormat));
            PvAttrUint32Get(Camera.Handle, "PacketSize", &value);
            if (!settingsDialog->isAutoPacketSize())
                settingsDialog->setPacketSize(value);
            PvResult = PvCameraClose(Camera.Handle);
            if (PvResult != ePvErrSuccess) {
                qWarning() << "[GIGE_CAMERA] - viewCameraNetInfo() - Error closing Camera";
                printPvError();
            }
            emit configurated();
            PvUnInitialize();
            _settingsAction->setEnabled(true);
            _startAction->setEnabled(true);
            _stopAction->setEnabled(false);
        } else {
            qWarning() << "[GIGE_CAMERA] - run() - Error opening" << QString(cameraList[settingsDialog->getSelectedCamera()].DisplayName);
            printPvError();
        }
        if (sAttributes.count() > 0)
            sAddress << "Supported Attributes:" << sAttributes;
        settingsDialog->setCameraIPData(sAddress);
    } else {
           qWarning("[GIGE_CAMERA] - run() - could not retrieve camera IP settings.\n");
    }
}
