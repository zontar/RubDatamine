#include "bdmaker.h"
#include "ui_bdmaker.h"
#include <QDebug>

BDMaker::BDMaker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BDMaker)
{
    ui->setupUi(this);
    userParser.parse();
 //   createBD();
}

BDMaker::~BDMaker()
{
    delete ui;
}

void BDMaker::createBD()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("db.sqlite");
    if(!db.open())
    {
        qDebug() << "cannot open db.sqlite";
        return;
    }
    qDebug() << "successfully open db.sqlite";
    QSqlQuery query;
    qDebug() << query.exec("CREATE TABLE \"Friends\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"who\" INTEGER, \"whom\" INTEGER)");
    qDebug() << query.exec("CREATE TABLE \"Likes\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"user\" INTEGER, \"post\" INTEGER)");
    qDebug() << query.exec("CREATE TABLE \"Posts\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"author\" INTEGER, \"thread\" INTEGER, \"time\" INTEGER, \"likes\" INTEGER, \"text\" TEXT)");
    qDebug() << query.exec("CREATE TABLE \"Sections\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"name\" TEXT, \"parent\" INTEGER, \"threads\" INTEGER)");
    qDebug() << query.exec("CREATE TABLE \"Threads\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"name\" TEXT, \"section\" INTEGER, \"author\" TEXT, \"create\" INTEGER, \"posts\" INTEGER)");
    qDebug() << query.exec("CREATE TABLE \"Users\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"nickname\" TEXT, \"create\" INTEGER, \"birth\" INTEGER, \"views\" INTEGER, \"likesput\" INTEGER, \"likesgot\" INTEGER, \"posts\" INTEGER, \"prd\" REAL, \"lastactivity\" INTEGER)");
    qDebug() << query.exec("CREATE INDEX \"PostIdIdx\" ON \"Posts\" (\"id\")");
    qDebug() << query.exec("CREATE INDEX \"SectionsIdIdx\" ON \"Sections\" (\"id\")");
    qDebug() << query.exec("CREATE INDEX \"ThreadIdIdx\" ON \"Threads\" (\"id\")");
    qDebug() << query.exec("CREATE INDEX \"ThreadNameIdx\" ON \"Threads\" (\"name\")");
    qDebug() << query.exec("CREATE INDEX \"UsersIdIdx\" ON \"Users\" (\"id\")");
    db.close();
}
