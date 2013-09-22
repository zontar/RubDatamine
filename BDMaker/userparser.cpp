#include "userparser.h"
#include <QRegExp>
#include <QFile>
#include <QDir>
#include <QTextCodec>
#include <QDebug>

UserParser::UserParser(QObject *parent) :
    QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("db.sqlite");

}

void UserParser::parse()
{
    if(!db.open())
    {
        qDebug() << "cannot open db.sqlite";
        return;
    }
    qDebug() << "successfully open db.sqlite";
    QDir storage(QDir::currentPath());
    storage.cd("users storage");
    foreach(QString dir, storage.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir subdir(storage.absoluteFilePath(dir));
        foreach(QString file, subdir.entryList(QDir::Files))
        {
            parseFile(subdir.absoluteFilePath(file));
        }
    }
    db.close();
}

void UserParser::addUser(UserInfo user)
{
    QSqlQuery query;
    qDebug() << query.exec("INSERT INTO \"Users\" ( \"id\",\"nickname\",\"create\",\"views\",\"likeput\",\"likegot\",\"posts\",\"ppd\",\"lastactivity\" ) \
VALUES ( "+QString::number(user.id)+",\
'"+user.nickname+"',\
'"+QString::number(user.createtime.toTime_t())+"',\
'"+QString::number(user.views)+"',\
'"+QString::number(user.likesput)+"',\
'"+QString::number(user.likesgot)+"',\
'"+QString::number(user.posts)+"',\
'"+QString::number(user.ppd)+"',\
'"+QString::number(user.lastactivity.toTime_t())+"' )");
}

void UserParser::parseFile(const QString &fileName)
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
    UserInfo user;

    reg.setPattern("\"username_box\".*>(.*)</font>");
    pos = reg.indexIn(data,pos+1);
    user.nickname = reg.cap(1);

    reg.setPattern("Всего сообщений:</span>\\s(.*)</li>");
    pos = reg.indexIn(data, pos+1);
    if(pos!=-1) user.posts = reg.cap(1).replace(',',"").toInt();
    else user.posts = 0;

    reg.setPattern("Сообщений в день:</span>\\s(.*)</li>");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) user.ppd = reg.cap(1).toDouble();
    else user.ppd = 0;

    reg.setPattern("Всего благодарностей:</span>\\s([,\\d]*)</li>");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) user.likesput = reg.cap(1).replace(',',"").toInt();
    else user.likesput = 0;

    reg.setPattern("Спасибо:\\s([,\\d]*)\\s");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) user.likesgot = reg.cap(1).replace(',',"").toInt();
    else user.likesput = 0;

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

    reg.setPattern("Регистрация:</span>\\s(.*)</li>");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1)
    {
        QDateTime dt = QDateTime::fromString(reg.cap(1),"dd.MM.yyyy");
        user.createtime = dt;
    }
    else user.createtime = QDateTime::fromTime_t(0);

    reg.setPattern("была посещена <strong>([,\\d]*)</strong>");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) user.views = reg.cap(1).replace(',',"").toInt();
    else user.views = 0;

    user.id = fileName.section('/',-1).toInt();
    addUser(user);
}
