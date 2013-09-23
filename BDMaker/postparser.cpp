#include "postparser.h"
#include <QRegExp>
#include <QFile>
#include <QDir>
#include <QTextCodec>
#include <QApplication>
#include <QDebug>

PostParser::PostParser(QObject *parent) :
    QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("db.sqlite");
}

void PostParser::parse()
{
    if(!db.open())
    {
        qDebug() << "cannot open db.sqlite";
        return;
    }
    qDebug() << "successfully open db.sqlite";
    logFile.setFileName("postparser.log");
    logFile.open(QIODevice::WriteOnly);
    log.setDevice(&logFile);
    QDir storage(QDir::currentPath());
    storage.cd("posts storage");
    errorCount = 0;
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
    logFile.close();
}

void PostParser::addPost(PostInfo post)
{
    qDebug() << "[Add post] id:" << post.id << "; time: " << post.create.toString("dd.MM.yyyy hh.mm") \
             << "; author:" << post.author << "; thead:" << post.thread << "; likes:" << post.likes;
    QSqlQuery query;
    query.exec("INSERT INTO \"Posts\" ( \"id\",\"author\",\"thread\",\"time\",\"likes\",\"text\" ) VALUES ( \
        '"+QString::number(post.id)+"',\
        '"+QString::number(post.author)+"',\
        '"+QString::number(post.thread)+"',\
        '"+QString::number(post.create.toTime_t())+"',\
        '"+QString::number(post.likes)+"',\
        '"+post.text+"' )");

    emit finished();
}

void PostParser::parseFile(const QString &fileName)
{
    // read file
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        errorCount++;
        log << "Cannot open "<<fileName;
        return;
    }
    QString data = QString::fromLocal8Bit(file.readAll());
    file.close();
    // !read file

    QRegExp reg;
    reg.setPatternSyntax(QRegExp::RegExp2);
    reg.setMinimal(true);
    int pos=0;
    PostInfo post;
    QString ids = fileName.section('/',-1);
    post.id = ids.toInt();


    reg.setPattern("<strong>Тема</strong>.*>(.*)</a>");
    pos = reg.indexIn(data);
    if(pos!=-1)
    {
        QSqlQuery query;
        query.exec("SELECT id FROM Threads WHERE name =\""+reg.cap(1)+"\"");
        if(query.next())
        {
            post.thread = query.value(0).toInt();
        }
        else
        {
            errorCount++;
            log << "["+ids+"] Thread <"+reg.cap(1)+"> not found!";
        }
    }

    reg.setPattern("date -->.*\\s(\\S*),\\s(\\d\\d):(\\d\\d)");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1)
    {
        QString ds = reg.cap(1);
        if(ds=="Сегодня") ds = "22.09.2013";
        else if(ds=="Вчера") ds = "21.09.2013";
        QDateTime dt = QDateTime::fromString(ds+" "+reg.cap(2)+" "+reg.cap(3),"dd.MM.yyyy hh mm");
        post.create = dt;
    }
    else
    {
        errorCount++;
        log << "["+ids+"] Cannot found creation date";
    }

    reg.setPattern("post_message.*>\\s*(\\S.*)\\s</div>\\s*<!--");
    pos = reg.indexIn(data,pos+1);
    qDebug() << reg.cap(1).size();
    if(pos!=-1) post.text = reg.cap(1);
    else
    {
        errorCount++;
        log<<"["<<ids<<"] cannot found message";
    }

    reg.setPattern("post_thanks_box.*</table>");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1)
    {
        QString likesblock = reg.cap();
        int lp;
        int likes = 0;
        reg.setPattern("<strong>(Эти|Этот)\\s(\\d*)[\\sп]");
        lp = reg.indexIn(likesblock);
        if(lp!=-1)
        {
            post.likes = reg.cap().toInt();
            reg.setPattern("\\?u=(\\d*)\"");

            while(lp!=-1)
            {
                lp = reg.indexIn(likesblock,lp+1);
                if(lp==-1) continue;
                likes++;
                QSqlQuery query;
                bool ret = query.exec("INSERT INTO \"Likes\" ( \"user\",\"post\" ) VALUES ( \
                           '"+reg.cap(1)+"',\
                           '"+ids+"')");
                if(!ret)
                {
                    errorCount++;
                    log<<"["+ids+"] cannot INSERT into Likes with user="<<reg.cap(1);
                }
            }
            post.likes = likes;
        }
        else
        {
            post.likes = 0;
        }
    }
    else post.likes = 0;


    reg.setPattern("popup menu.*thead\">(.*)</td>");
    pos = reg.indexIn(data);
    if(pos!=-1)
    {
        QString nickname = reg.cap(1);
        reg.setPattern("\\?u=(\\d*)\"");
        pos = reg.indexIn(data,pos);
        if(pos!=-1) post.author = reg.cap(1).toInt();
        else
        {
            QSqlQuery query;
            if(!query.exec("SELECT id FROM Guests WHERE nickname = '"+nickname+"'"))
            {
                errorCount++;
                log<<"["+ids+"] cannot SELECT from Guests with nivkname="<<nickname;
            }
            if(query.next())
            {
                post.author = query.value(0).toInt();
            }
            else
            {
                errorCount++;
                log<<"["+ids+"] cannot found author";
            }
        }
    }
    else
    {
        errorCount++;
        log<<"["+ids+"] cannot found author block";
    }


    addPost(post);
}
