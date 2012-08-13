#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GEMSFIT_GUI)
{
    ui->setupUi( this );

    // Apply stylesheets
    set_styles();

//    connect( ui->action1, SIGNAL( triggered() ), qApp, SLOT(aboutQt()) );

    // Associate icons with "next" and "previous" buttons
    // ui->toolButton_next1->setIcon( QIcon("./images/next.png") );
    // ui->toolButton_next1->setArrowType(Qt::RightArrow);
    // ui->toolButton_next1->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    // ui->toolButton_next1->setToolButtonStyle(Qt::ToolButtonIconOnly );
    // ui->toolButton_next1->setToolButtonStyle(Qt::ToolButtonTextUnderIcon );

    // Set tooltips for mouse hovering
    ui->action1->setToolTip(tr("What would you like to fit?"));
    ui->action2->setToolTip(tr("Upload GEMS3K input file"));
    ui->action3->setToolTip(tr("Numeric configuration of optimization problem"));
    ui->action4->setToolTip(tr("Specify fitting parameters"));
    ui->action5->setToolTip(tr("Location of measurement data"));
    ui->action6->setToolTip(tr("Statistics configuration"));
    ui->action7->setToolTip(tr("Printing configuration"));
    ui->action8->setToolTip(tr("Inspect collected parameters and write configuration file"));

    ui->actionFit->setToolTip(tr("Fittting mode: fit thermodynamic models"));
    ui->actionDataBase->setToolTip(tr("Database mode: create and manage PostgreSQL database tables"));


    // Set SIGNAL-SLOT connections between actions and subwindows
    connect( ui->action1, SIGNAL( triggered() ), this, SLOT( action1_slot()) );
    connect( ui->toolButton_next1, SIGNAL( clicked() ), ui->action2, SLOT( trigger()) );

    connect( ui->action2, SIGNAL( triggered() ), this, SLOT( action2_slot()) );
    connect( ui->toolButton_next2, SIGNAL( clicked() ), ui->action3, SLOT( trigger()) );
    connect( ui->toolButton_previous2, SIGNAL( clicked() ), ui->action1, SLOT( trigger()) );

    connect( ui->action3, SIGNAL( triggered() ), this, SLOT( action3_slot()) );
    connect( ui->toolButton_next3, SIGNAL( clicked() ), ui->action4, SLOT( trigger()) );
    connect( ui->toolButton_previous3, SIGNAL( clicked() ), ui->action2, SLOT( trigger()) );

    connect( ui->action4, SIGNAL( triggered() ), this, SLOT( action4_slot()) );
    connect( ui->toolButton_next4, SIGNAL( clicked() ), ui->action5, SLOT( trigger()) );
    connect( ui->toolButton_previous4, SIGNAL( clicked() ), ui->action3, SLOT( trigger()) );

    connect( ui->action5, SIGNAL( triggered() ), this, SLOT( action5_slot()) );
    connect( ui->toolButton_next5, SIGNAL( clicked() ), ui->action6, SLOT( trigger()) );
    connect( ui->toolButton_previous5, SIGNAL( clicked() ), ui->action4, SLOT( trigger()) );

    connect( ui->action6, SIGNAL( triggered() ), this, SLOT( action6_slot()) );
    connect( ui->toolButton_next6, SIGNAL( clicked() ), ui->action7, SLOT( trigger()) );
    connect( ui->toolButton_previous6, SIGNAL( clicked() ), ui->action5, SLOT( trigger()) );

    connect( ui->action7, SIGNAL( triggered() ), this, SLOT( action7_slot()) );
    connect( ui->toolButton_next7, SIGNAL( clicked() ), ui->action8, SLOT( trigger()) );
    connect( ui->toolButton_previous7, SIGNAL( clicked() ), ui->action6, SLOT( trigger()) );

    connect( ui->action8, SIGNAL( triggered() ), this, SLOT( action8_slot()) );
    connect( ui->toolButton_previous8, SIGNAL( clicked() ), ui->action7, SLOT( trigger()) );

    // At startup set the first subwindow to maximum size and hide the others
    ui->action1->trigger();
    ui->action1->setChecked( 1 );

    // Connections to upload activity model parameters from GEMS3K input files
    ui->comboBox_uploadGEMSinput->setCurrentIndex( 0 );
    connect( ui->comboBox_uploadGEMSinput, SIGNAL( currentIndexChanged(int) ), this, SLOT( show_upload() ) );

    connect( ui->pushButton_uploaddata, SIGNAL(clicked()), this, SLOT( upload_GEMS_input() ) );

    // Write configuration file
    connect( ui->pushButton_writeconfig, SIGNAL(clicked()), this, SLOT( write_config_file() ) );

    // Parameter boundaries and contraints
    connect( ui->spinBox_nFitParam, SIGNAL( valueChanged(int) ), this, SLOT( update_rows_bounds() ) );

    connect( ui->spinBox_nConstraints, SIGNAL( valueChanged(int) ), this, SLOT( update_rows_constraints() ) );

    connect( ui->spinBox_nPrint, SIGNAL( valueChanged(int) ), this, SLOT( update_rows_print_meas_comp() ) );

    connect( ui->pushButton_check, SIGNAL( clicked() ), SLOT( check_new_parameters() ) );

    // Meas data from csv file of posqgresql database
     ui->stackedWidget_csvsql->setCurrentIndex( 0 );
    connect( ui->comboBox_csvsql, SIGNAL( currentIndexChanged(int) ), this, SLOT( csv_or_sql() ) );

    // Set background color of QTabWidgetItem to red when clicked
    connect( ui->pushButton_fit_aDCc , SIGNAL( clicked() ), this, SLOT( find_clicked_param_aDCc() ) );

    connect( ui->pushButton_fit_aIPc , SIGNAL( clicked() ), this, SLOT( find_clicked_param_aIPxc() ) );

    // Run GEMSFIT
    connect( ui->pushButton_runGEMSFIT, SIGNAL( clicked() ), this, SLOT( run_GEMSFIT() ) );

    // set cell border color of aDCc/aIPc cell corresponding to activated cell in upper/lower bounds tablewidget
    connect( ui->tableWidget_bounds, SIGNAL( itemClicked(QTableWidgetItem* ) ), this, SLOT( set_cell_border() ));

    // DataBase mode
    connect( ui->actionDataBase, SIGNAL( triggered() ), this, SLOT( action_DB_mode_slot()) );

    // Fitting mode
    connect( ui->actionFit, SIGNAL( triggered() ), this, SLOT( action1_slot() ) );

    // Connect to DB
    connect( ui->okButton, SIGNAL( clicked() ), this, SLOT( establish_DB_connection() ) );

    // Refresh database connection
    QAction *refreshAction = new QAction(tr("Refresh"), ui->treeWidget);
    connect(refreshAction, SIGNAL(triggered()), SLOT(refreshDB()));
    ui->treeWidget->addAction(refreshAction);

    // Show new table in tableview_DB when table is activated
    connect( ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(showTable( ) ) );

    // Submit all pending changes to PostgreSQL database
    connect( ui->submitButton, SIGNAL( clicked() ), this, SLOT(submitall_DB()) );

    // Clear all pending changes to PostgreSQL database
    connect( ui->clearButton, SIGNAL( clicked() ), this, SLOT(clearall_DB()) );

    // Add context menu for mouse richt clicks
    ui->tableview_DB->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( ui->tableview_DB, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)) );

    // Display SQL expert query field
    connect( ui->pushButton_expertsql, SIGNAL(toggled(bool)), ui->groupBox_SQL, SLOT(setVisible(bool)) );
    connect( ui->pushButton_expertsql, SIGNAL(toggled(bool)), ui->pushButton_execSQL, SLOT(setVisible(bool)) );
    connect( ui->pushButton_expertsql, SIGNAL(toggled(bool)), this, SLOT(create_SQLscript_dialog() ) );

    // Create new measurement data table for activity models from template
    connect( ui->action_Activity_model_template_table, SIGNAL(triggered()), this, SLOT(create_act_model_data_table()) );

    // Execute expert SQL
    connect( ui->pushButton_execSQL, SIGNAL(clicked()), this, SLOT(execSQL()) );

    // Show example SQL scripts in sqlEdit
    dia = new SQLscript(this);
    connect( dia, SIGNAL(show_sqltemplate_signal( int )), this, SLOT( showSQLtemplate( int ) ) );


    QMetaObject::connectSlotsByName(this);


}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::set_styles()
{

    QPalette pal;
    pal.setColor(QPalette::Button, QColor("green"));
    ui->toolBar_fit_db->setAutoFillBackground(true);
    ui->toolBar_fit_db->setPalette( pal );

    QPalette pal1;
    pal1.setColor(QPalette::Button, QColor("black"));
    ui->toolBar->setAutoFillBackground(true);
    ui->toolBar->setPalette( pal1 );

    ui->subwindow_General_model->setWindowFlags( Qt::SubWindow | Qt::FramelessWindowHint );

    ui->subwindow_checkandwrite->setWindowFlags( Qt::SubWindow |  Qt::FramelessWindowHint );
    ui->subwindow_ChemicalSetup->setWindowFlags( Qt::SubWindow |  Qt::FramelessWindowHint );
    ui->subwindow_measdata->setWindowFlags( Qt::SubWindow | Qt::FramelessWindowHint );
    ui->subwindow_optimization->setWindowFlags(  Qt::SubWindow | Qt::FramelessWindowHint );
    ui->subwindow_Printing->setWindowFlags(  Qt::SubWindow | Qt::FramelessWindowHint );
    ui->subwindow_SQLmanager->setWindowFlags(  Qt::SubWindow | Qt::FramelessWindowHint );
    ui->subwindow_statistics->setWindowFlags(  Qt::SubWindow | Qt::FramelessWindowHint );
    ui->subwindow_upload_data->setWindowFlags(  Qt::SubWindow | Qt::FramelessWindowHint );


    ui->tabWidget->setStyleSheet(QString::fromUtf8("background-color: #505050"));
    ui->tabWidget_2->setStyleSheet(QString::fromUtf8("background-color: #505050"));
    ui->tabWidget_DB->setStyleSheet(QString::fromUtf8("background-color: #505050"));

    ui->tableWidget_aDCc->setStyleSheet("background-color:white; color:black");
    ui->pushButton_fit_aDCc->setStyleSheet(QString::fromUtf8("background-color: #B45F06; color:black"));

    ui->tableWidget_aIPxc->setStyleSheet("background-color:white; color:black");
    ui->pushButton_fit_aIPc->setStyleSheet(QString::fromUtf8("background-color: #B45F06; color:black"));

    ui->tableWidget_bounds->setStyleSheet("background-color:white; color:black");
    ui->tableWidget_constraints->setStyleSheet("background-color:white; color:black");
    ui->tableWidget_DC->setStyleSheet("background-color:white; color:black");
    ui->tableWidget_PrintPoints->setStyleSheet("background-color:white; color:black");
    ui->tableWidget_PrintPoints_3->setStyleSheet("background-color:white; color:black");
    ui->tableWidget_PrintPoints_4->setStyleSheet("background-color:white; color:black");

    ui->lineEdit_systemname->setStyleSheet("background-color:white; color:black");
    ui->lineEdit_password->setStyleSheet("background-color:white; color:black");
    ui->lineEdit_phasename->setStyleSheet("background-color:white; color:black");
    ui->lineEdit_dbname->setStyleSheet("background-color:white; color:black");
    ui->lineEdit_username->setStyleSheet("background-color:white; color:black");

    ui->spinBox_colsaDCc->setStyleSheet("background-color:white; color:black");
    ui->spinBox_colsidtable->setStyleSheet("background-color:white; color:black");
    ui->spinBox_colsinterarray->setStyleSheet("background-color:white; color:black");
    ui->spinBox_MCbars->setStyleSheet("background-color:white; color:black");
    ui->spinBox_MCruns->setStyleSheet("background-color:white; color:black");
    ui->spinBox_meascomp_valcode->setStyleSheet("background-color:white; color:black");
    ui->spinBox_nConstraints->setStyleSheet("background-color:white; color:black");
    ui->spinBox_nDC->setStyleSheet("background-color:white; color:black");
    ui->spinBox_nFitParam->setStyleSheet("background-color:white; color:black");
    ui->spinBox_Nintparam->setStyleSheet("background-color:white; color:black");
    ui->spinBox_nodes->setStyleSheet("background-color:white; color:black");
    ui->spinBox_nPrint->setStyleSheet("background-color:white; color:black");
    ui->comboBox_activitymodel->setStyleSheet("background-color:white; color:black");
    ui->comboBox_algo->setStyleSheet("background-color:white; color:black");
    ui->comboBox_csvsql->setStyleSheet("background-color:white; color:black");
    ui->comboBox_fitmode->setStyleSheet("background-color:white; color:black");
    ui->comboBox_meascomp_fileformat->setStyleSheet("background-color:white; color:black");
    ui->comboBox_perform->setStyleSheet("background-color:white; color:black");


    ui->tableview_DB->setStyleSheet("background-color:white; color:black");
    ui->treeWidget->setStyleSheet("background-color:black; color:white");
    ui->pushButton_execSQL->setStyleSheet("background-color:white; color:black");
    ui->pushButton_expertsql->setStyleSheet("background-color:white; color:black");
    ui->cancelButton->setStyleSheet("background-color:white; color:black");
    ui->okButton->setStyleSheet("background-color:white; color:black");
    ui->clearButton->setStyleSheet("background-color:white; color:black");
    ui->submitButton->setStyleSheet("background-color:white; color:black");
    ui->sqlEdit->setStyleSheet("background-color:white; color:black");
    ui->connGroupBox->setStyleSheet("background-color: #909090; color:white");
    ui->portSpinBox->setStyleSheet("background-color:white; color:black");
    ui->editDatabase->setStyleSheet("background-color:white; color:black");
    ui->editHostname->setStyleSheet("background-color:white; color:black");
    ui->editPassword->setStyleSheet("background-color:white; color:black");
    ui->editUsername->setStyleSheet("background-color:white; color:black");
    ui->comboDriver->setStyleSheet("background-color:white; color:black");


    // Set stylesheets for individual buttons
    ui->toolButton_next1->setStyleSheet( "min-width: 5em; padding: 4px;" );
    ui->toolButton_next2->setStyleSheet( "min-width: 5em; padding: 4px;" );
    ui->toolButton_next3->setStyleSheet( "min-width: 5em; padding: 4px;" );
    ui->toolButton_next4->setStyleSheet( "min-width: 5em; padding: 4px;" );
    ui->toolButton_next5->setStyleSheet( "min-width: 5em; padding: 4px;" );
    ui->toolButton_next6->setStyleSheet( "min-width: 5em; padding: 4px;" );
    ui->toolButton_next7->setStyleSheet( "min-width: 5em; padding: 4px;" );

    ui->toolButton_previous2->setStyleSheet( "min-width: 5em; padding: 4px;" );
    ui->toolButton_previous3->setStyleSheet( "min-width: 5em; padding: 4px;" );
    ui->toolButton_previous4->setStyleSheet( "min-width: 5em; padding: 4px;" );
    ui->toolButton_previous5->setStyleSheet( "min-width: 5em; padding: 4px;" );
    ui->toolButton_previous6->setStyleSheet( "min-width: 5em; padding: 4px;" );
    ui->toolButton_previous7->setStyleSheet( "min-width: 5em; padding: 4px;" );
    ui->toolButton_previous8->setStyleSheet( "min-width: 5em; padding: 4px;" );

}



