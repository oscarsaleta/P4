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

/*  P4 (Polynomial Planar Phase Portraits) GUI SOURCE CODE
 *  Software to study polynomial planar differential systems and represent
 *  their phase portrait in several spaces, such as Poincaré sphere. 
 *  URL: http://github.com/oscarsaleta/P4-src-gui
 *
 *  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
 *                           F. Dumortier, J. Llibre, O. Saleta
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/****************************************************************************
** Meta object code from reading C++ file 'win_find.h'
**
** Created: Fri Oct 23 09:30:50 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_find.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_find.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QFindDlg[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x0a,
      34,    9,    9,    9, 0x0a,
      59,    9,    9,    9, 0x0a,
      87,    9,    9,    9, 0x0a,
     116,    9,    9,    9, 0x0a,
     138,    9,    9,    9, 0x0a,
     160,    9,    9,    9, 0x0a,
     182,    9,    9,    9, 0x0a,
     204,    9,    9,    9, 0x0a,
     226,    9,    9,    9, 0x0a,
     247,    9,    9,    9, 0x0a,
     263,    9,    9,    9, 0x0a,
     283,    9,    9,    9, 0x0a,
     302,    9,    9,    9, 0x0a,
     316,    9,    9,    9, 0x0a,
     326,    9,    9,    9, 0x0a,
     338,    9,    9,    9, 0x0a,
     350,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QFindDlg[] = {
    "QFindDlg\0\0btn_maple_toggled(bool)\0"
    "btn_reduce_toggled(bool)\0"
    "btn_actionrun_toggled(bool)\0"
    "btn_actionprep_toggled(bool)\0"
    "btn_all_toggled(bool)\0btn_fin_toggled(bool)\0"
    "btn_inf_toggled(bool)\0btn_one_toggled(bool)\0"
    "btn_yes_toggled(bool)\0btn_no_toggled(bool)\0"
    "UpdateDlgData()\0Signal_Evaluating()\0"
    "Signal_Evaluated()\0OnBtnParams()\0"
    "OnBtnVf()\0OnBtnLoad()\0OnBtnSave()\0"
    "OnBtnEval()\0"
};

const QMetaObject QFindDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QFindDlg,
      qt_meta_data_QFindDlg, 0 }
};

const QMetaObject *QFindDlg::metaObject() const
{
    return &staticMetaObject;
}

void *QFindDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFindDlg))
	return static_cast<void*>(const_cast< QFindDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QFindDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: btn_maple_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: btn_reduce_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: btn_actionrun_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: btn_actionprep_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: btn_all_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: btn_fin_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: btn_inf_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: btn_one_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: btn_yes_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: btn_no_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: UpdateDlgData(); break;
        case 11: Signal_Evaluating(); break;
        case 12: Signal_Evaluated(); break;
        case 13: OnBtnParams(); break;
        case 14: OnBtnVf(); break;
        case 15: OnBtnLoad(); break;
        case 16: OnBtnSave(); break;
        case 17: OnBtnEval(); break;
        }
        _id -= 18;
    }
    return _id;
}
