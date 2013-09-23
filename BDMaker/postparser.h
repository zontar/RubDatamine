#ifndef POSTPARSER_H
#define POSTPARSER_H

#include "abstractparser.h"
#include "common.h"

class PostParser : public AbstractParser
{
    Q_OBJECT
public:
    explicit PostParser(QObject *parent = 0);
    ~PostParser();
    void prepare();

private:
    void addPost(PostInfo thread);
    void parseFile(const QString &fileName);

    int nextGuestId;
};

#endif // POSTPARSER_H
