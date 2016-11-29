/****************************************************************************
** Meta object code from reading C++ file 'win_intparams.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_intparams.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_intparams.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QIntParamsDlg_t {
    QByteArrayData data[12];
    char stringdata0[139];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QIntParamsDlg_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QIntParamsDlg_t qt_meta_stringdata_QIntParamsDlg = {
    {
QT_MOC_LITERAL(0, 0, 13), // "QIntParamsDlg"
QT_MOC_LITERAL(1, 14, 7), // "MarkBad"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 10), // "QLineEdit*"
QT_MOC_LITERAL(4, 34, 3), // "edt"
QT_MOC_LITERAL(5, 38, 13), // "OnFieldChange"
QT_MOC_LITERAL(6, 52, 5), // "dummy"
QT_MOC_LITERAL(7, 58, 15), // "btn_org_toggled"
QT_MOC_LITERAL(8, 74, 12), // "on_btn_reset"
QT_MOC_LITERAL(9, 87, 15), // "btn_red_toggled"
QT_MOC_LITERAL(10, 103, 16), // "btn_dots_toggled"
QT_MOC_LITERAL(11, 120, 18) // "btn_dashes_toggled"

    },
    "QIntParamsDlg\0MarkBad\0\0QLineEdit*\0edt\0"
    "OnFieldChange\0dummy\0btn_org_toggled\0"
    "on_btn_reset\0btn_red_toggled\0"
    "btn_dots_toggled\0btn_dashes_toggled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QIntParamsDlg[] = {

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
       5,    1,   57,    2, 0x0a /* Public */,
       5,    1,   60,    2, 0x0a /* Public */,
       7,    1,   63,    2, 0x0a /* Public */,
       8,    0,   66,    2, 0x0a /* Public */,
       9,    1,   67,    2, 0x0a /* Public */,
      10,    1,   70,    2, 0x0a /* Public */,
      11,    1,   73,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void QIntParamsDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QIntParamsDlg *_t = static_cast<QIntParamsDlg *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->MarkBad((*reinterpret_cast< QLineEdit*(*)>(_a[1]))); break;
        case 1: _t->OnFieldChange((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->OnFieldChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->btn_org_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->on_btn_reset(); break;
        case 5: _t->btn_red_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->btn_dots_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->btn_dashes_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject QIntParamsDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QIntParamsDlg.data,
      qt_meta_data_QIntParamsDlg,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QIntParamsDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QIntParamsDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QIntParamsDlg.stringdata0))
        return static_cast<void*>(const_cast< QIntParamsDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QIntParamsDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
