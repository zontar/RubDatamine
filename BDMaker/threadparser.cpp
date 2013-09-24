#include "threadparser.h"
#include <QRegExp>
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

ThreadParser::ThreadParser(QObject *parent) :
    AbstractParser(parent)
{
    logFileName = "thread.log";
    storageDir = "threads storage";
}

ThreadParser::~ThreadParser()
{

}

void ThreadParser::prepare()
{
    QSqlQuery query;
    query.exec("SELECT MIN(id) FROM Guests");
    if(query.next()) nextGuestId = query.value(0).toInt()-1;
    else
    {
        nextGuestId = -1;
    }
}

void ThreadParser::addThread(ThreadInfo thread)
{
    QSqlQuery query;
    query.prepare("INSERT INTO \"Threads\" ( \"id\",\"name\",\"section\",\"author\",\"create\",\"posts\" ) VALUES ( \
               :id, :name, :section, :author, :create, :posts )");
    query.bindValue(":id",thread.id);
    query.bindValue(":name",thread.name);
    query.bindValue(":section",thread.section);
    query.bindValue(":author",thread.author);
    query.bindValue(":create",thread.create.toTime_t());
    query.bindValue(":posts",thread.posts);
    if(!query.exec())
    {
        errorCount++;
        log(thread.id,"Cannot INSERT into Threads. "+query.lastError().text());
    }
    emit requestComplite();
}

void ThreadParser::parseFile(const QString &fileName)
{
    QString ids = fileName.section('/',-1);
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

    thread.id = ids.toInt();

    //Thread name block
    reg.setPattern("Перезагрузить страницу.*<strong>\\s*(.*\\S)\\s*</strong>");
    pos = reg.indexIn(data,6000); //skip css
    if(pos!=-1) thread.name = reg.cap(1);
    else
    {
        thread.name=="INVALID";
        errorCount++;
        log(ids,"Cannot find thread name");
    }
    //--Thread name block

    //Thread create date block
    reg.setPattern("date -->.*\\s(\\S*),\\s(\\d\\d):(\\d\\d)");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1)
    {
        QString ds = reg.cap(1);
        if(ds=="Сегодня") ds = "22.09.2013"; // date, when theads been downloaded from server
        else if(ds=="Вчера") ds = "21.09.2013";
        QDateTime dt = QDateTime::fromString(ds+" "+reg.cap(2)+" "+reg.cap(3),"dd.MM.yyyy hh mm");
        thread.create = dt;
    }
    else
    {
        thread.create = QDateTime::fromTime_t(0);
        errorCount++;
        log(ids, "Cannot find creation date");
    }
    //--Thread create date block


    //Thread author block
    reg.setPattern("postmenu(.*<)/div>");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1)
    {
        QString authorblock = reg.cap(1);

        reg.setPattern("member.php.*u=(\\d*)\">");
        if(reg.indexIn(authorblock)!=-1) // existing author;
        {
            thread.author = reg.cap(1).toInt();
        }
        else // guests
        {
            reg.setPattern("\\s(\\S.*\\S)\\s*<");
            if(reg.indexIn(authorblock)!=-1)
            {
                QSqlQuery query;
                query.prepare("SELECT id FROM Guests WHERE nickname = :nick");
                query.bindValue(":nick",reg.cap(1));
                if(query.exec())
                {
                    if(query.next())
                    {
                        thread.author = query.value(0).toInt();
                    }
                    else
                    {
                        query.prepare("INSERT INTO \"Guests\" ( \"id\",\"nickname\" ) VALUES ( :id, :nick)");
                        query.bindValue(":id",nextGuestId);
                        query.bindValue(":nick",reg.cap(1));
                        if(query.exec())
                        {
                            thread.author = nextGuestId;
                            nextGuestId--;
                        }
                        else
                        {
                            errorCount++;
                            thread.author = 0;
                            log(ids,"Cannot INSERT into Guests. "+query.lastError().text());
                        }
                    }
                }
                else
                {
                    errorCount++;
                    thread.author = 0;
                    log(ids,"Cannot SELECT from Guests. "+query.lastError().text());
                }
            }
            else
            {
                errorCount++;
                thread.author = 0;
                log(ids,"Cannot determinate thead author");
            }
        }
    }
    else
    {
        errorCount++;
        thread.author = 0;
        log(ids,"Cannot find author block");
    }
    //--Thread author block

    //Thread post count block
    reg.setPattern("Показано.*\\s(\\d*)\\.");
    pos = reg.indexIn(data);
    int postCount;
    if(pos!=-1) postCount = reg.cap(1).toInt();
    else
    { //its a magic time
        int p = 0;
        postCount = 0;
        while((p = data.indexOf("postmenu",p+1))!=-1) ++postCount;
        postCount/=3;
    }
    thread.posts = postCount;
    //--Thread post count block

    //Thread section block
    reg.setPattern("\"(\\d*)\"\\sclass=\"fjsel");
    pos = reg.indexIn(data);
    if(pos!=-1) thread.section = reg.cap(1).toInt();
    else
    {
        errorCount++;
        thread.section = 0;
        log(ids,"Cannot find section");
    }
    //--Thread section block

    addThread(thread);
}
