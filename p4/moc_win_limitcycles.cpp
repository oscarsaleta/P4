/****************************************************************************
** Meta object code from reading C++ file 'win_limitcycles.h'
**
** Created: Fri Oct 23 09:30:48 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_limitcycles.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_limitcycles.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QLimitCyclesDlg[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,
      31,   16,   16,   16, 0x0a,
      46,   16,   16,   16, 0x0a,
      61,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QLimitCyclesDlg[] = {
    "QLimitCyclesDlg\0\0onbtn_start()\0"
    "onbtn_cancel()\0onbtn_delall()\0"
    "onbtn_dellast()\0"
};

const QMetaObject QLimitCyclesDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QLimitCyclesDlg,
      qt_meta_data_QLimitCyclesDlg, 0 }
};

const QMetaObject *QLimitCyclesDlg::metaObject() const
{
    return &staticMetaObject;
}

void *QLimitCyclesDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QLimitCyclesDlg))
	return static_cast<void*>(const_cast< QLimitCyclesDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QLimitCyclesDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onbtn_start(); break;
        case 1: onbtn_cancel(); break;
        case 2: onbtn_delall(); break;
        case 3: onbtn_dellast(); break;
        }
        _id -= 4;
    }
    return _id;
}
