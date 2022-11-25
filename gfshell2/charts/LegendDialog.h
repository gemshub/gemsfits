#pragma once

#include <QDialog>

namespace Ui {
class LegendDialogData;
}

namespace jsonui17 {

class ChartData;
class GraphDialog;

/// \class LegendDialog dialog to edit plotting system description
class LegendDialog :public QDialog
{
    Q_OBJECT

    bool applyToDialog();
    void setChartData();

public:
    LegendDialog( GraphDialog* aGraph );
    ~LegendDialog();

protected slots:

    void CmBackground();
    void CmChangeLabelFont();
    void CmChangeGraphType(int);

    void CmApply();
    void CmHelp();
    void CmOk();

private:

    Ui::LegendDialogData *ui;
    ChartData* graph;
    GraphDialog* graphDlg;

    QColor backgroundColor;
    QFont labelFont;

    int oldGraphType;
};

} // namespace jsonui17

