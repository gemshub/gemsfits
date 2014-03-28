
#include <iostream>
#include <QMenu>
#include <QKeyEvent>
#include <QClipboard>

#include "fmodel.h"
#include "CalcDialog.h"


//--------------------------------------------------------------------------------------
//  class TMatrixModel

TMatrixModel::TMatrixModel( const QString& afname, int aNumCol, QObject * parent ):
    QAbstractTableModel(parent), fname(afname), numberStringColumns(aNumCol)
{
}

int TMatrixModel::rowCount( const QModelIndex & /*parent*/ ) const
{
     return matrix.size();
}	


int TMatrixModel::columnCount( const QModelIndex & /*parent*/ ) const
{
   return colHeads.size();
}	

QVariant TMatrixModel::data( const QModelIndex& index, int role ) const
{
	if(!index.isValid())
	 return QVariant();	

    switch( role )
    {
      case Qt::DisplayRole:
      case Qt::EditRole:
        {
          QVariant val = matrix.at(index.row()).at(index.column());
          if( index.column() >= numberStringColumns )
            return QString::number( val.toDouble(), 'g', 12);
          else
            return val;
        }
      case Qt::TextAlignmentRole:
            return int(Qt::AlignRight | Qt::AlignVCenter);
      default: break;
    }
     return QVariant();
}

bool TMatrixModel::setData( const QModelIndex &index, const QVariant& value, int role)
{
	if( index.isValid() && ( role == Qt::EditRole ) )
	{
       matrix[index.row()].replace(index.column(), value);
       emit dataChanged(index, index);
       return true;
	} 
	return false;
}


QVariant TMatrixModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
     switch( role )
      { case Qt::DisplayRole:

         if( orientation == Qt::Horizontal )
         {
              return colHeads.at(section);
         }
         else
          return QVariant(section);

        case Qt::TextAlignmentRole:
               return int(Qt::AlignLeft | Qt::AlignVCenter);
        default:
        break;
       }
	return QVariant();
}

Qt::ItemFlags TMatrixModel::flags( const QModelIndex & index ) const
{
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    flags |= Qt::ItemIsEditable;
    return flags;
}

void TMatrixModel::defineMode()
{
#if QT_VERSION >= 0x050000

    beginResetModel();
    //myData.clear();
    endResetModel();
#else
    reset();
#endif

}

// internal part
void TMatrixModel::matrixFromCsvString( const QString& valueCsv )
{
#if QT_VERSION >= 0x050000
    beginResetModel();
#endif
   int ii, jj;
  // clear old data
  colHeads.clear();
  matrix.clear();

  //
  const QStringList rows = valueCsv.split('\n', QString::KeepEmptyParts);

  // get header
  QStringList cells = rows[0].split(',', QString::KeepEmptyParts);
  for( ii=0; cells.count(); ii++ )
      colHeads.push_back( cells[ii] );

  // get values
  for( jj=1; jj<rows.count(); jj++ )
  {
     QVector<QVariant> vec;
     matrix.push_back( vec );
     cells = rows[jj].split(',', QString::KeepEmptyParts);
     for( ii=0; cells.count(); ii++ )
       if( ii < numberStringColumns )
         matrix[jj].push_back( cells[ii] );
       else
         matrix[jj].push_back( cells[ii].toDouble() );
  }

#if QT_VERSION >= 0x050000
    endResetModel();
#else
    reset();
#endif

}

QString TMatrixModel::matrixToCsvString( )
{
    QString valCsv;

    QVectorIterator<QString> head(colHeads);
    while (head.hasNext())
    {
        valCsv += head.next();
        if(head.hasNext() )
           valCsv += ",";
    }
    valCsv += "\n";

    QVectorIterator<QVector<QVariant> > line(matrix);
    while (head.hasNext())
    {
        int col =0;
        QVectorIterator<QVariant> valC(line.next());
        while (valC.hasNext())
        {
          QVariant val = valC.next();
          if( col  >= numberStringColumns )
              valCsv += QString::number( val.toDouble(), 'g', 12);
            else
              valCsv += val.toString();
          if(valC.hasNext() )
               valCsv += ",";
        }
        valCsv += "\n";
    }

   return valCsv;
}

