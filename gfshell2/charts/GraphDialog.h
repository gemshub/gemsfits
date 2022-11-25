#pragma once

#include <QDialog>
#include <QTableWidget>
#include <QItemDelegate>

namespace Ui {
class GraphDialogData;
}

namespace jsonui17 {

void helpWin( const std::string& name, const std::string& item );

class DragTableWidget;
class LabelDelegate;
class PlotChartView;
class ChartData;

/// \class GraphDialog plotting system main dialog
class GraphDialog: public QDialog
{
    friend class LegendDialog;

    Q_OBJECT

    void ShowLegend();
    //    void ShowIsolineLegend();

protected slots:

    void changeIcon( int row, int column );
    void changeNdx( int row, int column );
    void highlightRow( size_t row );

    void CmFragment();
    void CmLegend();
    void CmPrint();
    void CmSaveImage();
    void CmHelp();

public slots:

    /// Update graphic and legend
    void UpdateAll(const char* title=nullptr );
    void restoreRow();
    void updateFragment(QRectF  rect);

signals:
    void dataChanged( ChartData *achartData );

public:
    GraphDialog( ChartData *data, QWidget *parent,
                 const char *title= "Graphics Dialog" );
    ~GraphDialog();

    /// Update all graphic lines with new title
    void UpdatePlots( const char *title );

private:

    Ui::GraphDialogData *ui;
    QPushButton* bFragment;
    PlotChartView* plot;
    ChartData* gr_data;

    DragTableWidget* tbLegend;
    //LabelDelegate* dgLegend;

    // work part
    bool isFragment;
    size_t activeRow = std::string::npos;

    friend class LabelDelegate;
};


//=========================================================================

/// Drag Table Widget - added for legend table
class DragTableWidget: public QTableWidget
{
    QPoint startPos;
    GraphDialog* topDlg;


public:
    DragTableWidget( GraphDialog* top ):
        QTableWidget(top), topDlg(top) {}

    QSize sizeHint() const override
    {
        auto sizehint = QTableWidget::sizeHint();
        sizehint.setWidth(180);
        return sizehint;
    }

protected:


    void startDragN();
    void mousePressEvent( QMouseEvent *e )override;
    void mouseMoveEvent( QMouseEvent *e )override;
    void focusOutEvent(QFocusEvent* event)override;

};

/// \class LabelDelegate
/// Individual items in views are rendered and edited using delegates
class LabelDelegate: public QItemDelegate
{
    Q_OBJECT

    GraphDialog* topDlg;

public:

    LabelDelegate( GraphDialog* atopDlg ):
        QItemDelegate( atopDlg ), topDlg(atopDlg)
    {}

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
};

} // namespace jsonui17

