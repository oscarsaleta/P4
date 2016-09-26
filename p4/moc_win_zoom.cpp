/****************************************************************************
** Meta object code from reading C++ file 'win_zoom.h'
**
** Created: Fri Oct 23 09:30:52 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_zoom.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_zoom.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QZoomWnd[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x0a,
      30,    9,    9,    9, 0x0a,
      49,    9,    9,    9, 0x0a,
      66,    9,    9,    9, 0x0a,
      79,    9,    9,    9, 0x0a,
      94,    9,    9,    9, 0x0a,
     112,    9,  107,    9, 0x0a,
     120,    9,    9,    9, 0x0a,
     134,  132,    9,    9, 0x0a,
     157,  155,    9,    9, 0x0a,
     180,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QZoomWnd[] = {
    "QZoomWnd\0\0Signal_Evaluating()\0"
    "Signal_Evaluated()\0Signal_Changed()\0"
    "OnBtnClose()\0OnBtnRefresh()\0OnBtnPrint()\0"
    "bool\0close()\0configure()\0e\0"
    "customEvent(QEvent*)\0h\0hideEvent(QHideEvent*)\0"
    "AdjustHeight()\0"
};

const QMetaObject QZoomWnd::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QZoomWnd,
      qt_meta_data_QZoomWnd, 0 }
};

const QMetaObject *QZoomWnd::metaObject() const
{
    return &staticMetaObject;
}

void *QZoomWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QZoomWnd))
	return static_cast<void*>(const_cast< QZoomWnd*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QZoomWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: Signal_Evaluating(); break;
        case 1: Signal_Evaluated(); break;
        case 2: Signal_Changed(); break;
        case 3: OnBtnClose(); break;
        case 4: OnBtnRefresh(); break;
        case 5: OnBtnPrint(); break;
        case 6: { bool _r = close();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: configure(); break;
        case 8: customEvent((*reinterpret_cast< QEvent*(*)>(_a[1]))); break;
        case 9: hideEvent((*reinterpret_cast< QHideEvent*(*)>(_a[1]))); break;
        case 10: AdjustHeight(); break;
        }
        _id -= 11;
    }
    return _id;
}
