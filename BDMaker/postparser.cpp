#include "postparser.h"
#include <QRegExp>
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

PostParser::PostParser(QObject *parent) :
    AbstractParser(parent)
{
    logFileName = "post.log";
    storageDir = "posts storage";
}

PostParser::~PostParser()
{

}

void PostParser::prepare()
{
    QSqlQuery query;
    query.exec("SELECT MIN(id) FROM Guests");
    if(query.next()) nextGuestId = query.value(0).toInt()-1;
    else
    {
        nextGuestId = -1;
    }
}

void PostParser::addPost(PostInfo post)
{
    QSqlQuery query;
    query.prepare("INSERT INTO \"Posts\" ( \"id\",\"author\",\"thread\",\"time\",\"likes\",\"text\" )\
                  VALUES (:id, :author, :thread, :time, :likes, :text)");
    query.bindValue(":id",post.id);
    query.bindValue(":author",post.author);
    query.bindValue(":thread",post.thread);
    query.bindValue(":time",post.create.toTime_t());
    query.bindValue(":likes",post.likes);
    query.bindValue(":text",post.text);

    if(!query.exec())
    {
        errorCount++;
        log(post.id,"Cannot INSERT into Posts. "+query.lastError().text());
    }
    emit requestComplite();
}

void PostParser::parseFile(const QString &fileName)
{
    QString ids = fileName.section('/',-1);
    // read file
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
    {
        errorCount++;
        log(ids,"Cannot open "+fileName);
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

    post.id = ids.toInt();

    //Post thread block
    reg.setPattern("<strong>Тема</strong>.*>(.*)</a>");
    pos = reg.indexIn(data,6000); // skip css data
    if(pos!=-1)
    {
        QSqlQuery query;
        query.prepare("SELECT id FROM Threads WHERE name = :name");
        query.bindValue(":name",reg.cap(1));
        if(query.exec())
        {
            if(query.next())
            {
                post.thread = query.value(0).toInt();
                if(query.next())
                {
                    log(ids,"Thread <"+reg.cap(1)+"> has more then one id's!"); // not error, but bullshit
                }
            }
            else
            {
                errorCount++;
                post.thread = 0;
                log(ids,"Thread <"+reg.cap(1)+"> not found!");
            }
        }
        else
        {
            errorCount++;
            post.thread = 0;
            log(ids,"Cannot SELECT from Threads. "+query.lastError().text());
        }
    }
    //--Post thread block

    //Post create time block
    reg.setPattern("date -->.*\\s(\\S*),\\s(\\d\\d):(\\d\\d)");
    pos = reg.indexIn(data,pos+reg.cap().size());
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
        post.create = QDateTime::fromTime_t(0);
        log(ids,"Cannot found creation date");
    }
    //--Post create time block

    //Post text block
    reg.setPattern("post_message_\\d*\">\\s*(\\S.*)\\s</div>\\s*<!--");
    pos = reg.indexIn(data,pos+reg.cap().size());
    if(pos!=-1) post.text = reg.cap(1);
    else
    {
        errorCount++;
        log(ids,"Cannot found post text");
    }
    //--Post text block

    //Post likes block
    reg.setPattern("post_thanks_box.*</table>");
    pos = reg.indexIn(data,pos+reg.cap().size());
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
                query.prepare("INSERT INTO \"Likes\" ( \"user\",\"post\" ) VALUES ( :user, :post )");
                query.bindValue(":user",reg.cap(1));
                query.bindValue(":post",ids);
                if(!query.exec())
                {
                    errorCount++;
                    log(ids,"Cannot INSERT into Likes with user="+reg.cap(1)+". "+query.lastError().text());
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
    //--Post likes block

    //Post author block
    reg.setPattern("popup menu.*thead\">(.*)</td>");
    pos = reg.indexIn(data);
    if(pos!=-1)
    {
        QString nickname = reg.cap(1);
        reg.setPattern("u=(\\d*)\"");
        pos = reg.indexIn(data,pos+reg.cap().size());
        if(pos!=-1) post.author = reg.cap(1).toInt(); // registered user
        else
        {   //guest
            QSqlQuery query;
            // try to find existing guest
            query.prepare("SELECT id FROM Guests WHERE nickname = :nick");
            query.bindValue(":nick",nickname);
            if(!query.exec())
            {
                errorCount++;
                log(ids,"Cannot SELECT from Guests with nickname="+nickname+". "+query.lastError().text());
            }
            else
            {
                if(query.next()) // existing guest
                {
                    post.author = query.value(0).toInt();
                }
                else
                {
                    // try to add new guest
                    query.prepare("INSERT INTO \"Guests\" ( \"id\",\"nickname\" ) VALUES ( :id, :nick )");
                    query.bindValue(":id",nextGuestId);
                    query.bindValue(":nick",nickname);
                    if(!query.exec())
                    {
                        errorCount++;
                        post.author = 0;
                        log(ids,"Cannot INSERT into Guests. id="+QString::number(nextGuestId)+"; nickname="+nickname+"."+query.lastError().text());
                    }
                    else
                    {
                        post.author = nextGuestId;
                        nextGuestId--;
                    }
                }
            }
        }
    }
    else
    {
        errorCount++;
        post.author = 0;
        log(ids,"Cannot found author block");
    }
    //--Post author block

    addPost(post);
}
