/****************************************************************************
** Meta object code from reading C++ file 'LegendDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../gfshell2/dialogs/LegendDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LegendDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LegendDialog_t {
    QByteArrayData data[9];
    char stringdata[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LegendDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LegendDialog_t qt_meta_stringdata_LegendDialog = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 7),
QT_MOC_LITERAL(2, 21, 0),
QT_MOC_LITERAL(3, 22, 6),
QT_MOC_LITERAL(4, 29, 6),
QT_MOC_LITERAL(5, 36, 12),
QT_MOC_LITERAL(6, 49, 17),
QT_MOC_LITERAL(7, 67, 17),
QT_MOC_LITERAL(8, 85, 14)
    },
    "LegendDialog\0CmApply\0\0CmHelp\0accept\0"
    "CmBackground\0CmChangeLabelFont\0"
    "CmChangeGraphType\0languageChange"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LegendDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x09 /* Protected */,
       3,    0,   50,    2, 0x09 /* Protected */,
       4,    0,   51,    2, 0x09 /* Protected */,
       5,    0,   52,    2, 0x09 /* Protected */,
       6,    0,   53,    2, 0x09 /* Protected */,
       7,    1,   54,    2, 0x09 /* Protected */,
       8,    0,   57,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,

       0        // eod
};

void LegendDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LegendDialog *_t = static_cast<LegendDialog *>(_o);
        switch (_id) {
        case 0: _t->CmApply(); break;
        case 1: _t->CmHelp(); break;
        case 2: _t->accept(); break;
        case 3: _t->CmBackground(); break;
        case 4: _t->CmChangeLabelFont(); break;
        case 5: _t->CmChangeGraphType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->languageChange(); break;
        default: ;
        }
    }
}

const QMetaObject LegendDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_LegendDialog.data,
      qt_meta_data_LegendDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *LegendDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LegendDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LegendDialog.stringdata))
        return static_cast<void*>(const_cast< LegendDialog*>(this));
    if (!strcmp(_clname, "Ui::LegendDialogData"))
        return static_cast< Ui::LegendDialogData*>(const_cast< LegendDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int LegendDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
