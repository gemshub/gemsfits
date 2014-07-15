/********************************************************************************
** Form generated from reading UI file 'ProjectSettingsDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROJECTSETTINGSDIALOG_H
#define UI_PROJECTSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>

QT_BEGIN_NAMESPACE

class Ui_ProjectSettingsDialog
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *projDir;
    QToolButton *projDirButton;
    QLabel *label_2;
    QLineEdit *projName;
    QLabel *label_3;
    QLineEdit *ejdbDir;
    QToolButton *ejdbDirButton;
    QLabel *label_4;
    QLineEdit *ejdbName;
    QLabel *label_5;
    QLineEdit *experCollect;
    QLabel *label_6;
    QLineEdit *taskCollection;
    QLabel *label_9;
    QLineEdit *fitCollection;
    QLabel *label_7;
    QLineEdit *gemsDir;
    QToolButton *gemsDirButton;
    QLabel *label_8;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ProjectSettingsDialog)
    {
        if (ProjectSettingsDialog->objectName().isEmpty())
            ProjectSettingsDialog->setObjectName(QStringLiteral("ProjectSettingsDialog"));
        ProjectSettingsDialog->setWindowModality(Qt::WindowModal);
        ProjectSettingsDialog->resize(428, 498);
        QIcon icon;
        icon.addFile(QStringLiteral(":/modules/Icons/ModelingProjectIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        ProjectSettingsDialog->setWindowIcon(icon);
        ProjectSettingsDialog->setModal(false);
        gridLayout = new QGridLayout(ProjectSettingsDialog);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(ProjectSettingsDialog);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        projDir = new QLineEdit(ProjectSettingsDialog);
        projDir->setObjectName(QStringLiteral("projDir"));

        gridLayout->addWidget(projDir, 1, 0, 1, 1);

        projDirButton = new QToolButton(ProjectSettingsDialog);
        projDirButton->setObjectName(QStringLiteral("projDirButton"));

        gridLayout->addWidget(projDirButton, 1, 1, 1, 1);

        label_2 = new QLabel(ProjectSettingsDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        projName = new QLineEdit(ProjectSettingsDialog);
        projName->setObjectName(QStringLiteral("projName"));

        gridLayout->addWidget(projName, 3, 0, 1, 1);

        label_3 = new QLabel(ProjectSettingsDialog);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        ejdbDir = new QLineEdit(ProjectSettingsDialog);
        ejdbDir->setObjectName(QStringLiteral("ejdbDir"));

        gridLayout->addWidget(ejdbDir, 5, 0, 1, 1);

        ejdbDirButton = new QToolButton(ProjectSettingsDialog);
        ejdbDirButton->setObjectName(QStringLiteral("ejdbDirButton"));

        gridLayout->addWidget(ejdbDirButton, 5, 1, 1, 1);

        label_4 = new QLabel(ProjectSettingsDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 6, 0, 1, 1);

        ejdbName = new QLineEdit(ProjectSettingsDialog);
        ejdbName->setObjectName(QStringLiteral("ejdbName"));

        gridLayout->addWidget(ejdbName, 7, 0, 1, 1);

        label_5 = new QLabel(ProjectSettingsDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 8, 0, 1, 1);

        experCollect = new QLineEdit(ProjectSettingsDialog);
        experCollect->setObjectName(QStringLiteral("experCollect"));

        gridLayout->addWidget(experCollect, 9, 0, 1, 1);

        label_6 = new QLabel(ProjectSettingsDialog);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 10, 0, 1, 1);

        taskCollection = new QLineEdit(ProjectSettingsDialog);
        taskCollection->setObjectName(QStringLiteral("taskCollection"));

        gridLayout->addWidget(taskCollection, 11, 0, 1, 1);

        label_9 = new QLabel(ProjectSettingsDialog);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout->addWidget(label_9, 12, 0, 1, 1);

        fitCollection = new QLineEdit(ProjectSettingsDialog);
        fitCollection->setObjectName(QStringLiteral("fitCollection"));

        gridLayout->addWidget(fitCollection, 13, 0, 1, 1);

        label_7 = new QLabel(ProjectSettingsDialog);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 14, 0, 1, 1);

        gemsDir = new QLineEdit(ProjectSettingsDialog);
        gemsDir->setObjectName(QStringLiteral("gemsDir"));

        gridLayout->addWidget(gemsDir, 15, 0, 1, 1);

        gemsDirButton = new QToolButton(ProjectSettingsDialog);
        gemsDirButton->setObjectName(QStringLiteral("gemsDirButton"));

        gridLayout->addWidget(gemsDirButton, 15, 1, 1, 1);

        label_8 = new QLabel(ProjectSettingsDialog);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout->addWidget(label_8, 16, 0, 1, 1);

        buttonBox = new QDialogButtonBox(ProjectSettingsDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Help|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 17, 0, 1, 1);


        retranslateUi(ProjectSettingsDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), ProjectSettingsDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ProjectSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *ProjectSettingsDialog)
    {
        ProjectSettingsDialog->setWindowTitle(QApplication::translate("ProjectSettingsDialog", "GEMSFITS Project Setup", 0));
        label->setText(QApplication::translate("ProjectSettingsDialog", "Project folder path", 0));
        projDir->setText(QApplication::translate("ProjectSettingsDialog", ".", 0));
        projDirButton->setText(QApplication::translate("ProjectSettingsDialog", "...", 0));
        label_2->setText(QApplication::translate("ProjectSettingsDialog", "Project name", 0));
        projName->setText(QApplication::translate("ProjectSettingsDialog", "myproj1", 0));
        label_3->setText(QApplication::translate("ProjectSettingsDialog", "Database path", 0));
        ejdbDir->setText(QApplication::translate("ProjectSettingsDialog", "/EJDB", 0));
        ejdbDirButton->setText(QApplication::translate("ProjectSettingsDialog", "...", 0));
        label_4->setText(QApplication::translate("ProjectSettingsDialog", "Database file name", 0));
        ejdbName->setText(QApplication::translate("ProjectSettingsDialog", "myprojdb1", 0));
        label_5->setText(QApplication::translate("ProjectSettingsDialog", "Experimental samples database collection name", 0));
        experCollect->setText(QApplication::translate("ProjectSettingsDialog", "experiments", 0));
        label_6->setText(QApplication::translate("ProjectSettingsDialog", "Fitting tasks database collection name", 0));
        taskCollection->setText(QApplication::translate("ProjectSettingsDialog", "tasks", 0));
        label_9->setText(QApplication::translate("ProjectSettingsDialog", "Fitting results database collection name", 0));
        fitCollection->setText(QApplication::translate("ProjectSettingsDialog", "fits", 0));
        label_7->setText(QApplication::translate("ProjectSettingsDialog", "GEMS3K input file set path", 0));
        gemsDir->setText(QApplication::translate("ProjectSettingsDialog", "/GEMS", 0));
        gemsDirButton->setText(QApplication::translate("ProjectSettingsDialog", "...", 0));
        label_8->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ProjectSettingsDialog: public Ui_ProjectSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROJECTSETTINGSDIALOG_H
