/********************************************************************************
** Form generated from reading UI file 'CalcDialog4.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALCDIALOG4_H
#define UI_CALCDIALOG4_H

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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CalcDialogData
{
public:
    QGridLayout *gridLayout;
    QComboBox *pValue;
    QLabel *TextLabel1_3;
    QGroupBox *pButtonBoxC;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *PushButton1_3;
    QPushButton *PushButton1_2;
    QGroupBox *pButtonBox;
    QGridLayout *gridLayout1;
    QPushButton *pAssignBtn;
    QSpacerItem *spacerItem;
    QPushButton *pClearBtn;
    QPushButton *PushButton2;
    QPushButton *PushButton3;
    QPushButton *PushButton4;
    QPushButton *PushButton5;
    QPushButton *PushButton6;
    QPushButton *PushButton7;
    QSpacerItem *spacerItem1;
    QPushButton *PushButton8;
    QPushButton *PushButton9;
    QPushButton *PushButton10;
    QPushButton *PushButton11;
    QPushButton *PushButton12;
    QPushButton *PushButton13;
    QPushButton *PushButton14;
    QPushButton *PushButton15;
    QSpacerItem *spacerItem2;

    void setupUi(QDialog *CalcDialogData)
    {
        if (CalcDialogData->objectName().isEmpty())
            CalcDialogData->setObjectName(QStringLiteral("CalcDialogData"));
        CalcDialogData->setWindowModality(Qt::WindowModal);
        CalcDialogData->resize(549, 252);
        QFont font;
        font.setPointSize(10);
        CalcDialogData->setFont(font);
        CalcDialogData->setModal(true);
        gridLayout = new QGridLayout(CalcDialogData);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        pValue = new QComboBox(CalcDialogData);
        pValue->setObjectName(QStringLiteral("pValue"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pValue->sizePolicy().hasHeightForWidth());
        pValue->setSizePolicy(sizePolicy);
        pValue->setAcceptDrops(false);
        pValue->setAutoFillBackground(false);
        pValue->setEditable(true);

        gridLayout->addWidget(pValue, 0, 0, 1, 1);

        TextLabel1_3 = new QLabel(CalcDialogData);
        TextLabel1_3->setObjectName(QStringLiteral("TextLabel1_3"));
        TextLabel1_3->setAcceptDrops(false);
        TextLabel1_3->setWordWrap(false);

        gridLayout->addWidget(TextLabel1_3, 0, 1, 1, 1);

        pButtonBoxC = new QGroupBox(CalcDialogData);
        pButtonBoxC->setObjectName(QStringLiteral("pButtonBoxC"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        pButtonBoxC->setFont(font1);
        verticalLayout = new QVBoxLayout(pButtonBoxC);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalSpacer = new QSpacerItem(20, 111, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        PushButton1_3 = new QPushButton(pButtonBoxC);
        PushButton1_3->setObjectName(QStringLiteral("PushButton1_3"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(PushButton1_3->sizePolicy().hasHeightForWidth());
        PushButton1_3->setSizePolicy(sizePolicy1);
        PushButton1_3->setMinimumSize(QSize(0, 40));
        PushButton1_3->setDefault(true);

        verticalLayout->addWidget(PushButton1_3);

        PushButton1_2 = new QPushButton(pButtonBoxC);
        PushButton1_2->setObjectName(QStringLiteral("PushButton1_2"));
        sizePolicy1.setHeightForWidth(PushButton1_2->sizePolicy().hasHeightForWidth());
        PushButton1_2->setSizePolicy(sizePolicy1);

        verticalLayout->addWidget(PushButton1_2);


        gridLayout->addWidget(pButtonBoxC, 0, 2, 2, 1);

        pButtonBox = new QGroupBox(CalcDialogData);
        pButtonBox->setObjectName(QStringLiteral("pButtonBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pButtonBox->sizePolicy().hasHeightForWidth());
        pButtonBox->setSizePolicy(sizePolicy2);
        QFont font2;
        font2.setPointSize(10);
        font2.setBold(true);
        font2.setWeight(75);
        pButtonBox->setFont(font2);
        pButtonBox->setFlat(false);
        pButtonBox->setCheckable(false);
        gridLayout1 = new QGridLayout(pButtonBox);
        gridLayout1->setSpacing(6);
        gridLayout1->setContentsMargins(11, 11, 11, 11);
        gridLayout1->setObjectName(QStringLiteral("gridLayout1"));
        pAssignBtn = new QPushButton(pButtonBox);
        pAssignBtn->setObjectName(QStringLiteral("pAssignBtn"));
        pAssignBtn->setCheckable(true);

        gridLayout1->addWidget(pAssignBtn, 0, 0, 1, 2);

        spacerItem = new QSpacerItem(61, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout1->addItem(spacerItem, 0, 2, 1, 2);

        pClearBtn = new QPushButton(pButtonBox);
        pClearBtn->setObjectName(QStringLiteral("pClearBtn"));
        pClearBtn->setCheckable(true);

        gridLayout1->addWidget(pClearBtn, 0, 4, 1, 1);

        PushButton2 = new QPushButton(pButtonBox);
        PushButton2->setObjectName(QStringLiteral("PushButton2"));
        PushButton2->setCheckable(true);

        gridLayout1->addWidget(PushButton2, 1, 0, 1, 1);

        PushButton3 = new QPushButton(pButtonBox);
        PushButton3->setObjectName(QStringLiteral("PushButton3"));
        PushButton3->setCheckable(true);

        gridLayout1->addWidget(PushButton3, 1, 1, 1, 2);

        PushButton4 = new QPushButton(pButtonBox);
        PushButton4->setObjectName(QStringLiteral("PushButton4"));
        QFont font3;
        font3.setPointSize(12);
        PushButton4->setFont(font3);
        PushButton4->setCheckable(true);

        gridLayout1->addWidget(PushButton4, 1, 3, 1, 1);

        PushButton5 = new QPushButton(pButtonBox);
        PushButton5->setObjectName(QStringLiteral("PushButton5"));
        PushButton5->setCheckable(true);

        gridLayout1->addWidget(PushButton5, 1, 4, 1, 1);

        PushButton6 = new QPushButton(pButtonBox);
        PushButton6->setObjectName(QStringLiteral("PushButton6"));
        PushButton6->setCheckable(true);

        gridLayout1->addWidget(PushButton6, 2, 0, 1, 1);

        PushButton7 = new QPushButton(pButtonBox);
        PushButton7->setObjectName(QStringLiteral("PushButton7"));
        PushButton7->setCheckable(true);

        gridLayout1->addWidget(PushButton7, 2, 1, 1, 2);

        spacerItem1 = new QSpacerItem(41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout1->addItem(spacerItem1, 2, 3, 1, 1);

        PushButton8 = new QPushButton(pButtonBox);
        PushButton8->setObjectName(QStringLiteral("PushButton8"));
        PushButton8->setCheckable(true);

        gridLayout1->addWidget(PushButton8, 2, 4, 1, 1);

        PushButton9 = new QPushButton(pButtonBox);
        PushButton9->setObjectName(QStringLiteral("PushButton9"));
        PushButton9->setCheckable(true);

        gridLayout1->addWidget(PushButton9, 3, 0, 1, 1);

        PushButton10 = new QPushButton(pButtonBox);
        PushButton10->setObjectName(QStringLiteral("PushButton10"));
        PushButton10->setCheckable(true);

        gridLayout1->addWidget(PushButton10, 3, 1, 1, 2);

        PushButton11 = new QPushButton(pButtonBox);
        PushButton11->setObjectName(QStringLiteral("PushButton11"));
        PushButton11->setCheckable(true);

        gridLayout1->addWidget(PushButton11, 3, 3, 1, 1);

        PushButton12 = new QPushButton(pButtonBox);
        PushButton12->setObjectName(QStringLiteral("PushButton12"));
        PushButton12->setCheckable(true);

        gridLayout1->addWidget(PushButton12, 3, 4, 1, 1);

        PushButton13 = new QPushButton(pButtonBox);
        PushButton13->setObjectName(QStringLiteral("PushButton13"));
        PushButton13->setCheckable(true);

        gridLayout1->addWidget(PushButton13, 4, 0, 1, 1);

        PushButton14 = new QPushButton(pButtonBox);
        PushButton14->setObjectName(QStringLiteral("PushButton14"));
        PushButton14->setCheckable(true);

        gridLayout1->addWidget(PushButton14, 4, 1, 1, 2);

        PushButton15 = new QPushButton(pButtonBox);
        PushButton15->setObjectName(QStringLiteral("PushButton15"));
        PushButton15->setCheckable(true);

        gridLayout1->addWidget(PushButton15, 4, 3, 1, 1);

        spacerItem2 = new QSpacerItem(51, 21, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout1->addItem(spacerItem2, 4, 4, 1, 1);


        gridLayout->addWidget(pButtonBox, 1, 0, 1, 2);


        retranslateUi(CalcDialogData);

        QMetaObject::connectSlotsByName(CalcDialogData);
    } // setupUi

    void retranslateUi(QDialog *CalcDialogData)
    {
        CalcDialogData->setWindowTitle(QApplication::translate("CalcDialogData", "GEMS Data Object Cell Calculator", 0));
        pValue->clear();
        pValue->insertItems(0, QStringList()
         << QApplication::translate("CalcDialogData", "0", 0)
         << QApplication::translate("CalcDialogData", "--- (empty)", 0)
         << QApplication::translate("CalcDialogData", "0.4343  (ln-to-lg)", 0)
         << QApplication::translate("CalcDialogData", "4.184    (cal-to-J)", 0)
         << QApplication::translate("CalcDialogData", "273.15  (K-C)", 0)
         << QApplication::translate("CalcDialogData", "298.15  (Tr)", 0)
         << QApplication::translate("CalcDialogData", "8.31451 (R)", 0)
         << QApplication::translate("CalcDialogData", "2479      (RTr)", 0)
         << QApplication::translate("CalcDialogData", "1000", 0)
        );
        TextLabel1_3->setText(QApplication::translate("CalcDialogData", "<- Value of the Second Operand", 0));
        pButtonBoxC->setTitle(QString());
        PushButton1_3->setText(QApplication::translate("CalcDialogData", "   &Ok - Do it!  ", 0));
        PushButton1_3->setShortcut(QApplication::translate("CalcDialogData", "Alt+O", 0));
        PushButton1_2->setText(QApplication::translate("CalcDialogData", "&Cancel", 0));
        PushButton1_2->setShortcut(QApplication::translate("CalcDialogData", "Alt+C", 0));
#ifndef QT_NO_TOOLTIP
        pButtonBox->setToolTip(QApplication::translate("CalcDialogData", "After any operation, previous contents of all selected cells will be lost!", 0));
#endif // QT_NO_TOOLTIP
        pButtonBox->setTitle(QApplication::translate("CalcDialogData", "   Operation", 0));
#ifndef QT_NO_TOOLTIP
        pAssignBtn->setToolTip(QApplication::translate("CalcDialogData", "Set / Assign above Value to Cell(s)", 0));
#endif // QT_NO_TOOLTIP
        pAssignBtn->setText(QApplication::translate("CalcDialogData", "&=", 0));
#ifndef QT_NO_TOOLTIP
        pClearBtn->setToolTip(QApplication::translate("CalcDialogData", "Set Cell to 'No Data' Value", 0));
#endif // QT_NO_TOOLTIP
        pClearBtn->setText(QApplication::translate("CalcDialogData", "empty", 0));
        pClearBtn->setShortcut(QApplication::translate("CalcDialogData", "Alt+D", 0));
#ifndef QT_NO_TOOLTIP
        PushButton2->setToolTip(QApplication::translate("CalcDialogData", "Add a Value to Cell", 0));
#endif // QT_NO_TOOLTIP
        PushButton2->setText(QApplication::translate("CalcDialogData", "&+", 0));
        PushButton2->setShortcut(QApplication::translate("CalcDialogData", "Alt++", 0));
#ifndef QT_NO_TOOLTIP
        PushButton3->setToolTip(QApplication::translate("CalcDialogData", "Subtract a Value from Cell", 0));
#endif // QT_NO_TOOLTIP
        PushButton3->setText(QApplication::translate("CalcDialogData", "&-", 0));
        PushButton3->setShortcut(QApplication::translate("CalcDialogData", "Alt+-", 0));
#ifndef QT_NO_TOOLTIP
        PushButton4->setToolTip(QApplication::translate("CalcDialogData", "Multiply Cell to a Value", 0));
#endif // QT_NO_TOOLTIP
        PushButton4->setText(QApplication::translate("CalcDialogData", "&*", 0));
        PushButton4->setShortcut(QApplication::translate("CalcDialogData", "Alt+*", 0));
#ifndef QT_NO_TOOLTIP
        PushButton5->setToolTip(QApplication::translate("CalcDialogData", "Divide Cell by a Value (except 0)", 0));
#endif // QT_NO_TOOLTIP
        PushButton5->setText(QApplication::translate("CalcDialogData", "&/", 0));
        PushButton5->setShortcut(QApplication::translate("CalcDialogData", "Alt+/", 0));
#ifndef QT_NO_TOOLTIP
        PushButton6->setToolTip(QApplication::translate("CalcDialogData", "Raise Cell to a Value Power", 0));
#endif // QT_NO_TOOLTIP
        PushButton6->setText(QApplication::translate("CalcDialogData", "&^", 0));
        PushButton6->setShortcut(QApplication::translate("CalcDialogData", "Alt+^", 0));
#ifndef QT_NO_TOOLTIP
        PushButton7->setToolTip(QApplication::translate("CalcDialogData", "Invert Cell", 0));
#endif // QT_NO_TOOLTIP
        PushButton7->setText(QApplication::translate("CalcDialogData", "&1/x", 0));
        PushButton7->setShortcut(QApplication::translate("CalcDialogData", "Alt+1", 0));
#ifndef QT_NO_TOOLTIP
        PushButton8->setToolTip(QApplication::translate("CalcDialogData", "Square Root from (non-negative) Cell", 0));
#endif // QT_NO_TOOLTIP
        PushButton8->setText(QApplication::translate("CalcDialogData", "s&qrt", 0));
        PushButton8->setShortcut(QApplication::translate("CalcDialogData", "Alt+Q", 0));
#ifndef QT_NO_TOOLTIP
        PushButton9->setToolTip(QApplication::translate("CalcDialogData", "Decimal Logarithm from (positive) Cell", 0));
#endif // QT_NO_TOOLTIP
        PushButton9->setText(QApplication::translate("CalcDialogData", "&log", 0));
        PushButton9->setShortcut(QApplication::translate("CalcDialogData", "Alt+L", 0));
#ifndef QT_NO_TOOLTIP
        PushButton10->setToolTip(QApplication::translate("CalcDialogData", "Natural Logarithm from (positive) Cell", 0));
#endif // QT_NO_TOOLTIP
        PushButton10->setText(QApplication::translate("CalcDialogData", "l&n", 0));
        PushButton10->setShortcut(QApplication::translate("CalcDialogData", "Alt+N", 0));
#ifndef QT_NO_TOOLTIP
        PushButton11->setToolTip(QApplication::translate("CalcDialogData", "Decimal Exponent from Cell", 0));
#endif // QT_NO_TOOLTIP
        PushButton11->setText(QApplication::translate("CalcDialogData", "1&0^x", 0));
        PushButton11->setShortcut(QApplication::translate("CalcDialogData", "Alt+0", 0));
#ifndef QT_NO_TOOLTIP
        PushButton12->setToolTip(QApplication::translate("CalcDialogData", "Exponent from Cell", 0));
#endif // QT_NO_TOOLTIP
        PushButton12->setText(QApplication::translate("CalcDialogData", "&exp", 0));
        PushButton12->setShortcut(QApplication::translate("CalcDialogData", "Alt+E", 0));
#ifndef QT_NO_TOOLTIP
        PushButton13->setToolTip(QApplication::translate("CalcDialogData", "Sine from Cell (Radians)", 0));
#endif // QT_NO_TOOLTIP
        PushButton13->setText(QApplication::translate("CalcDialogData", "&sin", 0));
        PushButton13->setShortcut(QApplication::translate("CalcDialogData", "Alt+S", 0));
#ifndef QT_NO_TOOLTIP
        PushButton14->setToolTip(QApplication::translate("CalcDialogData", "Cosine from Cell (Radians)", 0));
#endif // QT_NO_TOOLTIP
        PushButton14->setText(QApplication::translate("CalcDialogData", "c&os", 0));
        PushButton14->setShortcut(QApplication::translate("CalcDialogData", "Alt+O", 0));
#ifndef QT_NO_TOOLTIP
        PushButton15->setToolTip(QApplication::translate("CalcDialogData", "Tangent from Cell", 0));
#endif // QT_NO_TOOLTIP
        PushButton15->setText(QApplication::translate("CalcDialogData", "&tg", 0));
        PushButton15->setShortcut(QApplication::translate("CalcDialogData", "Alt+T", 0));
    } // retranslateUi

};

namespace Ui {
    class CalcDialogData: public Ui_CalcDialogData {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALCDIALOG4_H
