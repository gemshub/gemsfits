#pragma once

#include <memory>
#include <QItemDelegate>
#include "v_json.h"

namespace jsonui17 {


/// \class JsonModel
/// Class for represents the data set and is responsible for fetching the data
/// is needed for viewing and for writing back any changes.
/// Reading/writing data from/to json objects
class JsonModel: public QAbstractItemModel
{
    Q_OBJECT

    friend class JsonDelegate;
    friend class ModelLineDialog;

public:

    explicit JsonModel( const std::string& json_string, const QStringList& header_names, QObject* parent = nullptr );
    ~JsonModel() {}

    /// Return internal data to const link
    const common::JsonFree& current_object() const
    {
        return  root_node;
    }

    /// Extern update data
    void setupModelData(const std::string& json_string);

    bool isCanBeResized( const QModelIndex& index ) const
    {
        return  lineFromIndex(index)->is_array();
    }
    bool isCanBeAdd( const QModelIndex& index ) const
    {
        auto line = lineFromIndex(index);
        return ( line->is_top() || line->get_parent()->is_object() ||
                 ( line->is_object()  && line->size() < 1 ) ) ;
    }
    bool isCanBeRemoved( const QModelIndex& index ) const
    {
        auto line = lineFromIndex(index);
        return  !line->is_top() && line->get_parent()->is_structured() ;
    }
    bool isCanBeCloned( const QModelIndex& index ) const
    {
        auto line = lineFromIndex(index);
        return  !line->is_top() && line->get_parent()->is_array();
    }
    bool isNumber( const QModelIndex& index ) const
    {
        auto line = lineFromIndex(index);
        return  line->is_number();
    }

    void resizeArray( QWidget* parent, const QModelIndex& index );
    void delObject( QWidget* parent, const QModelIndex& index );
    const QModelIndex addObject( QWidget* parent, const QModelIndex& index );
    const QModelIndex cloneObject( QWidget* parent, const QModelIndex& index );
    std::string  getFieldData( const QModelIndex& index ) const
    {
        return lineFromIndex(index)->dump();
    }
    virtual std::string  getFieldPath( const QModelIndex& index ) const
    {
        return lineFromIndex(index)->get_path();
    }
    void setFieldData(const QModelIndex &index, const std::string &data);
    void resetModelData()
    {
        beginResetModel();
        endResetModel();
    }

    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount( const QModelIndex& parent ) const override;
    int columnCount( const QModelIndex& parent  ) const override;
    QVariant data( const QModelIndex& index, int role ) const override;
    bool setData( const QModelIndex& index, const QVariant& value, int role ) override;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;
    Qt::ItemFlags flags( const QModelIndex& index ) const override;

protected:

    /// Names of columns
    QStringList header_data;
    /// Current document object
    common::JsonFree root_node;

    /// Return internal data link
    common::JsonFree& current_data() const
    {
        return  const_cast<common::JsonFree&>(root_node);
    }

    common::JsonFree* lineFromIndex(const QModelIndex& index) const;
    bool set_value_via_type( common::JsonFree& object, common::JsonFree::Type add_type, const QVariant& add_value );
};


/// \class JsonDelegate individual items in views are rendered and edited using delegates
class JsonDelegate: public QItemDelegate
{
    Q_OBJECT

public:

    JsonDelegate( QObject * parent = nullptr ):
        QItemDelegate( parent ) {}
    QWidget *createEditor( QWidget *parent,
                           const QStyleOptionViewItem &option,
                           const QModelIndex &index) const override;
};


} // namespace jsonui17

