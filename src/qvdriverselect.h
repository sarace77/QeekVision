#ifndef QVDRIVERSELECT_H
#define QVDRIVERSELECT_H

#include <QtGui>

namespace Ui {
class QVDriverSelect;
}

enum eDriver {
    DRIVER_OPENCV = 0,
    DRIVER_V4L = 1
};

class QVDriverSelect : public QWidget
{
    Q_OBJECT
    
public:
    QVDriverSelect(QWidget *parent = 0);
    QVDriverSelect(eDriver driverType, QWidget *parent = 0);
    ~QVDriverSelect();

    eDriver getDriverType();

signals:
    void accepted();

private:
    Ui::QVDriverSelect *ui;

private slots:
    void on_okButton_clicked();
};

#endif // QVDRIVERSELECT_H
