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
** Meta object code from reading C++ file 'win_view.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "win_view.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'win_view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QViewDlg_t {
    QByteArrayData data[14];
    char stringdata[169];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_QViewDlg_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_QViewDlg_t qt_meta_stringdata_QViewDlg = {
    {
QT_MOC_LITERAL(0, 0, 8), // "QViewDlg"
QT_MOC_LITERAL(1, 9, 7), // "MarkBad"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 10), // "QLineEdit*"
QT_MOC_LITERAL(4, 29, 3), // "edt"
QT_MOC_LITERAL(5, 33, 13), // "OnFieldChange"
QT_MOC_LITERAL(6, 47, 5), // "dummy"
QT_MOC_LITERAL(7, 53, 18), // "btn_sphere_toggled"
QT_MOC_LITERAL(8, 72, 17), // "btn_plane_toggled"
QT_MOC_LITERAL(9, 90, 18), // "btn_square_clicked"
QT_MOC_LITERAL(10, 109, 14), // "btn_U1_toggled"
QT_MOC_LITERAL(11, 124, 14), // "btn_U2_toggled"
QT_MOC_LITERAL(12, 139, 14), // "btn_V1_toggled"
QT_MOC_LITERAL(13, 154, 14) // "btn_V2_toggled"

    },
    "QViewDlg\0MarkBad\0\0QLineEdit*\0edt\0"
    "OnFieldChange\0dummy\0btn_sphere_toggled\0"
    "btn_plane_toggled\0btn_square_clicked\0"
    "btn_U1_toggled\0btn_U2_toggled\0"
    "btn_V1_toggled\0btn_V2_toggled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QViewDlg[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   59,    2, 0x0a /* Public */,
       5,    1,   62,    2, 0x0a /* Public */,
       7,    1,   65,    2, 0x0a /* Public */,
       8,    1,   68,    2, 0x0a /* Public */,
       9,    0,   71,    2, 0x0a /* Public */,
      10,    1,   72,    2, 0x0a /* Public */,
      11,    1,   75,    2, 0x0a /* Public */,
      12,    1,   78,    2, 0x0a /* Public */,
      13,    1,   81,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,
    QMetaType::Void, QMetaType::Bool,    2,

       0        // eod
};

void QViewDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QViewDlg *_t = static_cast<QViewDlg *>(_o);
        switch (_id) {
        case 0: _t->MarkBad((*reinterpret_cast< QLineEdit*(*)>(_a[1]))); break;
        case 1: _t->OnFieldChange((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->btn_sphere_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->btn_plane_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->btn_square_clicked(); break;
        case 5: _t->btn_U1_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->btn_U2_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->btn_V1_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->btn_V2_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject QViewDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QViewDlg.data,
      qt_meta_data_QViewDlg,  qt_static_metacall, Q_nullptrPTR, Q_nullptrPTR}
};


const QMetaObject *QViewDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QViewDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_nullptrPTR;
    if (!strcmp(_clname, qt_meta_stringdata_QViewDlg.stringdata))
        return static_cast<void*>(const_cast< QViewDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int QViewDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
