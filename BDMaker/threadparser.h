#ifndef THREADPARSER_H
#define THREADPARSER_H

#include <QObject>
#include <QtSql>
#include <QVector>
#include "common.h"

class ThreadParser : public QObject
{
    Q_OBJECT
public:
    explicit ThreadParser(QObject *parent = 0);
    void parse();
private:
    QSqlDatabase db;
    QVector<QString> guests;
    void addThread(ThreadInfo thread);
    void parseFile(const QString &fileName);
    void makeGuests();

signals:
    void finished();

public slots:

};

#endif // THREADPARSER_H
