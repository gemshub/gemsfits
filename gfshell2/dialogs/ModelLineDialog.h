#pragma once

#include <QDialog>
#include <QAbstractItemModel>
#include <QItemDelegate>
#include "v_json.h"

namespace Ui {
class ModelLineDialog;
}

namespace jsonio17 {
class FieldDef;
}

namespace jsonui17 {

class JsonModel;


/// \class ModelLineDialog provides a dialog that allow users to difine json object
class ModelLineDialog : public QDialog
{
    Q_OBJECT

protected slots:
    void setDatabyType( int ndx );

public:
    explicit ModelLineDialog( common::JsonFree* line, QWidget *parentWidget = nullptr);
    ~ModelLineDialog();

    /// Get object name
    std::string objectName() const;
    /// Get object type
    common::JsonFree::Type objectType() const;
    /// Get default value of object
    QVariant objectValue() const;
    /// Get object size (if object is array type)
    std::size_t arraySize() const;

private:
    Ui::ModelLineDialog *ui;
    void set_editor_by_type( common::JsonFree::Type jsontype );
};

} // namespace jsonui17

