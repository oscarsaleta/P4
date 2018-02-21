#  This file is part of P4
# 
#  Copyright (C) 1996-2018  J.C. Artés, P. De Maesschalck, F. Dumortier,
#                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
# 
#  P4 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
# 
#  You should have received a copy of the GNU Lesser General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

include(../../P4.pri)

QT += gui
QT += widgets
QT += printsupport
#CONFIG += debug

CONFIG += qt
CONFIG += static
CONFIG += c++11
CONFIG += fwhopr
CONFIG -= console

macx {
    CONFIG -= app_bundle
    QMAKE_LFLAGS += -L/usr/local/opt/qt/lib
    QMAKE_CXXFLAGS += -I/usr/local/opt/qt/include -I/usr/local/include
}

DESTDIR = $$BUILD_DIR/p4/

SOURCES += win_view.cpp \
    win_vf.cpp \
    win_sphere.cpp \
    win_settings.cpp \
    win_print.cpp \
    win_plot.cpp \
    win_params.cpp \
    win_orbits.cpp \
    win_main.cpp \
    win_limitcycles.cpp \
    win_legend.cpp \
    win_isoclines.cpp \
    win_intparams.cpp \
    win_gcf.cpp \
    win_find.cpp \
    P4ArbitraryCurveDlg.cpp \
    win_about.cpp \
    print_xfig.cpp \
    print_postscript.cpp \
    print_points.cpp \
    print_bitmap.cpp \
    plot_tools.cpp \
    plot_points.cpp \
    p4settings.cpp \
    math_separatrice.cpp \
    math_polynom.cpp \
    math_p4.cpp \
    math_orbits.cpp \
    math_numerics.cpp \
    math_limitcycles.cpp \
    math_isoclines.cpp \
    math_intblowup.cpp \
    math_gcf.cpp \
    math_findpoint.cpp \
    math_curve.cpp \
    math_charts.cpp \
    math_changedir.cpp \
    main.cpp \
    file_vf.cpp \
    file_tab.cpp \
    file_paths.cpp \
    win_zoom.cpp \
    p4application.cpp \
    win_event.cpp \
    win_separatrice.cpp \

HEADERS += win_view.h \
    P4VectorFieldDlg.h \
    P4WinSphere.h \
    win_settings.h \
    P4PrintDlg.h \
    P4PlotWnd.h \
    P4ParamsDlg.h \
    P4OrbitsDlg.h \
    P4StartDlg.h \
    P4LimitCyclesDlg.h \
    P4LegendWnd.h \
    win_isoclines.h \
    P4IntParamsDlg.h \
    P4GcfDlg.h \
    P4FindDlg.h \
    P4Event.h \
    P4ArbitraryCurveDlg.h \
    win_about.h \
    p4application.h \
    math_p4.h \
    P4InputVF.h \
    custom.h \
    color.h \
    win_zoom.h \
    ../version.h \
    file_paths.h \
    p4settings.h \
    main.h \
    file_tab.h \
    math_changedir.h \
    math_charts.h \
    math_separatrice.h \
    P4SepDlg.h \
    math_findpoint.h \
    math_gcf.h \
    math_intblowup.h \
    math_arbitrarycurve.h \
    math_polynom.h \
    math_orbits.h \
    math_numerics.h \
    math_limitcycles.h \
    math_isoclines.h \
    plot_tools.h \
    plot_points.h \
    print_points.h \
    print_bitmap.h \
    print_postscript.h \
    print_xfig.h
RC_FILE = p4.rc
