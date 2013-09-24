#include "datagenerator.h"
#include "ui_datagenerator.h"

#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTime>
#include <QDebug>

DataGenerator::DataGenerator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DataGenerator)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("db.sqlite");
    db.open();
}

DataGenerator::~DataGenerator()
{
    db.close();
    delete ui;
}

void DataGenerator::on_pbSelect_clicked()
{
    ui->leOutput->setText(QFileDialog::getSaveFileName(this,"Select output file"));

}

void DataGenerator::on_pbExecute_clicked()
{
    QFile file(ui->leOutput->text());
    if(!file.open(QIODevice::WriteOnly))
    {
        ui->leStatus->setText("Error: cannot open output file");
        return;
    }
    bool hasError=false;
    QTextStream out(&file);
    QSqlQuery query;
    QTime time;
    time.start();
    if(!query.exec(ui->leQuery->text()))
    {
        ui->leStatus->setText("Error: wrong request");
        file.close();
        return;
    }
    ui->leTime->setText(QString::number(time.elapsed()));
    ui->leStatus->setText("Working..");
    int columnCount = query.record().count();
    while(query.next())
    {
        for(int i=0;i<columnCount;++i) out << query.value(i).toString() << "\t";
        out << "\n";
    }
    if(!hasError) ui->leStatus->setText("Done");
    file.close();

}
