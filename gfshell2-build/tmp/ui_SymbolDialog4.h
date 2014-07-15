/********************************************************************************
** Form generated from reading UI file 'SymbolDialog4.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SYMBOLDIALOG4_H
#define UI_SYMBOLDIALOG4_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_SymbolDialogData
{
public:
    QGridLayout *gridLayout_2;
    QLabel *TextLabel1;
    QSpinBox *pLine;
    QLabel *TextLabel2;
    QSpinBox *pSize;
    QPushButton *PushButton10;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QPushButton *PushButton11;
    QSpacerItem *spacerItem;
    QFrame *pColor;
    QPushButton *PushButton12;

    void setupUi(QDialog *SymbolDialogData)
    {
        if (SymbolDialogData->objectName().isEmpty())
            SymbolDialogData->setObjectName(QStringLiteral("SymbolDialogData"));
        SymbolDialogData->resize(493, 180);
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        SymbolDialogData->setFont(font);
        SymbolDialogData->setModal(false);
        gridLayout_2 = new QGridLayout(SymbolDialogData);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        TextLabel1 = new QLabel(SymbolDialogData);
        TextLabel1->setObjectName(QStringLiteral("TextLabel1"));
        QFont font1;
        font1.setBold(false);
        font1.setWeight(50);
        TextLabel1->setFont(font1);
        TextLabel1->setWordWrap(false);

        gridLayout_2->addWidget(TextLabel1, 0, 0, 1, 1);

        pLine = new QSpinBox(SymbolDialogData);
        pLine->setObjectName(QStringLiteral("pLine"));
        pLine->setMinimum(-20);
        pLine->setMaximum(20);

        gridLayout_2->addWidget(pLine, 0, 1, 1, 1);

        TextLabel2 = new QLabel(SymbolDialogData);
        TextLabel2->setObjectName(QStringLiteral("TextLabel2"));
        TextLabel2->setFont(font1);
        TextLabel2->setWordWrap(false);

        gridLayout_2->addWidget(TextLabel2, 0, 2, 1, 1);

        pSize = new QSpinBox(SymbolDialogData);
        pSize->setObjectName(QStringLiteral("pSize"));

        gridLayout_2->addWidget(pSize, 0, 3, 1, 1);

        PushButton10 = new QPushButton(SymbolDialogData);
        PushButton10->setObjectName(QStringLiteral("PushButton10"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(PushButton10->sizePolicy().hasHeightForWidth());
        PushButton10->setSizePolicy(sizePolicy);
        PushButton10->setMinimumSize(QSize(100, 0));
        QFont font2;
        font2.setFamily(QStringLiteral("Arial"));
        font2.setBold(true);
        font2.setWeight(75);
        PushButton10->setFont(font2);
        PushButton10->setDefault(true);

        gridLayout_2->addWidget(PushButton10, 0, 4, 1, 1);

        groupBox = new QGroupBox(SymbolDialogData);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QFont font3;
        font3.setPointSize(10);
        font3.setBold(false);
        font3.setWeight(50);
        groupBox->setFont(font3);
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));

        gridLayout_2->addWidget(groupBox, 1, 0, 2, 4);

        PushButton11 = new QPushButton(SymbolDialogData);
        PushButton11->setObjectName(QStringLiteral("PushButton11"));
        sizePolicy.setHeightForWidth(PushButton11->sizePolicy().hasHeightForWidth());
        PushButton11->setSizePolicy(sizePolicy);
        PushButton11->setMinimumSize(QSize(100, 0));
        PushButton11->setFont(font2);

        gridLayout_2->addWidget(PushButton11, 1, 4, 1, 1);

        spacerItem = new QSpacerItem(20, 69, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(spacerItem, 2, 4, 1, 1);

        pColor = new QFrame(SymbolDialogData);
        pColor->setObjectName(QStringLiteral("pColor"));
        pColor->setFrameShape(QFrame::StyledPanel);
        pColor->setFrameShadow(QFrame::Raised);

        gridLayout_2->addWidget(pColor, 3, 0, 1, 3);

        PushButton12 = new QPushButton(SymbolDialogData);
        PushButton12->setObjectName(QStringLiteral("PushButton12"));
        sizePolicy.setHeightForWidth(PushButton12->sizePolicy().hasHeightForWidth());
        PushButton12->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(PushButton12, 3, 3, 1, 1);

#ifndef QT_NO_SHORTCUT
        TextLabel1->setBuddy(pLine);
        TextLabel2->setBuddy(pSize);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(PushButton10, PushButton11);

        retranslateUi(SymbolDialogData);
        QObject::connect(PushButton10, SIGNAL(clicked()), SymbolDialogData, SLOT(accept()));
        QObject::connect(PushButton11, SIGNAL(clicked()), SymbolDialogData, SLOT(reject()));

        QMetaObject::connectSlotsByName(SymbolDialogData);
    } // setupUi

    void retranslateUi(QDialog *SymbolDialogData)
    {
        SymbolDialogData->setWindowTitle(QApplication::translate("SymbolDialogData", "ColorDialogData", 0));
        TextLabel1->setText(QApplication::translate("SymbolDialogData", "&Line Size", 0));
        TextLabel2->setText(QApplication::translate("SymbolDialogData", "&Symbol Size", 0));
        PushButton10->setText(QApplication::translate("SymbolDialogData", "&Ok", 0));
        PushButton10->setShortcut(QApplication::translate("SymbolDialogData", "Alt+O", 0));
        groupBox->setTitle(QApplication::translate("SymbolDialogData", "Select symbol", 0));
        PushButton11->setText(QApplication::translate("SymbolDialogData", "&Cancel", 0));
        PushButton11->setShortcut(QApplication::translate("SymbolDialogData", "Alt+C", 0));
        PushButton12->setText(QApplication::translate("SymbolDialogData", "C&hange Color...", 0));
    } // retranslateUi

};

namespace Ui {
    class SymbolDialogData: public Ui_SymbolDialogData {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SYMBOLDIALOG4_H
