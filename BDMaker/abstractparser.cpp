#include "abstractparser.h"

#include <QApplication>
#include <QDir>

AbstractParser::AbstractParser(QObject *parent) :
    QObject(parent)
{
    logFileName = "default.log";
    storageDir = "default storage";
    transactionSize = 64;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("db.sqlite");
}

AbstractParser::~AbstractParser()
{
    logFile.close();
}

void AbstractParser::parse()
{
    if(!db.open())
    {
        emit stop(false);
        return;
    }

    //prepare log
    logFile.setFileName(logFileName);
    logFile.open(QIODevice::ReadWrite);
    logStream.setDevice(&logFile);

    //prepare storage
    QDir storage(QDir::currentPath());
    storage.cd(storageDir);

    //Additional prepare
    prepare();

    errorCount = 0;
    int requestsCount=0;

    db.transaction(); // transaction make it more faster, very more faster
    foreach(QString dir, storage.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir subdir(storage.absoluteFilePath(dir));
        foreach(QString file, subdir.entryList(QDir::Files))
        {
            parseFile(subdir.absoluteFilePath(file));
            requestsCount++;
            if(requestsCount>transactionSize)
            {
                db.commit();
                requestsCount=0;
                db.transaction();
            }
            QApplication::processEvents();
            if(errorCount>100) break; //many errors means when something went wrong
        }
        if(errorCount>100)
        {
            break;
        }
    }
    //close all
    db.commit();
    db.close();
    logFile.close();

    if(errorCount>100) emit stop(false);
    else emit stop(true);
}

void AbstractParser::log(int id, const QString text)
{
    log(QString::number(id),text);

}

void AbstractParser::log(const QString &id, const QString text)
{
    logStream << "["+id+"]" << text << "\n";
}

void AbstractParser::prepare()
{
    //this is dummy
}
