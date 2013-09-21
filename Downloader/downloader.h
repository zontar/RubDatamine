#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QMainWindow>

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

};

#endif // DOWNLOADER_H