void MainWindow::action1_slot()
{
    ui->action1->setChecked( 1 );
    ui->action2->setChecked( 0 );
    ui->action3->setChecked( 0 );
    ui->action4->setChecked( 0 );
    ui->action5->setChecked( 0 );
    ui->action6->setChecked( 0 );
    ui->action7->setChecked( 0 );
    ui->action8->setChecked( 0 );
    ui->actionFit->setChecked( 1 );
    ui->actionDataBase->setChecked( 0 );
    ui->subwindow_General_model->showMaximized();
    ui->subwindow_upload_data->hide();
    ui->subwindow_optimization->hide();
    ui->subwindow_ChemicalSetup->hide();
    ui->subwindow_measdata->hide();
    ui->subwindow_statistics->hide();
    ui->subwindow_Printing->hide();
    ui->subwindow_checkandwrite->hide();
    ui->subwindow_SQLmanager->hide();
}

void MainWindow::action2_slot()
{
    ui->action1->setChecked( 0 );
    ui->action3->setChecked( 0 );
    ui->action4->setChecked( 0 );
    ui->action5->setChecked( 0 );
    ui->action6->setChecked( 0 );
    ui->action7->setChecked( 0 );
    ui->action8->setChecked( 0 );
    ui->actionFit->setChecked( 1 );
    ui->actionDataBase->setChecked( 0 );
    ui->subwindow_General_model->hide();
    ui->subwindow_upload_data->showMaximized();
    ui->subwindow_optimization->hide();
    ui->subwindow_ChemicalSetup->hide();
    ui->subwindow_measdata->hide();
    ui->subwindow_statistics->hide();
    ui->subwindow_Printing->hide();
    ui->subwindow_checkandwrite->hide();
    ui->subwindow_SQLmanager->hide();
}

