/****************************************************************************
** Meta object code from reading C++ file 'win_separatrices.h'
**
** Created: Fri Oct 23 09:30:46 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_separatrices.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_separatrices.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QSepDlg[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x0a,
      28,    8,    8,    8, 0x0a,
      44,    8,    8,    8, 0x0a,
      57,    8,    8,    8, 0x0a,
      71,    8,    8,    8, 0x0a,
      89,    8,    8,    8, 0x0a,
     103,    8,    8,    8, 0x0a,
     121,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QSepDlg[] = {
    "QSepDlg\0\0onbtn_selectnext()\0onbtn_intnext()\0"
    "onbtn_cont()\0onbtn_start()\0setInitialPoint()\0"
    "SepEvent(int)\0onepsilon_enter()\0"
    "MarkBad(QLineEdit*)\0"
};

const QMetaObject QSepDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QSepDlg,
      qt_meta_data_QSepDlg, 0 }
};

const QMetaObject *QSepDlg::metaObject() const
{
    return &staticMetaObject;
}

void *QSepDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSepDlg))
	return static_cast<void*>(const_cast< QSepDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QSepDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onbtn_selectnext(); break;
        case 1: onbtn_intnext(); break;
        case 2: onbtn_cont(); break;
        case 3: onbtn_start(); break;
        case 4: setInitialPoint(); break;
        case 5: SepEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: onepsilon_enter(); break;
        case 7: MarkBad((*reinterpret_cast< QLineEdit*(*)>(_a[1]))); break;
        }
        _id -= 8;
    }
    return _id;
}
