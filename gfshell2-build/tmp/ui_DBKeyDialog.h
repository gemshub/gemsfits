/********************************************************************************
** Form generated from reading UI file 'DBKeyDialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DBKEYDIALOG_H
#define UI_DBKEYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_KeyDialogData
{
public:
    QGridLayout *gridLayout;
    QLabel *pLabel;
    QListWidget *pList;
    QPushButton *PushButton1;
    QPushButton *pFilterButton;
    QPushButton *pButton2;
    QPushButton *pButton3;
    QPushButton *bHelp;
    QPushButton *PushButton3;

    void setupUi(QDialog *KeyDialogData)
    {
        if (KeyDialogData->objectName().isEmpty())
            KeyDialogData->setObjectName(QStringLiteral("KeyDialogData"));
        KeyDialogData->setWindowModality(Qt::WindowModal);
        KeyDialogData->resize(703, 475);
        KeyDialogData->setMinimumSize(QSize(0, 0));
        QFont font;
        font.setPointSize(10);
        KeyDialogData->setFont(font);
        KeyDialogData->setSizeGripEnabled(false);
        KeyDialogData->setModal(true);
        gridLayout = new QGridLayout(KeyDialogData);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        pLabel = new QLabel(KeyDialogData);
        pLabel->setObjectName(QStringLiteral("pLabel"));
        pLabel->setAlignment(Qt::AlignCenter);
        pLabel->setWordWrap(false);

        gridLayout->addWidget(pLabel, 0, 0, 1, 3);

        pList = new QListWidget(KeyDialogData);
        pList->setObjectName(QStringLiteral("pList"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pList->sizePolicy().hasHeightForWidth());
        pList->setSizePolicy(sizePolicy);
        QPalette palette;
        QBrush brush(QColor(49, 106, 197, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Highlight, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::HighlightedText, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Highlight, brush);
        palette.setBrush(QPalette::Inactive, QPalette::HighlightedText, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Highlight, brush);
        palette.setBrush(QPalette::Disabled, QPalette::HighlightedText, brush1);
        pList->setPalette(palette);
        pList->setSelectionBehavior(QAbstractItemView::SelectRows);
        pList->setResizeMode(QListView::Adjust);

        gridLayout->addWidget(pList, 1, 0, 1, 6);

        PushButton1 = new QPushButton(KeyDialogData);
        PushButton1->setObjectName(QStringLiteral("PushButton1"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(PushButton1->sizePolicy().hasHeightForWidth());
        PushButton1->setSizePolicy(sizePolicy1);
        PushButton1->setMinimumSize(QSize(100, 0));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        PushButton1->setFont(font1);
        PushButton1->setDefault(true);

        gridLayout->addWidget(PushButton1, 2, 0, 1, 1);

        pFilterButton = new QPushButton(KeyDialogData);
        pFilterButton->setObjectName(QStringLiteral("pFilterButton"));
        sizePolicy1.setHeightForWidth(pFilterButton->sizePolicy().hasHeightForWidth());
        pFilterButton->setSizePolicy(sizePolicy1);
        pFilterButton->setFont(font1);

        gridLayout->addWidget(pFilterButton, 2, 1, 1, 1);

        pButton2 = new QPushButton(KeyDialogData);
        pButton2->setObjectName(QStringLiteral("pButton2"));
        sizePolicy1.setHeightForWidth(pButton2->sizePolicy().hasHeightForWidth());
        pButton2->setSizePolicy(sizePolicy1);
        pButton2->setFont(font1);

        gridLayout->addWidget(pButton2, 2, 2, 1, 1);

        pButton3 = new QPushButton(KeyDialogData);
        pButton3->setObjectName(QStringLiteral("pButton3"));
        sizePolicy1.setHeightForWidth(pButton3->sizePolicy().hasHeightForWidth());
        pButton3->setSizePolicy(sizePolicy1);
        pButton3->setFont(font1);

        gridLayout->addWidget(pButton3, 2, 3, 1, 1);

        bHelp = new QPushButton(KeyDialogData);
        bHelp->setObjectName(QStringLiteral("bHelp"));
        sizePolicy1.setHeightForWidth(bHelp->sizePolicy().hasHeightForWidth());
        bHelp->setSizePolicy(sizePolicy1);
        bHelp->setFont(font1);

        gridLayout->addWidget(bHelp, 2, 4, 1, 1);

        PushButton3 = new QPushButton(KeyDialogData);
        PushButton3->setObjectName(QStringLiteral("PushButton3"));
        sizePolicy1.setHeightForWidth(PushButton3->sizePolicy().hasHeightForWidth());
        PushButton3->setSizePolicy(sizePolicy1);
        PushButton3->setMinimumSize(QSize(100, 0));
        PushButton3->setFont(font1);

        gridLayout->addWidget(PushButton3, 2, 5, 1, 1);

        QWidget::setTabOrder(PushButton1, pFilterButton);
        QWidget::setTabOrder(pFilterButton, pButton2);
        QWidget::setTabOrder(pButton2, pButton3);
        QWidget::setTabOrder(pButton3, PushButton3);

        retranslateUi(KeyDialogData);
        QObject::connect(PushButton1, SIGNAL(clicked()), KeyDialogData, SLOT(accept()));
        QObject::connect(PushButton3, SIGNAL(clicked()), KeyDialogData, SLOT(reject()));
        QObject::connect(pFilterButton, SIGNAL(clicked()), KeyDialogData, SLOT(CmFilter()));
        QObject::connect(pButton2, SIGNAL(clicked()), KeyDialogData, SLOT(CmSelectAll()));
        QObject::connect(pButton3, SIGNAL(clicked()), KeyDialogData, SLOT(CmClearAll()));

        QMetaObject::connectSlotsByName(KeyDialogData);
    } // setupUi

    void retranslateUi(QDialog *KeyDialogData)
    {
        KeyDialogData->setWindowTitle(QApplication::translate("KeyDialogData", "List of GEMS database record keys", 0));
        pLabel->setText(QApplication::translate("KeyDialogData", "Selection of GEMSFIT database record keys", 0));
        PushButton1->setText(QApplication::translate("KeyDialogData", "&Ok", 0));
        PushButton1->setShortcut(QApplication::translate("KeyDialogData", "Alt+O", 0));
#ifndef QT_NO_TOOLTIP
        pFilterButton->setToolTip(QApplication::translate("KeyDialogData", "Here you can specify a template to filter a large list of record keys", 0));
#endif // QT_NO_TOOLTIP
        pFilterButton->setText(QApplication::translate("KeyDialogData", "Set &Filter", 0));
#ifndef QT_NO_TOOLTIP
        pButton2->setToolTip(QApplication::translate("KeyDialogData", "Here you can selected all list of record keys", 0));
#endif // QT_NO_TOOLTIP
        pButton2->setText(QApplication::translate("KeyDialogData", "&Select All", 0));
#ifndef QT_NO_TOOLTIP
        pButton3->setToolTip(QApplication::translate("KeyDialogData", "Here you can clearing all selection", 0));
#endif // QT_NO_TOOLTIP
        pButton3->setText(QApplication::translate("KeyDialogData", "C&lear All", 0));
        bHelp->setText(QApplication::translate("KeyDialogData", "&Help", 0));
        bHelp->setShortcut(QApplication::translate("KeyDialogData", "F1", 0));
        PushButton3->setText(QApplication::translate("KeyDialogData", "&Cancel", 0));
        PushButton3->setShortcut(QApplication::translate("KeyDialogData", "Alt+C", 0));
    } // retranslateUi

};

namespace Ui {
    class KeyDialogData: public Ui_KeyDialogData {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DBKEYDIALOG_H
