#ifndef POSTPARSER_H
#define POSTPARSER_H

#include <QObject>
#include <QtSql>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include "common.h"

class PostParser : public QObject
{
    Q_OBJECT
public:
    explicit PostParser(QObject *parent = 0);
    void parse();
private:
    QSqlDatabase db;
    QVector<QString> guests;
    QFile logFile;
    QTextStream log;
    int errorCount;
    void addPost(PostInfo thread);
    void parseFile(const QString &fileName);
    void makeGuests();

signals:
    void finished();
    void stop();

public slots:

};

#endif // POSTPARSER_H
