/****************************************************************************
** Meta object code from reading C++ file 'win_sphere.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_sphere.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_sphere.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QWinSphere_t {
    QByteArrayData data[21];
    char stringdata0[250];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QWinSphere_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QWinSphere_t qt_meta_stringdata_QWinSphere = {
    {
QT_MOC_LITERAL(0, 0, 10), // "QWinSphere"
QT_MOC_LITERAL(1, 11, 11), // "resizeEvent"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 13), // "QResizeEvent*"
QT_MOC_LITERAL(4, 38, 1), // "e"
QT_MOC_LITERAL(5, 40, 14), // "mouseMoveEvent"
QT_MOC_LITERAL(6, 55, 12), // "QMouseEvent*"
QT_MOC_LITERAL(7, 68, 15), // "mousePressEvent"
QT_MOC_LITERAL(8, 84, 17), // "mouseReleaseEvent"
QT_MOC_LITERAL(9, 102, 9), // "SetupPlot"
QT_MOC_LITERAL(10, 112, 7), // "refresh"
QT_MOC_LITERAL(11, 120, 13), // "keyPressEvent"
QT_MOC_LITERAL(12, 134, 10), // "QKeyEvent*"
QT_MOC_LITERAL(13, 145, 24), // "CalculateHeightFromWidth"
QT_MOC_LITERAL(14, 170, 4), // "int*"
QT_MOC_LITERAL(15, 175, 5), // "width"
QT_MOC_LITERAL(16, 181, 6), // "height"
QT_MOC_LITERAL(17, 188, 9), // "maxheight"
QT_MOC_LITERAL(18, 198, 11), // "aspectratio"
QT_MOC_LITERAL(19, 210, 18), // "refreshAfterResize"
QT_MOC_LITERAL(20, 229, 20) // "updatePointSelection"

    },
    "QWinSphere\0resizeEvent\0\0QResizeEvent*\0"
    "e\0mouseMoveEvent\0QMouseEvent*\0"
    "mousePressEvent\0mouseReleaseEvent\0"
    "SetupPlot\0refresh\0keyPressEvent\0"
    "QKeyEvent*\0CalculateHeightFromWidth\0"
    "int*\0width\0height\0maxheight\0aspectratio\0"
    "refreshAfterResize\0updatePointSelection"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QWinSphere[] = {

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
       1,    1,   64,    2, 0x0a /* Public */,
       5,    1,   67,    2, 0x0a /* Public */,
       7,    1,   70,    2, 0x0a /* Public */,
       8,    1,   73,    2, 0x0a /* Public */,
       9,    0,   76,    2, 0x0a /* Public */,
      10,    0,   77,    2, 0x0a /* Public */,
      11,    1,   78,    2, 0x0a /* Public */,
      13,    4,   81,    2, 0x0a /* Public */,
      19,    0,   90,    2, 0x0a /* Public */,
      20,    0,   91,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    4,
    QMetaType::Void, 0x80000000 | 6,    4,
    QMetaType::Void, 0x80000000 | 6,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 12,    4,
    QMetaType::Void, 0x80000000 | 14, 0x80000000 | 14, QMetaType::Int, QMetaType::Double,   15,   16,   17,   18,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void QWinSphere::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QWinSphere *_t = static_cast<QWinSphere *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resizeEvent((*reinterpret_cast< QResizeEvent*(*)>(_a[1]))); break;
        case 1: _t->mouseMoveEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 2: _t->mousePressEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 3: _t->mouseReleaseEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 4: _t->SetupPlot(); break;
        case 5: _t->refresh(); break;
        case 6: _t->keyPressEvent((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 7: _t->CalculateHeightFromWidth((*reinterpret_cast< int*(*)>(_a[1])),(*reinterpret_cast< int*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 8: _t->refreshAfterResize(); break;
        case 9: _t->updatePointSelection(); break;
        default: ;
        }
    }
}

const QMetaObject QWinSphere::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QWinSphere.data,
      qt_meta_data_QWinSphere,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *QWinSphere::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QWinSphere::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QWinSphere.stringdata0))
        return static_cast<void*>(const_cast< QWinSphere*>(this));
    return QWidget::qt_metacast(_clname);
}

int QWinSphere::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
