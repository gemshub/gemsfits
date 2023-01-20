
#include <QJsonArray>
#include "charts/chart_model.h"
#include "charts/graph_data.h"

namespace jsonui17 {

ChartDataModel::ChartDataModel( QAbstractTableModel *tableModel, QObject *parent) :
    QAbstractTableModel(parent), m_model(tableModel)
{
    //    connect signals from the model
    connect(m_model, &QAbstractTableModel::dataChanged, this, &ChartDataModel::modelUpdated);
    connect(m_model, &QAbstractTableModel::rowsInserted, this, &ChartDataModel::modelRowsAdded);
    connect(m_model, &QAbstractTableModel::rowsRemoved, this, &ChartDataModel::modelRowsRemoved);
    connect(m_model, &QAbstractTableModel::columnsInserted, this, &ChartDataModel::modelColumnsAdded);
    connect(m_model, &QAbstractTableModel::columnsRemoved, this, &ChartDataModel::modelColumnsRemoved);
    connect(m_model, &QAbstractTableModel::modelReset, this, &ChartDataModel::modelReset);
    connect(m_model, &QAbstractTableModel::destroyed, this, &ChartDataModel::destroyed);
}


int ChartDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_model->rowCount();
}

int ChartDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_model->columnCount()+1;
}

QVariant ChartDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section == 0)
            return "Index";
        else
            return m_model->headerData( section-1, orientation, role );
    } else {
        return m_model->headerData( section, orientation, role );
    }
}

QVariant ChartDataModel::data(const QModelIndex &index, int role) const
{
    switch( graphType )
    {
    default:
        break;

    case LineChart:
        if (role == Qt::DisplayRole)
        {
            if( index.column() == 0 )
                return index.row();
            return m_model->data( mIndex( index), role);
        }
        else if (role == Qt::EditRole)
        {
            if( index.column() > 0 )
                return m_model->data( mIndex( index), role);
        }
        break;

    case AreaChart:
        if (role == Qt::DisplayRole  )
        {
            if( index.column() == 0 )
            {
                return index.row();
            }
            else
            {
                size_t ii;
                auto nline = index.column()-1;
                for ( ii=0; ii<xcolumns.size(); ii++)
                {
                    if( xcolumns[ii] == nline)  // abscissa
                        return m_model->data( mIndex(index), role);
                }
                double value=0;
                for ( ii=0; ii<ycolumns.size(); ii++)
                {
                    if( getXColumn( y_xcolumns[ii] ) >= -1  )
                    {
                        auto val = m_model->data( mIndex(index.row(), ycolumns[ii]+1), role).toDouble();
                        if( val > std::numeric_limits<double>::lowest() )
                            value += m_model->data( mIndex(index.row(), ycolumns[ii]+1), role).toDouble();
                    }
                    if( ycolumns[ii] == nline)  // ordinate
                        return value;
                }
            }
        }
        break;
    }
    return QVariant();

}

bool ChartDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole && index.column() > 0 )
    {
        m_model->setData( mIndex(index), value, role);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags ChartDataModel::flags(const QModelIndex &index) const
{
    if( index.column() > 0 )
        return m_model->flags( mIndex(index) );
    return QAbstractItemModel::flags(index);
}

// Connect m_model signald

void ChartDataModel::modelUpdated(QModelIndex mtopLeft, QModelIndex mbottomRight)
{
    emit dataChanged(indexM(mtopLeft), indexM(mbottomRight));
}

void ChartDataModel::modelRowsAdded(QModelIndex m_parent, int start, int end)
{
    beginInsertRows(indexM(m_parent), start, end);
    endInsertRows();
}

void ChartDataModel::modelRowsRemoved(QModelIndex m_parent, int start, int end)
{
    beginRemoveRows(indexM(m_parent), start, end);
    endRemoveRows();
}

void ChartDataModel::modelColumnsAdded(QModelIndex m_parent, int start, int end)
{
    beginInsertColumns(indexM(m_parent), start, end);
    endInsertColumns();
}

void ChartDataModel::modelColumnsRemoved(QModelIndex m_parent, int start, int end)
{
    beginRemoveColumns(indexM(m_parent), start, end);
    endRemoveColumns();
}

// ------------------------ work with selection

nlohmann::json ChartDataModel::toBsonObject() const
{
    nlohmann::json object;
    object["gxclms"] = xcolumns;
    object["gyclms"] = ycolumns;
    return object;
}

void ChartDataModel::fromBsonObject( const nlohmann::json& object )
{
    xcolumns.clear();
    xcolumns = object.value("gxclms", xcolumns);
    ycolumns.clear();
    ycolumns = object.value("gyclms", ycolumns);
}

void ChartDataModel::toJsonObject(QJsonObject& json) const
{
    QJsonArray xArray;
    for( auto it: xcolumns )
        xArray.append(it);
    json["gxclms"] = xArray;
    QJsonArray yArray;
    for( auto it: ycolumns )
        yArray.append(it);
    json["gyclms"] = yArray;
}


void ChartDataModel::fromJsonObject(const QJsonObject& json)
{
    std::vector<int> columns;
    QJsonArray array = json["gxclms"].toArray();
    for(int ii=0; ii<array.size(); ii++)
        columns.push_back(array[ii].toInt());
    setXColumns( columns );
    columns.clear();
    array = json["gyclms"].toArray();
    for(int ii=0; ii<array.size(); ii++)
        columns.push_back(array[ii].toInt());
    setYColumns( columns, false );
}

} // namespace jsonui17
