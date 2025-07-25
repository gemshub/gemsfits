//-------------------------------------------------------------------
// $Id:  $
//
// Declaration of TMatrixModel, TMatrixTable, TMatrixDelegate
// and TCellInput classes
//
// Copyright (C) 2010-2023  S.V.Dmytriyeva
// Uses EJDB (https://ejdb.org),
//    yaml-cpp (https://code.google.com/p/yaml-cpp/)
//
// This file is part of the GEMSFITS GUI, which uses the
// Qt v.5 cross-platform App & UI framework (http://qt-project.org)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of LGPL v.3 license
//
// See http://gems.web.psi.ch/GEMSFIT for more information
// E-mail gems2.support@psi.ch
//-----------------------------------------------------------------

#pragma once

#include <vector>
#include <QAbstractTableModel>
#include <QItemDelegate>
#include <QLineEdit>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QtPrintSupport/QPrinter>
#include "charts/graph_data.h"

namespace jsonui17 {
class GraphDialog;
}

#ifdef __APPLE__
const char  splitRow = '\r';
const char  splitCol = '\t';
#else
const char  splitRow = '\n';
const char  splitCol = '\t';
#endif

//const std::string emptiness("---");

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

class TSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    TSortFilterProxyModel(QObject *parent = 0):
        QSortFilterProxyModel(parent)
    { }

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

};



/*!
  \ class TMatrixModel
  \ class for represents the data set and is responsible for fetchin
  \ the data is neaded for viewing and for writing back any changes.
  \ Reading/writing data from/to std::vector classes
  
*/

class TMatrixModel: public QAbstractTableModel
{
    Q_OBJECT

    QString fname;
    int numberStringColumns;
    QVector< QString > colHeads;
    QVector< QVector<QVariant> > matrix;

    // graph charts information

    /// Abscissa columns list
    std::vector<int> xColumns;
    /// Ordinate columns list
    std::vector<int> yColumns;
    /// Descriptions of model extracting graph data
    std::vector<std::shared_ptr<jsonui17::ChartDataModel>> chart_models;
    /// Description of 2D plotting widget
    std::shared_ptr<jsonui17::ChartData> chart_data;
    jsonui17::GraphDialog *graph_dlg = nullptr;

    char dbl_format;
    int dbl_precision;

    QString ValToString(double val, int digits) const;
    double ValFromString(const QVariant& strval);
    void setGraphData(QSortFilterProxyModel *pmodel, const std::string& title);

public:

    TMatrixModel(const QString& fname, int aNumCol, char format = 'g', int precision = 12, QObject *parent = nullptr);
    ~TMatrixModel();

    const jsonui17::ChartData *getGraphData()
    {
        return chart_data.get();
    }

    int rowCount(const QModelIndex & parent) const;
    int columnCount(const QModelIndex & parent) const;
    QVariant data(const QModelIndex & index, int role) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex & index ) const;

    void matrixFromCsvFile(const QString& dir);
    void matrixToCsvFile(const QString& dir);
    void matrixFromCsvString(const QString& valueCsv);
    QString matrixToCsvString();
    void matrixToBson(common::JsonFree& object);
    void matrixFromBson(QSortFilterProxyModel *pmodel, const common::JsonFree& object);
    int getNumberStringColumns() const
    {   return numberStringColumns; }

    std::string getName()
    {   return fname.toStdString(); }

    void toggle_X(int ncolmn);
    void toggle_Y(int ncolmn);

    // get graph info
    void showGraphData(QSortFilterProxyModel *pmodel, const std::string& title);
    void CloseGraph();
    int findRow(int *xyndx, double *reg);

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

    void focusOutEvent(QFocusEvent * event);
    void focusInEvent(QFocusEvent * event);
    void keyPressEvent(QKeyEvent* e);

    QList<QAction*> itemActions(const QModelIndex & current);

    Selection getSelectionRange(bool paste_ = false);
    QString createString(Selection& sel);
    QString createHeader();
    void pasteIntoArea(Selection& sel, bool transpose);
    void setFromString(char splitrow, const QString& str,
                        Selection sel, bool transpose);

protected slots:
    void slotPopupContextMenu(const QPoint& pos);

public slots:
    void CmCalc();
    void SelectColumn();
    void SelectAll();
    void CutData();
    void ClearData();
    void CopyData();
    void PasteData();
    void PasteTransposedData();
    void ToggleX();
    void ToggleY();


public:
    TMatrixTable(QWidget * parent = 0);
    ~TMatrixTable() {}

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

    TMatrixDelegate(int nStringColumn, QObject * parent = 0);
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

};


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
    TCellInput(int in, int im, QWidget * parent = 0);
    ~TCellInput()
    {}

    virtual QString getData()
    {
        return text();
    }

    virtual void setData(QString data)
    {
        setText(data);
    }

    bool dataCh()
    { return edited; }
};

