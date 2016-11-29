/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4-src-gui
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/****************************************************************************
** Meta object code from reading C++ file 'win_orbits.h'
**
** Created: Fri Oct 23 09:30:48 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_orbits.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_orbits.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QOrbitsDlg[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      28,   11,   11,   11, 0x0a,
      43,   11,   11,   11, 0x0a,
      61,   11,   11,   11, 0x0a,
      78,   11,   11,   11, 0x0a,
      95,   11,   11,   11, 0x0a,
     110,   11,   11,   11, 0x0a,
     128,  126,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QOrbitsDlg[] = {
    "QOrbitsDlg\0\0OrbitEvent(int)\0onbtn_select()\0"
    "onbtn_backwards()\0onbtn_continue()\0"
    "onbtn_forwards()\0onbtn_delall()\0"
    "onbtn_dellast()\0,\0setInitialPoint(double,double)\0"
};

const QMetaObject QOrbitsDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QOrbitsDlg,
      qt_meta_data_QOrbitsDlg, 0 }
};

const QMetaObject *QOrbitsDlg::metaObject() const
{
    return &staticMetaObject;
}

void *QOrbitsDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QOrbitsDlg))
	return static_cast<void*>(const_cast< QOrbitsDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QOrbitsDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OrbitEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: onbtn_select(); break;
        case 2: onbtn_backwards(); break;
        case 3: onbtn_continue(); break;
        case 4: onbtn_forwards(); break;
        case 5: onbtn_delall(); break;
        case 6: onbtn_dellast(); break;
        case 7: setInitialPoint((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        }
        _id -= 8;
    }
    return _id;
}
