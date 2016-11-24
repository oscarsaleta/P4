#!/bin/bash

echo "== P4 installer for root =="


echo "Downloading newest version from GitHub..."
# set progress bar ?
wget --help | grep -q '\--show-progress' && \
  _PROGRESS_OPT="-q --show-progress" || _PROGRESS_OPT=""
# set latest download link
DL_LINK=$(curl -s https://api.github.com/repos/oscarsaleta/P4/releases/latest | grep linux | awk -e '/"browser_download_url"/{print $NF}' | awk -F'"' '$0=$2')
# create folder in /tmp for downloading and extracting
TMPDIR=/tmp/p4install
mkdir -p $TMPDIR
# set file name for download
FNAME=$TMPDIR/p4.tar.gz
# download archive
wget -O $FNAME $_PROGRESS_OPT $DL_LINK

echo "Installing p4 in /opt/..."
# extract downloaded archive
tar -xzf $FNAME -C $TMPDIR --strip-components=1
# move p4 to /opt
sudo mv $TMPDIR/p4 /opt/p4
# create sum_tables and set permissions to write
sudo mkdir -p /opt/p4/sum_tables
sudo chmod 777 /opt/p4/sum_tables
# create symlink to sum_tables for compatibility reasons
sudo ln -s /opt/p4/sum_tables /opt/p4/sumtables
# remove temporary files
rm -r $TMPDIR

echo "Installing symlink to p4 in /usr/bin/..."
# create symlink from p4/bin/p4 to /usr/bin/p4 to have p4 in PATH
sudo ln -s /opt/p4/bin/p4 /usr/bin/p4

echo "Done!"