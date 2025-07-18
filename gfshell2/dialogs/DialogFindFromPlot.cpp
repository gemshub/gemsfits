#include <QComboBox>
#include "DialogFindFromPlot.h"
#include "ui_DialogFindFromPlot.h"

DialogFindFromPlot::DialogFindFromPlot(const jsonui17::ChartData* data, QWidget *parent):
    QDialog(parent),
    ui(new Ui::DialogFindFromPlot), grdata(data)
{
    ui->setupUi(this);

    if( data )
    {
        ui->xminVal->setMaximum(data->region[1]);
        ui->xminVal->setMinimum(data->region[0]);
        ui->xminVal->setValue(data->part[0]);

        ui->xmaxVal->setMaximum(data->region[1]);
        ui->xmaxVal->setMinimum(data->region[0]);
        ui->xmaxVal->setValue(data->part[1]);

        ui->yminVal->setMaximum(data->region[3]);
        ui->yminVal->setMinimum(data->region[2]);
        ui->yminVal->setValue(data->part[2]);

        ui->ymaxVal->setMaximum(data->region[3]);
        ui->ymaxVal->setMinimum(data->region[2]);
        ui->ymaxVal->setValue(data->part[3]);

        for(size_t ii=0; ii<data->linesNumber(); ii++)
            ui->namesBox->addItem( data->lineData(ii).getName().c_str());

        ChangeIndex(0);
        QObject::connect(ui->namesBox, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeIndex(int)));
    }
}

DialogFindFromPlot::~DialogFindFromPlot()
{
    delete ui;
}

void DialogFindFromPlot::getData(int *xyndx, double *reg)
{
    xyndx[0] = ui->xNum->value();
    xyndx[1] = ui->yNum->value();
    reg[0] = ui->xminVal->value();
    reg[1] = ui->xmaxVal->value();
    reg[2] = ui->yminVal->value();
    reg[3] = ui->xmaxVal->value();
}

void DialogFindFromPlot::ChangeIndex(int index)
{
    int xc=-1, yc=0;
    grdata->getXYColumns(index, xc, yc);
    ui->xNum->setValue(xc);
    ui->yNum->setValue(yc);
}
