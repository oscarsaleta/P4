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

#!/bin/bash

echo "== P4 uninstaller for root =="

INSTALLDIR=/usr/local/p4
# remove main folder
if [[ -d /opt/p4 ]]; then
    echo "Removing $INSTALLDIR...";
    sudo rm -r $INSTALLDIR;
    echo "Done.";
fi

BINDIR=/usr/local/bin
# remove symlink
if [[ -f $BINDIR/p4 ]]; then
    echo "Removing $BINDIR/p4...";
    sudo rm $BINDIR/p4;
    echo "Done."
fi

APPDIR=/usr/share/applications
# remove desktop entry
if [[ -f $APPDIR/p4.desktop ]]; then
    echo "Removing $APPDIR/p4.desktop...";
    sudo rm $APPDIR/p4.desktop;
    echo "Done."
fi

CONFDIR=$HOME/.config
# remove config file
if [[ -d $CONFDIR/P4 ]]; then
    echo "Removing $CONFDIR/P4...";
    rm -rf $CONFDIR/P4;
    echo "Done.";
fi

# end
echo "Uninstallation complete!"
