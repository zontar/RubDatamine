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
    storage.cd("threads storage");
    foreach(QString dir, storage.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir subdir(storage.absoluteFilePath(dir));
        foreach(QString file, subdir.entryList(QDir::Files))
        {
            parseFile(subdir.absoluteFilePath(file));
            QApplication::processEvents();
        }
    }
    makeGuests();
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

    reg.setPattern("Перезагрузить страницу.*<strong>\\s*(.*\\S)\\s*</strong>");
    pos = reg.indexIn(data);
    thread.name = reg.cap(1);

    reg.setPattern("date -->.*\\s(\\S*),\\s(\\d\\d):(\\d\\d)");
    pos = reg.indexIn(data,pos+1);
    QString ds = reg.cap(1);
    if(ds=="Сегодня") ds = "22.09.2013";
    else if(ds=="Вчера") ds = "21.09.2013";
    QDateTime dt = QDateTime::fromString(ds+" "+reg.cap(2)+" "+reg.cap(3),"dd.MM.yyyy hh mm");
    thread.create = dt;

    reg.setPattern("postmenu(.*<)/div>");
    pos = reg.indexIn(data,pos+1);
    QString authorblock = reg.cap(1);
    reg.setPattern("member.php.*u=(\\d*)\">");
    if(reg.indexIn(authorblock)!=-1)
    {
        thread.author = reg.cap(1).toInt();
    }
    else
    {
        reg.setPattern("\\s(\\S.*\\S)\\s*<");
        reg.indexIn(authorblock);
        guests << reg.cap(1);
        thread.author = -guests.size();
    }

    reg.setPattern("Показано.*\\s(\\d*)\\.");
    pos = reg.indexIn(data);
    int postCount;
    if(pos!=-1) postCount = reg.cap(1).toInt();
    else
    {
        int p = 0;
        postCount = 0;
        while((p = data.indexOf("postmenu",p+1))!=-1) ++postCount;
        postCount/=3;
    }
    thread.posts = postCount;

    reg.setPattern("\"(\\d*)\"\\sclass=\"fjsel");
    pos = reg.indexIn(data);
    thread.section = reg.cap(1).toInt();

    thread.id = fileName.section('/',-1).toInt();
    addThread(thread);
}

void ThreadParser::makeGuests()
{
    QSqlQuery query;
    for(int i=0;i<guests.size();++i)
    {
        query.exec("INSERT INTO \"Guests\" ( \"id\",\"nickname\" ) VALUES ( \
            '"+QString::number(-i-1)+"',\
                '"+guests.at(i)+"' )");

    }
}