void MainWindow::action3_slot()
{
    ui->action1->setChecked( 0 );
    ui->action2->setChecked( 0 );
    ui->action4->setChecked( 0 );
    ui->action5->setChecked( 0 );
    ui->action6->setChecked( 0 );
    ui->action7->setChecked( 0 );
    ui->action8->setChecked( 0 );
    ui->actionFit->setChecked( 1 );
    ui->actionDataBase->setChecked( 0 );
    ui->subwindow_General_model->hide();
    ui->subwindow_upload_data->hide();
    ui->subwindow_optimization->showMaximized();
    ui->subwindow_ChemicalSetup->hide();
    ui->subwindow_measdata->hide();
    ui->subwindow_statistics->hide();
    ui->subwindow_Printing->hide();
    ui->subwindow_checkandwrite->hide();
    ui->subwindow_SQLmanager->hide();
}

void MainWindow::action4_slot()
{
    ui->action1->setChecked( 0 );
    ui->action2->setChecked( 0 );
    ui->action3->setChecked( 0 );
    ui->action5->setChecked( 0 );
    ui->action6->setChecked( 0 );
    ui->action7->setChecked( 0 );
    ui->action8->setChecked( 0 );
    ui->actionFit->setChecked( 1 );
    ui->actionDataBase->setChecked( 0 );
    ui->subwindow_General_model->hide();
    ui->subwindow_upload_data->hide();
    ui->subwindow_optimization->hide();
    ui->subwindow_ChemicalSetup->showMaximized();
    ui->subwindow_measdata->hide();
    ui->subwindow_statistics->hide();
    ui->subwindow_Printing->hide();
    ui->subwindow_checkandwrite->hide();
    ui->subwindow_SQLmanager->hide();
}

void MainWindow::action5_slot()
{
    ui->action1->setChecked( 0 );
    ui->action2->setChecked( 0 );
    ui->action3->setChecked( 0 );
    ui->action4->setChecked( 0 );
    ui->action6->setChecked( 0 );
    ui->action7->setChecked( 0 );
    ui->action8->setChecked( 0 );
    ui->actionFit->setChecked( 1 );
    ui->actionDataBase->setChecked( 0 );
    ui->subwindow_General_model->hide();
    ui->subwindow_upload_data->hide();
    ui->subwindow_optimization->hide();
    ui->subwindow_ChemicalSetup->hide();
    ui->subwindow_measdata->showMaximized();
    ui->subwindow_statistics->hide();
    ui->subwindow_Printing->hide();
    ui->subwindow_checkandwrite->hide();
    ui->subwindow_SQLmanager->hide();
}

void MainWindow::action6_slot()
{
    ui->action1->setChecked( 0 );
    ui->action2->setChecked( 0 );
    ui->action3->setChecked( 0 );
    ui->action4->setChecked( 0 );
    ui->action5->setChecked( 0 );
    ui->action7->setChecked( 0 );
    ui->action8->setChecked( 0 );
    ui->actionFit->setChecked( 1 );
    ui->actionDataBase->setChecked( 0 );
    ui->subwindow_General_model->hide();
    ui->subwindow_upload_data->hide();
    ui->subwindow_optimization->hide();
    ui->subwindow_ChemicalSetup->hide();
    ui->subwindow_measdata->hide();
    ui->subwindow_statistics->showMaximized();
    ui->subwindow_Printing->hide();
    ui->subwindow_checkandwrite->hide();
    ui->subwindow_SQLmanager->hide();
}

void MainWindow::action7_slot()
{
    ui->action1->setChecked( 0 );
    ui->action2->setChecked( 0 );
    ui->action3->setChecked( 0 );
    ui->action4->setChecked( 0 );
    ui->action5->setChecked( 0 );
    ui->action6->setChecked( 0 );
    ui->action8->setChecked( 0 );
    ui->actionFit->setChecked( 1 );
    ui->actionDataBase->setChecked( 0 );
    ui->subwindow_General_model->hide();
    ui->subwindow_upload_data->hide();
    ui->subwindow_optimization->hide();
    ui->subwindow_ChemicalSetup->hide();
    ui->subwindow_measdata->hide();
    ui->subwindow_statistics->hide();
    ui->subwindow_Printing->showMaximized();
    ui->subwindow_checkandwrite->hide();
    ui->subwindow_SQLmanager->hide();
}

void MainWindow::action8_slot()
{
    ui->action1->setChecked( 0 );
    ui->action2->setChecked( 0 );
    ui->action3->setChecked( 0 );
    ui->action4->setChecked( 0 );
    ui->action5->setChecked( 0 );
    ui->action6->setChecked( 0 );
    ui->action7->setChecked( 0 );
    ui->actionFit->setChecked( 1 );
    ui->actionDataBase->setChecked( 0 );
    ui->subwindow_General_model->hide();
    ui->subwindow_upload_data->hide();
    ui->subwindow_optimization->hide();
    ui->subwindow_ChemicalSetup->hide();
    ui->subwindow_measdata->hide();
    ui->subwindow_statistics->hide();
    ui->subwindow_Printing->hide();
    ui->subwindow_checkandwrite->showMaximized();
    ui->subwindow_SQLmanager->hide();
}

void MainWindow::show_upload()
{
    if( ui->comboBox_uploadGEMSinput->currentIndex() == 0 )
        ui->pushButton_uploaddata->show();
    else
        ui->pushButton_uploaddata->hide();
}

void MainWindow::upload_GEMSFIT_input()
{


}

