#ifndef BDMAKER_H
#define BDMAKER_H

#include <QMainWindow>
#include <QtSql>
#include <QTime>
#include "sectionparser.h"

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
    SectionParser parser;
    QTimer timer;
    int elapsedTime;
    int finishedRequests;
    int totalCount;
    void createBD();
protected slots:
    void onTimer();
    void on_pbGo_clicked();
    void on_pbStop_clicked();
    void requestFinished();
};

#endif // BDMAKER_H
