#include <QtGui/QApplication>
#include "videowriterdemowindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    videoWriterDemoWindow w;
    //w.show();
    
    return a.exec();
}
