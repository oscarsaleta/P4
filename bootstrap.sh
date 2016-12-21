#!/bin/bash
echo "======== COMPILATION AND INSTALLATION OF P4 ========"

echo "=== Checking for older P4 versions..."
if test -d $HOME/p4
then
    if whiptail --yesno "Older P4 install found in $HOME/p4. Do you want to remove it?" 20 60
    then
        rm -rf $HOME/p4
        if test -f $HOME/.local/share/applications/p4.desktop
        then
            rm -f $HOME/.local/share/applications/p4.desktop
        fi
    fi
    if whiptail --yesno "Do you want to remove P4 configuration?" 20 60
    then
        if test -d $HOME/.config/P4
        then
            rm -rf $HOME/.config/P4
        fi
    fi
elif test -d /usr/local/p4
then
    if whiptail --yesno "Older P4 install found in /usr/local/p4. Do you want to remove it?" 20 60
    then
        sudo rm -rf $HOME/p4
        if test -f /usr/share/applications/p4.desktop
        then
            sudo rm -f /usr/share/applications/p4.desktop
        fi
    fi
    if whiptail --yesno "Do you want to remove P4 configuration?" 20 60
    then
        if test -d $HOME/.config/P4
        then
            rm -rf $HOME/.config/P4
        fi
    fi
else
    echo "No older P4 version found."
fi

echo "=== Compiling P4..."
rm -rf build p4
make distclean
qmake -r P4.pro
make -j2
make install

echo "=== Installing P4..."
if whiptail --yesno "Do you want to install P4 in $HOME/p4?\n(If not, you will be asked whether you want to install it in /usr/local/p4)" 20 60
then
    cp -r p4 $HOME/
    INSTALLED=true
    INSTALLDIR=$HOME/p4
elif whiptail --yesno "Do you want to install P4 in /usr/local/p4?" 20 60
then
    sudo cp -r p4 /usr/local
    INSTALLED=true
    INSTALLDIR=/usr/local/p4
fi

if ($INSTALLED)
then
    echo "=== Creating application entry..."
    TMPDIR=/tmp/p4install
    mkdir -p $TMPDIR
    APPNAME=$HOME/.local/share/applications/p4.desktop
    TMPAPPNAME=$TMPDIR/p4.desktop
    VERSION=$(cat src-gui/version.h | grep -we VERSION | awk '{gsub("#define VERSION \"","\"");gsub("-.*,","");print}' | awk -F'"' '$0=$2')
    echo "[Desktop Entry]" > $TMPAPPNAME
    echo "Name=P4" >> $TMPAPPNAME
    echo "Version="$VERSION >> $TMPAPPNAME
    echo "Comment=Polynomial Planar Phase Portraits" >> $TMPAPPNAME
    echo "Icon=$INSTALLDIR/bin/p4smallicon.png" >> $TMPAPPNAME
    echo "Keywords=Math" >> $TMPAPPNAME
    echo "Exec=$INSTALLDIR/bin/p4" >> $TMPAPPNAME
    echo "Terminal=false" >> $TMPAPPNAME
    echo "Type=Application" >> $TMPAPPNAME
    echo "Categories=Education;Science;Math" >> $TMPAPPNAME
    echo "StartupNotify=true" >> $TMPAPPNAME
    mv $TMPAPPNAME $APPNAME
    chmod 664 $APPNAME
    # remove temporary files
    rm -r $TMPDIR
fi
