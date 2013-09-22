#include "bdmaker.h"
#include "ui_bdmaker.h"

#include <QDebug>

BDMaker::BDMaker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BDMaker)
{
    ui->setupUi(this);
    userParser.parse();
//    createBD();
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
        query.exec("CREATE TABLE \"Likes\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"user\" INTEGER, \"post\" INTEGER)");
        query.exec("CREATE TABLE \"Posts\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"author\" INTEGER, \"thread\" INTEGER, \"time\" INTEGER, \"likes\" INTEGER, \"text\" TEXT)");
        query.exec("CREATE TABLE \"Sections\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"name\" TEXT, \"parent\" INTEGER, \"threads\" INTEGER)");
        query.exec("CREATE TABLE \"Threads\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"name\" TEXT, \"section\" INTEGER, \"author\" TEXT, \"create\" INTEGER, \"posts\" INTEGER)");
        query.exec("CREATE TABLE \"Users\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"nickname\" TEXT, \"create\" INTEGER, \"views\" INTEGER, \"likeput\" INTEGER, \"likegot\" INTEGER, \"posts\" INTEGER, \"ppd\" REAL, \"lastactivity\" INTEGER)");
        query.exec("CREATE INDEX \"PostIdIdx\" ON \"Posts\" (\"id\")");
        query.exec("CREATE INDEX \"SectionsIdIdx\" ON \"Sections\" (\"id\")");
        query.exec("CREATE INDEX \"ThreadIdIdx\" ON \"Threads\" (\"id\")");
        query.exec("CREATE INDEX \"ThreadNameIdx\" ON \"Threads\" (\"name\")");
        query.exec("CREATE INDEX \"UsersIdIdx\" ON \"Users\" (\"id\")");
        query.exec("CREATE INDEX \"UsersNicknameIdx\" ON \"Users\" (\"nickname\")");
    db.close();
}
