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
** Meta object code from reading C++ file 'win_print.h'
**
** Created: Fri Oct 23 09:30:46 2009
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_print.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_print.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QPrintDlg[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x0a,
      30,   10,   10,   10, 0x0a,
      50,   10,   10,   10, 0x0a,
      71,   10,   10,   10, 0x0a,
      92,   10,   10,   10, 0x0a,
     108,   10,  103,   10, 0x0a,
     160,  121,  103,   10, 0x0a,
     220,  216,   10,   10, 0x0a,
     244,   10,  240,   10, 0x0a,
     273,   10,  266,   10, 0x0a,
     294,   10,  266,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QPrintDlg[] = {
    "QPrintDlg\0\0OnDefaultPrinter()\0"
    "OnEpsImagePrinter()\0OnXfigImagePrinter()\0"
    "OnJpegImagePrinter()\0OnCancel()\0bool\0"
    "ReadDialog()\0edt,presult,defvalue,minvalue,maxvalue\0"
    "ReadFloatField(QLineEdit*,double*,double,double,double)\0"
    "edt\0MarkBad(QLineEdit*)\0int\0"
    "GetChosenResolution()\0double\0"
    "GetChosenLineWidth()\0GetChosenSymbolSize()\0"
};

const QMetaObject QPrintDlg::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QPrintDlg,
      qt_meta_data_QPrintDlg, 0 }
};

const QMetaObject *QPrintDlg::metaObject() const
{
    return &staticMetaObject;
}

void *QPrintDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QPrintDlg))
	return static_cast<void*>(const_cast< QPrintDlg*>(this));
    return QDialog::qt_metacast(_clname);
}

int QPrintDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: OnDefaultPrinter(); break;
        case 1: OnEpsImagePrinter(); break;
        case 2: OnXfigImagePrinter(); break;
        case 3: OnJpegImagePrinter(); break;
        case 4: OnCancel(); break;
        case 5: { bool _r = ReadDialog();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 6: { bool _r = ReadFloatField((*reinterpret_cast< QLineEdit*(*)>(_a[1])),(*reinterpret_cast< double*(*)>(_a[2])),(*reinterpret_cast< double(*)>(_a[3])),(*reinterpret_cast< double(*)>(_a[4])),(*reinterpret_cast< double(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 7: MarkBad((*reinterpret_cast< QLineEdit*(*)>(_a[1]))); break;
        case 8: { int _r = GetChosenResolution();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 9: { double _r = GetChosenLineWidth();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 10: { double _r = GetChosenSymbolSize();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        }
        _id -= 11;
    }
    return _id;
}
