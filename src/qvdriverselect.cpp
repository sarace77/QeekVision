#include "qvdriverselect.h"
#include "ui_qvdriverselect.h"

QVDriverSelect::QVDriverSelect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QVDriverSelect)
{
    ui->setupUi(this);
}

QVDriverSelect::QVDriverSelect(eDriver driverType, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QVDriverSelect)
{
    ui->setupUi(this);
    ui->driverComboBox->clear();
    ui->driverComboBox->addItem("OpenCV");
#ifdef _ENABLE_GIG_E_CAMERA_SUPPORT
    ui->driverComboBox->addItem("AVT GigE");
#endif //_ENABLE_GIG_E_CAMERA_SUPPORT
    ui->driverComboBox->addItem("V4L");
    ui->driverComboBox->setCurrentIndex(driverType);
}

QVDriverSelect::~QVDriverSelect()
{
    delete ui;
}

eDriver QVDriverSelect::getDriverType() {
    return (eDriver) ui->driverComboBox->currentIndex();
}

void QVDriverSelect::on_okButton_clicked() {
    emit accepted();
    close();
}
