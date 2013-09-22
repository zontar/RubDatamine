#include "threadparser.h"
#include <QRegExp>
#include <QFile>
#include <QDir>
#include <QTextCodec>
#include <QApplication>
#include <QDebug>

ThreadParser::ThreadParser(QObject *parent) :
    QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("db.sqlite");
}

void ThreadParser::parse()
{
    if(!db.open())
    {
        qDebug() << "cannot open db.sqlite";
        return;
    }
    qDebug() << "successfully open db.sqlite";
    QDir storage(QDir::currentPath());
    storage.cd("sections storage");
    foreach(QString dir, storage.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir subdir(storage.absoluteFilePath(dir));
        foreach(QString file, subdir.entryList(QDir::Files))
        {
            parseFile(subdir.absoluteFilePath(file));
            QApplication::processEvents();
        }
    }
    db.close();
}

void ThreadParser::addThread(ThreadInfo thread)
{
    QSqlQuery query;
    query.exec("INSERT INTO \"Threads\" ( \"id\",\"name\",\"section\",\"author\",\"create\",\"posts\" ) VALUES ( \
        '"+QString::number(thread.id)+"',\
        '"+thread.name+"',\
        '"+QString::number(thread.section)+"',\
        '"+QString::number(thread.author)+"',\
        '"+QString::number(thread.create.toTime_t())+"',\
            '"+QString::number(thread.posts)+"' )");

    emit finished();
}

void ThreadParser::parseFile(const QString &fileName)
{
    // read file
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QString data = QString::fromLocal8Bit(file.readAll());
    file.close();
    // !read file

    QRegExp reg;
    reg.setPatternSyntax(QRegExp::RegExp2);
    reg.setMinimal(true);
    int pos=0;
    ThreadInfo thread;



    thread.id = fileName.section('/',-1).toInt();
    addThread(thread);
}
