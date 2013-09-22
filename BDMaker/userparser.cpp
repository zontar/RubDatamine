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
    if(!db.open())
    {
        qDebug() << "cannot open db.sqlite";
        return;
    }
    qDebug() << "successfully open db.sqlite";
}

void UserParser::parse()
{
    QDir storage(QDir::currentPath());
    storage.cd("users storage");
    foreach(QString dir, storage.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        qDebug() << "\tsubdir:" << storage.absoluteFilePath(dir);
        QDir subdir(storage.absoluteFilePath(dir));
        foreach(QString file, subdir.entryList(QDir::Files))
        {
            qDebug() << "\t\tfile:" << subdir.absoluteFilePath(file);
            parseFile(subdir.absoluteFilePath(file));
        }
    }
}

void UserParser::addUser(UserInfo user)
{
    QSqlQuery query;
    query.exec("INSERT INTO \"Users\" ( \"id\",\"nickname\",\"create\",\"views\",\"likeput\",\"likegot\",\"posts\",\"ppd\",\"lastactivity\" ) \
               VALUES ( '"+QString::number(user.id)+"',\
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
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
  //  QByteArray ba = file.readAll();
    QString data = QString::fromLocal8Bit(file.readAll());
    file.close();
    QRegExp reg;
    reg.setPatternSyntax(QRegExp::RegExp2);
    reg.setMinimal(true);
    int pos=0;
    reg.setPattern("\"username_box\".*>(.*)</font>");
    pos = reg.indexIn(data,pos+1);
    qDebug() << pos << reg.cap(1);
    reg.setPattern("Всего сообщений:</span>\\s(.*)</li>");
    pos = reg.indexIn(data, pos+1);
    qDebug() << pos << reg.cap(1);
    reg.setPattern("Сообщений в день:</span>\\s(.*)</li>");
    pos = reg.indexIn(data,pos+1);
    qDebug() << pos << reg.cap(1);
    reg.setPattern("Всего благодарностей:</span>\\s(\\d*)</li>");
    pos = reg.indexIn(data,pos+1);
    qDebug() << pos << reg.cap(1);
    reg.setPattern("Спасибо:\\s(\\d*)\\s");
    pos = reg.indexIn(data,pos+1);
    qDebug() << pos << reg.cap(1);
    reg.setPattern("Последняя активность:</span>\\s(.*)\\s<span class=\"time\">(.*)</span>");
    pos = reg.indexIn(data,pos+1);
    qDebug() << pos << reg.cap(1);
    reg.setPattern("Регистрация:</span>\\s(.*)</li>");
    pos = reg.indexIn(data,pos+1);
    qDebug() << pos << reg.cap(1);
    reg.setPattern("Реферралы:</span>\\s(\\d*)<");
    pos = reg.indexIn(data,pos+1);
    qDebug() << pos << reg.cap(1);
}
