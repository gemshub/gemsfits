#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include "json_model.h"
#include "ModelLineDialog.h"

namespace jsonui17 {

JsonModel::JsonModel( const std::string& json_string, const QStringList& header_names,  QObject* parent ):
    QAbstractItemModel(parent), header_data( header_names ), root_node( common::JsonFree::object())
{
    setupModelData(json_string);
    connect( this, SIGNAL( dataChanged( const QModelIndex&, const QModelIndex& ) ),
             parent,  SLOT( objectChanged()) );
}


common::JsonFree* JsonModel::lineFromIndex(const QModelIndex &index) const
{
    if( index.isValid() )  {
        return static_cast<common::JsonFree*>(index.internalPointer());
    }
    else {
        return const_cast<common::JsonFree*>(&root_node);
    }
}

void JsonModel::setupModelData(const std::string& json_string)
{
    try{
        beginResetModel();
        if( json_string.empty() )
            root_node.clear();
        else
            root_node = common::JsonFree::parse(json_string);
        endResetModel();
    }
    catch( ... )
    {
        endResetModel();
        throw;
    }
}

QModelIndex JsonModel::index( int row, int column, const QModelIndex &parent ) const
{
    auto parentItem = lineFromIndex( parent );
    if( parentItem->size() > 0 )
        return createIndex( row, column, parentItem->child_from_ndx(row) );
    else
        return QModelIndex();
}

QModelIndex JsonModel::parent(const QModelIndex& child) const
{
    if( !child.isValid() )
        return QModelIndex();

    auto childItem = lineFromIndex(child);
    auto parentItem = childItem->get_parent();
    if( parentItem == &root_node )
        return QModelIndex();
    return createIndex( parentItem->get_ndx(), 0, parentItem );
}

int JsonModel::rowCount( const QModelIndex& parent ) const
{
    if ( !parent.isValid() )
        return root_node.size();
    auto parentItem = lineFromIndex( parent );
    return parentItem->size();
}

int JsonModel::columnCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    return 2;
}

Qt::ItemFlags JsonModel::flags( const QModelIndex& index ) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    auto item = lineFromIndex( index );
    if( index.column() == 1 && ( !item->is_object() && !item->is_array() && !item->is_null() )) {
        flags |= Qt::ItemIsEditable;
        return flags;
    }
    else
        return (flags & ~Qt::ItemIsEditable);
}

QVariant JsonModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if( role == Qt::DisplayRole  && orientation == Qt::Horizontal && section< header_data.size() )
        return header_data[section];
    return QVariant();
}

QVariant JsonModel::data( const QModelIndex& index, int role ) const
{
    if( !index.isValid() )
        return QVariant();

    switch( role )  {
    case Qt::DisplayRole:
    case Qt::EditRole: {
        auto item = lineFromIndex( index );
        if( index.column() == 0 )
            return item->get_key().c_str();
        else
            if( index.column() == 1 )
                return item->get_field_value().c_str();
    }
        break;
    case Qt::ForegroundRole:
        if( index.column() == 0 ) {
            return QVariant( QColor( Qt::darkCyan ) );
        }
        break;
    default: break;
    }
    return QVariant();
}

bool JsonModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    if( index.isValid() && ( role == Qt::EditRole ) ) {
        if( index.column() == 1 ) {
            auto line = lineFromIndex( index );
            set_value_via_type(*line, line->type(), value);
        }
        return true;
    }
    return false;
}

//*****************************************************************

void JsonModel::resizeArray( QWidget* parentWidget, const QModelIndex& index )
{
    auto line =  lineFromIndex(index);
    ModelLineDialog dlg( line, parentWidget);
    if( !dlg.exec() )
        return;
    QVariant new_value = dlg.objectValue();
    auto new_size = dlg.arraySize();

    // message to delete old data
    if(rowCount(index)) {
        beginRemoveRows( index.siblingAtColumn(0), 0, rowCount( index ) );
        endRemoveRows();
    }
    line->array_resize( new_size, new_value.toString().toStdString() );
    // message to add lines
    if(new_size>0) {
        beginInsertRows( index.siblingAtColumn(0), 0, new_size-1 );
        endInsertRows();
    }
}

void JsonModel::delObject( QWidget* parentWidget, const QModelIndex& index )
{
    auto line =  lineFromIndex(index);
    QString msg = QString("Confirm deletion of %1 data object?").arg( line->get_key().c_str() );
    QMessageBox::StandardButton reply;

    reply = QMessageBox::question( parentWidget->window(), "Data object to delete", msg,
                                   QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No)
        return;
    auto* parent_line = line->get_parent();

    if( parent_line ) {
       beginRemoveRows( parent(index), index.row(), index.row() );
       parent_line->remove_child(line);
       endRemoveRows();
    }
}

