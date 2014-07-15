/****************************************************************************
** Meta object code from reading C++ file 'HelpWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../gfshell2/dialogs/HelpWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HelpWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_HelpBrowser_t {
    QByteArrayData data[1];
    char stringdata[12];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HelpBrowser_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HelpBrowser_t qt_meta_stringdata_HelpBrowser = {
    {
QT_MOC_LITERAL(0, 0, 11)
    },
    "HelpBrowser"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HelpBrowser[] = {

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

void HelpBrowser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObject HelpBrowser::staticMetaObject = {
    { &QTextBrowser::staticMetaObject, qt_meta_stringdata_HelpBrowser.data,
      qt_meta_data_HelpBrowser,  qt_static_metacall, 0, 0}
};


const QMetaObject *HelpBrowser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HelpBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HelpBrowser.stringdata))
        return static_cast<void*>(const_cast< HelpBrowser*>(this));
    return QTextBrowser::qt_metacast(_clname);
}

int HelpBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextBrowser::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
struct qt_meta_stringdata_SearchWidget_t {
    QByteArrayData data[8];
    char stringdata[81];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SearchWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SearchWidget_t qt_meta_stringdata_SearchWidget = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 15),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 3),
QT_MOC_LITERAL(4, 34, 6),
QT_MOC_LITERAL(5, 41, 16),
QT_MOC_LITERAL(6, 58, 17),
QT_MOC_LITERAL(7, 76, 4)
    },
    "SearchWidget\0requestShowLink\0\0url\0"
    "search\0searchingStarted\0searchingFinished\0"
    "hits"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SearchWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   37,    2, 0x08 /* Private */,
       5,    0,   38,    2, 0x08 /* Private */,
       6,    1,   39,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QUrl,    3,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,

       0        // eod
};

void SearchWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SearchWidget *_t = static_cast<SearchWidget *>(_o);
        switch (_id) {
        case 0: _t->requestShowLink((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 1: _t->search(); break;
        case 2: _t->searchingStarted(); break;
        case 3: _t->searchingFinished((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SearchWidget::*_t)(const QUrl & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SearchWidget::requestShowLink)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject SearchWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SearchWidget.data,
      qt_meta_data_SearchWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *SearchWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SearchWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SearchWidget.stringdata))
        return static_cast<void*>(const_cast< SearchWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int SearchWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void SearchWidget::requestShowLink(const QUrl & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_HelpWindow_t {
    QByteArrayData data[19];
    char stringdata[214];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HelpWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HelpWindow_t qt_meta_stringdata_HelpWindow = {
    {
QT_MOC_LITERAL(0, 0, 10),
QT_MOC_LITERAL(1, 11, 14),
QT_MOC_LITERAL(2, 26, 0),
QT_MOC_LITERAL(3, 27, 12),
QT_MOC_LITERAL(4, 40, 4),
QT_MOC_LITERAL(5, 45, 13),
QT_MOC_LITERAL(6, 59, 6),
QT_MOC_LITERAL(7, 66, 10),
QT_MOC_LITERAL(8, 77, 12),
QT_MOC_LITERAL(9, 90, 9),
QT_MOC_LITERAL(10, 100, 11),
QT_MOC_LITERAL(11, 112, 9),
QT_MOC_LITERAL(12, 122, 10),
QT_MOC_LITERAL(13, 133, 10),
QT_MOC_LITERAL(14, 144, 14),
QT_MOC_LITERAL(15, 159, 18),
QT_MOC_LITERAL(16, 178, 12),
QT_MOC_LITERAL(17, 191, 13),
QT_MOC_LITERAL(18, 205, 8)
    },
    "HelpWindow\0languageChange\0\0loadResource\0"
    "name\0filterIndices\0filter\0showAddres\0"
    "syncContents\0helpAbout\0helpVersion\0"
    "helpPrint\0helpOnHelp\0actionFind\0"
    "actionFindNext\0actionFindPrevious\0"
    "actionZoomIn\0actionZoomOut\0showFind"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HelpWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   89,    2, 0x09 /* Protected */,
       3,    1,   90,    2, 0x09 /* Protected */,
       5,    1,   93,    2, 0x09 /* Protected */,
       7,    1,   96,    2, 0x09 /* Protected */,
       8,    0,   99,    2, 0x09 /* Protected */,
       9,    0,  100,    2, 0x09 /* Protected */,
      10,    0,  101,    2, 0x09 /* Protected */,
      11,    0,  102,    2, 0x09 /* Protected */,
      12,    0,  103,    2, 0x09 /* Protected */,
      13,    0,  104,    2, 0x09 /* Protected */,
      14,    0,  105,    2, 0x09 /* Protected */,
      15,    0,  106,    2, 0x09 /* Protected */,
      16,    0,  107,    2, 0x09 /* Protected */,
      17,    0,  108,    2, 0x09 /* Protected */,
      18,    0,  109,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QUrl,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QUrl,    4,
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

void HelpWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HelpWindow *_t = static_cast<HelpWindow *>(_o);
        switch (_id) {
        case 0: _t->languageChange(); break;
        case 1: _t->loadResource((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 2: _t->filterIndices((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->showAddres((*reinterpret_cast< const QUrl(*)>(_a[1]))); break;
        case 4: _t->syncContents(); break;
        case 5: _t->helpAbout(); break;
        case 6: _t->helpVersion(); break;
        case 7: _t->helpPrint(); break;
        case 8: _t->helpOnHelp(); break;
        case 9: _t->actionFind(); break;
        case 10: _t->actionFindNext(); break;
        case 11: _t->actionFindPrevious(); break;
        case 12: _t->actionZoomIn(); break;
        case 13: _t->actionZoomOut(); break;
        case 14: _t->showFind(); break;
        default: ;
        }
    }
}

const QMetaObject HelpWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_HelpWindow.data,
      qt_meta_data_HelpWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *HelpWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HelpWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HelpWindow.stringdata))
        return static_cast<void*>(const_cast< HelpWindow*>(this));
    if (!strcmp(_clname, "Ui::HelpWindowData"))
        return static_cast< Ui::HelpWindowData*>(const_cast< HelpWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int HelpWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
