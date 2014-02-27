/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Thu Nov 24 13:47:08 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      36,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      27,   11,   11,   11, 0x0a,
      42,   11,   11,   11, 0x0a,
      57,   11,   11,   11, 0x0a,
      72,   11,   11,   11, 0x0a,
      87,   11,   11,   11, 0x0a,
     102,   11,   11,   11, 0x0a,
     117,   11,   11,   11, 0x0a,
     132,   11,   11,   11, 0x0a,
     146,   11,   11,   11, 0x0a,
     166,   11,   11,   11, 0x0a,
     189,   11,   11,   11, 0x0a,
     215,   11,   11,   11, 0x0a,
     235,   11,   11,   11, 0x0a,
     256,   11,   11,   11, 0x0a,
     282,   11,   11,   11, 0x0a,
     312,   11,   11,   11, 0x0a,
     325,   11,   11,   11, 0x0a,
     351,   11,   11,   11, 0x0a,
     382,  378,   11,   11, 0x0a,
     428,   11,   11,   11, 0x0a,
     442,   11,   11,   11, 0x0a,
     463,  460,   11,   11, 0x0a,
     497,   11,   11,   11, 0x0a,
     519,   11,   11,   11, 0x0a,
     545,   11,   11,   11, 0x0a,
     557,   11,   11,   11, 0x0a,
     569,   11,   11,   11, 0x0a,
     584,   11,   11,   11, 0x0a,
     598,   11,   11,   11, 0x0a,
     613,   11,   11,   11, 0x0a,
     628,   11,   11,   11, 0x0a,
     652,   11,   11,   11, 0x0a,
     682,   11,   11,   11, 0x0a,
     692,   11,   11,   11, 0x0a,
     718,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0action1_slot()\0action2_slot()\0"
    "action3_slot()\0action4_slot()\0"
    "action5_slot()\0action6_slot()\0"
    "action7_slot()\0action8_slot()\0"
    "show_upload()\0upload_GEMS_input()\0"
    "check_new_parameters()\0write_check_config_file()\0"
    "write_config_file()\0update_rows_bounds()\0"
    "update_rows_constraints()\0"
    "update_rows_print_meas_comp()\0"
    "csv_or_sql()\0find_clicked_param_aDCc()\0"
    "find_clicked_param_aIPxc()\0,,,\0"
    "find_id_fit_param_in_bounds(int&,int,int,int)\0"
    "run_GEMSFIT()\0set_cell_border()\0,,\0"
    "find_id_fit_param(int&,int&,int&)\0"
    "action_DB_mode_slot()\0establish_DB_connection()\0"
    "refreshDB()\0showTable()\0submitall_DB()\0"
    "clearall_DB()\0insertrow_DB()\0"
    "removerow_DB()\0ShowContextMenu(QPoint)\0"
    "create_act_model_data_table()\0execSQL()\0"
    "create_SQLscript_dialog()\0"
    "showSQLtemplate(int)\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    if (!strcmp(_clname, "System_Properties"))
        return static_cast< System_Properties*>(const_cast< MainWindow*>(this));
    if (!strcmp(_clname, "SqlDBManager"))
        return static_cast< SqlDBManager*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: action1_slot(); break;
        case 1: action2_slot(); break;
        case 2: action3_slot(); break;
        case 3: action4_slot(); break;
        case 4: action5_slot(); break;
        case 5: action6_slot(); break;
        case 6: action7_slot(); break;
        case 7: action8_slot(); break;
        case 8: show_upload(); break;
        case 9: upload_GEMS_input(); break;
        case 10: check_new_parameters(); break;
        case 11: write_check_config_file(); break;
        case 12: write_config_file(); break;
        case 13: update_rows_bounds(); break;
        case 14: update_rows_constraints(); break;
        case 15: update_rows_print_meas_comp(); break;
        case 16: csv_or_sql(); break;
        case 17: find_clicked_param_aDCc(); break;
        case 18: find_clicked_param_aIPxc(); break;
        case 19: find_id_fit_param_in_bounds((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const int(*)>(_a[2])),(*reinterpret_cast< const int(*)>(_a[3])),(*reinterpret_cast< const int(*)>(_a[4]))); break;
        case 20: run_GEMSFIT(); break;
        case 21: set_cell_border(); break;
        case 22: find_id_fit_param((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 23: action_DB_mode_slot(); break;
        case 24: establish_DB_connection(); break;
        case 25: refreshDB(); break;
        case 26: showTable(); break;
        case 27: submitall_DB(); break;
        case 28: clearall_DB(); break;
        case 29: insertrow_DB(); break;
        case 30: removerow_DB(); break;
        case 31: ShowContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 32: create_act_model_data_table(); break;
        case 33: execSQL(); break;
        case 34: create_SQLscript_dialog(); break;
        case 35: showSQLtemplate((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 36;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
