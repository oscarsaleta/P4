#!/bin/bash

# get version from src-gui/version.h
VERSION="$(awk '/VERSION.*".*"/{print $NF}' ~/git/P4/src-gui/version.h | awk -F'"' '$0=$2')"

# set name of deb file
DEB_DIR=p4_$VERSION-1
$(rm -r $DEB_DIR)

# set name of p4 install directory
P4_DIR=$DEB_DIR/opt/p4

# create directories for program
$(mkdir -p $P4_DIR/bin $P4_DIR/help $P4_DIR/sum_tables)
$(chmod 777 $P4_DIR/sum_tables)
$(ln -s $P4_DIR/sum_tables $P4_DIR/sumtables)

# add maple scripts
$(cp ~/git/P4/src-mpl/p4*.m $P4_DIR/bin/)

# add executables
$(cp ~/git/P4/src-gui/build-p4-Desktop-Release/p4 $P4_DIR/bin/)
$(cp ~/git/P4/src-gui/build-lyapunov-Desktop-Release/lyapunov $P4_DIR/bin/)
$(cp ~/git/P4/src-gui/build-separatrice-Desktop-Release/separatrice $P4_DIR/bin/)

# add help directory
$(cp ~/git/P4/help/*.* $P4_DIR/help/)
$(cp -r ~/git/P4/help/screenshots/ $P4_DIR/help/)

# add icons to p4/bin
$(cp ~/git/P4/help/p4_flag.png $P4_DIR/bin/portrait.png)
$(cp ~/git/P4/help/p4smallicon.png $P4_DIR/bin/)
$(cp ~/git/P4/help/newp4icon.png $P4_DIR/bin/)

# create symlink to p4 in /usr/bin
$(mkdir -p $DEB_DIR/usr/bin)
$(ln -s $P4_DIR/bin/p4 $DEB_DIR/usr/bin/p4)

# create DEBIAN folder and control file
$(mkdir -p $DEB_DIR/DEBIAN)
CONTROL=$DEB_DIR/DEBIAN/control
$(echo "Package: P4" >> $CONTROL)
$(echo "Version: $VERSION-1" >> $CONTROL)
$(echo "Section: math" >> $CONTROL)
$(echo "Priority: optional" >> $CONTROL)
$(echo "Architecture: amd64" >> $CONTROL)
$(echo "Depends: libstdc++6 (>=4.8.4), libc6 (>=2.19), libqt5printsupport5 (>=5.2.1), libqt5widgets5 (>=5.2.1), libqt5gui5 (>=5.2.1), libqt5core5a (>=5.2.1)" >> $CONTROL)
$(echo "Maintainer: Oscar Saleta <osr@mat.uab.cat>" >> $CONTROL)
$(echo "Description: Planar Polynomial Phase Portraits" >> $CONTROL)
$(echo " Mathematical software to compute and plot phase portraits" >> $CONTROL)
$(echo " for planar polynomial differential equations." >> $CONTROL)

# create deb file
dpkg-deb --build $DEB_DIR