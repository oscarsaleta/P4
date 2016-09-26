/****************************************************************************
** Meta object code from reading C++ file 'win_sphere.h'
**
** Created: Fri Oct 23 09:30:45 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_sphere.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_sphere.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QWinSphere[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      14,   12,   11,   11, 0x0a,
      41,   12,   11,   11, 0x0a,
      70,   12,   11,   11, 0x0a,
     100,   12,   11,   11, 0x0a,
     132,   11,   11,   11, 0x0a,
     144,   11,   11,   11, 0x0a,
     154,   12,   11,   11, 0x0a,
     215,  180,   11,   11, 0x0a,
     262,   11,   11,   11, 0x0a,
     283,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QWinSphere[] = {
    "QWinSphere\0\0e\0resizeEvent(QResizeEvent*)\0"
    "mouseMoveEvent(QMouseEvent*)\0"
    "mousePressEvent(QMouseEvent*)\0"
    "mouseReleaseEvent(QMouseEvent*)\0"
    "SetupPlot()\0refresh()\0keyPressEvent(QKeyEvent*)\0"
    "width,height,maxheight,aspectratio\0"
    "CalculateHeightFromWidth(int*,int*,int,double)\0"
    "refreshAfterResize()\0updatePointSelection()\0"
};

const QMetaObject QWinSphere::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QWinSphere,
      qt_meta_data_QWinSphere, 0 }
};

const QMetaObject *QWinSphere::metaObject() const
{
    return &staticMetaObject;
}

void *QWinSphere::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QWinSphere))
	return static_cast<void*>(const_cast< QWinSphere*>(this));
    return QWidget::qt_metacast(_clname);
}

int QWinSphere::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: resizeEvent((*reinterpret_cast< QResizeEvent*(*)>(_a[1]))); break;
        case 1: mouseMoveEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 2: mousePressEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 3: mouseReleaseEvent((*reinterpret_cast< QMouseEvent*(*)>(_a[1]))); break;
        case 4: SetupPlot(); break;
        case 5: refresh(); break;
        case 6: keyPressEvent((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        case 7: CalculateHeightFromWidth((*reinterpret_cast< int*(*)>(_a[1])),(*reinterpret_cast< int*(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 8: refreshAfterResize(); break;
        case 9: updatePointSelection(); break;
        }
        _id -= 10;
    }
    return _id;
}
