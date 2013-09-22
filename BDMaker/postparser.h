#ifndef POSTPARSER_H
#define POSTPARSER_H

#include <QObject>
#include <QtSql>

class PostParser : public QObject
{
    Q_OBJECT
public:
    explicit PostParser(QObject *parent = 0);
private:
    QSqlDatabase db;
    void recreate();

signals:

public slots:

};

#endif // POSTPARSER_H
