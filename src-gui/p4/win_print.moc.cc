/****************************************************************************
** Meta object code from reading C++ file 'win_print.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_print.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_print.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QPrintDlg_t {
    QByteArrayData data[20];
    char stringdata0[244];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QPrintDlg_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QPrintDlg_t qt_meta_stringdata_QPrintDlg = {
    {
QT_MOC_LITERAL(0, 0, 9), // "QPrintDlg"
QT_MOC_LITERAL(1, 10, 16), // "OnDefaultPrinter"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 17), // "OnEpsImagePrinter"
QT_MOC_LITERAL(4, 46, 18), // "OnXfigImagePrinter"
QT_MOC_LITERAL(5, 65, 18), // "OnJpegImagePrinter"
QT_MOC_LITERAL(6, 84, 8), // "OnCancel"
QT_MOC_LITERAL(7, 93, 10), // "ReadDialog"
QT_MOC_LITERAL(8, 104, 14), // "ReadFloatField"
QT_MOC_LITERAL(9, 119, 10), // "QLineEdit*"
QT_MOC_LITERAL(10, 130, 3), // "edt"
QT_MOC_LITERAL(11, 134, 7), // "double*"
QT_MOC_LITERAL(12, 142, 7), // "presult"
QT_MOC_LITERAL(13, 150, 8), // "defvalue"
QT_MOC_LITERAL(14, 159, 8), // "minvalue"
QT_MOC_LITERAL(15, 168, 8), // "maxvalue"
QT_MOC_LITERAL(16, 177, 7), // "MarkBad"
QT_MOC_LITERAL(17, 185, 19), // "GetChosenResolution"
QT_MOC_LITERAL(18, 205, 18), // "GetChosenLineWidth"
QT_MOC_LITERAL(19, 224, 19) // "GetChosenSymbolSize"

    },
    "QPrintDlg\0OnDefaultPrinter\0\0"
    "OnEpsImagePrinter\0OnXfigImagePrinter\0"
    "OnJpegImagePrinter\0OnCancel\0ReadDialog\0"
    "ReadFloatField\0QLineEdit*\0edt\0double*\0"
    "presult\0defvalue\0minvalue\0maxvalue\0"
    "MarkBad\0GetChosenResolution\0"
    "GetChosenLineWidth\0GetChosenSymbolSize"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QPrintDlg[] = {

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
       8,    5,   75,    2, 0x0a /* Public */,
      16,    1,   86,    2, 0x0a /* Public */,
      17,    0,   89,    2, 0x0a /* Public */,
      18,    0,   90,    2, 0x0a /* Public */,
      19,    0,   91,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Bool, 0x80000000 | 9, 0x80000000 | 11, QMetaType::Double, QMetaType::Double, QMetaType::Double,   10,   12,   13,   14,   15,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Int,
    QMetaType::Double,
    QMetaType::Double,

       0        // eod
};

void QPrintDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QPrintDlg *_t = static_cast<QPrintDlg *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnDefaultPrinter(); break;
        case 1: _t->OnEpsImagePrinter(); break;
        case 2: _t->OnXfigImagePrinter(); break;
        case 3: _t->OnJpegImagePrinter(); break;
        case 4: _t->OnCancel(); break;
        case 5: { bool _r = _t->ReadDialog();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: { bool _r = _t->ReadFloatField((*reinterpret_cast< QLineEdit*(*)>(_a[1])),(*reinterpret_cast< double*(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4])),(*reinterpret_cast< double(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: _t->MarkBad((*reinterpret_cast< QLineEdit*(*)>(_a[1]))); break;
        case 8: { int _r = _t->GetChosenResolution();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 9: { double _r = _t->GetChosenLineWidth();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 10: { double _r = _t->GetChosenSymbolSize();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject QPrintDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QPrintDlg.data,
      qt_meta_data_QPrintDlg,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QPrintDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QPrintDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QPrintDlg.stringdata0))
        return static_cast<void*>(const_cast< QPrintDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int QPrintDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
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