void MainWindow::upload_GEMS_input()
{
    int row, col, id;

    qDebug("inside GEMSFIT_input upload_GEMS_input()");
    string param_file_ = ui->lineEdit_systemname_2->displayText().toStdString();
    string phase_name_ = ui->lineEdit_phasename_2->displayText().toStdString();

    // load parameters fro mGEMS3K input file into parameters struct
    uploadparam( phase_name_, param_file_ );

    // read data from parameters struct and insert them into GUI
    ui->spinBox_nDC->setValue( species.size() );
    ui->spinBox_colsaDCc->setValue( sysparam->cols_aDCc );
    ui->spinBox_colsidtable->setValue( sysparam->cols_aIPx );
    ui->spinBox_colsinterarray->setValue( sysparam->cols_aIPc );
    ui->spinBox_Nintparam->setValue( sysparam->rows_aIPc );

    ui->tableWidget_aDCc->setRowCount( species.size() );
    ui->tableWidget_aDCc->setColumnCount( sysparam->cols_aDCc );

    ui->tableWidget_DC->setRowCount( species.size() );

    ui->tableWidget_aIPxc->setRowCount( sysparam->rows_aIPc );
    ui->tableWidget_aIPxc->setColumnCount( sysparam->cols_aIPx + sysparam->cols_aIPc );


    // copy values from GEMS input file to aDCc table
    for( row=0; row<ui->tableWidget_aDCc->rowCount(); row++ )
    {
            for( col=0; col<ui->tableWidget_aDCc->columnCount(); col++ )
            {
                id = row * sysparam->cols_aDCc + col;
                ui->tableWidget_aDCc->setItem(row, col, new QTableWidgetItem( QString::fromStdString( sysparam->aDCc[ id ] ) ) );
            }
    }

    // copy values from GEMS input file to DC table
    for( row=0; row<species.size(); row++ )
    {
        ui->tableWidget_DC->setItem(row, 0, new QTableWidgetItem( QString::fromStdString( species.at( row ) ) ) );
    }

    // copy values from GEMS input file to aIPxc table
    int id_aIPc=0, id_aIPx=0;

    for( row=0; row<ui->tableWidget_aIPxc->rowCount(); row++ )
    {
            for( col=0; col<ui->tableWidget_aIPxc->columnCount(); col++ )
            {
                id = row * (sysparam->cols_aIPx + sysparam->cols_aIPc)  + col;

                if( col<sysparam->cols_aIPx )
                {
  //                  cout<<" sysparam->aIPx[ "<<id_aIPx<<" ] = "<<sysparam->aIPx[ id_aIPx ]<<endl;
                    ui->tableWidget_aIPxc->setItem(row, col, new QTableWidgetItem( QString::fromStdString( species.at( atoi(sysparam->aIPx[ id_aIPx ].c_str()) ) ) ) );
                    id_aIPx++;
                }else if( col >= sysparam->cols_aIPx )
                {
                    ui->tableWidget_aIPxc->setItem(row, col, new QTableWidgetItem( QString::fromStdString( sysparam->aIPc[ id_aIPc ] ) ) );
                    id_aIPc++;
                }
            }
    }

    ui->lineEdit_systemname->setText( QString::fromStdString( param_file ) );

    ui->lineEdit_phasename->setText( QString::fromStdString( phase_name ) );

    ui->comboBox_activitymodel->setCurrentIndex( activity_model );

}


void MainWindow::write_config_file()
{

    QFile ffile( "GEMSFIT_input_a.dat" );
    if( ffile.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        QTextStream out(&ffile);
        out << ui->textEdit_seeconfig->toPlainText();
    }

}


void MainWindow::update_rows_bounds()
{
//	qDebug("inside slot");
    int row, col;

    ui->tableWidget_bounds->setRowCount( ui->spinBox_nFitParam->value() );

 /*   if( ui->tableWidget_bounds->rowCount() > 0 )
    {
        for( row=0; row<ui->tableWidget_bounds->rowCount(); row++ )
        {
            for( col=0; col<ui->tableWidget_bounds->columnCount(); col++ )
            {
                ui->tableWidget_bounds->setItem(row, col, new QTableWidgetItem( QString::number(0) ) );
            }
        }
    }
*/
}

void MainWindow::update_rows_constraints()
{
//	qDebug("inside slot");
    int row, col;

    ui->tableWidget_constraints->setRowCount( ui->spinBox_nConstraints->value() );

    if( ui->tableWidget_constraints->rowCount() > 0 )
    {
        for( row=0; row<ui->tableWidget_constraints->rowCount(); row++ )
        {
            for( col=0; col<ui->tableWidget_constraints->columnCount(); col++ )
            {
                ui->tableWidget_constraints->setItem(row, col, new QTableWidgetItem( QString::number(0) ) );
            }
        }
    }


}

void MainWindow::update_rows_print_meas_comp()
{
//	qDebug("inside slot");
    int row, col;
    ui->tableWidget_PrintPoints->setRowCount( ui->spinBox_nPrint->value() );

    if( ui->tableWidget_PrintPoints->rowCount() > 0 )
    {
        for( row=0; row<ui->tableWidget_PrintPoints->rowCount(); row++ )
        {
            for( col=0; col<ui->tableWidget_PrintPoints->columnCount(); col++ )
            {
               ui->tableWidget_PrintPoints->setItem(row, col, new QTableWidgetItem( QString::number(0) ) );
            }
        }
    }

}


