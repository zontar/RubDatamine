#include "userparser.h"
#include <QRegExp>
#include <QSqlQuery>
#include <QSqlError>

UserParser::UserParser(QObject *parent) :
    AbstractParser(parent)
{
    logFileName = "user.log";
    storageDir = "users storage";
}

UserParser::~UserParser()
{

}

void UserParser::addUser(UserInfo user)
{
    QSqlQuery query;
    query.prepare("INSERT INTO \"Users\" ( \"id\",\"nickname\",\"create\",\"views\",\"likeput\",\"likegot\",\"posts\",\"ppd\",\"lastactivity\" )\
                  VALUES ( :id, :nick, :create, :views, :likeput, :likegot, :posts, :ppd, :last)");
    query.bindValue(":id",user.id);
    query.bindValue(":nick",user.nickname);
    query.bindValue(":create",user.createtime.toTime_t());
    query.bindValue(":views",user.views);
    query.bindValue(":likeput",user.likesput);
    query.bindValue(":likegot",user.likesgot);
    query.bindValue(":posts",user.posts);
    query.bindValue(":ppd",user.ppd);
    query.bindValue(":last",user.lastactivity.toTime_t());

    if(!query.exec())
    {
        errorCount++;
        log(user.id,"Cannot INSERT into Threads. "+query.lastError().text());
    }
    emit requestComplite();
}

void UserParser::parseFile(const QString &fileName)
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
    UserInfo user;

    user.id = ids.toInt();

    //User nickname block
    reg.setPattern("\"username_box\".*>(.*)</font>");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) user.nickname = reg.cap(1);
    else
    {
        errorCount++;
        log(ids, "Cannot find username");
        user.nickname = "INVALID";
    }
    //--User nickname block

    //User post count block
    reg.setPattern("Всего сообщений:</span>\\s(.*)</li>");
    pos = reg.indexIn(data, pos+1);
    if(pos!=-1) user.posts = reg.cap(1).replace(',',"").toInt();
    else user.posts = 0; //not error because may be hidden
    //--User post count block

    //User PPD block
    reg.setPattern("Сообщений в день:</span>\\s(.*)</li>");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) user.ppd = reg.cap(1).toDouble();
    else user.ppd = 0;
    //--User PPD block

    //User likesput block
    reg.setPattern("Всего благодарностей:</span>\\s([,\\d]*)</li>");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) user.likesput = reg.cap(1).replace(',',"").toInt();
    else user.likesput = 0;
    //--User likesput block

    //User likesget block
    reg.setPattern("Спасибо:\\s([,\\d]*)\\s");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) user.likesgot = reg.cap(1).replace(',',"").toInt();
    else user.likesgot = 0;
    //User likesget block

    //User last activity block
    reg.setPattern("Последняя активность:</span>\\s(.*)\\s<span class=\"time\">(.*)</span>");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1)
    {
        QString ds = reg.cap(1);
        QString ts = reg.cap(2);
        if(ds=="Сегодня") ds = "22.09.2013";
        else if(ds=="Вчера") ds = "21.09.2013";
        QDateTime dt = QDateTime::fromString(ds+" "+ts,"dd.MM.yyyy hh:mm");
        user.lastactivity = dt;
    }
    else user.lastactivity = QDateTime::fromTime_t(0);
    //--User last activity block

    //User register date block
    reg.setPattern("Регистрация:</span>\\s(.*)</li>");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1)
    {
        QDateTime dt = QDateTime::fromString(reg.cap(1),"dd.MM.yyyy");
        user.createtime = dt;
    }
    else user.createtime = QDateTime::fromTime_t(0);
    //--User register date block

    //User views block
    reg.setPattern("была посещена <strong>([,\\d]*)</strong>");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) user.views = reg.cap(1).replace(',',"").toInt();
    else user.views = 0;
    //--User views block

    addUser(user);
}
