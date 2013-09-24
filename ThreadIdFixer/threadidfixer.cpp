#include "threadidfixer.h"
#include "ui_threadidfixer.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSqlQuery>
#include <QFileDialog>
#include <QFile>
#include <QRegExp>
#include <QApplication>

#include <QDebug>

ThreadIdFixer::ThreadIdFixer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ThreadIdFixer)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("db.sqlite");
    db.open();
    connect(&net,SIGNAL(finished(QNetworkReply*)),this,SLOT(requestFinished(QNetworkReply*)));
}

ThreadIdFixer::~ThreadIdFixer()
{
    db.commit();
    db.close();
    delete ui;
}

void ThreadIdFixer::requestFinished(QNetworkReply *reply)
{
    int id = reply->property("id").toInt();
    QRegExp reg;
    reg.setPatternSyntax(QRegExp::RegExp2);
    reg.setMinimal(true);
    reg.setPattern("alt=\"Комбинированный вид\".*t=(\\d*)\">");
    QString data = QString::fromLocal8Bit(reply->readAll());
    if(reg.indexIn(data)!=-1)
    {

        QSqlQuery query;
        query.prepare("UPDATE Posts SET \"thread\"=:thread WHERE \"id\"=:id");
        query.bindValue(":thread",reg.cap(1));
        query.bindValue(":id",id);
        if(query.exec())
        {
            ui->teLog->append("["+QString::number(id)+"] -> <"+reg.cap(1)+">");
            processed++;
            ui->leProcessed->setText(QString::number(processed));
            if(processed%64==0) db.commit();
        }
        else
        {
            ui->teLog->append("["+QString::number(id)+"] Error UPDATE thread="+reg.cap(1));
            errors++;
            ui->leProcessed->setText(QString::number(errors));
        }

    }
    else
    {
        ui->teLog->append("["+QString::number(id)+"] Error parse page");
        errors++;
        ui->leProcessed->setText(QString::number(errors));
    }

}

void ThreadIdFixer::on_pbGo_clicked()
{
    processed = errors = 0;
    QFile file(ui->leFileName->text());
    if(!file.open(QIODevice::ReadOnly))
    {
        ui->teLog->append("Error open file: "+ui->leFileName->text());
        return;
    }
    QRegExp reg;
    reg.setPatternSyntax(QRegExp::RegExp2);
    reg.setMinimal(true);
    reg.setPattern("\\[(\\d*)\\]Thread <.*> has more then one id");
    db.transaction();
    while(!file.atEnd())
    {
        QString line = QString::fromLocal8Bit(file.readLine());
        line.chop(1);
        if(reg.indexIn(line)!=-1)
        {
            QString url = "http://rub.altai.su/showthread.php?p="+reg.cap(1);
            QNetworkReply *r = net.get(QNetworkRequest(QUrl(url)));
            r->setProperty("id",reg.cap(1));
        }
        else
        {
            ui->teLog->append("<!> other error: "+line);
        }
        QApplication::processEvents();
    }
    file.close();
}

void ThreadIdFixer::on_pbSelect_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Select file for parse");
    ui->leFileName->setText(fileName);
}
