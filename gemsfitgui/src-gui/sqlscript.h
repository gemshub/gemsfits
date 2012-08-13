#ifndef SQLSCRIPT_H
#define SQLSCRIPT_H

#include <iostream>

#include <QDialog>


namespace Ui {
    class SQLscript;
}


class SQLscript : public QDialog
{
    Q_OBJECT

public:
    explicit SQLscript(QWidget *parent = 0);
    ~SQLscript();

    // Index of double-clicked item in SQLscript tree widget
    int id_sqlscript;

private:
    Ui::SQLscript *uii;

public slots:
    void dialog_showSQLtemplate( );

signals:
    void show_sqltemplate_signal( int );

};

#endif // SQLSCRIPT_H
