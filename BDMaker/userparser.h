#ifndef USERPARSER_H
#define USERPARSER_H

#include <QObject>
#include <QtSql>
#include "common.h"

class UserParser : public QObject
{
    Q_OBJECT
public:
    explicit UserParser(QObject *parent = 0);
    void parse();
private:
    QSqlDatabase db;
    void addUser(UserInfo user);
    void parseFile(const QString &fileName);

signals:

public slots:

};

#endif // USERPARSER_H
