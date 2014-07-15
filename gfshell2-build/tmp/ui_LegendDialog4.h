/********************************************************************************
** Form generated from reading UI file 'LegendDialog4.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LEGENDDIALOG4_H
#define UI_LEGENDDIALOG4_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LegendDialogData
{
public:
    QGridLayout *gridLayout_2;
    QGroupBox *buttonGroup1;
    QVBoxLayout *verticalLayout;
    QComboBox *comboBox1;
    QPushButton *pBtnOk;
    QPushButton *pBtnApply;
    QGroupBox *pAxis_2;
    QGridLayout *gridLayout;
    QLabel *TextLabel1;
    QSpinBox *pAxisX;
    QLineEdit *pXname;
    QLabel *TextLabel1_3;
    QSpinBox *pAxisY;
    QLineEdit *pYname;
    QLabel *TextLabel1_2;
    QLabel *TextLabel1_2_2;
    QLineEdit *pX0;
    QLineEdit *pXn;
    QLabel *TextLabel1_2_2_2;
    QLineEdit *pY0;
    QLineEdit *pYn;
    QLabel *TextLabel1_2_3;
    QLabel *TextLabel1_2_2_3;
    QLineEdit *pfX0;
    QLineEdit *pfXn;
    QLabel *TextLabel1_2_2_2_2;
    QLineEdit *pfY0;
    QLineEdit *pfYn;
    QLabel *textLabel2;
    QPushButton *pBtnChangeFont;
    QLineEdit *pLabelFont;
    QPushButton *pBthCancel;
    QSpacerItem *verticalSpacer;
    QPushButton *pBtnBackgr;
    QPushButton *pHelp;

    void setupUi(QDialog *LegendDialogData)
    {
        if (LegendDialogData->objectName().isEmpty())
            LegendDialogData->setObjectName(QStringLiteral("LegendDialogData"));
        LegendDialogData->setWindowModality(Qt::WindowModal);
        LegendDialogData->resize(553, 352);
        QFont font;
        font.setPointSize(10);
        LegendDialogData->setFont(font);
        LegendDialogData->setModal(true);
        gridLayout_2 = new QGridLayout(LegendDialogData);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        buttonGroup1 = new QGroupBox(LegendDialogData);
        buttonGroup1->setObjectName(QStringLiteral("buttonGroup1"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(buttonGroup1->sizePolicy().hasHeightForWidth());
        buttonGroup1->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(buttonGroup1);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        comboBox1 = new QComboBox(buttonGroup1);
        comboBox1->setObjectName(QStringLiteral("comboBox1"));

        verticalLayout->addWidget(comboBox1);


        gridLayout_2->addWidget(buttonGroup1, 0, 0, 2, 1);

        pBtnOk = new QPushButton(LegendDialogData);
        pBtnOk->setObjectName(QStringLiteral("pBtnOk"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        pBtnOk->setFont(font1);
        pBtnOk->setDefault(true);

        gridLayout_2->addWidget(pBtnOk, 0, 1, 1, 1);

        pBtnApply = new QPushButton(LegendDialogData);
        pBtnApply->setObjectName(QStringLiteral("pBtnApply"));
        pBtnApply->setFont(font1);

        gridLayout_2->addWidget(pBtnApply, 1, 1, 2, 1);

        pAxis_2 = new QGroupBox(LegendDialogData);
        pAxis_2->setObjectName(QStringLiteral("pAxis_2"));
        sizePolicy.setHeightForWidth(pAxis_2->sizePolicy().hasHeightForWidth());
        pAxis_2->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(pAxis_2);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        TextLabel1 = new QLabel(pAxis_2);
        TextLabel1->setObjectName(QStringLiteral("TextLabel1"));
        TextLabel1->setWordWrap(false);

        gridLayout->addWidget(TextLabel1, 0, 0, 1, 2);

        pAxisX = new QSpinBox(pAxis_2);
        pAxisX->setObjectName(QStringLiteral("pAxisX"));
        pAxisX->setMaximum(20);

        gridLayout->addWidget(pAxisX, 0, 2, 1, 1);

        pXname = new QLineEdit(pAxis_2);
        pXname->setObjectName(QStringLiteral("pXname"));
        pXname->setMaxLength(9);

        gridLayout->addWidget(pXname, 0, 3, 1, 2);

        TextLabel1_3 = new QLabel(pAxis_2);
        TextLabel1_3->setObjectName(QStringLiteral("TextLabel1_3"));
        TextLabel1_3->setWordWrap(false);

        gridLayout->addWidget(TextLabel1_3, 1, 0, 1, 2);

        pAxisY = new QSpinBox(pAxis_2);
        pAxisY->setObjectName(QStringLiteral("pAxisY"));
        pAxisY->setMaximum(20);

        gridLayout->addWidget(pAxisY, 1, 2, 1, 1);

        pYname = new QLineEdit(pAxis_2);
        pYname->setObjectName(QStringLiteral("pYname"));
        pYname->setMaxLength(9);

        gridLayout->addWidget(pYname, 1, 3, 1, 2);

        TextLabel1_2 = new QLabel(pAxis_2);
        TextLabel1_2->setObjectName(QStringLiteral("TextLabel1_2"));
        TextLabel1_2->setWordWrap(false);

        gridLayout->addWidget(TextLabel1_2, 2, 0, 1, 1);

        TextLabel1_2_2 = new QLabel(pAxis_2);
        TextLabel1_2_2->setObjectName(QStringLiteral("TextLabel1_2_2"));
        TextLabel1_2_2->setWordWrap(false);

        gridLayout->addWidget(TextLabel1_2_2, 2, 1, 1, 1);

        pX0 = new QLineEdit(pAxis_2);
        pX0->setObjectName(QStringLiteral("pX0"));

        gridLayout->addWidget(pX0, 2, 2, 1, 2);

        pXn = new QLineEdit(pAxis_2);
        pXn->setObjectName(QStringLiteral("pXn"));

        gridLayout->addWidget(pXn, 2, 4, 1, 1);

        TextLabel1_2_2_2 = new QLabel(pAxis_2);
        TextLabel1_2_2_2->setObjectName(QStringLiteral("TextLabel1_2_2_2"));
        TextLabel1_2_2_2->setWordWrap(false);

        gridLayout->addWidget(TextLabel1_2_2_2, 3, 1, 1, 1);

        pY0 = new QLineEdit(pAxis_2);
        pY0->setObjectName(QStringLiteral("pY0"));

        gridLayout->addWidget(pY0, 3, 2, 1, 2);

        pYn = new QLineEdit(pAxis_2);
        pYn->setObjectName(QStringLiteral("pYn"));

        gridLayout->addWidget(pYn, 3, 4, 1, 1);

        TextLabel1_2_3 = new QLabel(pAxis_2);
        TextLabel1_2_3->setObjectName(QStringLiteral("TextLabel1_2_3"));
        TextLabel1_2_3->setWordWrap(false);

        gridLayout->addWidget(TextLabel1_2_3, 4, 0, 1, 1);

        TextLabel1_2_2_3 = new QLabel(pAxis_2);
        TextLabel1_2_2_3->setObjectName(QStringLiteral("TextLabel1_2_2_3"));
        TextLabel1_2_2_3->setWordWrap(false);

        gridLayout->addWidget(TextLabel1_2_2_3, 4, 1, 1, 1);

        pfX0 = new QLineEdit(pAxis_2);
        pfX0->setObjectName(QStringLiteral("pfX0"));

        gridLayout->addWidget(pfX0, 4, 2, 1, 2);

        pfXn = new QLineEdit(pAxis_2);
        pfXn->setObjectName(QStringLiteral("pfXn"));

        gridLayout->addWidget(pfXn, 4, 4, 1, 1);

        TextLabel1_2_2_2_2 = new QLabel(pAxis_2);
        TextLabel1_2_2_2_2->setObjectName(QStringLiteral("TextLabel1_2_2_2_2"));
        TextLabel1_2_2_2_2->setWordWrap(false);

        gridLayout->addWidget(TextLabel1_2_2_2_2, 5, 1, 1, 1);

        pfY0 = new QLineEdit(pAxis_2);
        pfY0->setObjectName(QStringLiteral("pfY0"));

        gridLayout->addWidget(pfY0, 5, 2, 1, 2);

        pfYn = new QLineEdit(pAxis_2);
        pfYn->setObjectName(QStringLiteral("pfYn"));

        gridLayout->addWidget(pfYn, 5, 4, 1, 1);

        textLabel2 = new QLabel(pAxis_2);
        textLabel2->setObjectName(QStringLiteral("textLabel2"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(textLabel2->sizePolicy().hasHeightForWidth());
        textLabel2->setSizePolicy(sizePolicy1);
        textLabel2->setWordWrap(false);

        gridLayout->addWidget(textLabel2, 6, 0, 1, 1);

        pBtnChangeFont = new QPushButton(pAxis_2);
        pBtnChangeFont->setObjectName(QStringLiteral("pBtnChangeFont"));

        gridLayout->addWidget(pBtnChangeFont, 6, 1, 1, 1);

        pLabelFont = new QLineEdit(pAxis_2);
        pLabelFont->setObjectName(QStringLiteral("pLabelFont"));
        QFont font2;
        font2.setPointSize(11);
        pLabelFont->setFont(font2);
        pLabelFont->setReadOnly(true);

        gridLayout->addWidget(pLabelFont, 6, 2, 1, 3);


        gridLayout_2->addWidget(pAxis_2, 2, 0, 5, 1);

        pBthCancel = new QPushButton(LegendDialogData);
        pBthCancel->setObjectName(QStringLiteral("pBthCancel"));
        pBthCancel->setFont(font1);

        gridLayout_2->addWidget(pBthCancel, 3, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 143, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 4, 1, 1, 1);

        pBtnBackgr = new QPushButton(LegendDialogData);
        pBtnBackgr->setObjectName(QStringLiteral("pBtnBackgr"));
        pBtnBackgr->setFont(font1);

        gridLayout_2->addWidget(pBtnBackgr, 5, 1, 1, 1);

        pHelp = new QPushButton(LegendDialogData);
        pHelp->setObjectName(QStringLiteral("pHelp"));
        pHelp->setFont(font1);

        gridLayout_2->addWidget(pHelp, 6, 1, 1, 1);

        QWidget::setTabOrder(pBtnOk, pBtnApply);
        QWidget::setTabOrder(pBtnApply, pBthCancel);
        QWidget::setTabOrder(pBthCancel, pBtnBackgr);
        QWidget::setTabOrder(pBtnBackgr, pAxisX);
        QWidget::setTabOrder(pAxisX, pXname);
        QWidget::setTabOrder(pXname, pYname);
        QWidget::setTabOrder(pYname, pX0);
        QWidget::setTabOrder(pX0, pXn);
        QWidget::setTabOrder(pXn, pY0);
        QWidget::setTabOrder(pY0, pYn);
        QWidget::setTabOrder(pYn, pfX0);
        QWidget::setTabOrder(pfX0, pfXn);
        QWidget::setTabOrder(pfXn, pfY0);
        QWidget::setTabOrder(pfY0, pfYn);
        QWidget::setTabOrder(pfYn, pBtnChangeFont);
        QWidget::setTabOrder(pBtnChangeFont, pLabelFont);
        QWidget::setTabOrder(pLabelFont, comboBox1);

        retranslateUi(LegendDialogData);
        QObject::connect(pBtnApply, SIGNAL(clicked()), LegendDialogData, SLOT(CmApply()));
        QObject::connect(pBtnOk, SIGNAL(clicked()), LegendDialogData, SLOT(accept()));
        QObject::connect(pBthCancel, SIGNAL(clicked()), LegendDialogData, SLOT(reject()));
        QObject::connect(pBtnBackgr, SIGNAL(clicked()), LegendDialogData, SLOT(CmBackground()));

        QMetaObject::connectSlotsByName(LegendDialogData);
    } // setupUi

    void retranslateUi(QDialog *LegendDialogData)
    {
        LegendDialogData->setWindowTitle(QApplication::translate("LegendDialogData", "Legend and Configuration for: ", 0));
        buttonGroup1->setTitle(QApplication::translate("LegendDialogData", "Graph Type", 0));
        comboBox1->clear();
        comboBox1->insertItems(0, QStringList()
         << QApplication::translate("LegendDialogData", "0 - Lines/Symbols", 0)
         << QApplication::translate("LegendDialogData", "1 - Cumulative", 0)
         << QApplication::translate("LegendDialogData", "2 - Isolines", 0)
         << QApplication::translate("LegendDialogData", "3 - 3D plotting widget", 0)
        );
#ifndef QT_NO_TOOLTIP
        pBtnOk->setToolTip(QApplication::translate("LegendDialogData", "Apply changes to the graph and close this dialog", 0));
#endif // QT_NO_TOOLTIP
        pBtnOk->setText(QApplication::translate("LegendDialogData", "&OK", 0));
#ifndef QT_NO_TOOLTIP
        pBtnApply->setToolTip(QApplication::translate("LegendDialogData", "Apply changes to the graph and do not close this dialog", 0));
#endif // QT_NO_TOOLTIP
        pBtnApply->setText(QApplication::translate("LegendDialogData", "&Apply", 0));
        pAxis_2->setTitle(QApplication::translate("LegendDialogData", "Axis Setup", 0));
        TextLabel1->setText(QApplication::translate("LegendDialogData", "Abscissa grid", 0));
#ifndef QT_NO_TOOLTIP
        pAxisX->setToolTip(QApplication::translate("LegendDialogData", "Set the maximum number of major scale intervals for  bottom axis", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pXname->setToolTip(QApplication::translate("LegendDialogData", "Change the title of a bottom axis", 0));
#endif // QT_NO_TOOLTIP
        TextLabel1_3->setText(QApplication::translate("LegendDialogData", "Ordinate grid", 0));
#ifndef QT_NO_TOOLTIP
        pAxisY->setToolTip(QApplication::translate("LegendDialogData", "Set the maximum number of major scale intervals for  left axis", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pYname->setToolTip(QApplication::translate("LegendDialogData", "Change the title of a left axis", 0));
#endif // QT_NO_TOOLTIP
        TextLabel1_2->setText(QApplication::translate("LegendDialogData", "Graph", 0));
        TextLabel1_2_2->setText(QApplication::translate("LegendDialogData", "x:", 0));
#ifndef QT_NO_TOOLTIP
        pX0->setToolTip(QApplication::translate("LegendDialogData", "Change minimum x of the scale", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pXn->setToolTip(QApplication::translate("LegendDialogData", "Change  maximum x of the scale", 0));
#endif // QT_NO_TOOLTIP
        TextLabel1_2_2_2->setText(QApplication::translate("LegendDialogData", "y:", 0));
#ifndef QT_NO_TOOLTIP
        pY0->setToolTip(QApplication::translate("LegendDialogData", "Change minimum y of the scale", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pYn->setToolTip(QApplication::translate("LegendDialogData", "Change maximum y of the scale", 0));
#endif // QT_NO_TOOLTIP
        TextLabel1_2_3->setText(QApplication::translate("LegendDialogData", "Fragment", 0));
        TextLabel1_2_2_3->setText(QApplication::translate("LegendDialogData", "x:", 0));
#ifndef QT_NO_TOOLTIP
        pfX0->setToolTip(QApplication::translate("LegendDialogData", "Minimum", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pfXn->setToolTip(QApplication::translate("LegendDialogData", "Maximum", 0));
#endif // QT_NO_TOOLTIP
        TextLabel1_2_2_2_2->setText(QApplication::translate("LegendDialogData", "y:", 0));
#ifndef QT_NO_TOOLTIP
        pfY0->setToolTip(QApplication::translate("LegendDialogData", "Minimum", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pfYn->setToolTip(QApplication::translate("LegendDialogData", "Maximum", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        textLabel2->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        textLabel2->setText(QApplication::translate("LegendDialogData", "Label font:", 0));
        pBtnChangeFont->setText(QApplication::translate("LegendDialogData", "...", 0));
#ifndef QT_NO_TOOLTIP
        pLabelFont->setToolTip(QApplication::translate("LegendDialogData", "This label font is common for all graphs", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pBthCancel->setToolTip(QApplication::translate("LegendDialogData", "Discard  changes and close this dialog", 0));
#endif // QT_NO_TOOLTIP
        pBthCancel->setText(QApplication::translate("LegendDialogData", "&Cancel", 0));
#ifndef QT_NO_TOOLTIP
        pBtnBackgr->setToolTip(QApplication::translate("LegendDialogData", "Set background color of the graph area", 0));
#endif // QT_NO_TOOLTIP
        pBtnBackgr->setText(QApplication::translate("LegendDialogData", "&Background", 0));
        pHelp->setText(QApplication::translate("LegendDialogData", "&Help", 0));
        pHelp->setShortcut(QApplication::translate("LegendDialogData", "F1", 0));
    } // retranslateUi

};

namespace Ui {
    class LegendDialogData: public Ui_LegendDialogData {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LEGENDDIALOG4_H
