/********************************************************************************
** Form generated from reading UI file 'DialogFindFromPlot.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGFINDFROMPLOT_H
#define UI_DIALOGFINDFROMPLOT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_DialogFindFromPlot
{
public:
    QGridLayout *gridLayout;
    QLabel *label_4;
    QComboBox *namesBox;
    QLabel *label;
    QLabel *label_5;
    QSpinBox *xNum;
    QLabel *label_6;
    QDoubleSpinBox *xminVal;
    QLabel *label_7;
    QDoubleSpinBox *xmaxVal;
    QLabel *label_2;
    QLabel *label_9;
    QSpinBox *yNum;
    QLabel *label_10;
    QDoubleSpinBox *yminVal;
    QLabel *label_11;
    QDoubleSpinBox *ymaxVal;
    QLabel *label_3;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *DialogFindFromPlot)
    {
        if (DialogFindFromPlot->objectName().isEmpty())
            DialogFindFromPlot->setObjectName(QStringLiteral("DialogFindFromPlot"));
        DialogFindFromPlot->resize(453, 210);
        gridLayout = new QGridLayout(DialogFindFromPlot);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_4 = new QLabel(DialogFindFromPlot);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 0, 0, 1, 2);

        namesBox = new QComboBox(DialogFindFromPlot);
        namesBox->setObjectName(QStringLiteral("namesBox"));

        gridLayout->addWidget(namesBox, 0, 2, 1, 3);

        label = new QLabel(DialogFindFromPlot);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 1, 0, 1, 2);

        label_5 = new QLabel(DialogFindFromPlot);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 1, 2, 1, 1);

        xNum = new QSpinBox(DialogFindFromPlot);
        xNum->setObjectName(QStringLiteral("xNum"));
        xNum->setMinimum(-1);
        xNum->setMaximum(15);
        xNum->setValue(-1);

        gridLayout->addWidget(xNum, 1, 3, 1, 2);

        label_6 = new QLabel(DialogFindFromPlot);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout->addWidget(label_6, 2, 0, 1, 1);

        xminVal = new QDoubleSpinBox(DialogFindFromPlot);
        xminVal->setObjectName(QStringLiteral("xminVal"));
        xminVal->setDecimals(6);
        xminVal->setMinimum(-1e+09);
        xminVal->setMaximum(1e+09);

        gridLayout->addWidget(xminVal, 2, 1, 1, 2);

        label_7 = new QLabel(DialogFindFromPlot);
        label_7->setObjectName(QStringLiteral("label_7"));

        gridLayout->addWidget(label_7, 2, 3, 1, 1);

        xmaxVal = new QDoubleSpinBox(DialogFindFromPlot);
        xmaxVal->setObjectName(QStringLiteral("xmaxVal"));
        xmaxVal->setDecimals(10);
        xmaxVal->setMinimum(-1e+09);
        xmaxVal->setMaximum(1e+09);

        gridLayout->addWidget(xmaxVal, 2, 4, 1, 1);

        label_2 = new QLabel(DialogFindFromPlot);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 3, 0, 1, 2);

        label_9 = new QLabel(DialogFindFromPlot);
        label_9->setObjectName(QStringLiteral("label_9"));

        gridLayout->addWidget(label_9, 3, 2, 1, 1);

        yNum = new QSpinBox(DialogFindFromPlot);
        yNum->setObjectName(QStringLiteral("yNum"));
        yNum->setMinimum(-1);
        yNum->setMaximum(15);
        yNum->setValue(-1);

        gridLayout->addWidget(yNum, 3, 3, 1, 2);

        label_10 = new QLabel(DialogFindFromPlot);
        label_10->setObjectName(QStringLiteral("label_10"));

        gridLayout->addWidget(label_10, 4, 0, 1, 1);

        yminVal = new QDoubleSpinBox(DialogFindFromPlot);
        yminVal->setObjectName(QStringLiteral("yminVal"));
        yminVal->setDecimals(6);
        yminVal->setMinimum(-1e+09);
        yminVal->setMaximum(1e+09);

        gridLayout->addWidget(yminVal, 4, 1, 1, 2);

        label_11 = new QLabel(DialogFindFromPlot);
        label_11->setObjectName(QStringLiteral("label_11"));

        gridLayout->addWidget(label_11, 4, 3, 1, 1);

        ymaxVal = new QDoubleSpinBox(DialogFindFromPlot);
        ymaxVal->setObjectName(QStringLiteral("ymaxVal"));
        ymaxVal->setDecimals(10);
        ymaxVal->setMinimum(-1e+09);
        ymaxVal->setMaximum(1e+09);

        gridLayout->addWidget(ymaxVal, 4, 4, 1, 1);

        label_3 = new QLabel(DialogFindFromPlot);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 5, 0, 1, 1);

        buttonBox = new QDialogButtonBox(DialogFindFromPlot);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 6, 4, 1, 1);


        retranslateUi(DialogFindFromPlot);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogFindFromPlot, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogFindFromPlot, SLOT(reject()));

        QMetaObject::connectSlotsByName(DialogFindFromPlot);
    } // setupUi

    void retranslateUi(QDialog *DialogFindFromPlot)
    {
        DialogFindFromPlot->setWindowTitle(QApplication::translate("DialogFindFromPlot", "Dialog", 0));
        label_4->setText(QApplication::translate("DialogFindFromPlot", "Select plot ordinate", 0));
        label->setText(QApplication::translate("DialogFindFromPlot", "Abscissa", 0));
        label_5->setText(QApplication::translate("DialogFindFromPlot", "Column", 0));
        label_6->setText(QApplication::translate("DialogFindFromPlot", "min", 0));
        label_7->setText(QApplication::translate("DialogFindFromPlot", "max", 0));
        label_2->setText(QApplication::translate("DialogFindFromPlot", "Ordinate", 0));
        label_9->setText(QApplication::translate("DialogFindFromPlot", "Column", 0));
        label_10->setText(QApplication::translate("DialogFindFromPlot", "min", 0));
        label_11->setText(QApplication::translate("DialogFindFromPlot", "max", 0));
        label_3->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class DialogFindFromPlot: public Ui_DialogFindFromPlot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGFINDFROMPLOT_H
