/****************************************************************************
** Meta object code from reading C++ file 'win_gcf.h'
**
** Created: Fri Oct 23 09:30:50 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_gcf.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_gcf.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QGcfDlg[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x0a,
      26,    8,    8,    8, 0x0a,
      49,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QGcfDlg[] = {
    "QGcfDlg\0\0onbtn_evaluate()\0"
    "btn_dots_toggled(bool)\0btn_dashes_toggled(bool)\0"
};

const QMetaObject QGcfDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QGcfDlg,
      qt_meta_data_QGcfDlg, 0 }
};

const QMetaObject *QGcfDlg::metaObject() const
{
    return &staticMetaObject;
}

void *QGcfDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QGcfDlg))
	return static_cast<void*>(const_cast< QGcfDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QGcfDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onbtn_evaluate(); break;
        case 1: btn_dots_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: btn_dashes_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 3;
    }
    return _id;
}
