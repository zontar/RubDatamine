#ifndef SECTIONPARSER_H
#define SECTIONPARSER_H

#include <QObject>
#include <QtSql>
#include "common.h"

class SectionParser : public QObject
{
    Q_OBJECT
public:
    explicit SectionParser(QObject *parent = 0);
    void parse();
private:
    QSqlDatabase db;
    void addSection(SectionInfo section);
    void parseFile(const QString &fileName);

signals:
    void finished();

public slots:

};

#endif // SECTIONPARSER_H
