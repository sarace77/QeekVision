#include <QtGui/QApplication>
#include "circlesmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CirclesMainWindow w;
    w.showMaximized();
    
    return a.exec();
}
