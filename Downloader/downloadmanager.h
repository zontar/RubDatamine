#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QDir>

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = 0);
    void setRange(int min, int max);
    void setMaxRequests(int val);
    int getMaxRequests() const;
    void setMaxFilesInFolder(int val);
    int getMaxFilesInFolder() const;
    void setBaseUrl(const QString &val);
    QString getBaseUrl() const;
    void doIt();
    void doIt(int min, int max);
    void stop();

private:
    QNetworkAccessManager net;
    QString baseurl;
    int rangeMin;
    int rangeMax;
    int maxRequests;
    int runnedRequests;
    QDir storage;
    int nonExists;
    int maxFilesInFolder;

signals:
    void requestEnd(bool isFounded);

public slots:
protected slots:
    void requestFinished(QNetworkReply *reply);

};

#endif // DOWNLOADMANAGER_H
