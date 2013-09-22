#include "postparser.h"
#include <QDebug>

PostParser::PostParser(QObject *parent) :
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

void PostParser::recreate()
{
    QSqlQuery query;
//    query.exec("CREATE TABLE (x INTEGER PRIMARY KEY ASC, y, z);")
}
