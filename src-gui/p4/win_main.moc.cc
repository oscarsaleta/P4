/****************************************************************************
** Meta object code from reading C++ file 'win_main.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_main.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_main.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QStartDlg_t {
    QByteArrayData data[16];
    char stringdata0[137];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QStartDlg_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QStartDlg_t qt_meta_stringdata_QStartDlg = {
    {
QT_MOC_LITERAL(0, 0, 9), // "QStartDlg"
QT_MOC_LITERAL(1, 10, 6), // "OnQuit"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 6), // "OnPlot"
QT_MOC_LITERAL(4, 25, 12), // "OnViewFinite"
QT_MOC_LITERAL(5, 38, 14), // "OnViewInfinite"
QT_MOC_LITERAL(6, 53, 6), // "OnHelp"
QT_MOC_LITERAL(7, 60, 6), // "OnFind"
QT_MOC_LITERAL(8, 67, 8), // "OnBrowse"
QT_MOC_LITERAL(9, 76, 7), // "OnAbout"
QT_MOC_LITERAL(10, 84, 16), // "OnFilenameChange"
QT_MOC_LITERAL(11, 101, 8), // "Showtext"
QT_MOC_LITERAL(12, 110, 8), // "QWidget*"
QT_MOC_LITERAL(13, 119, 3), // "win"
QT_MOC_LITERAL(14, 123, 7), // "caption"
QT_MOC_LITERAL(15, 131, 5) // "fname"

    },
    "QStartDlg\0OnQuit\0\0OnPlot\0OnViewFinite\0"
    "OnViewInfinite\0OnHelp\0OnFind\0OnBrowse\0"
    "OnAbout\0OnFilenameChange\0Showtext\0"
    "QWidget*\0win\0caption\0fname"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QStartDlg[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x0a /* Public */,
       3,    0,   65,    2, 0x0a /* Public */,
       4,    0,   66,    2, 0x0a /* Public */,
       5,    0,   67,    2, 0x0a /* Public */,
       6,    0,   68,    2, 0x0a /* Public */,
       7,    0,   69,    2, 0x0a /* Public */,
       8,    0,   70,    2, 0x0a /* Public */,
       9,    0,   71,    2, 0x0a /* Public */,
      10,    1,   72,    2, 0x0a /* Public */,
      11,    3,   75,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    0x80000000 | 12, 0x80000000 | 12, QMetaType::QString, QMetaType::QString,   13,   14,   15,

       0        // eod
};

void QStartDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QStartDlg *_t = static_cast<QStartDlg *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->OnQuit(); break;
        case 1: _t->OnPlot(); break;
        case 2: _t->OnViewFinite(); break;
        case 3: _t->OnViewInfinite(); break;
        case 4: _t->OnHelp(); break;
        case 5: _t->OnFind(); break;
        case 6: _t->OnBrowse(); break;
        case 7: _t->OnAbout(); break;
        case 8: _t->OnFilenameChange((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: { QWidget* _r = _t->Showtext((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< QWidget**>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObject QStartDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QStartDlg.data,
      qt_meta_data_QStartDlg,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QStartDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QStartDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QStartDlg.stringdata0))
        return static_cast<void*>(const_cast< QStartDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QStartDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
