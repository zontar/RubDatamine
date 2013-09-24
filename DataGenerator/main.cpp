#include "datagenerator.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DataGenerator w;
    w.show();

    return a.exec();
}
