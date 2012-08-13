#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QLabel>
#include <QIODevice>
#include <QTextStream>
#include <QProcess>
#include <QPalette>
#include <QToolButton>
#include <QtSql>
#include <QMessageBox>
#include "ui_mainwindow.h"
#include "system_properties.h"
#include "sqldbmanager.h"
#include "sqlscript.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow, public System_Properties, public SqlDBManager
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // return currently active database
    QSqlDatabase currentDatabase() const;

    SQLscript* dia;

private:
    Ui::GEMSFIT_GUI *ui;

public slots:

    void action1_slot();

    void action2_slot();

    void action3_slot();

    void action4_slot();

    void action5_slot();

    void action6_slot();

    void action7_slot();

    void action8_slot();

    void show_upload();

    void upload_GEMS_input();

    void upload_GEMSFIT_input();

    void check_new_parameters();

    void write_check_config_file();

    void write_config_file();

    // read number of fitting parameters and update row number of bounds table
    void update_rows_bounds();

    // read number of constraints and update row number of constraints table
    void update_rows_constraints();

    // read number of data points to print and update row number of meas_comp table
    void update_rows_print_meas_comp();

    // read measurement data from *.csv file or PostgreSQL database
    void csv_or_sql();

    // when an item of the aDCc/aIPxc array is double-clicked, its background color changes to red
    void find_clicked_param_aDCc();

    void find_clicked_param_aIPxc();

    void find_id_fit_param_in_bounds( int &, const int, const int, const int );

    // run GEMSFIT with new GEMSFIT_input.dat file
    void run_GEMSFIT();

    // set cell border color of aDCc/aIPc cell corresponding to activated cell in upper/lower bounds tablewidget
    void set_cell_border();
    void find_id_fit_param( int &, int &, int &);
	
    // Enter DataBase mode
    void action_DB_mode_slot();

    // Establish connection to database
    void establish_DB_connection();

    // Refresh database tree
    void refreshDB();

    // Show current table
    void showTable();

    // Submit pending changes to PoostgreSQL database
    void submitall_DB();

    // Clear pending changes to PoostgreSQL database
    void clearall_DB();

    // Insert row into DB table
    void insertrow_DB();

    // Remove row into DB table
    void removerow_DB();

    // Mouse right clicks on tableviewDB
    void ShowContextMenu(const QPoint&);

    // Create new measurement data table for activity models from template
    void create_act_model_data_table();

    // Execute expert SQL
    void execSQL();

    // Create SQLscript dialog
    void create_SQLscript_dialog();

    // Show example SQL scripts
    void showSQLtemplate( int );

    // Set stylesheets
    void set_styles();

    /*    // read number of DC and update row number of aDCc table
    void update_rows_aDCc();

    // read number of columns of aDCc array and update aDCc table
    void update_cols_aDCc();

    // read number of rows and columns of aIPx and aIPc arrays and update dimensions of aIPxc table
    void update_rowscols_aIPxc();

    // read number of rows of aIPx and aIPc arrays and update row number of aIPxc table
    void update_rows_aIPxc();

    // Write data from aDCc table into aDCc array
    void write_to_aDCc_measdata();
*/

private:
    void setActive(QTreeWidgetItem *);

    QString activeDb;

};

#endif // MAINWINDOW_H
