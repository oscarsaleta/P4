#  This file is part of P4
# 
#  Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck,
#                           F. Dumortier, J. Llibre, O. Saleta
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

#
# LYAPUNOV_MPF PROJECT FILE.  Use qmake to build makefile
#

include(../../P4.pri)

DESTDIR = $$BUILD_DIR/lyapunov_mpf/

CONFIG += console c++11

SOURCES = lyapunov_mpf.cpp lypcoeff_mpf.cpp polynom_mpf.cpp \
          checktbl_mpf.cpp createtbl_mpf.cpp readvf_mpf.cpp
HEADERS = lyapunov_mpf.h ../version.h

unix:LIBS += -lgmp -lmpfr

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/mpir/dll/x64/release/ -lmpir
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/mpir/dll/x64/debug/ -lmpir

INCLUDEPATH += $$PWD/mpir/dll/x64/Release
DEPENDPATH += $$PWD/mpir/dll/x64/Release

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/mpfr/dll/x64/release/ -lmpfr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/mpfr/dll/x64/debug/ -lmpfr

INCLUDEPATH += $$PWD/mpfr/dll/x64/Release
DEPENDPATH += $$PWD/mpfr/dll/x64/Release
