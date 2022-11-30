#pragma once

#include <QJsonObject>
#include <QtCore/QAbstractTableModel>
#include "bson.h"

#ifndef NO_JSONIO
namespace jsonio17 {
class JsonBase;
}
#endif

namespace jsonui17 {

enum GRAPHTYPES {
    LineChart = 0,
    AreaChart = 1,
    BarChart  = 2,
    Isolines  = 3,   // under construction
    lines_3D  = 4    // for future using
};

/// \class ChartDataModel
/// \brief The ChartDataModel class is a vertical model for line,
///  spline, and scatter series.
///
/// Model enable using a data model derived from the QAbstractTableModel
/// class as a data source for a chart. First column of model used to iterate
/// by index
class ChartDataModel : public QAbstractTableModel
{
    Q_OBJECT

public Q_SLOTS:
    // for the model
    void modelUpdated(QModelIndex topLeft, QModelIndex bottomRight);
    void modelRowsAdded(QModelIndex parent, int start, int end);
    void modelRowsRemoved(QModelIndex parent, int start, int end);
    void modelColumnsAdded(QModelIndex parent, int start, int end);
    void modelColumnsRemoved(QModelIndex parent, int start, int end);

signals:
    void changedXSelections();
    void changedYSelections( bool updateNames );

public:

    explicit ChartDataModel( QAbstractTableModel* tableModel, QObject* parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /// Update GRAPHTYPES ( LineChart,  AreaChart,  BarChart, Isolines ... )
    void setGraphType( GRAPHTYPES type )
    {
        graphType   = type;
    }

    // working with X, Y columns selection

    /// Get Abscissa colunm index for line
    int getXColumn( int line ) const
    {
        if( line == -2 )
            return line;

        if( line>=0 && line < static_cast<int>(xcolumns.size()) )
            return xcolumns[static_cast<size_t>(line)];
        else
            return -1;
    }

    /// Get number of Abscissa lines
    int getAbscissaNumber() const
    {
        return static_cast<int>(xcolumns.size());
    }

    /// Get Abscissa name from index
    QString abscissaIndexName( int ndx ) const
    {
        if( ndx == -2 )
            return QString("Off");
        if( ndx == -1 )
            return QString("#");
        return QString("%1").arg(ndx);
    }

    /// Get Abscissa index from name
    int indexAbscissaName( const QString name ) const
    {
        if( name == QString("Off") )
            return -2;
        if( name == QString("#") )
            return -1;
        return name.toInt();
    }

    /// Get list of Abscissa indexes to QComboBox
    QStringList getAbscissaIndexes() const
    {
        QStringList lst;
        lst << abscissaIndexName( -2 );
        lst << abscissaIndexName( -1 );
        for( int ii=0; ii<getAbscissaNumber(); ii++ )
            lst <<  abscissaIndexName( ii );
        return lst;
    }

    /// Get number of series
    size_t getSeriesNumber() const
    {
        return ycolumns.size();
    }

    /// Get ordinate column index for line
    int getYColumn( size_t line ) const
    {
        if( line < ycolumns.size() )
           return ycolumns[line];
        else
           return 0;
    }

    /// Set abscissa columns
    void setXColumns( const std::vector<int>& axcolumns )
    {
        xcolumns.clear();
        for( auto cl:axcolumns )
            appendXColumn( cl );
        emit changedXSelections();
    }

    /// Set ordinate columns
    void setYColumns( const std::vector<int>& aycolumns,  bool updateNames  )
    {
        ycolumns.clear();
        for( auto cl:aycolumns )
            appendYColumn( cl );
        emit changedYSelections( updateNames );
    }

    /// Get line name
    std::string getName( size_t line ) const
    {
        //        return std::to_string(line);
        return m_model->headerData( getYColumn( line ) , Qt::Horizontal, Qt::DisplayRole ).toString().toStdString();
    }

    /// List of the column of the model that contains the x-coordinates of data points
    const std::vector<int>& XColumns() const
    { return xcolumns; }
    /// List of the column of the model that contains the y-coordinates of data points
    const std::vector<int>& YColumns() const
    { return ycolumns; }


#ifndef NO_JSONIO
    void toJsonNode( jsonio17::JsonBase& object ) const;
    void fromJsonNode( const jsonio17::JsonBase& object );
#else

    void toBsonObject( bson *obj ) const;
    void fromBsonObject( const char *obj );
#endif

    void toJsonObject( QJsonObject& json ) const;
    void fromJsonObject( const QJsonObject& json );

protected:

    /// Extern Table wir
    QAbstractTableModel *m_model;

    /// GRAPHTYPES ( LineChart,  AreaChart,  BarChart, Isolines ... )
    GRAPHTYPES graphType = LineChart;

    /// List of the column of the model that contains the x-coordinates of data points
    std::vector<int> xcolumns;
    /// List of the column of the model that contains the y-coordinates of data points
    std::vector<int> ycolumns;
    /// List that contains the x-coordinates of every chart line (y-coordinate).
    /// Must be the same size as ycolumns, internal for showAreaChart
    std::vector<int> y_xcolumns;

    QModelIndex mIndex(int row, int column ) const
    {
        if(column == 0 )
            return QModelIndex();
        else
            return m_model->index( row, column-1 );
    }

    QModelIndex mIndex(const QModelIndex &index) const
    {
        return mIndex( index.row(), index.column() );
    }

    QModelIndex indexM(const QModelIndex &m_index) const
    {
        if( m_index.isValid() )
            return index( m_index.row(), m_index.column()+1 );
        return QModelIndex();
    }

    void appendXColumn( int xclm )
    {
        if( xclm >= 0 && xclm < m_model->columnCount() )
            xcolumns.push_back(xclm);
    }

    void appendYColumn( int yclm )
    {
        if( yclm >= 0 && yclm < m_model->columnCount() )
            ycolumns.push_back(yclm);
    }

    ///  Clear list that contains the x-coordinates of every chart line (y-coordinate).
    void clearXColumn()
    {
        y_xcolumns.clear();
    }
    ///  Add line to list that contains the x-coordinates of every chart line (y-coordinate).
    void addXColumn( int line )
    {
        y_xcolumns.push_back(line);
    }

    friend class PlotChartViewPrivate;
};

} // namespace jsonui17

