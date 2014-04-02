//-------------------------------------------------------------------
// $Id: GraphDialog.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of GraphDialog classes
// (Plotting system maim dialog )
//
// Copyright (C) 2013 S.Dmytriyeva
// Uses  string class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------
#ifndef graph_dialog_h
#define graph_dialog_h

#include <QResizeEvent>
#include <QDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QItemDelegate>

#include "ui_GraphDialog4.h"
#include  "qwtplot.h"
#include  "fmodel.h"

// Widgets for legend and plotting dialogs
void paintIcon( QIcon &icon, TPlotLine &plLine );

//=========================================================================
// Added for new legend table
class DragTableWidget: public QTableWidget
{
    QPoint startPos;

    public:
        DragTableWidget(QWidget* parent):
            QTableWidget(parent) {}

        virtual ~DragTableWidget() {}

    protected:

    void startDrag();
    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
};

// class LabelDelegate
// individual items in views are rendered and edited using delegates
class LabelDelegate: public QItemDelegate
{
   Q_OBJECT

  QVector<int> first;
  QVector<int> maxXndx;
  bool isIsoline;

 public:

   LabelDelegate( QVector<int> first, QVector<int> maxXndx, QObject * parent = 0 );
   QWidget *createEditor(QWidget *parent,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) const;
   void setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const;
   void setIsoline( bool aIso )
   {
      isIsoline = aIso;
   }

};


//=======================================================================

class GraphDialog: public QDialog, public Ui::GraphDialogData
{
    Q_OBJECT

    TPlotWidget* plot;
    TMatrixModel *pModel;
    DragTableWidget *tbLegend;
    LabelDelegate *dgLegend;

    // work part
    bool isFragment;

    void ShowLegend();
    void ShowIsolineLegend();

    void closeEvent(QCloseEvent*);

protected slots:
    void changeIcon( int row, int column );
    void changeNdx( int row, int column );
    void languageChange();
    void CmFragment();
    void CmLegend();
    void CmPrint();
    void CmSave();
    void CmHelp();

public slots:
    void selectedFragment( const QRectF& );

public:

    GraphData gr_data;

    GraphDialog(TMatrixModel *pmodule, const GraphData& data, QWidget *parent );
    ~GraphDialog();

    string modelName() const
    {  return  pModel->getName();   }

    // Show dialog
    void ShowNew( const char *capAdd );

    // Add new point to graph
    void AddPoint( int nPlot, int nPoint );

    // Update changes
    void Apply();

    // Save changes to DataBase record
    void SaveGraphData();

    QColor getBackgrColor();
    void setBackgrColor( QColor color );

};

#endif   // graph_dialog_h
