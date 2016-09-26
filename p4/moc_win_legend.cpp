/****************************************************************************
** Meta object code from reading C++ file 'win_legend.h'
**
** Created: Fri Oct 23 09:30:49 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_legend.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_legend.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QLegendWnd[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_QLegendWnd[] = {
    "QLegendWnd\0"
};

const QMetaObject QLegendWnd::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QLegendWnd,
      qt_meta_data_QLegendWnd, 0 }
};

const QMetaObject *QLegendWnd::metaObject() const
{
    return &staticMetaObject;
}

void *QLegendWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QLegendWnd))
	return static_cast<void*>(const_cast< QLegendWnd*>(this));
    return QWidget::qt_metacast(_clname);
}

int QLegendWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
