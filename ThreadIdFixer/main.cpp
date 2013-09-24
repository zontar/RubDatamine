#include "threadidfixer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ThreadIdFixer w;
    w.show();

    return a.exec();
}
