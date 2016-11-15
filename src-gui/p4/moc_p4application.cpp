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
** Meta object code from reading C++ file 'p4application.h'
**
** Created: Fri Oct 23 09:30:51 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "p4application.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'p4application.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QP4Application[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,
      36,   15,   15,   15, 0x0a,
      58,   15,   15,   15, 0x0a,
      83,   15,   15,   15, 0x0a,
     100,   15,   15,   15, 0x0a,
     116,   15,   15,   15, 0x0a,
     131,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QP4Application[] = {
    "QP4Application\0\0Signal_Evaluating()\0"
    "Signal_Evaluated(int)\0Signal_GcfEvaluated(int)\0"
    "Signal_Changed()\0Signal_Loaded()\0"
    "Signal_Saved()\0cathProcessError(QProcess::ProcessError)\0"
};

const QMetaObject QP4Application::staticMetaObject = {
    { &QApplication::staticMetaObject, qt_meta_stringdata_QP4Application,
      qt_meta_data_QP4Application, 0 }
};

const QMetaObject *QP4Application::metaObject() const
{
    return &staticMetaObject;
}

void *QP4Application::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QP4Application))
	return static_cast<void*>(const_cast< QP4Application*>(this));
    return QApplication::qt_metacast(_clname);
}

int QP4Application::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: Signal_Evaluating(); break;
        case 1: Signal_Evaluated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: Signal_GcfEvaluated((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: Signal_Changed(); break;
        case 4: Signal_Loaded(); break;
        case 5: Signal_Saved(); break;
        case 6: cathProcessError((*reinterpret_cast< QProcess::ProcessError(*)>(_a[1]))); break;
        }
        _id -= 7;
    }
    return _id;
}
