#!/bin/bash
echo "======== COMPILATION AND INSTALLATION OF P4 ========"

echo "=== Checking for older P4 versions..."
if test -d $HOME/p4
then
    if whiptail --title "Uninstalling older versions" --yesno "Older P4 install found in $HOME/p4. Do you want to remove it?" 20 60
    then
        rm -rf $HOME/p4
        if test -f $HOME/.local/share/applications/p4.desktop
        then
            rm -f $HOME/.local/share/applications/p4.desktop
        fi
    fi
fi
if test -d /usr/local/p4
then
    if whiptail --title "Uninstalling older versions" --yesno "Older P4 install found in /usr/local/p4. Do you want to remove it?" 20 60
    then
        sudo rm -rf /usr/local/p4
        if test -f /usr/share/applications/p4.desktop
        then
            sudo rm -f /usr/share/applications/p4.desktop
        fi
        if test -f /usr/local/bin/p4
        then
            sudo rm -f /usr/local/bin/p4
        fi
    fi
else
    echo "No older P4 version found."
fi
if test -d $HOME/.config/P4
then
    if whiptail --title "Uninstalling older versions" --yesno --defaultno "Do you want to remove P4 configuration?" 20 60
    then
        if test -d $HOME/.config/P4
        then
            rm -rf $HOME/.config/P4
        fi
    fi
fi
echo "Done."

echo "=== Compiling P4..."
rm -rf build p4 >/dev/null 2>&1
make distclean >/dev/null 2>&1
qmake -r P4.pro >/dev/null 2>&1 || (echo "Error, check that qmake is a valid command." && exit 1)
make -s -j2 >/dev/null 2>&1
make -s install >/dev/null 2>&1
echo "Done."

echo "=== Installing P4..."
if whiptail --title "Install P4" --yesno --yes-button "User" --no-button "Root" "Where do you want to install P4?\n\nUser: $HOME/p4\nRoot: /usr/local/p4" 20 60
then
    mv p4 $HOME/
    INSTALLED=true
    INSTALLDIR=$HOME/p4
    ln -s $INSTALLDIR/sumtables $INSTALLDIR/sum_tables
    if whiptail --title "Create shortcut?" --yesno "Do you want to create an alias for executing P4 from the terminal?\nIf not, you will be able to execute it by typing\n\n ~/p4/bin/p4\n\nin the command line." 20 60
    then
        grep $HOME/.profile -e "P4_DIR" || echo "P4_DIR=$HOME/p4/\nexport P4_DIR\nalias p4=$P4_DIR/bin/p4" >> $HOME/.profile
        source $HOME/.profile
    fi
else
    sudo mv p4 /usr/local
    INSTALLED=true
    INSTALLDIR=/usr/local/p4
    sudo ln -s $INSTALLDIR/sumtables $INSTALLDIR/sum_tables
    sudo ln -s $INSTALLDIR/bin/p4 /usr/local/bin/p4
fi
echo "Done."

#if ($INSTALLED)
#then
#    echo "=== Creating application entry..."
#    TMPDIR=/tmp/p4install
#    mkdir -p $TMPDIR
#    APPNAME=$HOME/.local/share/applications/p4.desktop
#    TMPAPPNAME=$TMPDIR/p4.desktop
#    VERSION=$(cat src-gui/version.h | grep -we VERSION | awk '{gsub("#define VERSION \"","\"");gsub("-.*,","");print}' | awk -F'"' '$0=$2')
#    echo "[Desktop Entry]" > $TMPAPPNAME
#    echo "Name=P4" >> $TMPAPPNAME
#    echo "Version="$VERSION >> $TMPAPPNAME
#    echo "Comment=Polynomial Planar Phase Portraits" >> $TMPAPPNAME
#    echo "Icon=$INSTALLDIR/bin/p4smallicon.png" >> $TMPAPPNAME
#    echo "Keywords=Math" >> $TMPAPPNAME
#    echo "Exec=env P4_DIR=$INSTALLDIR $INSTALLDIR/bin/p4" >> $TMPAPPNAME
#    echo "Path=$HOME" >> $TMPAPPNAME
#    echo "Terminal=true" >> $TMPAPPNAME
#    echo "Type=Application" >> $TMPAPPNAME
#    echo "Categories=Education;Science;Math" >> $TMPAPPNAME
#    echo "StartupNotify=true" >> $TMPAPPNAME
#    mv $TMPAPPNAME $APPNAME
#    chmod 664 $APPNAME
    # remove temporary files
#    rm -r $TMPDIR
#    echo "=== Creating application entry... Done."
#fi
