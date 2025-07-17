#pragma once

#include <QTreeView>
#include "json_model.h"

namespace jsonui17 {

class ExpandedLevelLine;

/// \class JsonView implements a tree view of json based structure
/// that displays items from a JsonBaseModel model.
class JsonView: public QTreeView
{
    Q_OBJECT

public slots:

    void CmCalc();
    void CmAddObject();
    void CmDelObject();
    void CmResizeArray();
    void CmCloneObject();

    void CopyFieldPath();
    void CopyField();
    void PasteField();

protected slots:

    void slotPopupContextMenu( const QPoint& pos );
    void changeCurrent( int section );

public:

    /// Extern flag to keep data fields expanded
    static bool expandedFields;

    /// Constructor
    JsonView(QWidget* parent = nullptr);

    /// Update model data with restore Expanded Level
    void updateModelData( const std::string& json_string );

    /// Reset model data with restore Expanded Level
    void resetModel();

    /// Return internal data to const link
    const common::JsonFree& current_object()
    {
        return  view_model()->current_object();
    }

    /// Return internal data to json string
    virtual std::string saveToJson()
    {
        return current_object().dump();
    }

protected:

    const JsonModel* current_index_model( const QModelIndex& index ) const
    {
        if( index.isValid() )
            return dynamic_cast<const JsonModel*>( index.model() );
        return nullptr;
    }

    JsonModel* view_model()
    {
        return dynamic_cast<JsonModel*>( model() );
    }

    void keyPressEvent(QKeyEvent* e) override;

    void save_expanded_state(std::shared_ptr<ExpandedLevelLine> topList);
    void restore_expanded_state(std::shared_ptr<ExpandedLevelLine> topList);
    void save_expanded_on_level(const QModelIndex& index, std::shared_ptr<ExpandedLevelLine> parent);
    void restore_expanded_on_level(const QModelIndex& index, std::shared_ptr<ExpandedLevelLine> top_data);
};

} // namespace jsonui17

