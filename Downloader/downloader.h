#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QMainWindow>
#include <QTimer>
#include "downloadmanager.h"

namespace Ui {
class Downloader;
}

class Downloader : public QMainWindow
{
    Q_OBJECT

public:
    explicit Downloader(QWidget *parent = 0);
    ~Downloader();

private:
    Ui::Downloader *ui;
    DownloadManager m;
    int finishedCount;
    int foundedCount;
    int notFoundedCount;
    int totalCount;
    int prevFoundedCount;
    QTimer timer;
    int elapsedTime;

protected slots:
    void on_pbGo_clicked();
    void on_pbStop_clicked();
    void requestEnded(bool isFounded);
    void onTimer();
};

#endif // DOWNLOADER_H
