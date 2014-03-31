#ifndef _tmodel_h
#define _tmodel_h

#include <vector>
using namespace std;

#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QLineEdit>
#include <QTableView>

#include <QAbstractItemDelegate>
#include <QFontMetrics>
#include <QModelIndex>
#include <QSize>


#ifdef __APPLE__
const char  splitRow = '\r';
const char  splitCol = '\t';
#else
const char  splitRow = '\n';
const char  splitCol = '\t';
#endif

const string emptiness("---");

struct Selection {
    int N1;
    int N2;
    int M1;
    int M2;

    Selection(int n1, int n2, int m1, int m2 ):
     N1(n1), N2(n2), M1(m1), M2(m2)
     {}

    Selection(const Selection& sel):
     N1(sel.N1), N2(sel.N2), M1(sel.M1), M2(sel.M2)
     {}

};

//===========================================
// TVectorModel class
//===========================================

/*
QVector< QVector<T> > vectors = QVector< QVector<T> >();

int totalSize = 0;
for (int i = 0; i < vectors.size(); ++i)
    totalSize += vectors.at(i).size();

QVector<T> collected;
collected.reserve(totalSize);

for (int i = 0; i < vectors.size(); ++i)
    collected << vectors[i];
*/
/*!
  \ class TMatrixModel
  \ class for represents the data set and is responsible for fetchin
  \ the data is neaded for viewing and for writing back any changes.
  \ Reading/writing data from/to vector classes
  
*/


class TMatrixModel: public QAbstractTableModel
{
	Q_OBJECT
	
    QString fname;
    int numberStringColumns;
    QVector< QString > colHeads;
    QVector< QVector<QVariant> > matrix;

    //QString ValToString( double val ) const;
    //double ValFromString( const QVariant& strval  );


public:
	  
     TMatrixModel( const QString& fname, int aNumCol, QObject * parent = 0 );
	 
	 int rowCount ( const QModelIndex & parent ) const;	
	 int columnCount ( const QModelIndex & parent  ) const;
	 QVariant data ( const QModelIndex & index, int role ) const;
	 bool setData ( const QModelIndex & index, const QVariant & value, int role );
     QVariant headerData ( int section, Qt::Orientation orientation, int role ) const;
     Qt::ItemFlags flags( const QModelIndex & index ) const;

     void defineMode();
     void matrixFromCsvFile( const QString& dir );
     void matrixToCsvFile( const QString& dir );
     void matrixFromCsvString( const QString& valueCsv );
     QString matrixToCsvString( );
};

/*!
  \ class TMatrixTable implements a table view
  \ that displays items from a model. This class is used to
  \ provide standard tables that were previously provided by
  \ the QTable class, but using the more flexible approach
  \ provided by Qt's model/view architecture.
*/

class TMatrixTable: public QTableView
{
        Q_OBJECT

        bool vScroll;
        bool hScroll;

        void focusOutEvent( QFocusEvent * event );
        void focusInEvent( QFocusEvent * event );
        void keyPressEvent(QKeyEvent* e);

        QList<QAction*> itemActions( const QModelIndex & current);

        Selection getSelectionRange( bool paste_ = false );
        QString createString( Selection& sel );
        QString createHeader();
        void pasteIntoArea( Selection& sel, bool transpose);
        void  setFromString(char splitrow, const QString& str,
                Selection sel, bool transpose);

 protected slots:
//    void currentChanged( const QModelIndex& current, const QModelIndex& previous );
    void slotPopupContextMenu(const QPoint& pos);

 public slots:
    void CmHelp();
    void CmCalc();
    void SelectColumn();
    void SelectAll();
    void CutData();
    void ClearData();
    void CopyData();
    void PasteData();
    void PasteTransposedData();

 public:
    TMatrixTable( QWidget * parent = 0 );
};


/*!
  \ class TVectorDelegate
  \ individual items in views are rendered and edited using delegates

*/

class TMatrixDelegate: public QItemDelegate
{
	Q_OBJECT

    int numberStringColumns;

 public:
	
   TMatrixDelegate( int nStringColumn, QObject * parent = 0 );
   QWidget *createEditor(QWidget *parent,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
	                      const QModelIndex &index) const;

};


//===========================================
// TCell... classes
//===========================================


/*!
  \class TCellInput
  \brief elemetary cell - Line Input 
*/
class TCellInput:
            public QLineEdit
{
    Q_OBJECT

    int iN, iM;   // row, col
    bool edited;

protected:
   // void keyPressEvent(QKeyEvent*);

protected slots:
    void setCh() { edited=true; }

public:
    TCellInput(  int in, int im, QWidget * parent = 0 );
    ~TCellInput()
    {}

    virtual QString getData()
    {
        return text();
    }

    virtual void setData( QString data )
    {
        setText( data );
    }

    bool dataCh()
    { return edited; }
};


#endif   // _tmodel_h
