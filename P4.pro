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

include(P4.pri)

TEMPLATE = subdirs
SUBDIRS = src-gui src-mpl

INSTALL_DIR = $$PROJECT_ROOT/p4

bins.path = $$INSTALL_DIR/bin
bins.files = \
            $$PROJECT_ROOT/build/p4/p4 \
            $$PROJECT_ROOT/build/lyapunov/lyapunov \
            $$PROJECT_ROOT/build/lyapunov_mpf/lyapunov_mpf \
            $$PROJECT_ROOT/build/separatrice/separatrice \
            $$PROJECT_ROOT/src-mpl/p4.m \
            $$PROJECT_ROOT/src-mpl/p4gcf.m \
            $$PROJECT_ROOT/help/p4smallicon.png \
            $$PROJECT_ROOT/help/portrait.png
bins.commands = \
            (cp $$BUILD_DIR/p4/p4 $$BUILD_DIR/lyapunov/lyapunov $$BUILD_DIR/lyapunov_mpf/lyapunov_mpf $$BUILD_DIR/separatrice/separatrice $$PROJECT_ROOT/src-mpl/p4.m $$PROJECT_ROOT/src-mpl/p4gcf.m $$INSTALL_DIR/bin)

helps.path = $$INSTALL_DIR/help
helps.files = \
            $$PROJECT_ROOT/help/*

sumtables.path = $$INSTALL_DIR
sumtables.extra = \
            (mkdir $$INSTALL_DIR/sumtables); \
            (chmod -R 777 $$INSTALL_DIR/sumtables); \

INSTALLS += bins helps sumtables
