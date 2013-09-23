#include "sectionparser.h"
#include <QRegExp>
#include <QSqlQuery>
#include <QSqlError>

#include <QDebug>

SectionParser::SectionParser(QObject *parent) :
    AbstractParser(parent)
{
    logFileName = "section.log";
    storageDir = "sections storage";
}

SectionParser::~SectionParser()
{

}

void SectionParser::addSection(SectionInfo section)
{
    QSqlQuery query;
    query.prepare("INSERT INTO \"Sections\" ( \"id\",\"name\",\"parent\",\"threads\" ) VALUES ( :id, :name, :parent, :threads )");
    query.bindValue(":id",section.id);
    query.bindValue(":name",section.name);
    query.bindValue(":parent",section.parent);
    query.bindValue(":threads",section.threads);
    if(!query.exec())
    {
        errorCount++;
        log(section.id,"Cannot INSERT into Sections. "+query.lastError().text());
    }
    emit requestComplite();
}

void SectionParser::parseFile(const QString &fileName)
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

    SectionInfo section;

    section.id = ids.toInt();

    //Section name block
    reg.setPattern("Перезагрузить страницу.*<strong>\\s*(.*\\S)\\s*</strong>");
    pos = reg.indexIn(data);
    if(pos!=-1) section.name = reg.cap(1);
    else
    {
        errorCount++;
        log(ids,"Cannot find section name");
    }
    //--Section name block

    //Section threads count block
    reg.setPattern("Показаны темы.*\\s(\\d*)<");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) section.threads = reg.cap(1).toInt();
    else
    {
        errorCount++;
        section.threads=0;
        log(ids,"Cannot find threads count");
    }
    //--Section threads count block

    //Section parent
    reg.setPattern("Показать родительский раздел.*\\?f=(\\d*)\"");
    pos = reg.indexIn(data,pos+1);
    if(pos!=-1) section.parent = reg.cap(1).toInt();
    else section.parent = 1;
    //--Section parent

    addSection(section);
}
