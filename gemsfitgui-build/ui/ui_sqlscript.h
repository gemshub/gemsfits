/********************************************************************************
** Form generated from reading UI file 'sqlscript.ui'
**
** Created: Fri Jan 27 16:10:11 2012
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SQLSCRIPT_H
#define UI_SQLSCRIPT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QTreeWidget>

QT_BEGIN_NAMESPACE

class Ui_SQLscript
{
public:
    QTreeWidget *treeWidget_sqlscripts;

    void setupUi(QDialog *SQLscript)
    {
        if (SQLscript->objectName().isEmpty())
            SQLscript->setObjectName(QString::fromUtf8("SQLscript"));
        SQLscript->resize(400, 300);
        treeWidget_sqlscripts = new QTreeWidget(SQLscript);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        new QTreeWidgetItem(treeWidget_sqlscripts);
        treeWidget_sqlscripts->setObjectName(QString::fromUtf8("treeWidget_sqlscripts"));
        treeWidget_sqlscripts->setGeometry(QRect(30, 10, 331, 271));

        retranslateUi(SQLscript);

        QMetaObject::connectSlotsByName(SQLscript);
    } // setupUi

    void retranslateUi(QDialog *SQLscript)
    {
        SQLscript->setWindowTitle(QApplication::translate("SQLscript", "SQLscript", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget_sqlscripts->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("SQLscript", "PostgreSQL scripts", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = treeWidget_sqlscripts->isSortingEnabled();
        treeWidget_sqlscripts->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = treeWidget_sqlscripts->topLevelItem(0);
        ___qtreewidgetitem1->setText(0, QApplication::translate("SQLscript", "Update table", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem2 = treeWidget_sqlscripts->topLevelItem(1);
        ___qtreewidgetitem2->setText(0, QApplication::translate("SQLscript", "Drop table", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem3 = treeWidget_sqlscripts->topLevelItem(2);
        ___qtreewidgetitem3->setText(0, QApplication::translate("SQLscript", "Create table from another table", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem4 = treeWidget_sqlscripts->topLevelItem(3);
        ___qtreewidgetitem4->setText(0, QApplication::translate("SQLscript", "Copy data from csv file into table", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem5 = treeWidget_sqlscripts->topLevelItem(4);
        ___qtreewidgetitem5->setText(0, QApplication::translate("SQLscript", "Insert values into table", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem6 = treeWidget_sqlscripts->topLevelItem(5);
        ___qtreewidgetitem6->setText(0, QApplication::translate("SQLscript", "Truncate table", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem7 = treeWidget_sqlscripts->topLevelItem(6);
        ___qtreewidgetitem7->setText(0, QApplication::translate("SQLscript", "Create procedural language", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem8 = treeWidget_sqlscripts->topLevelItem(7);
        ___qtreewidgetitem8->setText(0, QApplication::translate("SQLscript", "Select distinct ", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem9 = treeWidget_sqlscripts->topLevelItem(8);
        ___qtreewidgetitem9->setText(0, QApplication::translate("SQLscript", "Drop function with cascade", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem10 = treeWidget_sqlscripts->topLevelItem(9);
        ___qtreewidgetitem10->setText(0, QApplication::translate("SQLscript", "Count query return values", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem11 = treeWidget_sqlscripts->topLevelItem(10);
        ___qtreewidgetitem11->setText(0, QApplication::translate("SQLscript", "Export table to csv", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem12 = treeWidget_sqlscripts->topLevelItem(11);
        ___qtreewidgetitem12->setText(0, QApplication::translate("SQLscript", "Export specific data from table to csv", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem13 = treeWidget_sqlscripts->topLevelItem(12);
        ___qtreewidgetitem13->setText(0, QApplication::translate("SQLscript", "Select distinct values and subtract value ", 0, QApplication::UnicodeUTF8));
        treeWidget_sqlscripts->setSortingEnabled(__sortingEnabled);

    } // retranslateUi

};

namespace Ui {
    class SQLscript: public Ui_SQLscript {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SQLSCRIPT_H
