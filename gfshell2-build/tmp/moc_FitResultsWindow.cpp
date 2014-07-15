/****************************************************************************
** Meta object code from reading C++ file 'FitResultsWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../gfshell2/dialogs/FitResultsWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FitResultsWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_FitResultsWindow_t {
    QByteArrayData data[24];
    char stringdata[293];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_FitResultsWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_FitResultsWindow_t qt_meta_stringdata_FitResultsWindow = {
    {
QT_MOC_LITERAL(0, 0, 16),
QT_MOC_LITERAL(1, 17, 10),
QT_MOC_LITERAL(2, 28, 0),
QT_MOC_LITERAL(3, 29, 4),
QT_MOC_LITERAL(4, 34, 10),
QT_MOC_LITERAL(5, 45, 16),
QT_MOC_LITERAL(6, 62, 14),
QT_MOC_LITERAL(7, 77, 12),
QT_MOC_LITERAL(8, 90, 15),
QT_MOC_LITERAL(9, 106, 20),
QT_MOC_LITERAL(10, 127, 22),
QT_MOC_LITERAL(11, 150, 11),
QT_MOC_LITERAL(12, 162, 12),
QT_MOC_LITERAL(13, 175, 14),
QT_MOC_LITERAL(14, 190, 6),
QT_MOC_LITERAL(15, 197, 12),
QT_MOC_LITERAL(16, 210, 9),
QT_MOC_LITERAL(17, 220, 7),
QT_MOC_LITERAL(18, 228, 9),
QT_MOC_LITERAL(19, 238, 8),
QT_MOC_LITERAL(20, 247, 9),
QT_MOC_LITERAL(21, 257, 19),
QT_MOC_LITERAL(22, 277, 7),
QT_MOC_LITERAL(23, 285, 7)
    },
    "FitResultsWindow\0CmOpenFile\0\0dir_\0"
    "CmSaveFile\0CmSaveBsonRecord\0CmDeleteRecord\0"
    "CmBackupJSON\0CmAboutGEMSFITS\0"
    "CmAbout_Graph_Dialog\0CmAbout_Results_window\0"
    "CmPlotTable\0CmPrintTable\0CmFindFromPlot\0"
    "CmCalc\0SelectColumn\0SelectAll\0CutData\0"
    "ClearData\0CopyData\0PasteData\0"
    "PasteTransposedData\0ToggleX\0ToggleY"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_FitResultsWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,  124,    2, 0x0a /* Public */,
       1,    0,  127,    2, 0x2a /* Public | MethodCloned */,
       4,    0,  128,    2, 0x0a /* Public */,
       5,    0,  129,    2, 0x0a /* Public */,
       6,    0,  130,    2, 0x0a /* Public */,
       7,    0,  131,    2, 0x0a /* Public */,
       8,    0,  132,    2, 0x0a /* Public */,
       9,    0,  133,    2, 0x0a /* Public */,
      10,    0,  134,    2, 0x0a /* Public */,
      11,    0,  135,    2, 0x0a /* Public */,
      12,    0,  136,    2, 0x0a /* Public */,
      13,    0,  137,    2, 0x0a /* Public */,
      14,    0,  138,    2, 0x0a /* Public */,
      15,    0,  139,    2, 0x0a /* Public */,
      16,    0,  140,    2, 0x0a /* Public */,
      17,    0,  141,    2, 0x0a /* Public */,
      18,    0,  142,    2, 0x0a /* Public */,
      19,    0,  143,    2, 0x0a /* Public */,
      20,    0,  144,    2, 0x0a /* Public */,
      21,    0,  145,    2, 0x0a /* Public */,
      22,    0,  146,    2, 0x0a /* Public */,
      23,    0,  147,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void FitResultsWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        FitResultsWindow *_t = static_cast<FitResultsWindow *>(_o);
        switch (_id) {
        case 0: _t->CmOpenFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->CmOpenFile(); break;
        case 2: _t->CmSaveFile(); break;
        case 3: _t->CmSaveBsonRecord(); break;
        case 4: _t->CmDeleteRecord(); break;
        case 5: _t->CmBackupJSON(); break;
        case 6: _t->CmAboutGEMSFITS(); break;
        case 7: _t->CmAbout_Graph_Dialog(); break;
        case 8: _t->CmAbout_Results_window(); break;
        case 9: _t->CmPlotTable(); break;
        case 10: _t->CmPrintTable(); break;
        case 11: _t->CmFindFromPlot(); break;
        case 12: _t->CmCalc(); break;
        case 13: _t->SelectColumn(); break;
        case 14: _t->SelectAll(); break;
        case 15: _t->CutData(); break;
        case 16: _t->ClearData(); break;
        case 17: _t->CopyData(); break;
        case 18: _t->PasteData(); break;
        case 19: _t->PasteTransposedData(); break;
        case 20: _t->ToggleX(); break;
        case 21: _t->ToggleY(); break;
        default: ;
        }
    }
}

const QMetaObject FitResultsWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_FitResultsWindow.data,
      qt_meta_data_FitResultsWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *FitResultsWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FitResultsWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FitResultsWindow.stringdata))
        return static_cast<void*>(const_cast< FitResultsWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int FitResultsWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
