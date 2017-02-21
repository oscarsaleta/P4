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

echo "== P4 uninstaller =="

# remove main folder
if [[ -d $HOME/p4 ]]; then
    echo "Removing "$HOME"/p4...";
    rm -rf $HOME/p4;
    echo "Done.";
fi

# remove desktop entry
if [[ -f $HOME/.local/share/applications/p4.desktop ]]; then
    echo "Removing $HOME/.local/share/applications/p4.desktop...";
    rm $HOME/.local/share/applications/p4.desktop;
    echo "Done.";
fi

# remove config file
if [[ -d $HOME/.config/P4 ]]; then
    echo "Removing $HOME/.config/P4...";
    rm -rf $HOME/.config/P4;
    echo "Done.";
fi


# end
echo "Uninstallation complete!"
