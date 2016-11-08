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
** Meta object code from reading C++ file 'win_main.h'
**
** Created: Fri Oct 23 09:30:48 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_main.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_main.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QStartDlg[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x0a,
      20,   10,   10,   10, 0x0a,
      29,   10,   10,   10, 0x0a,
      44,   10,   10,   10, 0x0a,
      61,   10,   10,   10, 0x0a,
      70,   10,   10,   10, 0x0a,
      79,   10,   10,   10, 0x0a,
      90,   10,   10,   10, 0x0a,
     100,   10,   10,   10, 0x0a,
     160,  135,  126,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QStartDlg[] = {
    "QStartDlg\0\0OnQuit()\0OnPlot()\0"
    "OnViewFinite()\0OnViewInfinite()\0"
    "OnHelp()\0OnFind()\0OnBrowse()\0OnAbout()\0"
    "OnFilenameChange(QString)\0QWidget*\0"
    "win,parent,caption,fname\0"
    "Showtext(QWidget*,QWidget*,QString,QString)\0"
};

const QMetaObject QStartDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QStartDlg,
      qt_meta_data_QStartDlg, 0 }
};

const QMetaObject *QStartDlg::metaObject() const
{
    return &staticMetaObject;
}

void *QStartDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QStartDlg))
	return static_cast<void*>(const_cast< QStartDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QStartDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OnQuit(); break;
        case 1: OnPlot(); break;
        case 2: OnViewFinite(); break;
        case 3: OnViewInfinite(); break;
        case 4: OnHelp(); break;
        case 5: OnFind(); break;
        case 6: OnBrowse(); break;
        case 7: OnAbout(); break;
        case 8: OnFilenameChange((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: { QWidget* _r = Showtext((*reinterpret_cast< QWidget*(*)>(_a[1])),(*reinterpret_cast< QWidget*(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< QString(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< QWidget**>(_a[0]) = _r; }  break;
        }
        _id -= 10;
    }
    return _id;
}
