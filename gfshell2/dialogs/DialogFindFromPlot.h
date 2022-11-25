#ifndef DIALOGFINDFROMPLOT_H
#define DIALOGFINDFROMPLOT_H

#include <QDialog>
#include "charts/graph_data.h"

namespace Ui {
class DialogFindFromPlot;
}

class DialogFindFromPlot : public QDialog
{
    Q_OBJECT

    const jsonui17::ChartData* grdata;
public slots:
    void ChangeIndex(int index);

public:
    explicit DialogFindFromPlot(const jsonui17::ChartData* data, QWidget *parent = 0);
    ~DialogFindFromPlot();

    void getData( int *xyndx, double *reg );

private:
    Ui::DialogFindFromPlot *ui;
};

#endif // DIALOGFINDFROMPLOT_H
