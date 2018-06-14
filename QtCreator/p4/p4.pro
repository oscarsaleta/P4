#  This file is part of P4
# 
#  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier,
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


QT += gui
QT += widgets
QT += printsupport

CONFIG += qt
#CONFIG += static
CONFIG += c++14
CONFIG += fwhopr

unix {
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
}

SOURCES += file_paths.cpp \
    main.cpp \
    math_arbitrarycurve.cpp \
    math_changedir.cpp \
    math_charts.cpp \
    math_desep.cpp \
    math_findpoint.cpp \
    math_gcf.cpp \
    math_isoclines.cpp \
    math_limitcycles.cpp \
    math_numerics.cpp \
    math_orbits.cpp \
    math_p4.cpp \
    math_polynom.cpp \
    math_regions.cpp \
    math_saddlesep.cpp \
    math_separatingcurves.cpp \
    math_separatrice.cpp \
    math_sesep.cpp \
    P4AboutDlg.cpp \
    P4Application.cpp \
    P4ArbitraryCurveDlg.cpp \
    P4Event.cpp \
    P4FindDlg.cpp \
    P4GcfDlg.cpp \
    P4InputVF.cpp \
    P4IntParamsDlg.cpp \
    P4IsoclinesDlg.cpp \
    P4LegendWnd.cpp \
    P4LimitCyclesDlg.cpp \
    P4OrbitsDlg.cpp \
    P4ParamsDlg.cpp \
    P4ParentStudy.cpp \
    P4PlotWnd.cpp \
    P4PrintDlg.cpp \
    P4SeparatingCurvesDlg.cpp \
    P4SepDlg.cpp \
    p4settings.cpp \
    P4SettingsDlg.cpp \
    P4StartDlg.cpp \
    P4VectorFieldDlg.cpp \
    P4VFParams.cpp \
    P4VFSelectDlg.cpp \
    P4VFStudy.cpp \
    P4ViewDlg.cpp \
    P4WinInputSphere.cpp \
    P4WinSphere.cpp \
    P4ZoomWnd.cpp \
    plot_points.cpp \
    plot_tools.cpp \
    print_bitmap.cpp \
    print_points.cpp \
    print_postscript.cpp \
    print_xfig.cpp \
    P4ProcessWnd.cpp

HEADERS += ../version.h \
    color.hpp \
    custom.hpp \
    file_paths.hpp \
    main.hpp \
    math_arbitrarycurve.hpp \
    math_changedir.hpp \
    math_charts.hpp \
    math_desep.hpp \
    math_findpoint.hpp \
    math_gcf.hpp \
    math_isoclines.hpp \
    math_limitcycles.hpp \
    math_numerics.hpp \
    math_orbits.hpp \
    math_p4.hpp \
    math_polynom.hpp \
    math_regions.hpp \
    math_saddlesep.hpp \
    math_separatingcurves.hpp \
    math_separatrice.hpp \
    math_sesep.hpp \
    P4AboutDlg.hpp \
    P4Application.hpp \
    P4ArbitraryCurveDlg.hpp \
    P4Event.hpp \
    P4FindDlg.hpp \
    P4GcfDlg.hpp \
    P4InputVF.hpp \
    P4IntParamsDlg.hpp \
    P4IsoclinesDlg.hpp \
    P4LegendWnd.hpp \
    P4LimitCyclesDlg.hpp \
    P4OrbitsDlg.hpp \
    P4ParamsDlg.hpp \
    P4ParentStudy.hpp \
    P4PlotWnd.hpp \
    P4PrintDlg.hpp \
    P4SeparatingCurvesDlg.hpp \
    P4SepDlg.hpp \
    P4SettingsDlg.hpp \
    p4settings.hpp \
    P4StartDlg.hpp \
    P4VectorFieldDlg.hpp \
    P4VFParams.hpp \
    P4VFSelectDlg.hpp \
    P4VFStudy.hpp \
    P4ViewDlg.hpp \
    P4WinInputSphere.hpp \
    P4WinSphere.hpp \
    P4ZoomWnd.hpp \
    plot_points.hpp \
    plot_tools.hpp \
    print_bitmap.hpp \
    print_points.hpp \
    print_postscript.hpp \
    print_xfig.hpp \
    tables.hpp \
    P4ProcessWnd.hpp

RC_FILE = p4.rc
