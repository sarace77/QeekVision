#include "gigesettings.h"
#include "ui_gigesettings.h"

GigESettings::GigESettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GigESettings)
{
    ui->setupUi(this);
    connect(ui->cameraSelectCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(cameraChangedSlot()));
}

GigESettings::~GigESettings()
{
    delete ui;
}

void GigESettings::addCameraList(QStringList cameraList) {
    ui->cameraSelectCombo->addItems(cameraList);
}

void GigESettings::cameraChangedSlot() {
    emit cameraChanged();
}

int GigESettings::getExposure() {
    return ui->exposureSlider->value();
}

int GigESettings::getGain() {
    return ui->gainDial->value();
}

quint8 GigESettings::getSelectedCamera() {
    return ui->cameraSelectCombo->currentIndex();
}

void GigESettings::setCameraIPData(QStringList ipData) {
    ui->cameraNetInfo->addItems(ipData);
}

void GigESettings::setExposure(int value) {
    ui->exposureSlider->setValue(value);
}

void GigESettings::setGain(int value) {
    ui->gainDial->setValue(value);
}
