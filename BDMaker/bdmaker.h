#ifndef BDMAKER_H
#define BDMAKER_H

#include <QMainWindow>
#include <QtSql>
#include "postparser.h"

namespace Ui {
class BDMaker;
}

class BDMaker : public QMainWindow
{
    Q_OBJECT

public:
    explicit BDMaker(QWidget *parent = 0);
    ~BDMaker();

private:
    Ui::BDMaker *ui;
    PostParser postParser;
    void createBD();
};

#endif // BDMAKER_H
