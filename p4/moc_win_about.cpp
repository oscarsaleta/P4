/****************************************************************************
** Meta object code from reading C++ file 'win_about.h'
**
** Created: Fri Oct 23 09:30:51 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_about.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_about.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QP4AboutDlg[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x0a,
      20,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QP4AboutDlg[] = {
    "QP4AboutDlg\0\0OnOk()\0OnSettings()\0"
};

const QMetaObject QP4AboutDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QP4AboutDlg,
      qt_meta_data_QP4AboutDlg, 0 }
};

const QMetaObject *QP4AboutDlg::metaObject() const
{
    return &staticMetaObject;
}

void *QP4AboutDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QP4AboutDlg))
	return static_cast<void*>(const_cast< QP4AboutDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int QP4AboutDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OnOk(); break;
        case 1: OnSettings(); break;
        }
        _id -= 2;
    }
    return _id;
}
