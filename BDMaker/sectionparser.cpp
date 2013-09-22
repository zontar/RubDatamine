#include "sectionparser.h"
#include <QRegExp>
#include <QFile>
#include <QDir>
#include <QTextCodec>
#include <QApplication>
#include <QDebug>

SectionParser::SectionParser(QObject *parent) :
    QObject(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("db.sqlite");
}

void SectionParser::parse()
{
    if(!db.open())
    {
        qDebug() << "cannot open db.sqlite";
        return;
    }
    qDebug() << "successfully open db.sqlite";
    QDir storage(QDir::currentPath());
    storage.cd("sections storage");
    foreach(QString dir, storage.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir subdir(storage.absoluteFilePath(dir));
        foreach(QString file, subdir.entryList(QDir::Files))
        {
            parseFile(subdir.absoluteFilePath(file));
            QApplication::processEvents();
        }
    }
    db.close();
}

void SectionParser::addSection(SectionInfo section)
{
    QSqlQuery query;
    query.exec("INSERT INTO \"Sections\" ( \"id\",\"name\",\"parent\",\"threads\" ) \
VALUES ( \
'"+QString::number(section.id)+"',\
'"+section.name+"',\
'"+QString::number(section.parent)+"',\
'"+QString::number(section.threads)+"' )");
    emit finished();
}

void SectionParser::parseFile(const QString &fileName)
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
    SectionInfo section;

    reg.setPattern("Перезагрузить страницу.*<strong>\\s*(.*\\S)\\s*</strong>");
    pos = reg.indexIn(data);

    section.name = reg.cap(1);

    reg.setPattern("Показаны темы.*\\s(\\d*)<");
    pos = reg.indexIn(data,pos+1);
    section.threads = reg.cap(1).toInt();

    reg.setPattern("Показать родительский раздел.*\\?f=(\\d*)\"");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) section.parent = reg.cap(1).toInt();
    else section.parent = 1;

    section.id = fileName.section('/',-1).toInt();
    addSection(section);
}
