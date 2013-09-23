#ifndef THREADPARSER_H
#define THREADPARSER_H

#include "common.h"
#include "abstractparser.h"

class ThreadParser : public AbstractParser
{
    Q_OBJECT
public:
    explicit ThreadParser(QObject *parent = 0);
    ~ThreadParser();
    void parse();

protected:
    void prepare();

private:
    void addThread(ThreadInfo thread);
    void parseFile(const QString &fileName);
    int nextGuestId;
};

#endif // THREADPARSER_H
