#include "bdmaker.h"
#include "ui_bdmaker.h"

#include <QDebug>

BDMaker::BDMaker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BDMaker)
{
    ui->setupUi(this);
    timer.setInterval(1000);
    connect(&userParser,SIGNAL(finished()),this,SLOT(requestFinished()));
    connect(&timer,SIGNAL(timeout()),this,SLOT(onTimer()));

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

void BDMaker::onTimer()
{
    int current = finishedRequests;
    ++elapsedTime;
    int rps = current/elapsedTime;
    ui->leElapsed->setText(QString::number(elapsedTime));
    ui->leRPS->setText(QString::number(rps));
    ui->leRemaining->setText(QString::number((totalCount-current)/rps));
}

void BDMaker::on_pbGo_clicked()
{
    totalCount = 1748;
    ui->leTotal->setText(QString::number(totalCount));
    elapsedTime = finishedRequests = 0;
    timer.start();
    userParser.parse();
}

void BDMaker::on_pbStop_clicked()
{
    timer.stop();
}

void BDMaker::requestFinished()
{
    ++finishedRequests;
    ui->leParsed->setText(QString("%1 (%2%)").arg(finishedRequests).arg(((double)finishedRequests/totalCount)*100));
    if(totalCount==finishedRequests) on_pbStop_clicked();
}
