
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QMimeData>
#include <QDrag>
#include <QtPrintSupport/QPrinter>
#include <QPrintDialog>
#include <QHeaderView>
#include <QFileDialog>
#include <QFileInfo>

#include "ui_GraphDialog4.h"
#include "charts/GraphDialog.h"
#include "charts/LegendDialog.h"
#include "charts/SymbolDialog.h"
#include "charts/chart_view.h"

bool ChooseFileSave( QWidget* par, std::string& path_, const char* title,
                     const QString& filter )
{
    QString path = path_.c_str();
    QString filt;

    if( filter.isEmpty() )
        filt += "All files (*.*)";
    else
        filt += filter;

    QString selectedFilter;
    QString fn = QFileDialog::getSaveFileName( par, title,  path, filt,
                                               &selectedFilter, QFileDialog::DontConfirmOverwrite);

    if ( !fn.isEmpty() )
    {
        QFileInfo flinfo(fn);
        if( flinfo.suffix().isEmpty() ) // solwing for linux
        {
            int posb = selectedFilter.lastIndexOf(".");
            int pose = selectedFilter.indexOf(")", posb);
            if( posb > 0 && pose > posb )
            {
                QString ext = selectedFilter.mid(posb+1, pose-posb-1);
                fn += "."+ext;
            }
        }
        path_ = fn.toStdString();
        return true;
    }
    else
    {
        path_  = "";
        return false;
    }

}

namespace jsonui17 {

QString imageFilters();


void helpWin( const std::string& name, const std::string& item )
{
  Q_UNUSED(name)
  Q_UNUSED(item)
}

}


