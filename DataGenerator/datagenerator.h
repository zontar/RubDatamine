#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QMainWindow>
#include <QSqlDatabase>

namespace Ui {
class DataGenerator;
}

class DataGenerator : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataGenerator(QWidget *parent = 0);
    ~DataGenerator();

private:
    Ui::DataGenerator *ui;
    QSqlDatabase db;

protected slots:
    void on_pbSelect_clicked();
    void on_pbExecute_clicked();
};

#endif // DATAGENERATOR_H
