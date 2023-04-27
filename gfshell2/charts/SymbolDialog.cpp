
#include <QColorDialog>
#include <QRadioButton>
#include "ui_SymbolDialog4.h"
#include "charts/SymbolDialog.h"

namespace jsonui17 {

SymbolDialog::SymbolDialog( const SeriesLineData& aData, QWidget* parent):
    QDialog( parent ), linedata(aData),
    ui(new Ui::SymbolDialogData)
{
    ui->setupUi(this);

    smbGroup = new QButtonGroup();
    smbGroup->setExclusive(true);

    // make symbol icons by program
    QRadioButton *pp;
    SeriesLineData plot_( "Scale",  0, 40, 0, 0, 0, 60 );
    for(int ii=0; ii<20; ii++ )
    {
        pp = new QRadioButton(ui->pSymbols);
        plot_.setMarkerShape( ii );
        pp->setIcon(markerShapeIcon(plot_));
        smbGroup->addButton(pp, ii);
        ui->gridLayout->addWidget(pp, ii/5, ii%5, 1, 1);
    }

    ui->pColor->setAutoFillBackground( true);
    QPalette pl = ui->pColor->palette();
    pl.setColor( QPalette::Window, linedata.getColor() );
    ui->pColor->setPalette(pl);

    ui->pLineSize->setValue( linedata.getPenSize() );
    ui->pLineStyle->setCurrentIndex( linedata.getPenStyle() );
    ui->pSpline->setChecked( linedata.getSpline() );
    ui->pSymbolSize->setValue( linedata.getMarkerSize() );
    smbGroup->button( linedata.getMarkerShape()%20 )->setChecked( true );

    std::string s  = "Customize plot curve: ";
    s += linedata.getName();
    setWindowTitle(s.c_str());

    QObject::connect( ui->buttonBox, &QDialogButtonBox::accepted, this, &SymbolDialog::accept);
    QObject::connect( ui->buttonBox, &QDialogButtonBox::rejected, this, &SymbolDialog::reject);
    QObject::connect( ui->buttomColor, &QPushButton::clicked, this, &SymbolDialog::CmSelectColor);
}

SymbolDialog::~SymbolDialog()
{
    delete ui;
}


SeriesLineData& SymbolDialog::GetPlotLine()
{
    int type = smbGroup->checkedId();
    QColor cl = ui->pColor->palette().color(QPalette::Window); //backgroundColor();
    linedata.setChanges( type, ui->pSymbolSize->value(), ui->pLineSize->value(),
                         ui->pLineStyle->currentIndex(), ui->pSpline->isChecked(), cl );
    return linedata;
}

void SymbolDialog::CmSelectColor()
{
    QPalette pl = ui->pColor->palette();
    QColor cl = QColorDialog::getColor(pl.color(QPalette::Window), this);

    if( cl.isValid() )
    {   pl.setColor( QPalette::Window, cl);
        ui->pColor->setPalette(pl);
    }
}

} // namespace jsonui17

//--------------------- End of SymbolDialog.cpp ---------------------------

