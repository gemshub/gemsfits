/********************************************************************************
** Form generated from reading UI file 'GraphDialog4.ui'
**
** Created by: Qt User Interface Compiler version 5.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHDIALOG4_H
#define UI_GRAPHDIALOG4_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GraphDialogData
{
public:
    QGridLayout *gridLayout;
    QFrame *framePrn;
    QHBoxLayout *horizontalLayout;
    QSplitter *splitter;
    QFrame *plotFrame;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *pGrpLegend;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout1;
    QLabel *TextLabel1;
    QPushButton *pFragmentBtn;
    QPushButton *pBtn4;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pBtn3;
    QPushButton *PushButton5;
    QPushButton *pHelp;
    QButtonGroup *buttonGroup;

    void setupUi(QDialog *GraphDialogData)
    {
        if (GraphDialogData->objectName().isEmpty())
            GraphDialogData->setObjectName(QStringLiteral("GraphDialogData"));
        GraphDialogData->setWindowModality(Qt::WindowModal);
        GraphDialogData->resize(895, 471);
        QFont font;
        font.setPointSize(9);
        GraphDialogData->setFont(font);
        GraphDialogData->setModal(false);
        gridLayout = new QGridLayout(GraphDialogData);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        framePrn = new QFrame(GraphDialogData);
        framePrn->setObjectName(QStringLiteral("framePrn"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(framePrn->sizePolicy().hasHeightForWidth());
        framePrn->setSizePolicy(sizePolicy);
        framePrn->setFrameShape(QFrame::StyledPanel);
        framePrn->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(framePrn);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        splitter = new QSplitter(framePrn);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        plotFrame = new QFrame(splitter);
        plotFrame->setObjectName(QStringLiteral("plotFrame"));
        sizePolicy.setHeightForWidth(plotFrame->sizePolicy().hasHeightForWidth());
        plotFrame->setSizePolicy(sizePolicy);
        plotFrame->setMinimumSize(QSize(400, 400));
        plotFrame->setFrameShape(QFrame::StyledPanel);
        plotFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(plotFrame);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        splitter->addWidget(plotFrame);
        widget = new QWidget(splitter);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout_3 = new QVBoxLayout(widget);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        pGrpLegend = new QGroupBox(widget);
        pGrpLegend->setObjectName(QStringLiteral("pGrpLegend"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pGrpLegend->sizePolicy().hasHeightForWidth());
        pGrpLegend->setSizePolicy(sizePolicy1);
        verticalLayout = new QVBoxLayout(pGrpLegend);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));

        verticalLayout_3->addWidget(pGrpLegend);

        groupBox = new QGroupBox(widget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy2);
        gridLayout1 = new QGridLayout(groupBox);
        gridLayout1->setSpacing(6);
        gridLayout1->setContentsMargins(11, 11, 11, 11);
        gridLayout1->setObjectName(QStringLiteral("gridLayout1"));
        TextLabel1 = new QLabel(groupBox);
        TextLabel1->setObjectName(QStringLiteral("TextLabel1"));
        sizePolicy2.setHeightForWidth(TextLabel1->sizePolicy().hasHeightForWidth());
        TextLabel1->setSizePolicy(sizePolicy2);
        TextLabel1->setAlignment(Qt::AlignVCenter);
        TextLabel1->setWordWrap(true);

        gridLayout1->addWidget(TextLabel1, 0, 0, 1, 1);


        verticalLayout_3->addWidget(groupBox);

        splitter->addWidget(widget);

        horizontalLayout->addWidget(splitter);


        gridLayout->addWidget(framePrn, 0, 0, 1, 6);

        pFragmentBtn = new QPushButton(GraphDialogData);
        buttonGroup = new QButtonGroup(GraphDialogData);
        buttonGroup->setObjectName(QStringLiteral("buttonGroup"));
        buttonGroup->addButton(pFragmentBtn);
        pFragmentBtn->setObjectName(QStringLiteral("pFragmentBtn"));
        sizePolicy2.setHeightForWidth(pFragmentBtn->sizePolicy().hasHeightForWidth());
        pFragmentBtn->setSizePolicy(sizePolicy2);
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        pFragmentBtn->setFont(font1);

        gridLayout->addWidget(pFragmentBtn, 1, 0, 1, 1);

        pBtn4 = new QPushButton(GraphDialogData);
        buttonGroup->addButton(pBtn4);
        pBtn4->setObjectName(QStringLiteral("pBtn4"));
        sizePolicy2.setHeightForWidth(pBtn4->sizePolicy().hasHeightForWidth());
        pBtn4->setSizePolicy(sizePolicy2);
        pBtn4->setFont(font1);
        pBtn4->setCheckable(false);

        gridLayout->addWidget(pBtn4, 1, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(104, 21, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 2, 1, 1);

        pBtn3 = new QPushButton(GraphDialogData);
        buttonGroup->addButton(pBtn3);
        pBtn3->setObjectName(QStringLiteral("pBtn3"));
        sizePolicy2.setHeightForWidth(pBtn3->sizePolicy().hasHeightForWidth());
        pBtn3->setSizePolicy(sizePolicy2);
        pBtn3->setFont(font1);

        gridLayout->addWidget(pBtn3, 1, 3, 1, 1);

        PushButton5 = new QPushButton(GraphDialogData);
        buttonGroup->addButton(PushButton5);
        PushButton5->setObjectName(QStringLiteral("PushButton5"));
        sizePolicy2.setHeightForWidth(PushButton5->sizePolicy().hasHeightForWidth());
        PushButton5->setSizePolicy(sizePolicy2);
        PushButton5->setFont(font1);

        gridLayout->addWidget(PushButton5, 1, 4, 1, 1);

        pHelp = new QPushButton(GraphDialogData);
        buttonGroup->addButton(pHelp);
        pHelp->setObjectName(QStringLiteral("pHelp"));
        sizePolicy2.setHeightForWidth(pHelp->sizePolicy().hasHeightForWidth());
        pHelp->setSizePolicy(sizePolicy2);
        pHelp->setFont(font1);

        gridLayout->addWidget(pHelp, 1, 5, 1, 1);

        QWidget::setTabOrder(pFragmentBtn, pBtn3);
        QWidget::setTabOrder(pBtn3, pBtn4);

        retranslateUi(GraphDialogData);
        QObject::connect(pBtn4, SIGNAL(clicked()), GraphDialogData, SLOT(CmLegend()));
        QObject::connect(pFragmentBtn, SIGNAL(clicked()), GraphDialogData, SLOT(CmFragment()));
        QObject::connect(pBtn3, SIGNAL(clicked()), GraphDialogData, SLOT(CmPrint()));
        QObject::connect(PushButton5, SIGNAL(clicked()), GraphDialogData, SLOT(CmSave()));

        QMetaObject::connectSlotsByName(GraphDialogData);
    } // setupUi

    void retranslateUi(QDialog *GraphDialogData)
    {
        GraphDialogData->setWindowTitle(QApplication::translate("GraphDialogData", "GEM-Selektor v.3 Graphics Dialog", 0));
        pGrpLegend->setTitle(QApplication::translate("GraphDialogData", "Legend| x# |Label Y ", 0));
        groupBox->setTitle(QString());
        TextLabel1->setText(QApplication::translate("GraphDialogData", "Click legend symbols to adjust curves; select abscissae under x#; edit Y (ordinate) labels, drag-drop them to plot area", 0));
#ifndef QT_NO_TOOLTIP
        pFragmentBtn->setToolTip(QApplication::translate("GraphDialogData", "Toggle between a fragment and a full plot; adjust sizes under Customize...", 0));
#endif // QT_NO_TOOLTIP
        pFragmentBtn->setText(QApplication::translate("GraphDialogData", "&Fragment", 0));
#ifndef QT_NO_TOOLTIP
        pBtn4->setToolTip(QApplication::translate("GraphDialogData", "Click to customize graphics type, x and y axis, legend labels, fonts, background color", 0));
#endif // QT_NO_TOOLTIP
        pBtn4->setText(QApplication::translate("GraphDialogData", "&Customize", 0));
#ifndef QT_NO_TOOLTIP
        pBtn3->setToolTip(QApplication::translate("GraphDialogData", "Print the plot and its legend on system printers", 0));
#endif // QT_NO_TOOLTIP
        pBtn3->setText(QApplication::translate("GraphDialogData", "&Print", 0));
#ifndef QT_NO_TOOLTIP
        PushButton5->setToolTip(QApplication::translate("GraphDialogData", "Save the plot as graphical image in a (vector or bitmap) graphic format file", 0));
#endif // QT_NO_TOOLTIP
        PushButton5->setText(QApplication::translate("GraphDialogData", "Save Image", 0));
#ifndef QT_NO_TOOLTIP
        pHelp->setToolTip(QApplication::translate("GraphDialogData", "Show help pages for this widget", 0));
#endif // QT_NO_TOOLTIP
        pHelp->setText(QApplication::translate("GraphDialogData", "&Help", 0));
        pHelp->setShortcut(QApplication::translate("GraphDialogData", "F1", 0));
    } // retranslateUi

};

namespace Ui {
    class GraphDialogData: public Ui_GraphDialogData {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHDIALOG4_H
