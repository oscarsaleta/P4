/****************************************************************************
** Meta object code from reading C++ file 'win_plot.h'
**
** Created: Fri Oct 23 09:30:47 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_plot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_plot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QPlotWnd[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x0a,
      30,    9,    9,    9, 0x0a,
      49,    9,    9,    9, 0x0a,
      66,    9,    9,    9, 0x0a,
      79,    9,    9,    9, 0x0a,
      94,    9,    9,    9, 0x0a,
     108,    9,    9,    9, 0x0a,
     122,    9,    9,    9, 0x0a,
     139,    9,    9,    9, 0x0a,
     151,    9,    9,    9, 0x0a,
     162,    9,    9,    9, 0x0a,
     177,    9,    9,    9, 0x0a,
     196,    9,    9,    9, 0x0a,
     215,    9,    9,    9, 0x0a,
     233,    9,  228,    9, 0x0a,
     245,  241,    9,    9, 0x0a,
     292,  289,    9,    9, 0x0a,
     313,    9,    9,    9, 0x0a,
     327,  325,    9,    9, 0x0a,
     350,  348,    9,    9, 0x0a,
     373,    9,    9,    9, 0x0a,
     386,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QPlotWnd[] = {
    "QPlotWnd\0\0Signal_Evaluating()\0"
    "Signal_Evaluated()\0Signal_Changed()\0"
    "OnBtnClose()\0OnBtnRefresh()\0OnBtnLegend()\0"
    "OnBtnOrbits()\0OnBtnIntParams()\0"
    "OnBtnView()\0OnBtnGCF()\0OnBtnPlotSep()\0"
    "OnBtnPlotAllSeps()\0OnBtnLimitCycles()\0"
    "OnBtnPrint()\0bool\0close()\0,,,\0"
    "OpenZoomWindow(double,double,double,double)\0"
    "id\0CloseZoomWindow(int)\0configure()\0"
    "e\0customEvent(QEvent*)\0h\0"
    "hideEvent(QHideEvent*)\0getDlgData()\0"
    "AdjustHeight()\0"
};

const QMetaObject QPlotWnd::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QPlotWnd,
      qt_meta_data_QPlotWnd, 0 }
};

const QMetaObject *QPlotWnd::metaObject() const
{
    return &staticMetaObject;
}

void *QPlotWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPlotWnd))
	return static_cast<void*>(const_cast< QPlotWnd*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QPlotWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        case 5: OnBtnLegend(); break;
        case 6: OnBtnOrbits(); break;
        case 7: OnBtnIntParams(); break;
        case 8: OnBtnView(); break;
        case 9: OnBtnGCF(); break;
        case 10: OnBtnPlotSep(); break;
        case 11: OnBtnPlotAllSeps(); break;
        case 12: OnBtnLimitCycles(); break;
        case 13: OnBtnPrint(); break;
        case 14: { bool _r = close();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 15: OpenZoomWindow((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4]))); break;
        case 16: CloseZoomWindow((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: configure(); break;
        case 18: customEvent((*reinterpret_cast< QEvent*(*)>(_a[1]))); break;
        case 19: hideEvent((*reinterpret_cast< QHideEvent*(*)>(_a[1]))); break;
        case 20: getDlgData(); break;
        case 21: AdjustHeight(); break;
        }
        _id -= 22;
    }
    return _id;
}
