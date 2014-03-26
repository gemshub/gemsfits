#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

    QSettings *settings;

public slots:
    void CmSave();
    void CmProjectDir();
    void CmResourceDir();
    void CmHelpFile();
    void CmGemsFit2File();
    void CmGenerateHelp();
    void CmHelp();

public:
    explicit PreferencesDialog(QSettings *aSet=0,QWidget *parent = 0);
    ~PreferencesDialog();

private:
    Ui::PreferencesDialog *ui;
};

#endif // PREFERENCESDIALOG_H
