#include "downloader.h"
#include "ui_downloader.h"

Downloader::Downloader(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Downloader)
{

    ui->setupUi(this);
    m.setBaseUrl("http://rub.altai.su/showpost.php?p=%1");
    timer.setInterval(1000);
    connect(&m,SIGNAL(requestEnd(bool)),this,SLOT(requestEnded(bool)));
    connect(&timer,SIGNAL(timeout()),this,SLOT(onTimer()));
}

Downloader::~Downloader()
{
    delete ui;
}

void Downloader::on_pbGo_clicked()
{
    elapsedTime = prevFoundedCount = finishedCount = foundedCount = notFoundedCount = 0;
    int first = ui->leFirstPost->text().toInt();
    int last = ui->leLastPost->text().toInt();
    totalCount = last-first+1;
    ui->leTotal->setText(QString::number(totalCount));
    timer.start();
    m.doIt(first,last);
}

void Downloader::on_pbStop_clicked()
{
 //   timer.stop();
    m.stop();
}

void Downloader::requestEnded(bool isFounded)
{
    ++finishedCount;
    if(isFounded)
    {
        ++foundedCount;
        ui->leFound->setText(QString("%1 (%2%)").arg(foundedCount).arg(((double)foundedCount/finishedCount)*100));
    }
    else
    {
        ++notFoundedCount;
        ui->leNotFound->setText(QString("%1 (%2%)").arg(notFoundedCount).arg(((double)notFoundedCount/finishedCount)*100));
    }
    ui->leProcessed->setText(QString("%1 (%2%)").arg(finishedCount).arg(((double)finishedCount/totalCount)*100));
    if(totalCount==finishedCount) on_pbStop_clicked();
}

void Downloader::onTimer()
{
    int current = totalCount;
    ++elapsedTime;
    int rps = finishedCount/elapsedTime;
    ui->leElapsed->setText(QString::number(elapsedTime));
    ui->leRPS->setText(QString::number(rps));
    ui->leRemaining->setText(QString::number((totalCount-finishedCount)/rps));
    prevFoundedCount = current;
}
