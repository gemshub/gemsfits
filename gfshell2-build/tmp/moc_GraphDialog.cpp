/****************************************************************************
** Meta object code from reading C++ file 'GraphDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../gfshell2/dialogs/GraphDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GraphDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LabelDelegate_t {
    QByteArrayData data[1];
    char stringdata[14];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LabelDelegate_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LabelDelegate_t qt_meta_stringdata_LabelDelegate = {
    {
QT_MOC_LITERAL(0, 0, 13)
    },
    "LabelDelegate"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LabelDelegate[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void LabelDelegate::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject LabelDelegate::staticMetaObject = {
    { &QItemDelegate::staticMetaObject, qt_meta_stringdata_LabelDelegate.data,
      qt_meta_data_LabelDelegate,  qt_static_metacall, 0, 0}
};


const QMetaObject *LabelDelegate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LabelDelegate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LabelDelegate.stringdata))
        return static_cast<void*>(const_cast< LabelDelegate*>(this));
    return QItemDelegate::qt_metacast(_clname);
}

int LabelDelegate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QItemDelegate::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_GraphDialog_t {
    QByteArrayData data[13];
    char stringdata[119];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GraphDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GraphDialog_t qt_meta_stringdata_GraphDialog = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 10),
QT_MOC_LITERAL(2, 23, 0),
QT_MOC_LITERAL(3, 24, 3),
QT_MOC_LITERAL(4, 28, 6),
QT_MOC_LITERAL(5, 35, 9),
QT_MOC_LITERAL(6, 45, 14),
QT_MOC_LITERAL(7, 60, 10),
QT_MOC_LITERAL(8, 71, 8),
QT_MOC_LITERAL(9, 80, 7),
QT_MOC_LITERAL(10, 88, 6),
QT_MOC_LITERAL(11, 95, 6),
QT_MOC_LITERAL(12, 102, 16)
    },
    "GraphDialog\0changeIcon\0\0row\0column\0"
    "changeNdx\0languageChange\0CmFragment\0"
    "CmLegend\0CmPrint\0CmSave\0CmHelp\0"
    "selectedFragment"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GraphDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   59,    2, 0x09 /* Protected */,
       5,    2,   64,    2, 0x09 /* Protected */,
       6,    0,   69,    2, 0x09 /* Protected */,
       7,    0,   70,    2, 0x09 /* Protected */,
       8,    0,   71,    2, 0x09 /* Protected */,
       9,    0,   72,    2, 0x09 /* Protected */,
      10,    0,   73,    2, 0x09 /* Protected */,
      11,    0,   74,    2, 0x09 /* Protected */,
      12,    1,   75,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QRectF,    2,

       0        // eod
};

void GraphDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GraphDialog *_t = static_cast<GraphDialog *>(_o);
        switch (_id) {
        case 0: _t->changeIcon((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->changeNdx((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->languageChange(); break;
        case 3: _t->CmFragment(); break;
        case 4: _t->CmLegend(); break;
        case 5: _t->CmPrint(); break;
        case 6: _t->CmSave(); break;
        case 7: _t->CmHelp(); break;
        case 8: _t->selectedFragment((*reinterpret_cast< const QRectF(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject GraphDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GraphDialog.data,
      qt_meta_data_GraphDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *GraphDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GraphDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GraphDialog.stringdata))
        return static_cast<void*>(const_cast< GraphDialog*>(this));
    if (!strcmp(_clname, "Ui::GraphDialogData"))
        return static_cast< Ui::GraphDialogData*>(const_cast< GraphDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int GraphDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
