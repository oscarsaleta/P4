/****************************************************************************
** Meta object code from reading C++ file 'win_zoom.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_zoom.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_zoom.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QZoomWnd_t {
    QByteArrayData data[17];
    char stringdata0[170];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QZoomWnd_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QZoomWnd_t qt_meta_stringdata_QZoomWnd = {
    {
QT_MOC_LITERAL(0, 0, 8), // "QZoomWnd"
QT_MOC_LITERAL(1, 9, 17), // "Signal_Evaluating"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 16), // "Signal_Evaluated"
QT_MOC_LITERAL(4, 45, 14), // "Signal_Changed"
QT_MOC_LITERAL(5, 60, 10), // "OnBtnClose"
QT_MOC_LITERAL(6, 71, 12), // "OnBtnRefresh"
QT_MOC_LITERAL(7, 84, 10), // "OnBtnPrint"
QT_MOC_LITERAL(8, 95, 5), // "close"
QT_MOC_LITERAL(9, 101, 9), // "configure"
QT_MOC_LITERAL(10, 111, 11), // "customEvent"
QT_MOC_LITERAL(11, 123, 7), // "QEvent*"
QT_MOC_LITERAL(12, 131, 1), // "e"
QT_MOC_LITERAL(13, 133, 9), // "hideEvent"
QT_MOC_LITERAL(14, 143, 11), // "QHideEvent*"
QT_MOC_LITERAL(15, 155, 1), // "h"
QT_MOC_LITERAL(16, 157, 12) // "AdjustHeight"

    },
    "QZoomWnd\0Signal_Evaluating\0\0"
    "Signal_Evaluated\0Signal_Changed\0"
    "OnBtnClose\0OnBtnRefresh\0OnBtnPrint\0"
    "close\0configure\0customEvent\0QEvent*\0"
    "e\0hideEvent\0QHideEvent*\0h\0AdjustHeight"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QZoomWnd[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x0a /* Public */,
       3,    0,   70,    2, 0x0a /* Public */,
       4,    0,   71,    2, 0x0a /* Public */,
       5,    0,   72,    2, 0x0a /* Public */,
       6,    0,   73,    2, 0x0a /* Public */,
       7,    0,   74,    2, 0x0a /* Public */,
       8,    0,   75,    2, 0x0a /* Public */,
       9,    0,   76,    2, 0x0a /* Public */,
      10,    1,   77,    2, 0x0a /* Public */,
      13,    1,   80,    2, 0x0a /* Public */,
      16,    0,   83,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 14,   15,
    QMetaType::Void,

       0        // eod
};

void QZoomWnd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QZoomWnd *_t = static_cast<QZoomWnd *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->Signal_Evaluating(); break;
        case 1: _t->Signal_Evaluated(); break;
        case 2: _t->Signal_Changed(); break;
        case 3: _t->OnBtnClose(); break;
        case 4: _t->OnBtnRefresh(); break;
        case 5: _t->OnBtnPrint(); break;
        case 6: { bool _r = _t->close();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: _t->configure(); break;
        case 8: _t->customEvent((*reinterpret_cast< QEvent*(*)>(_a[1]))); break;
        case 9: _t->hideEvent((*reinterpret_cast< QHideEvent*(*)>(_a[1]))); break;
        case 10: _t->AdjustHeight(); break;
        default: ;
        }
    }
}

const QMetaObject QZoomWnd::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QZoomWnd.data,
      qt_meta_data_QZoomWnd,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QZoomWnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QZoomWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QZoomWnd.stringdata0))
        return static_cast<void*>(const_cast< QZoomWnd*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QZoomWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
