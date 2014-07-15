/****************************************************************************
** Meta object code from reading C++ file 'DBKeyDialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../gfshell2/dialogs/DBKeyDialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DBKeyDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_DBKeyDialog_t {
    QByteArrayData data[6];
    char stringdata[52];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DBKeyDialog_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DBKeyDialog_t qt_meta_stringdata_DBKeyDialog = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 11),
QT_MOC_LITERAL(2, 24, 0),
QT_MOC_LITERAL(3, 25, 10),
QT_MOC_LITERAL(4, 36, 8),
QT_MOC_LITERAL(5, 45, 6)
    },
    "DBKeyDialog\0CmSelectAll\0\0CmClearAll\0"
    "CmFilter\0CmHelp"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DBKeyDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x09 /* Protected */,
       3,    0,   35,    2, 0x09 /* Protected */,
       4,    0,   36,    2, 0x09 /* Protected */,
       5,    0,   37,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DBKeyDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DBKeyDialog *_t = static_cast<DBKeyDialog *>(_o);
        switch (_id) {
        case 0: _t->CmSelectAll(); break;
        case 1: _t->CmClearAll(); break;
        case 2: _t->CmFilter(); break;
        case 3: _t->CmHelp(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject DBKeyDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DBKeyDialog.data,
      qt_meta_data_DBKeyDialog,  qt_static_metacall, 0, 0}
};


const QMetaObject *DBKeyDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DBKeyDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DBKeyDialog.stringdata))
        return static_cast<void*>(const_cast< DBKeyDialog*>(this));
    if (!strcmp(_clname, "Ui::KeyDialogData"))
        return static_cast< Ui::KeyDialogData*>(const_cast< DBKeyDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int DBKeyDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
struct qt_meta_stringdata_DBKeyFilter_t {
    QByteArrayData data[7];
    char stringdata[55];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DBKeyFilter_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DBKeyFilter_t qt_meta_stringdata_DBKeyFilter = {
    {
QT_MOC_LITERAL(0, 0, 11),
QT_MOC_LITERAL(1, 12, 8),
QT_MOC_LITERAL(2, 21, 0),
QT_MOC_LITERAL(3, 22, 9),
QT_MOC_LITERAL(4, 32, 4),
QT_MOC_LITERAL(5, 37, 6),
QT_MOC_LITERAL(6, 44, 10)
    },
    "DBKeyFilter\0EvSetAll\0\0EvGetList\0CmOk\0"
    "CmHelp\0setKeyLine"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DBKeyFilter[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x09 /* Protected */,
       3,    0,   40,    2, 0x09 /* Protected */,
       4,    0,   41,    2, 0x09 /* Protected */,
       5,    0,   42,    2, 0x09 /* Protected */,
       6,    0,   43,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DBKeyFilter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DBKeyFilter *_t = static_cast<DBKeyFilter *>(_o);
        switch (_id) {
        case 0: _t->EvSetAll(); break;
        case 1: _t->EvGetList(); break;
        case 2: _t->CmOk(); break;
        case 3: _t->CmHelp(); break;
        case 4: _t->setKeyLine(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject DBKeyFilter::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DBKeyFilter.data,
      qt_meta_data_DBKeyFilter,  qt_static_metacall, 0, 0}
};


const QMetaObject *DBKeyFilter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DBKeyFilter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DBKeyFilter.stringdata))
        return static_cast<void*>(const_cast< DBKeyFilter*>(this));
    return QDialog::qt_metacast(_clname);
}

int DBKeyFilter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
