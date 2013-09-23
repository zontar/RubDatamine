#include "bdmaker.h"
#include "ui_bdmaker.h"

#include <QDebug>

BDMaker::BDMaker(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BDMaker)
{
    ui->setupUi(this);
    timer.setInterval(1000);
    connect(&parser,SIGNAL(requestComplite()),this,SLOT(requestFinished()));
    connect(&timer,SIGNAL(timeout()),this,SLOT(onTimer()));
    connect(&parser,SIGNAL(stop(bool)),this,SLOT(parserStopped(bool)));
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
    db.transaction();
    QSqlQuery query;
    query.exec("CREATE TABLE \"Guests\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"nickname\" TEXT)");
    query.exec("CREATE TABLE \"Likes\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"user\" INTEGER, \"post\" INTEGER)");
    query.exec("CREATE TABLE \"Posts\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"author\" INTEGER, \"thread\" INTEGER, \"time\" INTEGER, \"likes\" INTEGER, \"text\" TEXT)");
    query.exec("CREATE TABLE \"Sections\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"name\" TEXT, \"parent\" INTEGER, \"threads\" INTEGER)");
    query.exec("CREATE TABLE \"Threads\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"name\" TEXT, \"section\" INTEGER, \"author\" INTEGER, \"create\" INTEGER, \"posts\" INTEGER)");
    query.exec("CREATE TABLE \"Users\" (\"key\" INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, \"id\" INTEGER UNIQUE, \"nickname\" TEXT, \"create\" INTEGER, \"views\" INTEGER, \"likeput\" INTEGER, \"likegot\" INTEGER, \"posts\" INTEGER, \"ppd\" REAL, \"lastactivity\" INTEGER)");
    query.exec("CREATE INDEX \"PostIdIdx\" ON \"Posts\" (\"id\")");
    query.exec("CREATE INDEX \"SectionsIdIdx\" ON \"Sections\" (\"id\")");
    query.exec("CREATE INDEX \"ThreadNameIdx\" on \"Threads\" (\"name\")");
    query.exec("CREATE INDEX \"UsersIdIdx\" ON \"Users\" (\"id\")");
    query.exec("CREATE INDEX \"UsersNicknameIdx\" ON \"Users\" (\"nickname\")");
    db.commit();
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
    totalCount = ui->leTotal->text().toInt();
    elapsedTime = finishedRequests = 0;
    timer.start();
    parser.parse();
}

void BDMaker::on_pbStop_clicked()
{
    timer.stop();
}

void BDMaker::requestFinished()
{
    ++finishedRequests;
    ui->leParsed->setText(QString("%1 (%2%)").arg(finishedRequests).arg(((double)finishedRequests/totalCount)*100));
}

void BDMaker::parserStopped(bool success)
{
    Q_UNUSED(success);
    timer.stop();
}
