#ifndef QVDRIVERSELECT_H
#define QVDRIVERSELECT_H

#include <QWidget>

namespace Ui {
class QVDriverSelect;
}

enum eDriver {
    DRIVER_OPENCV = 0,
    DRIVER_PV_API = 1,
    DRIVER_V4L = 2
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
