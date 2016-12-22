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

echo "=== Checking for dependencies..."
if ldconfig -p | grep libQt5PrintSupport >/dev/null && ldconfig -p | grep libQt5Widgets >/dev/null && ldconfig -p | grep libQt5Gui >/dev/null && ldconfig -p | grep libQt5Core >/dev/null
then
    echo "libQt5Core, libQt5Gui, libQt5Widgets and libQt5PrintSupport found in the system."
else
    whiptail --title "Missing Qt5 libraries" --msgbox "Some Qt5 libraries were not found in the system. Install them using the proper method for your distribution, e.g.:\n\n - Debian-based (Debian/Ubuntu/Mint): sudo apt install qt5-default\n - Fedora-based (Fedora/Kokora/Arquetype): sudo dnf install qt5*-devel --allowerasing\n - Arch-based (Archlinux/Antergos): sudo pacman -S qt5-base" 20 60
    exit 1
fi
if ldconfig -p | grep libstdc++ >/dev/null && ldconfig -p | grep libgcc_s >/dev/null && ldconfig -p | grep libc >/dev/null
then
    echo "GCC libraries found in the system."
else
    whiptail --title "Missing GCC libraries" --msgbox "Some GCC libraries were not found in the system. Install them using the proper method for your distribution, e.g.:\n\n - Debian-based (Debian/Ubuntu/Mint): sudo apt install gcc g++\n - Fedora-based (Fedora/Kokora/Arquetype): sudo dnf group install 'Development Tools'\n - Arch-based (Archlinux/Antergos): sudo pacman -S gcc" 20 60
    exit 1
fi
echo "Done."

echo "=== Compiling P4..."
whiptail --infobox --title "Compiling P4..." "Compiling P4, be patient!" 20 60
echo "Removing old builds..."
rm -rf build p4 >/dev/null 2>&1
make distclean >/dev/null 2>&1
echo "Running qmake..."
if ! qmake -r P4.pro >/dev/null 2>&1
then
    echo "Error, check that qmake is a valid command."
    exit 1
fi
CPUCNT=$(grep -c ^processor /proc/cpuinfo)
echo "Compiling... Will use $CPUCNT jobs for make"
make -s -j$CPUCNT >/dev/null 2>&1
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
        grep $HOME/.profile -e "P4_DIR" || (echo "P4_DIR=$HOME/p4/" >> $HOME/.profile && echo "export P4_DIR" >> $HOME/.profile && echo "PATH=$P4_DIR/bin:$PATH" >> $HOME/.profile)
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

echo "=== Relogging might be necessary for the PATH variable to refresh."

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
