#include <QtGui/QApplication>
#include "gigeqeekdemowindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GigeQeekDemoWindow w;
    w.show();
    
    return a.exec();
}