// add object to and of object
const QModelIndex JsonModel::addObject( QWidget* parentWidget, const QModelIndex& cindex )
{
    int row;
    QModelIndex parentIndex;
    auto line =  lineFromIndex(cindex);
    if( line->is_object() && line->size() < 1 ) {
        parentIndex = cindex.siblingAtColumn(0);
        row = 0;
    }
    else {
        parentIndex = parent(cindex).siblingAtColumn(0);
        row =  rowCount( parentIndex );
    }

    ModelLineDialog dlg(nullptr, parentWidget);
    if( !dlg.exec() )
        return cindex;

    std::string new_object_key = dlg.objectName();
    common::JsonFree::Type new_object_type = dlg.objectType();
    QVariant new_object_value = dlg.objectValue();

    auto parent_item =  lineFromIndex(parentIndex);
    ErrorIf(parent_item->contains(new_object_key), "JsonFree", "Can't add existing key "+ new_object_key );
    try{
        // start insert row
        beginInsertRows( parentIndex, row, row );
        set_value_via_type((*parent_item)[new_object_key], new_object_type, new_object_value);
        endInsertRows();
    }
    catch( ... )
    {
        endInsertRows();
        throw;
    }
    return index( row, 0, parentIndex);
}

// add object to and of object
const QModelIndex JsonModel::cloneObject( QWidget*, const QModelIndex& cindex )
{
    QModelIndex parentIndex = parent(cindex);
    auto line =  lineFromIndex(cindex);

    auto parent =  lineFromIndex(parentIndex);
    if( !parent->is_array() )
        return cindex;
    int row = rowCount(parentIndex);
    try{
        beginInsertRows( parentIndex, row, row );
        parent->push_back(common::JsonFree(*line));
        endInsertRows();
        return index( row, 0, parentIndex);
    }
    catch( ... )
    {
        endInsertRows();
        throw;
    }
}

bool JsonModel::set_value_via_type(common::JsonFree& object, common::JsonFree::Type add_type, const QVariant &add_value)
{
    bool ok;
    switch( add_type )
    {
    case common::JsonFree::Null:
        object.set_null();
        break;
    case common::JsonFree::Bool:
        object = add_value.toString() == "true";
        break;
    case common::JsonFree::Int:
    {
        int ivalue = add_value.toInt(&ok);
        if( !ok )
            ivalue = 0;
        object = ivalue;
    }
        break;
    case common::JsonFree::Double:
    {
        auto dvalue = add_value.toDouble(&ok);
        if( !ok )
            dvalue = 0.;
        object = dvalue;
    }
        break;
    case common::JsonFree::String:
        object = add_value.toString().toStdString();
        break;
    case common::JsonFree::Object:
        object = common::JsonFree::object();
        break;
    case common::JsonFree::Array:
        object = common::JsonFree::array();
        break;
    }
    return true;
}

void  JsonModel::setFieldData( const QModelIndex& index, const std::string& data )
{
    auto line =  lineFromIndex(index);
    try {
        beginResetModel();
        if( data.empty() )
            line->clear();
        else
            *line = common::JsonFree::parse(data);
        endResetModel();
    }
    catch( ... )
    {
        endResetModel();
        throw;
    }
}

//-------------------------------------------------------------------------------------
// TJsonDomDelegate -  individuak items in views are rendered and edited using delegates
//-------------------------------------------------------------------------------------

QWidget *JsonDelegate::createEditor( QWidget *parent, const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    if( index.isValid() )
    {
        auto themodel = dynamic_cast<const JsonModel*>( index.model() );
        auto line =  themodel->lineFromIndex(index);
        if( index.column() > 0 )
        {
            switch( line->type()  )
            {
            case common::JsonFree::Bool:
            {
                QComboBox *accessComboBox = new QComboBox(parent);
                accessComboBox->addItem(tr("false"));
                accessComboBox->addItem(tr("true"));
                return accessComboBox;
            }
            case common::JsonFree::Int:
            case common::JsonFree::Double:
            {
                QLineEdit *lineEdit = new QLineEdit(parent);
                lineEdit->setValidator(new QDoubleValidator(lineEdit));
                return lineEdit;
            }
            case common::JsonFree::Null:
            case common::JsonFree::String:
            {
                QLineEdit *lineEdit = new QLineEdit(parent);
                return lineEdit;
            }
                // main constructions
            case common::JsonFree::Object:
            case common::JsonFree::Array:
            default:
                Error( "JsonFree", "Can't edit type "+ std::string(line->type_name()) );
            }
        }
    }
    return QAbstractItemDelegate::createEditor( parent, option,  index );
}

} // namespace jsonui17
