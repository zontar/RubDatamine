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
    logFile.open(QIODevice::ReadWrite);
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
            if(errorCount>100) break;
            log.flush();
        }
        if(errorCount>100)
        {
            emit stop();
            break;
        }
    }
    db.close();
    logFile.close();
    emit stop();
}

void PostParser::addPost(PostInfo post)
{
    QSqlQuery query;
    bool ret = query.exec("INSERT INTO \"Posts\" ( \"id\",\"author\",\"thread\",\"time\",\"likes\",\"text\" ) VALUES ( \
        '"+QString::number(post.id)+"',\
        '"+QString::number(post.author)+"',\
        '"+QString::number(post.thread)+"',\
        '"+QString::number(post.create.toTime_t())+"',\
        '"+QString::number(post.likes)+"',\
        '"+post.text+"' )");
    if(!ret)
    {
        errorCount++;
        log << "["<<post.id<<"] Cannot INSERT into Posts\n";

    }
    emit finished();
}

void PostParser::parseFile(const QString &fileName)
{
    // read file
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        errorCount++;
        log << "Cannot open "<<fileName<<"\n";
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
            log << "["+ids+"] Thread <"+reg.cap(1)+"> not found!\n";
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
        log << "["+ids+"] Cannot found creation date\n";
    }

    reg.setPattern("post_message_\\d*\">\\s*(\\S.*)\\s</div>\\s*<!--");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) post.text = reg.cap(1);
    else
    {
        errorCount++;
        log<<"["<<ids<<"] cannot found message\n";
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
                    log<<"["+ids+"] cannot INSERT into Likes with user="<<reg.cap(1)<<"\n";
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
                log<<"["+ids+"] cannot SELECT from Guests with nivkname="<<nickname<<"\n";
            }
            if(query.next())
            {
                post.author = query.value(0).toInt();
            }
            else
            {
                errorCount++;
                log<<"["+ids+"] cannot found author\n";
            }
        }
    }
    else
    {
        errorCount++;
        log<<"["+ids+"] cannot found author block\n";
    }


    addPost(post);
}
