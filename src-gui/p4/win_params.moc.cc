/****************************************************************************
** Meta object code from reading C++ file 'win_params.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_params.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_params.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QParamsDlg_t {
    QByteArrayData data[7];
    char stringdata0[95];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QParamsDlg_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QParamsDlg_t qt_meta_stringdata_QParamsDlg = {
    {
QT_MOC_LITERAL(0, 0, 10), // "QParamsDlg"
QT_MOC_LITERAL(1, 11, 15), // "btn_num_toggled"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 15), // "btn_alg_toggled"
QT_MOC_LITERAL(4, 44, 18), // "btn_sepyes_toggled"
QT_MOC_LITERAL(5, 63, 17), // "btn_sepno_toggled"
QT_MOC_LITERAL(6, 81, 13) // "OnLevelChange"

    },
    "QParamsDlg\0btn_num_toggled\0\0btn_alg_toggled\0"
    "btn_sepyes_toggled\0btn_sepno_toggled\0"
    "OnLevelChange"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QParamsDlg[] = {

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
       1,    1,   39,    2, 0x0a /* Public */,
       3,    1,   42,    2, 0x0a /* Public */,
       4,    1,   45,    2, 0x0a /* Public */,
       5,    1,   48,    2, 0x0a /* Public */,
       6,    1,   51,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void QParamsDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QParamsDlg *_t = static_cast<QParamsDlg *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->btn_num_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->btn_alg_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->btn_sepyes_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->btn_sepno_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->OnLevelChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject QParamsDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QParamsDlg.data,
      qt_meta_data_QParamsDlg,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QParamsDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QParamsDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QParamsDlg.stringdata0))
        return static_cast<void*>(const_cast< QParamsDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QParamsDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
