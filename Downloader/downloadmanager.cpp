#include "downloadmanager.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDir>
#include <QApplication>

DownloadManager::DownloadManager(QObject *parent) :
    QObject(parent)
{
    rangeMin = rangeMax = 0;
    maxFilesInFolder = 1000;
    maxRequests = 16;

    QObject::connect(&net,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestFinished(QNetworkReply*)));
    storage.setPath(QDir::currentPath());
    storage.mkpath("storage");
    storage.cd("storage");
    storage.mkpath("0");
}

void DownloadManager::setRange(int min, int max)
{
    rangeMin = min;
    rangeMax = max;
}

void DownloadManager::setMaxRequests(int val)
{
    maxRequests = val;
}

int DownloadManager::getMaxRequests() const
{
    return maxRequests;
}

void DownloadManager::setMaxFilesInFolder(int val)
{
    maxFilesInFolder = val;
}

int DownloadManager::getMaxFilesInFolder() const
{
    return maxFilesInFolder;
}

void DownloadManager::setBaseUrl(const QString &val)
{
    baseurl = val;
}

QString DownloadManager::getBaseUrl() const
{
    return baseurl;
}

void DownloadManager::doIt()
{
    if(baseurl.isEmpty()) return;
    runnedRequests = 0;
    finishedRequests = 0;
    for(int i=rangeMin;i<=rangeMax;++i)
    {
        while(runnedRequests>=maxRequests) QApplication::processEvents();
        net.get(QNetworkRequest(QUrl(baseurl.arg(i))))->setProperty("id",i);
        ++runnedRequests;
        QApplication::processEvents();
    }
}

void DownloadManager::doIt(int min, int max)
{
    rangeMin = min;
    rangeMax = max;
    doIt();
}

void DownloadManager::stop()
{

}

void DownloadManager::requestFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    --runnedRequests;
    if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()!=200)
    {
        emit requestEnd(false);
        return;
    }
    QByteArray data = reply->readAll();
    if(data.at(208)=='r')
    {
        emit requestEnd(false);
        return;
    }
    ++finishedRequests;
    int id = reply->property("id").toInt();
    int dirId = finishedRequests/maxFilesInFolder;
    if(finishedRequests%maxFilesInFolder==0)
    {
        storage.mkpath(QString::number(dirId));
    }
    QFile f(storage.path()+QDir::separator()+QString::number(dirId)+QDir::separator()+QString::number(id));
    f.open(QIODevice::WriteOnly);
    f.write(data);
    f.close();
    emit requestEnd(true);
}
