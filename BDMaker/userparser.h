#ifndef USERPARSER_H
#define USERPARSER_H

#include "abstractparser.h"
#include "common.h"

class UserParser : public AbstractParser
{
    Q_OBJECT
public:
    explicit UserParser(QObject *parent = 0);
    ~UserParser();

private:
    void addUser(UserInfo user);
    void parseFile(const QString &fileName);
};

#endif // USERPARSER_H