void MainWindow::write_check_config_file()
{
    unsigned int i, j;

    // remove text
    ui->textEdit_seeconfig->clear();

    // add new configuration information
    ui->textEdit_seeconfig->append(  " # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # " );
    ui->textEdit_seeconfig->append(  " - - - - - - - - - - - - - GEMSFIT CONFIGURATION FILE -  - - - - - - - " );
    ui->textEdit_seeconfig->append(  " # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # " );


    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  " # DataManager: get measurement data from CSV file (0) or PostgreSQL database (1)" );
    ui->textEdit_seeconfig->append(  " <DatSource>" );
    ui->textEdit_seeconfig->append(  QString::number(optstat->data_source) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  " # Optimization: call TSolMod wrapper (0) or perform full equilibrium computation with GEMS3K (1)" );
    ui->textEdit_seeconfig->append(  " <OptEqSolv>" );
    ui->textEdit_seeconfig->append(  QString::number(optstat->fitmode) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# DataManager: if <DatSource> == 0 then enter name of CSV file" );
    ui->textEdit_seeconfig->append(  "<DatCSVfile>" );
    ui->textEdit_seeconfig->append(  QString::fromStdString(optstat->csv_file) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# DataManager: username" );
    ui->textEdit_seeconfig->append(  "<DatUsername>" );
    ui->textEdit_seeconfig->append(  QString::fromStdString(optstat->username) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# DataManager: password" );
    ui->textEdit_seeconfig->append(  "<DatPasswd>" );
    ui->textEdit_seeconfig->append(  QString::fromStdString(optstat->passwd) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# DataManager: database name" );
    ui->textEdit_seeconfig->append(  "<DatDB>" );
    ui->textEdit_seeconfig->append(  QString::fromStdString(optstat->dbname) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# DataManager: data table name" );
    ui->textEdit_seeconfig->append(  "<DatTable>" );
    ui->textEdit_seeconfig->append(  QString::fromStdString(optstat->tablename) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Optimization: specify algorithm" );
    ui->textEdit_seeconfig->append(  "<OptAlgo>" );
    ui->textEdit_seeconfig->append(  QString::number(optstat->algo) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Optimization: Number of threads (default=1)" );
    ui->textEdit_seeconfig->append(  "<OptThreads>" );
    ui->textEdit_seeconfig->append(  QString::number(optstat->nodes) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# name of the system that will be fitted" );
    ui->textEdit_seeconfig->append(  "<SystemName>" );
    ui->textEdit_seeconfig->append(  QString::fromStdString(param_file) + "-lst.dat" );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# name of the phase that will be fitted" );
    ui->textEdit_seeconfig->append(  "<PhaseName>" );
    ui->textEdit_seeconfig->append(  QString::fromStdString(phase_name) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Dependent components names" );
    ui->textEdit_seeconfig->append(  "<DepComp>" );
    for( i=0; i<species.size(); i++ )
        ui->textEdit_seeconfig->append(  QString::fromStdString(species.at(i)) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Dependent components charges" );
    ui->textEdit_seeconfig->append(  "<DepCompCharge>" );
    for( i=0; i<charges.size(); i++ )
        ui->textEdit_seeconfig->append(  QString::number(charges.at(i)) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Name of activity model" );
    ui->textEdit_seeconfig->append(  "<ActivityModel>" );
    ui->textEdit_seeconfig->append(  ui->comboBox_activitymodel->currentText() );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# LsMod:	description of sizes of aIDc, aIPc and aIPx arrays" );
    ui->textEdit_seeconfig->append(  "<LsMod>" );
    QString LsMod;
    LsMod = QString::number(sysparam->rows_aIPc) + " " + QString::number(sysparam->cols_aIPx) + " " +  QString::number(sysparam->cols_aIPc);
    ui->textEdit_seeconfig->append( LsMod );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# IPxPH:  Collected indexation table for interaction parameters of non-ideal solutions." );
    ui->textEdit_seeconfig->append(  "<IPxPH>" );
    QString aipx;
    for( i=0; i<sysparam->rows_aIPc; i++ )
    {
        aipx.clear();
        for( j=0; j<sysparam->cols_aIPx; j++ )
        {
            aipx = aipx + " " + QString::fromStdString( sysparam->aIPx.at( i * sysparam->cols_aIPx + j )) + " ";
        }
        ui->textEdit_seeconfig->append( aipx );
    }

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# PMc: Collected interaction parameter coefficients for the (built-in) non-ideal mixing models" );
    ui->textEdit_seeconfig->append(  "<PMc>" );
    QString aipc, fit_aipc;
    QString F("F");
    for( i=0; i<sysparam->rows_aIPc; i++ )
    {
        aipc.clear();
        for( j=0; j<sysparam->cols_aIPc; j++ )
        {
            fit_aipc = QString::fromStdString( sysparam->aIPc.at( i * sysparam->cols_aIPc + j ));

            if( ui->tableWidget_aIPxc->item( i, j)->backgroundColor() == Qt::yellow )
            {
                fit_aipc = F + fit_aipc;
            }
            aipc = aipc + " " + fit_aipc + " ";
        }
        ui->textEdit_seeconfig->append( aipc );
    }


    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# DMc: Collected parameters per phase component for the non-ideal mixing models" );
    ui->textEdit_seeconfig->append(  "<DMc>" );
    QString adcc, fit_adcc;
    for( i=0; i<species.size(); i++ )
    {
        adcc.clear();
        for( j=0; j<sysparam->cols_aDCc; j++ )
        {
            fit_adcc = QString::fromStdString( sysparam->aDCc.at( i * sysparam->cols_aDCc + j ));

            if( ui->tableWidget_aDCc->item( i, j)->backgroundColor() == Qt::yellow )
            {
                fit_adcc = F + fit_adcc;
            }
            adcc = adcc + " " + fit_adcc + " ";
        }
        ui->textEdit_seeconfig->append( adcc );
    }

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# LsMod: Dimensions of <IPxPH> and <PMc> arrays [nPS*3]" );
    ui->textEdit_seeconfig->append(  "<LsMdc>" );
    ui->textEdit_seeconfig->append(  QString::number(sysparam->cols_aDCc) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Optimization: specify vector oupper and lower bounds" );
    ui->textEdit_seeconfig->append(  "<OptUpBounds>" );
   for( i=0; i<sysparam->bound_up.size(); i++ )
        ui->textEdit_seeconfig->append(  QString::number(sysparam->bound_up.at(i)) );

    ui->textEdit_seeconfig->append(  "<OptLoBounds>" );
   for( i=0; i<sysparam->bound_lo.size(); i++ )
        ui->textEdit_seeconfig->append(  QString::number(sysparam->bound_lo.at(i)) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Optimization: apply constraints (1=yes, 0=no)" );
    ui->textEdit_seeconfig->append(  "<OptConstraints>" );
    ui->textEdit_seeconfig->append(  QString::number(sysparam->constraint_bool) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Optimization: specify parameter vectors for constraint function" );
    ui->textEdit_seeconfig->append(  "<OptUpConstraints>" );
    for( i=0; i<sysparam->constraint_up.size(); i++ )
        ui->textEdit_seeconfig->append(  QString::number(sysparam->constraint_up.at(i)) );

    ui->textEdit_seeconfig->append(  "<OptLoConstraints>" );
    for( i=0; i<sysparam->constraint_lo.size(); i++ )
        ui->textEdit_seeconfig->append(  QString::number(sysparam->constraint_lo.at(i)) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Optimization: specify tolerance (default = 1e-04)" );
    ui->textEdit_seeconfig->append(  "<OptTol>" );
    ui->textEdit_seeconfig->append(  QString::number(optstat->tolerance) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Optimization: specify max number of evaluations" );
    ui->textEdit_seeconfig->append(  "<OptMaxEval>" );
    ui->textEdit_seeconfig->append(  QString::number(optstat->maxeval) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Optimization: perform optimization and statistics (0), only optimization with basic Statistics (1), only Statistics (2) with initial guesses as best fit parametters" );
    ui->textEdit_seeconfig->append(  "<OptDoWhat>" );
    ui->textEdit_seeconfig->append(  QString::number(optstat->perform) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Statistics: perform Monte Carlo runs -> yes (1)/no (0)" );
    ui->textEdit_seeconfig->append(  "<StatMCbool>" );
    ui->textEdit_seeconfig->append(  QString::number(optstat->MC) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Statistics: number of Monte Carlo runs for confidence interval generation" );
    ui->textEdit_seeconfig->append(  "<StatMCruns>" );
    ui->textEdit_seeconfig->append(  QString::number(optstat->MCruns) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Statistics: number of bars for printing results from Monte Carlo confidence interval generation" );
    ui->textEdit_seeconfig->append(  "<StatMCbars>" );
    ui->textEdit_seeconfig->append(  QString::number(optstat->MCbars) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Statistics: number of evaluations points per parameter for sensitivity evaluation" );
    ui->textEdit_seeconfig->append(  "<StatSensitivity>" );
    ui->textEdit_seeconfig->append(  QString::number(optstat->SSRpoints) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Statistics: number of function evaluations per axis (total value will be StatSSRSensitivity squared)" );
    ui->textEdit_seeconfig->append(  "<StatSSRSensitivity>" );
    ui->textEdit_seeconfig->append(  QString::number(optstat->SSRplotpoints) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "# Printing information: specifiy which datapoints to print to file and format of output file" );
    ui->textEdit_seeconfig->append(  "<PrintTemperatures>" );
    for( i=0; i<printinfo->meascomp_temp.size(); i++ )
        ui->textEdit_seeconfig->append(  QString::fromStdString( printinfo->meascomp_temp[ i ] ) );

    ui->textEdit_seeconfig->append(  "<PrintPressures>" );
    for( i=0; i<printinfo->meascomp_pres.size(); i++ )
        ui->textEdit_seeconfig->append(  QString::fromStdString( printinfo->meascomp_pres[ i ] ) );

    ui->textEdit_seeconfig->append(  "<PrintMolalities>" );
    for( i=0; i<printinfo->meascomp_mollo.size(); i++ )
    {
          ui->textEdit_seeconfig->append( QString::fromStdString( printinfo->meascomp_mollo.at( i )) + " - " + QString::fromStdString( printinfo->meascomp_molup.at( i )) );
    }

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "<PrintFormat>" );
    ui->textEdit_seeconfig->append(  QString::fromStdString(printinfo->meascomp_fileformat) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "<PrintFilename>" );
    ui->textEdit_seeconfig->append(  QString::fromStdString(printinfo->meascomp_filename) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "<PrintMeasValueCode>" );
    ui->textEdit_seeconfig->append(  QString::number( printinfo->meascomp_valcode ) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "<PrintLabelXaxis>" );
    ui->textEdit_seeconfig->append(  QString::fromStdString(printinfo->meascomp_labelX) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "<PrintLabelYaxis>" );
    ui->textEdit_seeconfig->append(  QString::fromStdString(printinfo->meascomp_labelY) );

    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "<PrintHead>" );
    ui->textEdit_seeconfig->append(  QString::fromStdString(printinfo->meascomp_headline) );


    ui->textEdit_seeconfig->append(  " " );
    ui->textEdit_seeconfig->append(  "#" );
}


void MainWindow::check_new_parameters()
{
    int i;

    // optimization general
    optstat->fitmode   = ui->comboBox_fitmode->currentIndex();

    optstat->perform   = ui->comboBox_perform->currentIndex();

    optstat->algo      = ui->comboBox_algo->currentIndex();

    optstat->nodes     = ui->spinBox_nodes->value();

    optstat->tolerance = ui->lineEdit_tol->text().toDouble();

    optstat->maxeval   = ui->lineEdit_neval->text().toDouble();

    // optimization bounds and constraints
    sysparam->constraint_bool = 0;
    if( ui->spinBox_nConstraints->value() > 0 )
        sysparam->constraint_bool = 1;

    sysparam->bound_lo.clear();
    sysparam->bound_up.clear();
    sysparam->constraint_lo.clear();
    sysparam->constraint_up.clear();

    sysparam->bound_lo.resize( ui->spinBox_nFitParam->value() );
    sysparam->bound_up.resize( ui->spinBox_nFitParam->value() );
    sysparam->constraint_lo.resize( ui->spinBox_nConstraints->value() );
    sysparam->constraint_up.resize( ui->spinBox_nConstraints->value() );

    for( i=0; i<ui->spinBox_nFitParam->value(); i++ )
    {
        sysparam->bound_lo[ i ] = ui->tableWidget_bounds->item( i, 0)->text().toDouble();
        sysparam->bound_up[ i ] = ui->tableWidget_bounds->item( i, 1 )->text().toDouble();
    }

    for( i=0; i<ui->spinBox_nConstraints->value(); i++ )
    {
        sysparam->constraint_lo[ i ] = ui->tableWidget_constraints->item( i, 0 )->text().toDouble();
        sysparam->constraint_up[ i ] = ui->tableWidget_constraints->item( i, 1 )->text().toDouble();
    }

    //statistics
    optstat->MC = ui->checkBox_MC->isChecked();

    optstat->MCruns = ui->spinBox_MCruns->value();

    optstat->MCbars = ui->spinBox_MCbars->value();

    optstat->SSRpoints = ui->spinBox_SSRpoints->value();

    optstat->SSRplotpoints = ui->spinBox_SSRplotpoints->value();

    // Measurement data
    optstat->data_source = ui->comboBox_csvsql->currentIndex();

    optstat->csv_file  = ui->lineEdit_csvfile->text().toStdString();

    optstat->dbname    = ui->lineEdit_dbname->text().toStdString();

    optstat->passwd    = ui->lineEdit_password->text().toStdString();

    optstat->username  = ui->lineEdit_username->text().toStdString();

    optstat->tablename = ui->lineEdit_tablename->text().toStdString();

    //Printing information
    printinfo->meascomp_filename = ui->lineEdit_meascomp_filename->text().toStdString();

    printinfo->meascomp_fileformat = ui->comboBox_meascomp_fileformat->currentText().toStdString();

    printinfo->meascomp_valcode  = ui->spinBox_meascomp_valcode->value();

    printinfo->meascomp_headline = ui->lineEdit_meascomp_headline->text().toStdString();

    printinfo->meascomp_labelX   = ui->lineEdit_meascomp_labelX->text().toStdString();

    printinfo->meascomp_labelY   = ui->lineEdit_meascomp_labelY->text().toStdString();

    printinfo->meascomp_temp.clear();
    printinfo->meascomp_temp.resize( ui->spinBox_nPrint->value() );
    printinfo->meascomp_pres.clear();
    printinfo->meascomp_pres.resize( ui->spinBox_nPrint->value() );
    printinfo->meascomp_mollo.clear();
    printinfo->meascomp_mollo.resize( ui->spinBox_nPrint->value() );
    printinfo->meascomp_molup.clear();
    printinfo->meascomp_molup.resize( ui->spinBox_nPrint->value() );

    for( i=0; i<printinfo->meascomp_mollo.size(); i++ )
    {
        printinfo->meascomp_temp[ i ]    = ui->tableWidget_PrintPoints->item( i, 0 )->text().toStdString();
        printinfo->meascomp_pres[ i ]    = ui->tableWidget_PrintPoints->item( i, 1 )->text().toStdString();
        printinfo->meascomp_mollo[ i ]    = ui->tableWidget_PrintPoints->item( i, 2 )->text().toStdString();
        printinfo->meascomp_molup[ i ]    = ui->tableWidget_PrintPoints->item( i, 3 )->text().toStdString();
    }




    // Write test config file to textEdit ui
    write_check_config_file();

}


void MainWindow::csv_or_sql()
{
    if( ui->comboBox_csvsql->currentIndex() == 0 )
    {
        ui->stackedWidget_csvsql->setCurrentIndex( 0 );
    }else if( ui->comboBox_csvsql->currentIndex() == 1 )
    {
        ui->stackedWidget_csvsql->setCurrentIndex( 1 );
    }

}


void MainWindow::find_clicked_param_aDCc()
{
    int row, col, current_row_in_bounds=0;
    row = ui->tableWidget_aDCc->currentRow();
    col = ui->tableWidget_aDCc->currentColumn();

    if( ui->tableWidget_aDCc->currentItem()->backgroundColor() != Qt::yellow )
    {
        ui->tableWidget_aDCc->currentItem()->setBackgroundColor( Qt::yellow );
        ui->tableWidget_bounds->insertRow( current_row_in_bounds );
        ui->spinBox_nFitParam->setValue( ui->tableWidget_bounds->rowCount() );
    }
    else
    {
        ui->tableWidget_aDCc->currentItem()->setBackgroundColor( Qt::white );
        ui->tableWidget_bounds->removeRow( current_row_in_bounds );
        ui->spinBox_nFitParam->setValue( ui->tableWidget_bounds->rowCount() );
    }
}


void MainWindow::find_clicked_param_aIPxc()
{
    int row, col, current_row_in_bounds=0;
    row = ui->tableWidget_aIPxc->currentRow();
    col = ui->tableWidget_aIPxc->currentColumn();

    find_id_fit_param_in_bounds( current_row_in_bounds, 0, row, col );

    if( ui->tableWidget_aIPxc->currentItem()->backgroundColor() != Qt::yellow )
    {
        ui->tableWidget_aIPxc->currentItem()->setBackgroundColor( Qt::yellow );
        ui->tableWidget_bounds->insertRow( current_row_in_bounds );
        ui->spinBox_nFitParam->setValue( ui->tableWidget_bounds->rowCount() );
    }
    else
    {
        ui->tableWidget_aIPxc->currentItem()->setBackgroundColor( Qt::white );
        ui->tableWidget_bounds->removeRow( current_row_in_bounds );
        ui->spinBox_nFitParam->setValue( ui->tableWidget_bounds->rowCount() );
    }
}

void MainWindow::run_GEMSFIT()
{
    QString program = "mpirun ";
    QStringList arguments;
    arguments << "-np " << QString::number(ui->spinBox_nodes->value()) << " gemsfit &";

    QProcess myProcess;
    myProcess.start(program, arguments);
}


void MainWindow::set_cell_border()
{
    int row=0, col=0, tabl=0;

    find_id_fit_param( tabl, row, col );

    if( !tabl )
    {
        ui->tableWidget_aIPxc->item( row, col )->setBackgroundColor( Qt::green );
    }
    else
    {
        ui->tableWidget_aDCc->item( row, col )->setBackgroundColor( Qt::green );
    }

}



void MainWindow::find_id_fit_param( int &tabl, int &row, int &col )
{
    vector<int> rows_fit_aIPc, cols_fit_aIPc, rows_fit_aDCc, cols_fit_aDCc;
    int id_fit_param, i, j;

    id_fit_param = ui->tableWidget_bounds->currentRow();

    cout<<"id_fit_param = "<<id_fit_param<<endl;

    for( i=0; i<ui->tableWidget_aIPxc->rowCount(); i++)
    {
        for( j=0; j<ui->tableWidget_aIPxc->columnCount(); j++)
        {
            if( ui->tableWidget_aIPxc->item( i, j )->backgroundColor() == Qt::green )
            {
                ui->tableWidget_aIPxc->item( i, j )->setBackgroundColor( Qt::yellow );
            }
            if( ui->tableWidget_aIPxc->item( i, j )->backgroundColor() == Qt::yellow )
            {
                rows_fit_aIPc.push_back( i );
                cols_fit_aIPc.push_back( j );
                cout<<"rows_fit_aIPc = "<<rows_fit_aIPc.at( rows_fit_aIPc.size()-1 ) << endl;
                cout<<"cols_fit_aIPc = "<<cols_fit_aIPc.at( cols_fit_aIPc.size()-1 ) << endl;
            }
        }
    }

    for( i=0; i<ui->tableWidget_aDCc->rowCount(); i++)
    {
        for( j=0; j<ui->tableWidget_aDCc->columnCount(); j++)
        {
            if( ui->tableWidget_aDCc->item( i, j )->backgroundColor() == Qt::green )
            {
                ui->tableWidget_aDCc->item( i, j )->setBackgroundColor( Qt::yellow );
            }
            if( ui->tableWidget_aDCc->item( i, j )->backgroundColor() == Qt::yellow )
            {
                rows_fit_aDCc.push_back( i );
                cols_fit_aDCc.push_back( j );
                cout<<"rows_fit_aDCc = "<<rows_fit_aDCc.at( rows_fit_aDCc.size()-1 ) << endl;
                cout<<"cols_fit_aDCc = "<<cols_fit_aDCc.at( cols_fit_aDCc.size()-1 ) << endl;
            }
        }
    }

    if( id_fit_param < rows_fit_aIPc.size() )
    {
        row  = rows_fit_aIPc.at( id_fit_param );
        col  = cols_fit_aIPc.at( id_fit_param );
        tabl = 0;
    }
    else
    {
        row  = rows_fit_aDCc.at( id_fit_param - rows_fit_aIPc.size() );
        col  = cols_fit_aDCc.at( id_fit_param - rows_fit_aIPc.size() );
        tabl = 1;
        cout<<"876 | row = "<<row<<" | col = "<<col<<endl;
    }

    cout<<"tabl = "<< tabl << " | row = "<<row<<" | col = "<<col<<endl;

}


// IN: aIPc (tabl=0) or aDCc (tabl=1), row and col in aDCc/aIPc . OUT: corresponding row in bounds table
void MainWindow::find_id_fit_param_in_bounds( int &id, const int tabl, const int row, const int col )
{
    vector<int> rows_fit_aIPc, cols_fit_aIPc, rows_fit_aDCc, cols_fit_aDCc;
    int id_fit_param, i, j, k;

    if( tabl==0 )
    {
        id_fit_param = ui->tableWidget_bounds->currentRow();
    }

    cout<<"id_fit_param = "<<id_fit_param<<endl;

    for( i=0; i<ui->tableWidget_aIPxc->rowCount(); i++)
    {
        for( j=0; j<ui->tableWidget_aIPxc->columnCount(); j++)
        {
            if( ui->tableWidget_aIPxc->item( i, j )->backgroundColor() == Qt::yellow || ui->tableWidget_aIPxc->item( i, j )->backgroundColor() == Qt::green )
            {
                rows_fit_aIPc.push_back( i );
                cols_fit_aIPc.push_back( j );
                cout<<"rows_fit_aIPc = "<<rows_fit_aIPc.at( rows_fit_aIPc.size()-1 ) << endl;
                cout<<"cols_fit_aIPc = "<<cols_fit_aIPc.at( cols_fit_aIPc.size()-1 ) << endl;
            }
        }
    }

    for( i=0; i<ui->tableWidget_aDCc->rowCount(); i++)
    {
        for( j=0; j<ui->tableWidget_aDCc->columnCount(); j++)
        {
            if( ui->tableWidget_aDCc->item( i, j )->backgroundColor() == Qt::yellow || ui->tableWidget_aDCc->item( i, j )->backgroundColor() == Qt::green )
            {
                rows_fit_aDCc.push_back( i );
                cols_fit_aDCc.push_back( j );
                cout<<"rows_fit_aDCc = "<<rows_fit_aDCc.at( rows_fit_aDCc.size()-1 ) << endl;
                cout<<"cols_fit_aDCc = "<<cols_fit_aDCc.at( cols_fit_aDCc.size()-1 ) << endl;
            }
        }
    }

    k=0;
    for( i=0; i<rows_fit_aIPc.size(); i++ )
    {
        if( rows_fit_aIPc[ i ]==row && cols_fit_aIPc[ i ]==col )
        {    id = k; }
        k++;
    }

    for( i=0; i<rows_fit_aDCc.size(); i++ )
    {
        if( rows_fit_aDCc[i]==row && cols_fit_aDCc[j]==col )
        {    id = k; }
        k++;
    }

}


void MainWindow::action_DB_mode_slot()
{
    ui->action1->setChecked( 0 );
    ui->action2->setChecked( 0 );
    ui->action3->setChecked( 0 );
    ui->action4->setChecked( 0 );
    ui->action5->setChecked( 0 );
    ui->action6->setChecked( 0 );
    ui->action7->setChecked( 0 );
    ui->action8->setChecked( 0 );
    ui->actionFit->setChecked( 0 );
    ui->subwindow_General_model->hide();
    ui->subwindow_upload_data->hide();
    ui->subwindow_optimization->hide();
    ui->subwindow_ChemicalSetup->hide();
    ui->subwindow_measdata->hide();
    ui->subwindow_statistics->hide();
    ui->subwindow_Printing->hide();
    ui->subwindow_checkandwrite->hide();
    ui->subwindow_SQLmanager->showMaximized();
    ui->tabWidget_DB->setCurrentIndex(0);
    ui->groupBox_SQL->hide();
}

void MainWindow::establish_DB_connection()
{
    driver = ui->comboDriver->currentText();
    dbName = ui->editDatabase->text();
    host   = ui->editHostname->text();
    user   = ui->editUsername->text();
    passwd = ui->editPassword->text();
    port   = ui->portSpinBox->value();

    addConnection( driver, dbName, host, user, passwd, port);

    // update database structure in widget
    refreshDB();
}

// Some general functions needed for dealing with the database treewidget
static void qSetBold(QTreeWidgetItem *item, bool bold)
{
    QFont font = item->font(0);
    font.setBold(bold);
    item->setFont(0, font);
}

static QString qDBCaption(const QSqlDatabase &db)
{
    QString nm = db.driverName();
    nm.append(QLatin1Char(':'));
    if (!db.userName().isEmpty())
        nm.append(db.userName()).append(QLatin1Char('@'));
    nm.append(db.databaseName());
    return nm;
}
//


void MainWindow::refreshDB()
{
    ui->treeWidget->clear();
    QStringList connectionNames = QSqlDatabase::connectionNames();

    bool gotActiveDb = false;
    for (int i = 0; i < connectionNames.count(); ++i) {
        QTreeWidgetItem *root = new QTreeWidgetItem(ui->treeWidget);
        QSqlDatabase db = QSqlDatabase::database(connectionNames.at(i), false);
        root->setText(0, qDBCaption(db));
        if (connectionNames.at(i) == activeDb) {
            gotActiveDb = true;
            setActive(root);
        }
        if (db.isOpen()) {
            QStringList tables = db.tables();
            for (int t = 0; t < tables.count(); ++t) {
                QTreeWidgetItem *table = new QTreeWidgetItem(root);
                table->setText(0, tables.at(t));
            }
        }
    }
    if (!gotActiveDb) {
        activeDb = connectionNames.value(0);
        setActive(ui->treeWidget->topLevelItem(0));
    }

    ui->treeWidget->doItemsLayout(); // HACK
}

void MainWindow::setActive(QTreeWidgetItem *item)
{
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        if (ui->treeWidget->topLevelItem(i)->font(0).bold())
            qSetBold(ui->treeWidget->topLevelItem(i), false);
    }

    if (!item)
        return;

    qSetBold(item, true);
    activeDb = QSqlDatabase::connectionNames().value(ui->treeWidget->indexOfTopLevelItem(item));
}

/*void MainWindow::updateActions()
{
    bool enableIns = qobject_cast<QSqlTableModel *>(table->model());
    bool enableDel = enableIns && table->currentIndex().isValid();

    insertRowAction->setEnabled(enableIns);
    deleteRowAction->setEnabled(enableDel);
}*/


QSqlDatabase MainWindow::currentDatabase() const
{
    return QSqlDatabase::database(activeDb);
}


void MainWindow::showTable()
{
    QString table = ui->treeWidget->currentItem()->text(0);

    cout << "table = " << table.toStdString() << endl;

    // QSqlTableModel *model = new QSqlTableModel( ui->tableview_DB, currentDatabase() );
    model = new QSqlTableModel( ui->tableview_DB, currentDatabase() );

    // Transfer edited values from inside view to database only after manual submitting them
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    // Set table
    model->setTable(currentDatabase().driver()->escapeIdentifier(table, QSqlDriver::TableName));

    // Select table
    model->select();

    // Associate SQL table with tableView
    ui->tableview_DB->setModel( model );

    // Resize columns and rows to fit contents
    ui->tableview_DB->resizeColumnsToContents();
    ui->tableview_DB->resizeRowsToContents();

    // Display SQL table content in tableview
    ui->tableview_DB->show();

    ui->tableview_DB->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);


}


void MainWindow::submitall_DB()
{
    model->submitAll();
}

void MainWindow::clearall_DB()
{
    model->revertAll();
}


// Insert row into DB table
void MainWindow::insertrow_DB()
{
    QModelIndex insertIndex = ui->tableview_DB->currentIndex();
    int row = insertIndex.row() == -1 ? 0 : insertIndex.row();
    model->insertRow(row);
    insertIndex = model->index(row, 0);
    ui->tableview_DB->setCurrentIndex(insertIndex);
    ui->tableview_DB->edit(insertIndex);
}


// Remove row into DB table
void MainWindow::removerow_DB()
{
    QModelIndexList currentSelection = ui->tableview_DB->selectionModel()->selectedIndexes();
    for( int i = 0; i < currentSelection.count(); ++i )
    {
        QMessageBox msgBox;
        QString row_del = QString::number( currentSelection.at(i).row() + 1 );
        QString text("You chose to delete row number " + row_del );
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(text);
        msgBox.setInformativeText("Are you sure?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();

        switch (ret)
        {
           case QMessageBox::Yes:
                model->removeRow(currentSelection.at(i).row());
                break;
           case QMessageBox::Cancel:
                // Don't delete
                break;
        }
    }
}

void MainWindow::ShowContextMenu(const QPoint& pos)
{
    // for most widgets
       QPoint globalPos = ui->tableview_DB->mapToGlobal(pos);
       // for QAbstractScrollArea and derived classes you would use:
       // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

       QMenu myMenu;
       myMenu.addAction("Insert row");
       myMenu.addAction("Delete row");

       QAction* selectedItem = myMenu.exec(globalPos);
       if( selectedItem->text() == "Insert row" )
       {
            insertrow_DB();
       }
       else if( selectedItem->text() == "Delete row" )
       {
           removerow_DB();
       }
       else
       { /*do nothing*/ }
}

void MainWindow::create_act_model_data_table()
{

    QString create_table = " CREATE TABLE test_meas_ft ( "
        " id character varying(20), "
        " temperature double precision, "
        " pressure double precision, "
        " molality_1 double precision, "
        " molality_2 double precision DEFAULT 0.0, "
        " code integer DEFAULT 1, "
        " val double precision, "
        " other_1 double precision, "
        " other_2 double precision, "
        " other_3 double precision, "
        " other_4 double precision, "
        " error double precision, "
        " species_1_1 character varying(20) DEFAULT '''Ca+2'''::character varying, "
        " species_1_2 character varying(20) DEFAULT '''Cl-'''::character varying, "
        " species_1_3 character varying(20), "
        " species_1_4 character varying(20), "
        " species_2_1 character varying(20) DEFAULT 0, "
        " species_2_2 character varying(20) DEFAULT 0, "
        " species_2_3 character varying(20), "
        " species_2_4 character varying(20), "
        " stoic_1_1 integer DEFAULT 1, "
        " stoic_1_2 integer DEFAULT 2, "
        " stoic_1_3 integer, "
        " stoic_1_4 integer, "
        " stoic_2_1 integer, "
        " stoic_2_2 integer, "
        " stoic_2_3 integer, "
        " stoic_2_4 integer, "
        " ende serial NOT NULL "
    "  ); ";

    //       "     \"Reference\" text, "
    //       "     CONSTRAINT cacl2meas_pkey PRIMARY KEY (ende) "

    qPrintable( create_table );

    cout << create_table.toStdString() << endl;

    QSqlQuery query("", currentDatabase() );
//    query.prepare("UPDATE adcc_old SET q = '555' WHERE r = '2.0'; ");
    query.prepare( create_table );

    // update database structure in treewidget
    refreshDB();

    if( !query.exec() )
    {
        QMessageBox::critical(0, "Sql ERROR",
            QString("Sql error appeared!<br><b>Error: </b> %1")
            .arg( query.lastError().text() ) );
    }

/*    QSqlQuery querys("", currentDatabase() );
    querys.prepare("SELECT q from adcc_old;");
    querys.exec();
    QString q;
    while ( querys.next() ) {
        q = querys.value(0).toString();
        cout << "q = " << q.toStdString() << endl;
    }*/


    // update database structure in treewidget
    refreshDB();

}


void MainWindow::execSQL()
{
/*
    QSqlQueryModel *model = new QSqlQueryModel(table);
    model->setQuery( QSqlQuery(sqlEdit->toPlainText(), connectionWidget->currentDatabase()) );
    table->setModel( model );

    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());
    else if (model->query().isSelect())
        emit statusMessage(tr("Query OK."));
    else
        emit statusMessage(tr("Query OK, number of affected rows: %1").arg(
                           model->query().numRowsAffected()));
*/


    QSqlQueryModel *model = new QSqlQueryModel;
    model->setQuery( QSqlQuery(ui->sqlEdit->toPlainText(), currentDatabase()) );
    model->setHeaderData(0, Qt::Horizontal, tr("r"));
    model->setHeaderData(1, Qt::Horizontal, tr("q"));

    if( model->lastError().type() != QSqlError::NoError )
    {
        QMessageBox::critical(0, "Sql ERROR",
            QString("Sql error appeared!<br><b>Error: </b> %1")
            .arg( model->lastError().text() ) );
    }

    ui->tableview_DB->setModel( model );

    ui->tableview_DB->show();

    // update database structure in treewidget
    refreshDB();


}


void MainWindow::create_SQLscript_dialog()
{
//    dia = new SQLscript(this);
    dia->show();
}

void MainWindow::showSQLtemplate( int idsql )
{
    switch( idsql )
    {
        case 0:
            ui->sqlEdit->append("UPDATE adcc_old SET q = '111' WHERE r = '2.0';");
            break;
        case 1:
            ui->sqlEdit->append("DROP TABLE adcc_old;");
            break;
        case 2:
            ui->sqlEdit->append("CREATE TABLE cacl2_meas_test AS "
                "SELECT * FROM cacl2_meas WHERE temperature < 400.0 AND pressure=1 AND molality_1 < 5.0;");
            break;
        case 3:
            ui->sqlEdit->append("COPY kcl_meas FROM '/home/hingerl/Documents/KCl_meas.csv' USING DELIMITERS ',' WITH CSV HEADER;");
            break;
        case 4:
            ui->sqlEdit->append(" INSERT INTO \"aIPc\" VALUES "
                "(1,'K+','K+','1','0.000000000000001','0','0','0','0','0'),"
                "(2,'K+','Na+','1','0.000000000000001','0','0','0','0','0'); ");
            break;
        case 5:
            ui->sqlEdit->append("TRUNCATE TABLE adcc_old;");
            break;
        case 6:
            ui->sqlEdit->append("CREATE PROCEDURAL LANGUAGE 'plpgsql';");
            break;
        case 7:
            ui->sqlEdit->append("SELECT DISTINCT temperature FROM nacl_meas WHERE pressure=1;");
            break;
        case 8:
            ui->sqlEdit->append("DROP table nacl_meas; DROP FUNCTION update_key_id_nacl_meas() CASCADE;");
            break;
        case 9:
           ui->sqlEdit->append("SELECT COUNT(*) FROM cacl2_meas WHERE pressure = 20.27;");
           break;
        case 10:
           ui->sqlEdit->append("COPY cacl2_meas TO '/home/ferdl/Documents/PostgreSQL/data_export_csv/test.csv' WITH DELIMITER ',' CSV HEADER;");
           break;
        case 11:
        ui->sqlEdit->append("COPY (SELECT * FROM cacl2_meas WHERE temperature = 323.15) TO '/home/ferdl/Documents/PostgreSQL/data_export_csv/test.csv' WITH DELIMITER ',' CSV HEADER;");
           break;
        case 12:
        ui->sqlEdit->append("SELECT DISTINCT (SELECT(temperature - 273.15)) AS Temp, pressure FROM cacl2_meas_revised ORDER BY Temp, pressure;");
           break;

    }


}