void TMatrixModel::matrixFromCsvFile( const QString& dir )
{
    QString valCsv;

    // read file
    QString fpath = dir + "/" + fname +".csv";
    QFile tmpStriam(fpath);
    if(tmpStriam.open( QIODevice::ReadOnly ))
    {
      valCsv = tmpStriam.readAll();
      tmpStriam.close();
    }

    //delete all comments // #
    int foundStart = valCsv.indexOf('#');
    int foundEnd;
    while( foundStart > 0 )
    {
      foundEnd = valCsv.indexOf('\n');
      if( foundEnd > 0 )
        valCsv.remove( foundStart, foundEnd-foundStart+1 );
      else
        {
           valCsv.remove( foundStart);
           break;
        }
      foundStart = valCsv.indexOf('#', foundStart );
    }

    // remove all ' ' and '\t'
    valCsv.remove(QChar(' '), Qt::CaseInsensitive);
    valCsv.remove(QChar('\t'), Qt::CaseInsensitive);

    //set up data
    matrixFromCsvString( valCsv );

}

void TMatrixModel::matrixToCsvFile( const QString& dir )
{
    // get string to output
    QString valCsv = matrixToCsvString( );

    // write file
    QString fpath = dir + "/" + fname +".csv";
    QFile tmpStriam(fpath);
    if(tmpStriam.open( QIODevice::Truncate))
    {
      tmpStriam.write(valCsv.toUtf8());
      tmpStriam.close();
    }

}

//-------------------------------------------------------------------------------------
// class TVectorTable implements a table view that displays items from a model.
//-------------------------------------------------------------------------------------

TMatrixTable::TMatrixTable( QWidget * parent ):
     QTableView( parent )
{
    vScroll =false, hScroll=false;


#if QT_VERSION >= 0x050000

    verticalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents/*QHeaderView::Stretch*/ );
    horizontalHeader()->setSectionResizeMode( QHeaderView::ResizeToContents/*QHeaderView::Stretch*/ );
#else
    verticalHeader()->setResizeMode( QHeaderView::ResizeToContents/*QHeaderView::Stretch*/ );
//    horizontalHeader()->setResizeMode( QHeaderView::ResizeToContents/*QHeaderView::Stretch*/ );

