/****************************************************************************
** Meta object code from reading C++ file 'win_orbits.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_orbits.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_orbits.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QOrbitsDlg_t {
    QByteArrayData data[10];
    char stringdata0[125];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QOrbitsDlg_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QOrbitsDlg_t qt_meta_stringdata_QOrbitsDlg = {
    {
QT_MOC_LITERAL(0, 0, 10), // "QOrbitsDlg"
QT_MOC_LITERAL(1, 11, 10), // "OrbitEvent"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 12), // "onbtn_select"
QT_MOC_LITERAL(4, 36, 15), // "onbtn_backwards"
QT_MOC_LITERAL(5, 52, 14), // "onbtn_continue"
QT_MOC_LITERAL(6, 67, 14), // "onbtn_forwards"
QT_MOC_LITERAL(7, 82, 12), // "onbtn_delall"
QT_MOC_LITERAL(8, 95, 13), // "onbtn_dellast"
QT_MOC_LITERAL(9, 109, 15) // "setInitialPoint"

    },
    "QOrbitsDlg\0OrbitEvent\0\0onbtn_select\0"
    "onbtn_backwards\0onbtn_continue\0"
    "onbtn_forwards\0onbtn_delall\0onbtn_dellast\0"
    "setInitialPoint"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QOrbitsDlg[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x0a /* Public */,
       3,    0,   57,    2, 0x0a /* Public */,
       4,    0,   58,    2, 0x0a /* Public */,
       5,    0,   59,    2, 0x0a /* Public */,
       6,    0,   60,    2, 0x0a /* Public */,
       7,    0,   61,    2, 0x0a /* Public */,
       8,    0,   62,    2, 0x0a /* Public */,
       9,    2,   63,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    2,    2,

       0        // eod
};

void QOrbitsDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QOrbitsDlg *_t = static_cast<QOrbitsDlg *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OrbitEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->onbtn_select(); break;
        case 2: _t->onbtn_backwards(); break;
        case 3: _t->onbtn_continue(); break;
        case 4: _t->onbtn_forwards(); break;
        case 5: _t->onbtn_delall(); break;
        case 6: _t->onbtn_dellast(); break;
        case 7: _t->setInitialPoint((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject QOrbitsDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QOrbitsDlg.data,
      qt_meta_data_QOrbitsDlg,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QOrbitsDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QOrbitsDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QOrbitsDlg.stringdata0))
        return static_cast<void*>(const_cast< QOrbitsDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QOrbitsDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
