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

int GigESettings::getPacketSize() {
    return ui->packetSizeSpinBox->value();
}

QString GigESettings::getPixelFormat() {
    return ui->pixelFormatCombo->currentText();
}

quint8 GigESettings::getSelectedCamera() {
    return ui->cameraSelectCombo->currentIndex();
}

bool GigESettings::isAutoPacketSize() {
    return ui->autoPacketSizeCheckBox->isChecked();
}

void GigESettings::setCameraIPData(QStringList ipData) {
    ui->addressLineEdit->setText(ipData.at(0));
    ui->netmaskLineEdit->setText(ipData.at(1));
    ui->gatewayLineEdit->setText(ipData.at(2));
    if (ipData.count() > 3) {
        ui->attrubutesListWidget->addItems(ipData.mid(3));
    }
}

void GigESettings::setExposure(int value) {
    ui->exposureSlider->setValue(value);
}

void GigESettings::setGain(int value) {
    ui->gainDial->setValue(value);
}

void GigESettings::setPacketSize(int value) {
    ui->packetSizeSpinBox->setValue(value);
}

void GigESettings::setPixelFormat(QString value) {
    for (int i = 0; i < ui->pixelFormatCombo->count(); i++) {
        if (value == ui->pixelFormatCombo->itemText(i)) {
            ui->pixelFormatCombo->setCurrentIndex(i);
            break;
        }
    }
}
