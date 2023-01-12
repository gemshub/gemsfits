//-------------------------------------------------------------------
// $Id: ProjectSettingsDialog.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of ProjectSettingsDialog class
//
// Copyright (C) 2014  S.V.Dmytriyeva
// Uses EJDB (https://ejdb.org),
//    yaml-cpp (https://code.google.com/p/yaml-cpp/)
//
// This file is part of the GEMSFITS GUI, which uses the
// Qt v.5 cross-platform App & UI framework (http://qt-project.org)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of LGPL v.3 license
//
// See http://gems.web.psi.ch/GEMSFIT for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

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

    Ui::ProjectSettingsDialog *ui;
    QSettings *settings;

public slots:
    void CmSave();
    void CmProjectDir();
    void CmEJDBDir();
    void CmGEMSDir();
    void CmHelp();

public:
    explicit ProjectSettingsDialog(QSettings *aSet=0,  QWidget *parent = 0);
    ~ProjectSettingsDialog();

    QSettings *getNewSettings() const
    {
        return settings;
    }

};

#endif // PROJECTSETTINGSDIALOG_H
