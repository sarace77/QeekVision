#include <QtGui/QApplication>
#include "qeekdemowindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QeekDemoWindow w;
    w.show();
    
    return a.exec();
}
