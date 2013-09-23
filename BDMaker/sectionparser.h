#ifndef SECTIONPARSER_H
#define SECTIONPARSER_H

#include "abstractparser.h"
#include "common.h"

class SectionParser : public AbstractParser
{
    Q_OBJECT
public:
    explicit SectionParser(QObject *parent = 0);
    ~SectionParser();

private:
    void addSection(SectionInfo section);
    void parseFile(const QString &fileName);
};

#endif // SECTIONPARSER_H
