/****************************************************************************
** Meta object code from reading C++ file 'win_params.h'
**
** Created: Fri Oct 23 09:30:47 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_params.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_params.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QParamsDlg[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      34,   11,   11,   11, 0x0a,
      56,   11,   11,   11, 0x0a,
      81,   11,   11,   11, 0x0a,
     105,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QParamsDlg[] = {
    "QParamsDlg\0\0btn_num_toggled(bool)\0"
    "btn_alg_toggled(bool)\0btn_sepyes_toggled(bool)\0"
    "btn_sepno_toggled(bool)\0OnLevelChange(int)\0"
};

const QMetaObject QParamsDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QParamsDlg,
      qt_meta_data_QParamsDlg, 0 }
};

const QMetaObject *QParamsDlg::metaObject() const
{
    return &staticMetaObject;
}

void *QParamsDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QParamsDlg))
	return static_cast<void*>(const_cast< QParamsDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QParamsDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: btn_num_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: btn_alg_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: btn_sepyes_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: btn_sepno_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: OnLevelChange((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 5;
    }
    return _id;
}
