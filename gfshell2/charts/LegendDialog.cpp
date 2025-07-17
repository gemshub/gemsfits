#include <QColorDialog>
#include <QFontDialog>
#include "ui_LegendDialog4.h"
#include "charts/LegendDialog.h"
#include "charts/GraphDialog.h"
#include "charts/graph_data.h"

namespace jsonui17 {

LegendDialog::LegendDialog( GraphDialog* aGraph ):
    QDialog(aGraph), ui(new Ui::LegendDialogData),
    graph((aGraph->gr_data)), graphDlg(aGraph)
{
    ui->setupUi(this);

    std::string cap = "Customize Graph: ";
    cap += graph->title;
    setWindowTitle( cap.c_str() );
    setChartData();

    //ui->comboBox1->hide();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    connect( ui->comboBox1, &QComboBox::activated/*highlighted*/, this, &LegendDialog::CmChangeGraphType);
#else
    connect( ui->comboBox1, SIGNAL(activated(int)), this, SLOT(CmChangeGraphType(int)));
#endif
    connect( ui->pBtnChangeFont, &QPushButton::clicked, this, &LegendDialog::CmChangeLabelFont);
    connect( ui->buttomColor, &QPushButton::clicked, this, &LegendDialog::CmBackground);

    QObject::connect( ui->buttonBox, &QDialogButtonBox::accepted, this, &LegendDialog::CmOk);
    QObject::connect( ui->buttonBox, &QDialogButtonBox::rejected, this, &LegendDialog::reject);
    QObject::connect( ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &LegendDialog::CmApply);
    QObject::connect( ui->buttonBox, &QDialogButtonBox::helpRequested, this, &LegendDialog::CmHelp);
}


LegendDialog::~LegendDialog()
{
    delete ui;
}


void  LegendDialog::CmChangeGraphType( int /*new_type*/ )
{

}

void LegendDialog::CmOk()
{
    if( !applyToDialog() )
        return;
    accept();
}

void LegendDialog::CmApply()
{
    applyToDialog();
}

void LegendDialog::CmChangeLabelFont()
{
    bool ok;
    QFont selected_font = QFontDialog::getFont(&ok, labelFont, this);
    if( ok )
    {
        labelFont = selected_font;
        ui->pLabelFont->setText( labelFont.toString() );
    }
}

void LegendDialog::CmBackground()
{
    QColor cl = QColorDialog::getColor(backgroundColor, this);

    if( cl.isValid() )
    {
        QPalette pl = ui->pColor->palette();
        pl.setColor(QPalette::Window, cl);
        ui->pColor->setPalette(pl);
        backgroundColor = cl;
    }
}

void LegendDialog::CmHelp()
{
    helpWin( "LegendDialogJsonui", "" );
}


//------------------------------------------------------------------------------

void LegendDialog::setChartData()
{
    ui->pTitle->setText(graph->title.c_str());

    oldGraphType = graph->getGraphType();
    ui->comboBox1->setCurrentIndex( graph->getGraphType() );

    ui->pAxisX->setValue( graph->axisTypeX );
    ui->pAxisY->setValue( graph->axisTypeY );

    ui->pXname->setText( graph->xName.c_str() );
    ui->pYname->setText( graph->yName.c_str() );

    QString str;
    ui->pX0->setValidator( new QDoubleValidator( ui->pX0 ) );
    ui->pX0->setText( str.setNum (graph->region[0]) );

    ui->pXn->setValidator( new QDoubleValidator( ui->pXn ) );
    ui->pXn->setText( str.setNum (graph->region[1]) );

    ui->pY0->setValidator( new QDoubleValidator( ui->pY0 ) );
    ui->pY0->setText( str.setNum (graph->region[2]) );

    ui->pYn->setValidator( new QDoubleValidator( ui->pYn ) );
    ui->pYn->setText( str.setNum (graph->region[3]) );

    ui->pfX0->setValidator( new QDoubleValidator( ui->pfX0 ) );
    ui->pfX0->setText( str.setNum (graph->part[0]) );

    ui->pfXn->setValidator( new QDoubleValidator( ui->pfXn ) );
    ui->pfXn->setText( str.setNum (graph->part[1]) );

    ui->pfY0->setValidator( new QDoubleValidator( ui->pfY0 ) );
    ui->pfY0->setText( str.setNum (graph->part[2]) );

    ui->pfYn->setValidator( new QDoubleValidator( ui->pfYn ) );
    ui->pfYn->setText( str.setNum (graph->part[3]) );

    backgroundColor = graph->getBackgroundColor();
    ui->pColor->setAutoFillBackground(true);
    QPalette pl = ui->pColor->palette();
    pl.setColor( QPalette::Window, backgroundColor );
    ui->pColor->setPalette(pl);

    labelFont = graph->axisFont;
    ui->pLabelFont->setText(labelFont.toString());
}


bool LegendDialog::applyToDialog()
{
    double x0 =  ui->pX0->text().toDouble();
    double xn =  ui->pXn->text().toDouble();
    double y0 =  ui->pY0->text().toDouble();
    double yn =  ui->pYn->text().toDouble();

    double fx0 =  ui->pfX0->text().toDouble();
    double fxn =  ui->pfXn->text().toDouble();
    double fy0 =  ui->pfY0->text().toDouble();
    double fyn =  ui->pfYn->text().toDouble();

    graph->title = ui->pTitle->text().toStdString();
    graph->setGraphType( ui->comboBox1->currentIndex() );
    graph->axisTypeX = ui->pAxisX->value();
    graph->axisTypeY = ui->pAxisY->value();
    graph->xName = ui->pXname->text().toStdString();
    graph->yName = ui->pYname->text().toStdString();

    graph->region[0] = std::min(x0, xn);
    graph->region[1] = std::max(x0, xn);
    graph->region[2] = std::min(y0, yn);
    graph->region[3] = std::max(y0, yn);

    graph->part[0] = std::min(fx0, fxn);
    graph->part[1] = std::max(fx0, fxn);
    graph->part[2] = std::min(fy0, fyn);
    graph->part[3] = std::max(fy0, fyn);

    graph->axisFont = labelFont;
    graph->setBackgroundColor(backgroundColor);

    graphDlg->UpdateAll(nullptr);
    emit graphDlg->dataChanged( graph );

    return true;
}


} // namespace jsonui17

//--------------------- End of LegendDialog.cpp ---------------------------

