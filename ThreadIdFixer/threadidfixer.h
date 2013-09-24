#ifndef THREADIDFIXER_H
#define THREADIDFIXER_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QSqlDatabase>

namespace Ui {
class ThreadIdFixer;
}

class ThreadIdFixer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ThreadIdFixer(QWidget *parent = 0);
    ~ThreadIdFixer();

private:
    Ui::ThreadIdFixer *ui;
    QNetworkAccessManager net;
    QSqlDatabase db;
    int processed;
    int errors;

protected slots:
    void requestFinished(QNetworkReply *reply);
    void on_pbGo_clicked();
    void on_pbSelect_clicked();
};

#endif // THREADIDFIXER_H
