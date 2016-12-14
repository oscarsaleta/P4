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
