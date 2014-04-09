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

    GraphData* grdata;
public slots:
    void ChangeIndex(int index);

public:
    explicit DialogFindFromPlot(GraphData* data, QWidget *parent = 0);
    ~DialogFindFromPlot();

    void getData( int *xyndx, double *reg );

private:
    Ui::DialogFindFromPlot *ui;
};

#endif // DIALOGFINDFROMPLOT_H
