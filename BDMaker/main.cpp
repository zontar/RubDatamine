#include "bdmaker.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BDMaker w;
    w.show();

    return a.exec();
}
