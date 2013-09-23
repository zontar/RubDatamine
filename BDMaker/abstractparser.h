#ifndef ABSTRACTPARSER_H
#define ABSTRACTPARSER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QSqlDatabase>

class AbstractParser : public QObject
{
    Q_OBJECT
public:
    explicit AbstractParser(QObject *parent = 0);
    virtual ~AbstractParser();
    void parse();

private:
    QFile logFile;
    QTextStream logStream;

protected:
    void log(int id, const QString text);
    void log(const QString &id, const QString text);
    virtual void parseFile(const QString &fileName) = 0;
    virtual void prepare();

    int errorCount;
    QSqlDatabase db;
    QString logFileName;
    QString storageDir;
    int transactionSize;

signals:
    void stop(bool success);
    void requestComplite();

public slots:

};

#endif // ABSTRACTPARSER_H
