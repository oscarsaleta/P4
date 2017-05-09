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

#!/bin/bash

echo "== P4 installer =="

# check if curl is installed
command -v curl >/dev/null 2>&1 || { echo >&2 "This install script requires curl but it could not be found in your system. Please install curl and try again"; exit 1;}

echo "Downloading newest version from GitHub..."
# set progress bar ?
wget --help | grep -q '\--show-progress' && \
  _PROGRESS_OPT="-q --show-progress" || _PROGRESS_OPT=""
# set latest download link
DL_LINK=$(curl -s https://api.github.com/repos/oscarsaleta/P4/releases/latest | grep linux | awk '/"browser_download_url"/{print $NF}' | awk -F'"' '$0=$2')
# create folder in /tmp for downloading and extracting
TMPDIR=/tmp/p4install
mkdir -p $TMPDIR
# set file name for download
FNAME=$TMPDIR/p4.tar.gz
# download archive
wget -O $FNAME $_PROGRESS_OPT $DL_LINK

echo "Installing p4 in $HOME/p4..."
# extract downloaded archive
tar -xzf $FNAME -C $TMPDIR --strip-components=1
# move p4 to /opt
mv $TMPDIR/p4 $HOME
# create sum_tables and set permissions to write
mkdir -p $HOME/p4/sum_tables
chmod 777 $HOME/p4/sum_tables
# create symlink to sum_tables for compatibility reasons
ln -sf $HOME/p4/sum_tables $HOME/p4/sumtables

echo "Creating application entry..."
APPNAME=$HOME/.local/share/applications/p4.desktop
TMPAPPNAME=$TMPDIR/p4.desktop
VERSION=$(curl -s https://api.github.com/repos/oscarsaleta/P4/releases/latest | grep tag_name | awk '{gsub("\"tag_name\": \"v","\"");gsub("-.*,","");print}' | awk -F'"' '$0=$2')
echo "[Desktop Entry]" > $TMPAPPNAME
echo "Name=P4" >> $TMPAPPNAME
echo "Version="$VERSION >> $TMPAPPNAME
echo "Comment=Polynomial Planar Phase Portraits" >> $TMPAPPNAME
echo "Icon=$HOME/p4/bin/p4smallicon.png" >> $TMPAPPNAME
echo "Keywords=Math" >> $TMPAPPNAME
echo "Exec=$HOME/p4/bin/p4" >> $TMPAPPNAME
echo "Terminal=false" >> $TMPAPPNAME
echo "Type=Application" >> $TMPAPPNAME
echo "Categories=Education;Science;Math" >> $TMPAPPNAME
echo "StartupNotify=true" >> $TMPAPPNAME

mv $TMPAPPNAME $APPNAME
chmod 664 $APPNAME

# remove temporary files
rm -r $TMPDIR

echo "Done!"