#endif

    setEditTriggers( QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed );
    //setFocusPolicy(Qt::WheelFocus/*Qt::StrongFocus*/);
    //setTabKeyNavigation( false );
    setContextMenuPolicy(Qt::CustomContextMenu);

    setVerticalScrollMode( QAbstractItemView::ScrollPerItem );
    setHorizontalScrollMode( QAbstractItemView::ScrollPerItem );
    setEditTriggers( QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed );
    
    connect( this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotPopupContextMenu(QPoint)));
}

 
 static bool no_menu_out = true;
 static bool no_menu_in = true;
 void TMatrixTable::focusOutEvent( QFocusEvent * event )
 {
     //   if( no_menu_out )
     //   clearSelection();
     //    no_menu_out = true;
	 QTableView::focusOutEvent( event );
 }
 
 void TMatrixTable::focusInEvent( QFocusEvent * event )
  {
    QTableView::focusInEvent( event );
    //if( no_menu_in )
    //selectionModel()->select( currentIndex(),
    //          QItemSelectionModel::SelectCurrent );
    //no_menu_in = true;
  }

 void TMatrixTable::slotPopupContextMenu(const QPoint &pos)
 {
     QMenu *menu = new QMenu(this);
     //no_menu_out = false;
     //no_menu_in = false;

     QAction* act;
     act =  new QAction(tr("&Help"), this);
     act->setIcon(QIcon(":/menu/Icons/ShowHelpWindowIcon.png"));
     act->setShortcut(tr("F1"));
     act->setStatusTip(tr("Help to the specified cell"));
     connect(act, SIGNAL(triggered()), this, SLOT(CmHelp()));
     menu->addAction(act);

     act =  new QAction(tr("&Calculator"), this);
     act->setShortcut(tr("F8"));
     act->setStatusTip(tr("Use calculator  to the specified cells"));
     connect(act, SIGNAL(triggered()), this, SLOT(CmCalc()));
     menu->addAction(act);

     menu->addSeparator();
        
     act =  new QAction(tr("Select co&lumn"), this);
     act->setShortcut(tr("Ctrl+L"));
     act->setStatusTip(tr("Select current column"));
     connect(act, SIGNAL(triggered()), this, SLOT(SelectColumn()));
     menu->addAction(act);

     act =  new QAction(tr("Select &all"), this);
     act->setShortcut(tr("Ctrl+A"));
     act->setStatusTip(tr("Select all cells"));
     connect(act, SIGNAL(triggered()), this, SLOT(SelectAll()));
     menu->addAction(act);

     menu->addSeparator();

     act =  new QAction(tr("C&ut"), this);
     act->setShortcut(tr("Ctrl+U"));
     act->setStatusTip(tr("Cut selected cells"));
     connect(act, SIGNAL(triggered()), this, SLOT(CutData()));
     menu->addAction(act);

     act =  new QAction(tr("&Copy"), this);
     act->setShortcut(tr("Ctrl+C"));
     act->setStatusTip(tr("Copy selected cells"));
     connect(act, SIGNAL(triggered()), this, SLOT(CopyData()));
     menu->addAction(act);

     act =  new QAction(tr("&Paste"), this);
     act->setShortcut(tr("Ctrl+V"));
     act->setStatusTip(tr("Paste to selected area"));
     connect(act, SIGNAL(triggered()), this, SLOT(PasteData()));
     menu->addAction(act);
 
     act =  new QAction(tr("Paste &transposed"), this);
     act->setShortcut(tr("Ctrl+T"));
     act->setStatusTip(tr("Paste transposed to selected area"));
     connect(act, SIGNAL(triggered()), this, SLOT(PasteTransposedData()));
     menu->addAction(act);

     act =  new QAction(tr("&Clear"), this);
     act->setShortcut(tr("Ctrl+Del"));
     act->setStatusTip(tr("Clear selected cells"));
     connect(act, SIGNAL(triggered()), this, SLOT(ClearData()));
     menu->addAction(act);

     menu->exec( viewport()->mapToGlobal(pos) );
     delete menu;
 }

 void TMatrixTable::keyPressEvent(QKeyEvent* e)
 {
    if ( e->modifiers() & Qt::ControlModifier ) 
    {
		switch ( e->key() ) 
		{
          case Qt::Key_L:
		    SelectColumn();
		    return;
          case Qt::Key_A:
		    SelectAll();
		    return;
		  case Qt::Key_U:
		    CutData();
		    return;
		  case Qt::Key_C:
		    CopyData();
		    return;
          case Qt::Key_V:
                    PasteData();
                    return;
		  case Qt::Key_T:
                     PasteTransposedData();
                                     return;
		  case Qt::Key_Delete:
		    ClearData();
                    return;
		}
	}
    switch( e->key() )
 	{
      case Qt::Key_F1:
            CmHelp();
            return;
 	  case Qt::Key_F8:
 	         CmCalc();
 	          return;
     }
 	QTableView::keyPressEvent(e);
 }

 // Help on F1 pressed on data field
 void TMatrixTable::CmHelp()
 {
   // pVisorImp->OpenHelp( 0, item.c_str());
 }


