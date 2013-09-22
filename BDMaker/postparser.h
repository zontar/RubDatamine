#ifndef POSTPARSER_H
#define POSTPARSER_H

#include <QObject>
#include <QtSql>

class PostParser : public QObject
{
    Q_OBJECT
public:
    explicit PostParser(QObject *parent = 0);
    void parse();
private:
    QSqlDatabase db;

signals:

public slots:

};

#endif // POSTPARSER_H