namespace jsonui17 {


/// The constructor
GraphDialog::GraphDialog( ChartData *adata, QWidget *parent, const char *title ):
        QDialog( parent ), ui(new Ui::GraphDialogData),
        gr_data(adata), isFragment(false)
{
    ui->setupUi(this);
    setWindowTitle( title );

    ui->splitter->setStretchFactor(0, 4);
    ui->splitter->setStretchFactor(1, 0);

    // Define legend table
    tbLegend = new DragTableWidget( this );
    tbLegend->setSelectionMode(QAbstractItemView::NoSelection);
    tbLegend->setColumnCount( 3 );
    tbLegend->setColumnWidth(0, 35 );
    tbLegend->setColumnWidth(1, 35 );
    tbLegend->horizontalHeader()->setStretchLastSection( true);
    tbLegend->verticalHeader()->setVisible(false);
    tbLegend->horizontalHeader()->setVisible(false);

    LabelDelegate *dgLegend = new LabelDelegate( this );
    tbLegend->setItemDelegate(dgLegend);
    ui->verticalLayout->addWidget( tbLegend );

    // define plot window
    plot = new PlotChartView( gr_data, this);
    ui->verticalLayout_2->addWidget( plot);
    // Insert labels in legend box
    if( gr_data->getGraphType() == LineChart || gr_data->getGraphType() == AreaChart )
      ShowLegend();

    connect(tbLegend, &DragTableWidget::cellClicked, this, &GraphDialog::changeIcon);
    connect(tbLegend, &DragTableWidget::cellChanged, this, &GraphDialog::changeNdx);

    bFragment =ui->buttonBox->addButton("&Fragment", QDialogButtonBox::ActionRole );
    QPushButton* bCustomize =ui->buttonBox->addButton("&Customize", QDialogButtonBox::ActionRole );
    QPushButton* bPrint =ui->buttonBox->addButton("&Print", QDialogButtonBox::ActionRole );
    QPushButton* bSaveImage =ui->buttonBox->addButton("&Save Image", QDialogButtonBox::ActionRole );

    //QObject::connect( ui->buttonBox, &QDialogButtonBox::rejected, this, &GraphDialog::reject);
    QObject::connect( ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &GraphDialog::close);
    QObject::connect( ui->buttonBox, &QDialogButtonBox::helpRequested, this, &GraphDialog::CmHelp);

    QObject::connect( bFragment, &QPushButton::clicked, this, &GraphDialog::CmFragment);
    QObject::connect( bCustomize, &QPushButton::clicked, this, &GraphDialog::CmLegend);
    QObject::connect( bPrint, &QPushButton::clicked, this, &GraphDialog::CmPrint);
    QObject::connect( bSaveImage, &QPushButton::clicked, this, &GraphDialog::CmSaveImage);
    QObject::connect( plot, SIGNAL(savetoFile()), this, SLOT(CmSaveImage()));

    QObject::connect( plot, &PlotChartView::fragmentChanged, this, &GraphDialog::updateFragment);

}

GraphDialog::~GraphDialog()
{
    delete tbLegend;
    delete ui;
}

void GraphDialog::UpdatePlots( const char* title )
{
    if( title )
        setWindowTitle( title );
   plot->updateAll();
}

void GraphDialog::UpdateAll(const char* title )
{
    if( title )
        setWindowTitle( title );

    disconnect(tbLegend, &DragTableWidget::cellClicked,
            this, &GraphDialog::changeIcon);
    disconnect(tbLegend, &DragTableWidget::cellChanged,
            this, &GraphDialog::changeNdx);

    // Insert labels in legend box
    if( gr_data->getGraphType() == LineChart || gr_data->getGraphType() == AreaChart )
      ShowLegend();

    plot->updateAll();
    //emit dataChanged( gr_data );

    connect(tbLegend, &DragTableWidget::cellClicked,
            this, &GraphDialog::changeIcon);
    connect(tbLegend, &DragTableWidget::cellChanged,
            this, &GraphDialog::changeNdx);
}


// slots ------------------------------------

void GraphDialog::CmLegend()
{
    LegendDialog legend( this );
    legend.exec();
}

void GraphDialog::CmFragment()
{
    if(  isFragment == false )
    {
        isFragment = true;
        bFragment->setText("&Full");
    }
    else
    {
        isFragment = false;
        bFragment->setText("&Fragment");
    }
    plot->updateFragment( isFragment );
}

void GraphDialog::CmSaveImage()
{
    std::string  fileName  =  "image.pdf";
    if( ChooseFileSave(  this, fileName, "Saving Graphics Image", imageFilters() ) )
    {
       plot->renderDocument( gr_data->title.c_str(), fileName.c_str() );
    }
}

// Print graph screen to printer device
void GraphDialog::CmPrint()
{

    QPrinter printer( QPrinter::PrinterResolution/*QPrinter::HighResolution*/ );

    QPrintDialog dialog( &printer );
    if ( dialog.exec()  )
    {
        printer.setDocName( gr_data->title.c_str() );
        QPainter p( &printer );
        plot->render( &p );
    }

}

void GraphDialog::CmHelp()
{
    helpWin( "GraphDialogJsonui", "" );
}

// Insert labels in legend box
void GraphDialog::ShowLegend()
{
    QTableWidgetItem *itemL, *itemN;
    Qt::ItemFlags flags;

    tbLegend->setAcceptDrops(true);
    tbLegend->setEditTriggers( QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed );

    tbLegend->clear();
    tbLegend->setRowCount( gr_data->linesNumber() );

    int nLines = 0;
    for( size_t mii=0; mii<gr_data->modelsNumber(); mii++ )
    {
        auto nLinN =  gr_data->modelData( mii )->getSeriesNumber();
        for( size_t jj=0; jj<nLinN; jj++, nLines++ )
        {
            tbLegend->setRowHeight(nLines, 21/*htF(ftString, 0)+4*/);

           auto icon = markerShapeIcon(gr_data->lineData(nLines) );
           itemL = new QTableWidgetItem(icon, "");
           flags = itemL->flags();
           itemL->setFlags(flags & ~Qt::ItemIsEditable);
           itemL->setToolTip("Legend column 1");
           tbLegend->setItem(nLines, 0, itemL );

           auto indexName = gr_data->modelData( mii )->abscissaIndexName(gr_data->lineData(nLines).getXColumn());
           itemN = new QTableWidgetItem(tr("%1").arg(indexName));
           itemN->setToolTip("Legend column 2");
           tbLegend->setItem(nLines, 1, itemN );

           itemN = new QTableWidgetItem(tr("%1").arg(gr_data->lineData(nLines).getName().c_str()));
           itemN->setToolTip("Legend column 3");
           tbLegend->setItem(nLines, 2, itemN );
         }
    }
}

void GraphDialog::changeIcon( int rowi, int column )
{
     auto row = static_cast<size_t>(rowi);
    if( column == 0 )
    {
     if( gr_data->getGraphType() == LineChart || gr_data->getGraphType() == AreaChart )
     {
            SymbolDialog cd( gr_data->lineData(row), this);
            if( cd.exec() )
            {
               gr_data->setLineData( row, cd.GetPlotLine() );
               auto icon = markerShapeIcon(gr_data->lineData(row) );
               tbLegend->item(rowi, column)->setIcon(icon);
               plot->updateLine(row);
               emit dataChanged( gr_data );
            }
     }
   }    else if( column ==  2  )
            {
               highlightRow( row );
            }

}

void GraphDialog::changeNdx( int rowi, int column )
{
    auto row = static_cast<size_t>(rowi);
    if( column == 1 &&  ( gr_data->getGraphType() == LineChart || gr_data->getGraphType() == AreaChart ) )
    {
       auto ndxX = tbLegend->item(rowi, column)->text();
       gr_data->setLineData( row, ndxX );
       if( gr_data->getGraphType() == LineChart )
            plot->updateLine( row );
       else if( gr_data->getGraphType() == AreaChart )
              plot->updateAll();
       emit dataChanged( gr_data );
    }
    if( column == 2 /*&&  gr_data->graphType == LineChart*/ )
    {
        std::string  name = tbLegend->item(rowi, column)->text().toStdString();
        gr_data->setLineData(static_cast<size_t>(row), name );
        emit dataChanged( gr_data );
    }
}

void GraphDialog::highlightRow( size_t row )
{
    if( activeRow == row ) // toggle
    {
        restoreRow();
        return;
    }
    restoreRow();
    activeRow = row;
    plot->highlightLine(row, true);
}

void GraphDialog::restoreRow()
{
    if( activeRow != std::string::npos)
        plot->highlightLine(activeRow, false);
    activeRow = std::string::npos;
}

void GraphDialog::updateFragment(QRectF  rect)
{
    gr_data->part[0] = rect.left();
    gr_data->part[1] = rect.right();
    gr_data->part[2] = rect.bottom();
    gr_data->part[3] = rect.top();
    isFragment = true;
    bFragment->setText("&Full");
    plot->resetFragment( isFragment );
    emit dataChanged( gr_data );
}

//=======================================================================================
// Added for new legend table

void DragTableWidget::startDragN(/*Qt::DropActions supportedActions*/)
{
    if( currentColumn() == 2 )
    {
        QTableWidgetItem *item = currentItem();
        QString text_ = item->text();

        QFontMetrics fm(this->font());
        QPixmap pixmap(fm.horizontalAdvance(text_)+2, fm.height()+2);
        pixmap.fill(QColor(0, 0, 0, 0));
        QPainter dc(&pixmap);
        dc.drawText(2, fm.height()-2, text_ );
        dc.end();

        QMimeData * mimeData = new QMimeData;
        mimeData->setText( text_ );
        QDrag *drag = new QDrag(this);
        drag->setMimeData( mimeData );
        drag->setPixmap(pixmap/*QPixmap::grabWidget(this)*/);
        //drag->setHotSpot( QPoint(0, pixmap.height()) );
        drag->setHotSpot( QPoint( pixmap.width() / 2, pixmap.height() / 2 ) );
        drag->exec();
     }
 }

void DragTableWidget::mousePressEvent( QMouseEvent *e )
{
        if(e->buttons() & Qt::LeftButton )
          startPos = e->pos();
        QTableWidget::mousePressEvent(e);
 }

void DragTableWidget::mouseMoveEvent( QMouseEvent *e )
{
        if(e->buttons() & Qt::LeftButton )
        { int delta = (e->pos()-startPos).manhattanLength();
            if( delta >= QApplication::startDragDistance() )
              startDragN();
        }
        QTableWidget::mouseMoveEvent(e);
}

void DragTableWidget::focusOutEvent(QFocusEvent *event)
{
    topDlg->restoreRow();
    QTableWidget::focusOutEvent(event);
}

//--------------------------------------------------------------------------

// Editing  Table of Labels
QWidget *LabelDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &,
                                     const QModelIndex &index) const
{

    if( index.column() == 2 )
    {
        QLineEdit *editor =  new QLineEdit( parent);
        editor->setMaxLength( 30 );
        return editor;
    }
    if( index.column() == 1 )
    {
        int ndx_plot = topDlg->gr_data->getPlot( static_cast<size_t>(index.row()) );
        if( ndx_plot >= 0 )
        {
            QStringList cbList = topDlg->gr_data->modelData(static_cast<size_t>(ndx_plot))->getAbscissaIndexes();
            if( cbList.count() < 2 ) // only one value correct
                return nullptr;
            QComboBox* editor =  new QComboBox( parent );
            editor->addItems(cbList);
            return editor;
        }
    }
    return nullptr;
}

void LabelDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
   if( index.column() == 1)
   {
       QComboBox *cellEdit = dynamic_cast<QComboBox*>(editor);
       if( cellEdit )
             model->setData(index, cellEdit->currentText(),  Qt::EditRole);
   }
   else
       QItemDelegate::setModelData( editor, model, index);
 }

} // namespace jsonui17

//--------------------- End of GraphDialog.cpp ---------------------------
