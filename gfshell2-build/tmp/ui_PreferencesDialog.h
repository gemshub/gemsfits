/********************************************************************************
** Form generated from reading UI file 'PreferencesDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCESDIALOG_H
#define UI_PREFERENCESDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_PreferencesDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *resourcesEdit;
    QToolButton *resourcesButton;
    QLabel *label_2;
    QLineEdit *usersEdit;
    QToolButton *usersButton;
    QLabel *label_3;
    QLineEdit *helpEdit;
    QToolButton *helpButton;
    QPushButton *pushButton;
    QLabel *label_5;
    QLineEdit *gemsfit2Edit;
    QToolButton *gemsfit2Button;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QCheckBox *commentsBox;
    QCheckBox *turnoff;
    QCheckBox *yamlBox;
    QLabel *label_4;
    QComboBox *experimentsBox;
    QLabel *label_6;
    QComboBox *searchBox;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *PreferencesDialog)
    {
        if (PreferencesDialog->objectName().isEmpty())
            PreferencesDialog->setObjectName(QStringLiteral("PreferencesDialog"));
        PreferencesDialog->resize(521, 405);
        QIcon icon;
        icon.addFile(QStringLiteral(":/menu/Icons/RemakeRecordIcon.png"), QSize(), QIcon::Normal, QIcon::Off);
        PreferencesDialog->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(PreferencesDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        groupBox = new QGroupBox(PreferencesDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        resourcesEdit = new QLineEdit(groupBox);
        resourcesEdit->setObjectName(QStringLiteral("resourcesEdit"));

        gridLayout->addWidget(resourcesEdit, 0, 1, 1, 1);

        resourcesButton = new QToolButton(groupBox);
        resourcesButton->setObjectName(QStringLiteral("resourcesButton"));

        gridLayout->addWidget(resourcesButton, 0, 2, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        usersEdit = new QLineEdit(groupBox);
        usersEdit->setObjectName(QStringLiteral("usersEdit"));

        gridLayout->addWidget(usersEdit, 1, 1, 1, 1);

        usersButton = new QToolButton(groupBox);
        usersButton->setObjectName(QStringLiteral("usersButton"));

        gridLayout->addWidget(usersButton, 1, 2, 1, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        helpEdit = new QLineEdit(groupBox);
        helpEdit->setObjectName(QStringLiteral("helpEdit"));

        gridLayout->addWidget(helpEdit, 2, 1, 1, 1);

        helpButton = new QToolButton(groupBox);
        helpButton->setObjectName(QStringLiteral("helpButton"));

        gridLayout->addWidget(helpButton, 2, 2, 1, 1);

        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        gridLayout->addWidget(pushButton, 2, 3, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 3, 0, 1, 1);

        gemsfit2Edit = new QLineEdit(groupBox);
        gemsfit2Edit->setObjectName(QStringLiteral("gemsfit2Edit"));

        gridLayout->addWidget(gemsfit2Edit, 3, 1, 1, 1);

        gemsfit2Button = new QToolButton(groupBox);
        gemsfit2Button->setObjectName(QStringLiteral("gemsfit2Button"));

        gridLayout->addWidget(gemsfit2Button, 3, 2, 1, 1);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(PreferencesDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        commentsBox = new QCheckBox(groupBox_2);
        commentsBox->setObjectName(QStringLiteral("commentsBox"));

        gridLayout_2->addWidget(commentsBox, 0, 0, 1, 2);

        turnoff = new QCheckBox(groupBox_2);
        turnoff->setObjectName(QStringLiteral("turnoff"));

        gridLayout_2->addWidget(turnoff, 1, 0, 1, 2);

        yamlBox = new QCheckBox(groupBox_2);
        yamlBox->setObjectName(QStringLiteral("yamlBox"));

        gridLayout_2->addWidget(yamlBox, 2, 0, 1, 2);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 3, 0, 1, 1);

        experimentsBox = new QComboBox(groupBox_2);
        experimentsBox->setObjectName(QStringLiteral("experimentsBox"));

        gridLayout_2->addWidget(experimentsBox, 3, 1, 1, 1);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_2->addWidget(label_6, 4, 0, 1, 1);

        searchBox = new QComboBox(groupBox_2);
        searchBox->setObjectName(QStringLiteral("searchBox"));

        gridLayout_2->addWidget(searchBox, 4, 1, 1, 1);


        verticalLayout->addWidget(groupBox_2);

        buttonBox = new QDialogButtonBox(PreferencesDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Help|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(PreferencesDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), PreferencesDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), PreferencesDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(PreferencesDialog);
    } // setupUi

    void retranslateUi(QDialog *PreferencesDialog)
    {
        PreferencesDialog->setWindowTitle(QApplication::translate("PreferencesDialog", "GEMSFITS Preferences", 0));
        groupBox->setTitle(QApplication::translate("PreferencesDialog", "Project folders locations", 0));
        label->setText(QApplication::translate("PreferencesDialog", " Program resources", 0));
        resourcesButton->setText(QApplication::translate("PreferencesDialog", "...", 0));
        label_2->setText(QApplication::translate("PreferencesDialog", "User's projects", 0));
        usersButton->setText(QApplication::translate("PreferencesDialog", "...", 0));
        label_3->setText(QApplication::translate("PreferencesDialog", "Help database", 0));
        helpButton->setText(QApplication::translate("PreferencesDialog", "...", 0));
        pushButton->setText(QApplication::translate("PreferencesDialog", "Generate ...", 0));
        label_5->setText(QApplication::translate("PreferencesDialog", "gemsfit2 code path", 0));
        gemsfit2Button->setText(QApplication::translate("PreferencesDialog", "...", 0));
        groupBox_2->setTitle(QApplication::translate("PreferencesDialog", "Settings", 0));
        commentsBox->setText(QApplication::translate("PreferencesDialog", "Add comments to exported gemsfit2 task specification TXT files", 0));
        turnoff->setText(QApplication::translate("PreferencesDialog", "Turn on runtime log output in the Run Status field ", 0));
        yamlBox->setText(QApplication::translate("PreferencesDialog", "View record data in &YAML Format (tbd)", 0));
        label_4->setText(QApplication::translate("PreferencesDialog", "Select template file for the experiments record", 0));
        label_6->setText(QApplication::translate("PreferencesDialog", "Select template file for database record search", 0));
    } // retranslateUi

};

namespace Ui {
    class PreferencesDialog: public Ui_PreferencesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCESDIALOG_H
