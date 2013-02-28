#ifndef QVDRIVERSELECT_H
#define QVDRIVERSELECT_H

#include <QWidget>

namespace Ui {
class QVDriverSelect;
}

enum eDriver {
    DRIVER_OPENCV = 0,
#ifdef _ENABLE_GIG_E_CAMERA_SUPPORT
    DRIVER_PV_API = 1,
    DRIVER_V4L = 2
#else
    DRIVER_V4L = 1
#endif //_ENABLE_GIG_E_CAMERA_SUPPORT
};

class QVDriverSelect : public QWidget
{
    Q_OBJECT
    
private:
    Ui::QVDriverSelect *ui;

private slots:
    void on_okButton_clicked();

public:
    QVDriverSelect(QWidget *parent = 0);
    QVDriverSelect(eDriver driverType, QWidget *parent = 0);
    ~QVDriverSelect();

    eDriver getDriverType();

signals:
    void accepted();

};

#endif // QVDRIVERSELECT_H
