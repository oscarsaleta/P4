/****************************************************************************
** Meta object code from reading C++ file 'win_settings.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_settings.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_settings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QSettingsDlg_t {
    QByteArrayData data[10];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QSettingsDlg_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QSettingsDlg_t qt_meta_stringdata_QSettingsDlg = {
    {
QT_MOC_LITERAL(0, 0, 12), // "QSettingsDlg"
QT_MOC_LITERAL(1, 13, 4), // "OnOk"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 8), // "OnCancel"
QT_MOC_LITERAL(4, 28, 7), // "OnReset"
QT_MOC_LITERAL(5, 36, 12), // "OnBrowseBase"
QT_MOC_LITERAL(6, 49, 11), // "OnBrowseSum"
QT_MOC_LITERAL(7, 61, 12), // "OnBrowseTemp"
QT_MOC_LITERAL(8, 74, 13), // "OnBrowseMaple"
QT_MOC_LITERAL(9, 88, 14) // "OnBrowseReduce"

    },
    "QSettingsDlg\0OnOk\0\0OnCancel\0OnReset\0"
    "OnBrowseBase\0OnBrowseSum\0OnBrowseTemp\0"
    "OnBrowseMaple\0OnBrowseReduce"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QSettingsDlg[] = {

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
       1,    0,   54,    2, 0x0a /* Public */,
       3,    0,   55,    2, 0x0a /* Public */,
       4,    0,   56,    2, 0x0a /* Public */,
       5,    0,   57,    2, 0x0a /* Public */,
       6,    0,   58,    2, 0x0a /* Public */,
       7,    0,   59,    2, 0x0a /* Public */,
       8,    0,   60,    2, 0x0a /* Public */,
       9,    0,   61,    2, 0x0a /* Public */,

 // slots: parameters
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

void QSettingsDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QSettingsDlg *_t = static_cast<QSettingsDlg *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnOk(); break;
        case 1: _t->OnCancel(); break;
        case 2: _t->OnReset(); break;
        case 3: _t->OnBrowseBase(); break;
        case 4: _t->OnBrowseSum(); break;
        case 5: _t->OnBrowseTemp(); break;
        case 6: _t->OnBrowseMaple(); break;
        case 7: _t->OnBrowseReduce(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject QSettingsDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QSettingsDlg.data,
      qt_meta_data_QSettingsDlg,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QSettingsDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QSettingsDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QSettingsDlg.stringdata0))
        return static_cast<void*>(const_cast< QSettingsDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int QSettingsDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
