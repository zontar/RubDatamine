#include "downloader.h"
#include "ui_downloader.h"

Downloader::Downloader(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Downloader)
{
    ui->setupUi(this);
}

Downloader::~Downloader()
{
    delete ui;
}
