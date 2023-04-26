#include <QSpinBox>
#include "ModelLineDialog.h"
#include "ui_ModelLineDialog.h"
#include "json_model.h"


namespace jsonui17 {


ModelLineDialog::ModelLineDialog( common::JsonFree* itline,  QWidget *parentWidget):
    QDialog( parentWidget ), ui(new Ui::ModelLineDialog)
{
    ui->setupUi(this);

    if( itline ) {
        ui->nameEdit->setText(itline->get_key().c_str());
        ui->nameEdit->setEnabled(false);
        ui->typeBox->setCurrentText("array");
        setDatabyType(5);

        if( itline->is_array() ) {
           ui->sizeBox->setValue(itline->size());
           if( itline->size()>0 ) {
               ui->defEdit->setText(itline->child_from_ndx(0)->dump(true).c_str());
           }
        }
    }
    else {
        ui->nameEdit->setText("");
        ui->nameEdit->setEnabled(true);
        ui->sizeBox->setValue(0);
        ui->defEdit->setText("");
        ui->typeBox->setCurrentText("string");
        setDatabyType(0);
    }

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QObject::connect( ui->typeBox, &QComboBox::currentIndexChanged,
                      this, &ModelLineDialog::setDatabyType);
#else
    QObject::connect( ui->typeBox, SIGNAL(currentIndexChanged(int)),
                      this, SLOT(setDatabyType(int)));
#endif
}

ModelLineDialog::~ModelLineDialog()
{
    delete ui;
}

void ModelLineDialog::setDatabyType( int )
{
    auto jsontype = objectType();
    set_editor_by_type( jsontype );
    ui->sizeBox->setEnabled(jsontype == common::JsonFree::Array);
}

void ModelLineDialog::set_editor_by_type(common::JsonFree::Type jstype)
{
    ui->defEdit->setEnabled( true );
    switch(jstype) {
    case common::JsonFree::Double:
        ui->defEdit->setValidator(new QDoubleValidator(ui->defEdit));
        ui->defEdit->setText("0.0");
        break;
    default:
        ui->defEdit->setValidator( nullptr );
        ui->defEdit->setText("");
        break;
    }
}

common::JsonFree::Type ModelLineDialog::objectType() const
{
    auto new_type = common::JsonFree::Null;
    switch(ui->typeBox->currentIndex()) {
    case 0:  new_type = common::JsonFree::String;
        break;
    case 1:  new_type = common::JsonFree::Double;
        break;
    case 2:  new_type = common::JsonFree::Bool;
        break;
    case 3:  new_type = common::JsonFree::Object;
        break;
    case 4:  new_type = common::JsonFree::Array;
        break;
    }
    return new_type;
}

std::string ModelLineDialog::objectName() const
{
    return ui->nameEdit->text().toStdString();
}

std::size_t ModelLineDialog::arraySize() const
{
    return ui->sizeBox->value();
}

QVariant ModelLineDialog::objectValue() const
{
    bool ok;
    QVariant value;
    auto jstype = objectType();
    switch(jstype) {
    case common::JsonFree::Null:
        break;
    case common::JsonFree::Bool:
        value = ( ui->defEdit->text() == "true" ? true : false );
        break;
    case common::JsonFree::String:
        value = ui->defEdit->text();
        break;
    case common::JsonFree::Double:
        value = ui->defEdit->text().toDouble(&ok);
        if( !ok )
            value = 0.0;
        break;
    case common::JsonFree::Object:
        value = ui->defEdit->text();
        break;
    case common::JsonFree::Array:
        value = ui->defEdit->text();
        break;
    }
    return value;
}


} // namespace jsonui17
