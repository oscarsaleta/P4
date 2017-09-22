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

#
# SEPARATRICE PROJECT FILE.  Use qmake to build makefile
#

include(../../P4.pri)
DESTDIR = $$BUILD_DIR/separatrice/

CONFIG  += console c++11
macx {
    CONFIG -= app_bundle
    QMAKE_LFLAGS += -L/usr/local/opt/qt/lib
    QMAKE_CXXFLAGS += -I/usr/local/opt/qt/include
}
HEADERS = ../version.h
SOURCES = separatrice.cpp
