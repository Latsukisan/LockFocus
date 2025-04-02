#include "QTTODO.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTTODO w;
    w.show();
    return a.exec();
}