// Calculator on F8 pressed on data field
void TMatrixTable::CmCalc()
{
  TMatrixModel *  model = ((TMatrixModel *)(currentIndex().model() ));
  QString res;

  CalcDialog calc(topLevelWidget() );
  if( calc.exec() )
    {
       foreach( QModelIndex ndx,  selectedIndexes()  )
       {
           res = calc.fun( ndx.data(Qt::EditRole).toDouble() );
           model->setData(ndx, res,  Qt::EditRole);
       }
   }
}

 void TMatrixTable::SelectColumn()
 {
    selectionModel()->select( currentIndex(),
                QItemSelectionModel::Columns|QItemSelectionModel::Select );
  }

  void TMatrixTable::SelectAll()
  {
	  selectAll();
  }

  void TMatrixTable::CutData()
  {
	  CopyData();
	  ClearData();
  }

  void TMatrixTable::ClearData()
  {
     TMatrixModel *  model = ((TMatrixModel *)(currentIndex().model() ));
     foreach( QModelIndex ndx,  selectedIndexes()  )
           model->setData(ndx, emptiness.c_str(),  Qt::EditRole);
 }

 void TMatrixTable::CopyData()
 {
	Selection sel = getSelectionRange();
	QString clipText = createString( sel );
    QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
 }

 void TMatrixTable::PasteData()
 {
    Selection sel = getSelectionRange( true );
    pasteIntoArea( sel, false);
 }

 void TMatrixTable::PasteTransposedData()
 {
    Selection sel = getSelectionRange( true );
	pasteIntoArea( sel, true);
 }
 
 //------------------------------------------------------------------------ 
  QString TMatrixTable::createString( Selection& sel )
  {
      int ii, jj;
	  QModelIndex index = currentIndex();
	  QString cText;
	  QString clipText;
	  
	  for(  ii=sel.N1; ii<=sel.N2; ii++ )
	  {
		if( ii > sel.N1 )
          clipText += splitRow;
  
		for( jj=sel.M1; jj<=sel.M2; jj++ )
		{
		  QModelIndex wIndex = 	index.sibling( ii, jj );
		  // selected all region if( selmodel->isSelected( wIndex ) )
		  if( jj > sel.M1 )
            clipText += splitCol;
		  cText = wIndex.data(Qt::EditRole).toString();
		  if( cText == emptiness.c_str() )
			  cText = "  ";//"\r"; 
	     clipText += cText;
		}
	 }
	return clipText;  
  }

  Selection TMatrixTable::getSelectionRange( bool paste_ )
  {
     QModelIndex index = currentIndex();

	  if( !paste_ && !selectionModel()->hasSelection()  )
		return Selection( index.row(), index.row(), index.column(), index.column() );

 	 int N1=-1, N2=0, M1=-1, M2=0; 
 	 foreach( QModelIndex ndx,  selectedIndexes()  )
 	 {
 	   if( N1 == -1 || M1 == -1)
 	   {  
 		  N1 = ndx.row();
 	      M1 = ndx.column();
 	   } 
 	   if( N1 > ndx.row() ) N1 = ndx.row();	 
 	   if( N2 < ndx.row() ) N2 = ndx.row();	 
 	   if( M1 > ndx.column() ) M1 = ndx.column();	 
 	   if( M2 < ndx.column() ) M2 = ndx.column();	 
  	 };
  
 	 if( paste_ && ( !selectionModel()->hasSelection() ||
        ( N1==N2 && M1==M2 ) ) ) // only one selected => all for end of table
            return Selection( index.row(), index.model()->rowCount( index )-1,
                               index.column(), index.model()->columnCount( index )-1 );

  	 return Selection( N1, N2, M1, M2 );
  }

  void  TMatrixTable::setFromString(char splitrow, const QString& str,
          Selection sel, bool transpose)
  {
     TMatrixModel *  model = ((TMatrixModel *)(currentIndex().model() ));
     if( str.isEmpty() )
  	    return;
  	
     QModelIndex wIndex;
     const QStringList rows = str.split(splitrow, QString::KeepEmptyParts);

     int ii, jj;
     int rowNum = sel.N1;
     const int nLimit = (transpose) ? (sel.N1 + sel.M2-sel.M1) : sel.N2;

        for( int it = 0; it < rows.count() && rowNum <= nLimit; it++, rowNum++)
  	{
  	    //if( rows[it].isEmpty() ) sd 29/10/2008 
  		// continue;

  	    const QStringList cells = rows[it].split('\t', QString::KeepEmptyParts);
  	    int cellNum = sel.M1;
  	    const int mLimit = (transpose) ? (sel.M1 + sel.N2-sel.N1) : sel.M2;
  	    for( int cellIt = 0;  cellIt < cells.count() && cellNum <= mLimit; cellIt++, cellNum++) 
  	    {
          string value = (const char*)cells[ cellIt ].toLatin1().data();
          strip( value );
  		  if( value.empty() || value == emptiness )
              value = "---";
    	  
  		   if( transpose ) 
  		   { ii = (cellNum-sel.M1)+sel.N1;
                     jj = (rowNum-sel.N1)+sel.M1;
  		   }
  		   else
  		   { ii = rowNum;
                     jj = cellNum;
  		   }

           wIndex = currentIndex().sibling( ii, jj );
           model->setData(wIndex, QString(value.c_str()), Qt::EditRole);
  		}
  	}
 }

  void TMatrixTable::pasteIntoArea( Selection& sel, bool transpose)
  {
      QString clipboard = QApplication::clipboard()->text(QClipboard::Clipboard);
      char splitrow = splitRow;

      int lastCR = clipboard.lastIndexOf(splitrow);
      if( lastCR < 0 )
      {
#ifdef __APPLE__
       splitrow = '\n';
#else
      splitrow = '\r';
#endif
         lastCR = clipboard.lastIndexOf(splitrow);
      }
      if( lastCR == clipboard.length() - 1 )
  	  clipboard.remove(lastCR, 1);
      QString undoString;

         const QStringList rows = clipboard.split(splitrow);
         const int clipN = rows.count();
         const bool largerN = transpose ?
           (clipN > (sel.M2 - sel.M1 + 1)) : (clipN > (sel.N2 - sel.N1 +1 ));
      
         int rowNum = sel.N1;
     	 bool largerM = false;
         for(int it = 0; it < rows.count(); it++, rowNum++)
  	     {
           int clipM = rows[it].count(splitCol) + 1;
  	       largerM = transpose ? (clipM > (sel.N2 - sel.N1 + 1 )) : (clipM > (sel.M2 - sel.M1 + 1));
  	       if( largerM )
  		      break;
  	     }
         if( largerN || largerM )
         {
           cout <<  "Object paste" <<
            "Pasting contents has larger dimensions then the object!" << endl;
         }
         undoString = createString( sel );
      
         setFromString(splitrow, clipboard, sel, transpose);
  }


 //-------------------------------------------------------------------------------------
 // TObjectDelegate -  individuak items in views are rendered and edited using delegates
 //-------------------------------------------------------------------------------------

 TMatrixDelegate::TMatrixDelegate(  QObject * parent ):
        QItemDelegate( parent )
 {
 }

 // Editing QTableView for objects in TCPage
 QWidget *TMatrixDelegate::createEditor(QWidget *parent,
         const QStyleOptionViewItem &option,
         const QModelIndex &index) const
 {
     TCellInput* editor =  new TCellInput( index.row(), index.column(), parent);
     return editor;
 }

 void TMatrixDelegate::setEditorData(QWidget *editor,
                         const QModelIndex &index) const
 {
    TCellInput *cellEdit = dynamic_cast<TCellInput*>(editor);
 	if( cellEdit)
 	{   
        cellEdit->setData( index.data(Qt::EditRole).toString());
 	}    
 }

 void TMatrixDelegate::setModelData(QWidget *editor,
                                  QAbstractItemModel *model,
                                  const QModelIndex &index) const
 {
    TCellInput *cellEdit = dynamic_cast<TCellInput*>(editor);
 	if( cellEdit)
 	{	
          if( cellEdit->dataCh() )
              model->setData(index, cellEdit->getData(),  Qt::EditRole);
 	}   
  }

 

//------------------------------------------------
// TCellInput
//------------------------------------------------

 class TDoubleValidator : public QDoubleValidator
 {
    // Q_OBJECT

 public:
     TDoubleValidator(QObject * parent):QDoubleValidator( parent){};
     ~TDoubleValidator(){};

     QValidator::State validate(QString &, int &) const;

 };

 QValidator::State TDoubleValidator::validate(QString &input, int &pos) const
 {
   if (input == "--" )
         return Intermediate;
   if (input == "---" )
         return Acceptable;

  return QDoubleValidator::validate(input, pos);
 }



TCellInput::TCellInput( int in, int im, QWidget * parent ):
    /*QDoubleSpinBox*/QLineEdit( parent ), iN(in), iM(im)
{
    edited = false;
    TDoubleValidator* dVal = new TDoubleValidator( this );
    dVal->setDecimals( 15 );
    setValidator( dVal );

    connect( this, SIGNAL(textEdited( const QString& )),
            this, SLOT(setCh()));
}

/*void
TCellInput::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
       case Qt::Key_F1:
              CmHelp();
              return;
    }
    QLineEdit::keyPressEvent(e);
}
*/
//---------------------- End of file  fmodel.cpp ---------------------------
