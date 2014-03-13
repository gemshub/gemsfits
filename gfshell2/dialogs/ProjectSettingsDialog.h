#ifndef PROJECTSETTINGSDIALOG_H
#define PROJECTSETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class ProjectSettingsDialog;
}

class ProjectSettingsDialog : public QDialog
{
    Q_OBJECT

    QSettings *settings;

public slots:
    void CmSave();
    void CmProjectDir();
    void CmEJDBDir();
    void CmGEMSDir();
    void CmHelp();

public:
    explicit ProjectSettingsDialog( QSettings *aSet=0,  QWidget *parent = 0);
    ~ProjectSettingsDialog();

    QSettings *getNewSettings() const
    {
      return settings;
    }

private:
    Ui::ProjectSettingsDialog *ui;
};

#endif // PROJECTSETTINGSDIALOG_H
