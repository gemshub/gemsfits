#include "sqlscript.h"
#include "ui_sqlscript.h"


SQLscript::SQLscript(QWidget *parent) :
    QDialog(parent), uii(new Ui::SQLscript)
{
    uii->setupUi( this );

    // Show example SQL scripts in sqlEdit
    connect( uii->treeWidget_sqlscripts, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT( dialog_showSQLtemplate( ) ) );


}


SQLscript::~SQLscript()
{


}


void SQLscript::dialog_showSQLtemplate( )
{
    int id_sql;
    QModelIndex id = uii->treeWidget_sqlscripts->currentIndex();
    id_sqlscript = id.row();

    std::cout << "double-clicked row = " << id.row() << std::endl;

    id_sql = id.row();
    emit show_sqltemplate_signal( id_sql );
}
