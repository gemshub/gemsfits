#pragma once

#include <QDialog>
#include <QButtonGroup>
#include "graph_data.h"

namespace Ui {
class SymbolDialogData;
}

namespace jsonui17 {

/// \class SymbolDialog - dialog to edit one plot line descriptions
class SymbolDialog : public QDialog
{
    Q_OBJECT

protected slots:
    virtual void CmSelectColor();

public:

    SymbolDialog(const SeriesLineData& aData, QWidget* parent = nullptr );
    virtual ~SymbolDialog();

    SeriesLineData& GetPlotLine();

private:

    SeriesLineData linedata;
    Ui::SymbolDialogData *ui;
    QButtonGroup *smbGroup;

};

} // namespace jsonui17

