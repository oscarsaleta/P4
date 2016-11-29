/****************************************************************************
** Meta object code from reading C++ file 'file_vf.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "file_vf.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'file_vf.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QInputVF_t {
    QByteArrayData data[9];
    char stringdata0[138];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QInputVF_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QInputVF_t qt_meta_stringdata_QInputVF = {
    {
QT_MOC_LITERAL(0, 0, 8), // "QInputVF"
QT_MOC_LITERAL(1, 9, 16), // "finishEvaluation"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 17), // "catchProcessError"
QT_MOC_LITERAL(4, 45, 22), // "QProcess::ProcessError"
QT_MOC_LITERAL(5, 68, 17), // "ReadProcessStdout"
QT_MOC_LITERAL(6, 86, 17), // "onTerminateButton"
QT_MOC_LITERAL(7, 104, 13), // "onClearButton"
QT_MOC_LITERAL(8, 118, 19) // "finishGcfEvaluation"

    },
    "QInputVF\0finishEvaluation\0\0catchProcessError\0"
    "QProcess::ProcessError\0ReadProcessStdout\0"
    "onTerminateButton\0onClearButton\0"
    "finishGcfEvaluation"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QInputVF[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x0a /* Public */,
       3,    1,   47,    2, 0x0a /* Public */,
       5,    0,   50,    2, 0x0a /* Public */,
       6,    0,   51,    2, 0x0a /* Public */,
       7,    0,   52,    2, 0x0a /* Public */,
       8,    0,   53,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 4,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void QInputVF::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QInputVF *_t = static_cast<QInputVF *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->finishEvaluation((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->catchProcessError((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        case 2: _t->ReadProcessStdout(); break;
        case 3: _t->onTerminateButton(); break;
        case 4: _t->onClearButton(); break;
        case 5: _t->finishGcfEvaluation(); break;
        default: ;
        }
    }
}

const QMetaObject QInputVF::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QInputVF.data,
      qt_meta_data_QInputVF,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QInputVF::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QInputVF::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QInputVF.stringdata0))
        return static_cast<void*>(const_cast< QInputVF*>(this));
    return QObject::qt_metacast(_clname);
}

int QInputVF::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
