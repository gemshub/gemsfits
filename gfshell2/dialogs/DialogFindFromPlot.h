#ifndef DIALOGFINDFROMPLOT_H
#define DIALOGFINDFROMPLOT_H

#include <QDialog>
#include "fmodel.h"

namespace Ui {
class DialogFindFromPlot;
}

class DialogFindFromPlot : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFindFromPlot(GraphData* data, QWidget *parent = 0);
    ~DialogFindFromPlot();

    void getData( string& xname, string& yname, int *xyndx, double *reg );


private:
    Ui::DialogFindFromPlot *ui;
};

#endif // DIALOGFINDFROMPLOT_H
