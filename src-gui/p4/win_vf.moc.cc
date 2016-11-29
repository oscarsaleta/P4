/****************************************************************************
** Meta object code from reading C++ file 'win_vf.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_vf.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_vf.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QVectorFieldDlg_t {
    QByteArrayData data[3];
    char stringdata0[34];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QVectorFieldDlg_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QVectorFieldDlg_t qt_meta_stringdata_QVectorFieldDlg = {
    {
QT_MOC_LITERAL(0, 0, 15), // "QVectorFieldDlg"
QT_MOC_LITERAL(1, 16, 16), // "numParamsChanged"
QT_MOC_LITERAL(2, 33, 0) // ""

    },
    "QVectorFieldDlg\0numParamsChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QVectorFieldDlg[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void QVectorFieldDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QVectorFieldDlg *_t = static_cast<QVectorFieldDlg *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->numParamsChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject QVectorFieldDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QVectorFieldDlg.data,
      qt_meta_data_QVectorFieldDlg,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QVectorFieldDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QVectorFieldDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QVectorFieldDlg.stringdata0))
        return static_cast<void*>(const_cast< QVectorFieldDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QVectorFieldDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_QVFParams_t {
    QByteArrayData data[3];
    char stringdata0[31];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QVFParams_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QVFParams_t qt_meta_stringdata_QVFParams = {
    {
QT_MOC_LITERAL(0, 0, 9), // "QVFParams"
QT_MOC_LITERAL(1, 10, 19), // "paramsSliderChanged"
QT_MOC_LITERAL(2, 30, 0) // ""

    },
    "QVFParams\0paramsSliderChanged\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QVFParams[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void QVFParams::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QVFParams *_t = static_cast<QVFParams *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->paramsSliderChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject QVFParams::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QVFParams.data,
      qt_meta_data_QVFParams,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QVFParams::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QVFParams::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QVFParams.stringdata0))
        return static_cast<void*>(const_cast< QVFParams*>(this));
    return QWidget::qt_metacast(_clname);
}

int QVFParams::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
