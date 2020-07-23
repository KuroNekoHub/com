#include "com.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    com w;
    w.show();
    return a.exec();
}
