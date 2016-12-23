# P4 (Polynomial Planar Phase Portraits)

## Description

P4 is a software that is able to draw the planar phase portrait for any polynomial differential system on the Poincaré or Poincaré-Lyapunov compactified planes.

<img src="help/screenshots/p4_main_window.png" alt="file not found" height="430px"/>
<img src="help/screenshots/p4_plot_poincare.png" alt="file not found" height="430px" align="right" />

## Installation

Check <a href="https://github.com/oscarsaleta/P4/releases/latest">releases page</a> to see more detailed instructions and download links.

### Linux

0. Check <a href="#dependencies">dependencies</a>,
1. `git clone https://github.com/oscarsaleta/P4.git && cd P4`,
2. `./compile_install_linux` and follow the instructions,
3. Done!

### Windows

1. Download *P4setup.exe* from <a href="https://github.com/oscarsaleta/P4/releases/latest">here</a>,
2. Execute installer and follow the instructions,
3. Done!

## Dependencies

**Maple must be installed in the computer in order to use P4.**

### Linux

* **Installation with `compile_install_linux` or compilation from source using `qmake`:**
    - Git: must have to work with *git projects*:
        + Debian-based (Debian/Ubuntu/Mint): `sudo apt install git`
        + Fedora-based (Fedora/Kokora/Arquetype): `sudo dnf install git`
        + Arch-based (Archlinux/Antergos): `sudo pacman -S git`
        + (See the <a href="https://wiki.archlinux.org/index.php/git">git ArchWiki page</a> for more information and configuration instructions).
	- Qt modules *QtCore*, *QtGui*, *QtWidgets*, *QtPrintSupport* (minimum required version 5.2.1, could work for 4.6.0 but this has not been tested):
        + Debian-based (Debian/Ubuntu/Mint): `sudo apt install qt5-default`
        + Fedora-based (Fedora/Kokora/Arquetype): `sudo dnf install qt5*-devel --allowerasing`
        + Arch-based (Archlinux/Antergos): `sudo pacman -S qt5-base`
	- `qmake` (version 3.0 recommended):
        + Debian-based (Debian/Ubuntu/Mint): `sudo apt install qt5-qmake`
        + Other distros: already installed if you followed the install instructions for Qt 5.
	- C++ compiler: `g++` (version >=6.0 recommended):
        + Debian-based (Debian/Ubuntu/Mint): `sudo apt install gcc g++`
        + Fedora-based (Fedora/Kokora/Arquetype): `sudo dnf group install 'Development Tools'`
        + Arch-based (Archlinux/Antergos): `sudo pacman -S gcc`
* **Compilation from source using Qt Creator:**
    - Dependencies itemized above,
	- Qt Creator IDE:
        + Debian-based (Debian/Ubuntu/Mint): `sudo apt install qtcreator`
        + Fedora-based (Fedora/Kokora/Arquetype): `sudo dnf install qt-creator`
        + Arch-based (Archlinux/Antergos): `sudo pacman -S qtcreator`
	
### Windows

* **Installation using `P4setup.exe`**:
	- Dependencies are taken care of.
* **Compilation from source using Qt Creator:**
	- Qt modules *QtCore*, *QtGui*, *QtWidgets*, *QtPrintSupport* (minimum required version 5.2.1, could work for 4.6.0 but this has not been tested).
        + Download from <a href="https://www.qt.io/">Qt website</a>,
	- C++ compiler: Microsoft Visual C++ (tested with version 14.0).
        + Download from <a href="https://www.visualstudio.com/vs/cplusplus/#downloadvs">Visual Studio Community</a>.

## Files

* **src-gui**:
    * **p4**: C++/Qt source code for the GUI of P4,
    * **lyapunov**: C++ code for numerical computation of Lyapunov constants
    * **separatrice**: C++ code for numerical computation of separatrices
* **src-mpl**: Maple scripts that drive the computations of P4 (finding singular points, algebraic computation of Lyapunov constants, blow-ups, etc.).
* **help**: Help files in *.html* format and supporting images.
* **binaries**: latest compiled version of P4 for both Linux and Windows operative systems (64-bit only).

## Manually build P4 from source

### Build using `qmake`

Qt's `qmake` makes it easy to compile the whole project with two commands.

* From the root of the project, run `qmake -r P4.pro` in order to generate the Makefiles.
* From this same directory, run `make` to compile **p4**, **lyapunov** and **separatrice**.
    - The output of the compilation will be found in a new directory named *build*.
    - The `make` command also compiles **mplstrip**, which is a C++ program used to generate Maple scripts inside *src-mpl*.
* **If your Maple version is NOT 2015**: go into *src-mpl* and execute `make -f MakeTexMaple clean` and `make p4.m`. This will generate **p4gcf.m** and **p4.m**, two Maple scripts needed by P4.
* From the root directory of the project, run `make install` to generate a *p4* folder with the layout explained <a href="#binary-tree">below</a>. This directory can be placed anywhere in the system and P4 can be executed using `/path/to/p4/bin/p4`.
* It is advisable that you set the environment variable `P4_DIR=/path/to/p4` to the actual path of the P4 directory.

### Build using Qt Creator

There is a special directory in the repository called *QtCreator*, which contains 4 subfolders:

* *p4*: contains the source of the main GUI elements. Load a project in Qt Creator through the file `QtCreator/p4/p4.pro`, select a Qt kit, and compile.
* *lyapunov*: contains the source of the Lyapunov constants calculator. Load a project in Qt Creator through the file `QtCreator/lyapunov/lyapunov.pro`, select a Qt kit, and compile.
* *separatrice*: contains the source of the separatrice numeric calculator. Load a project in Qt Creator through the file `QtCreator/separatrice/separatrice.pro`, select a Qt kit, and compile.
* *mplstrip*: contains the source of the *TeX-to-Maple* parser for generating the Maple scripts. Load a project in Qt Creator through the file `QtCreator/mplstrip/mplstrip.pro`, select a Qt kit, and compile.
    - If you need to update the Maple scripts, copy the `mplstrip` executable to *src-mpl/mplstrip* and run `make -f MakeTexMaple` from *src-mpl*.

Once all the projects have been compiled, create a directory following the instructions of <a href="#binary-tree">the next section</a> and place it anywhere in your system.

* **src-gui**: See <a href="#dependencies">dependencies</a>.
	- **Linux**: 
		+ **Using Autotools:** follow the usual `./configure`, `make`, `make install` procedure.
	- **Linux** and **Windows**:
		+ **Using Qt Creator:** build the three separate programs (*p4*, *separatrice* and *lyapunov*) using their respective *.pro* Qt configuration files (use a *Release* profile to get optimized executables).
* **src-mpl**: The Maple scripts in this repository are compatible with Maple 2015. Older versions of Maple might still work but have not been tested. If you have Maple 2015, nothing needs to be compiled (since the files are already compiled in the source tree).
	- **Linux** and **Windows**:
		+ **Using Qt Creator**: compile the *mplscript* program using the *mplscript.pro* file and then use the *Makefile* (`make`) to extract the Maple code from the *.tex* files in order to generate *p4.m* and *p4gcf.m*.

## Binary tree

The executable files have to be put in the following tree somewhere in the file system following this structure (in Linux, `make install` and the scripts `installP4.sh` and `installP4_root.sh` do this automatically; in Windows, the installer also does this automatically):

* **Linux**: in a folder named p4:
    * p4/bin: *p4*, *lyapunov*, *separatrice*, *p4.m*, *p4gcf.m*, *p4smallicon.png*, *portrait.png*,
    * p4/help: contents of **P4/help** folder,
    * p4/sumtables: empty folder used by P4 to store some files (**permissions must be read/write/execute for user**: `chmod 777 sumtables`).
    * p4/sum_tables: soft link to p4/sumtables (`ln -s p4/sumtables p4/sum_tables`).
* **Windows**: in a folder named p4:
    * p4\bin: *p4.exe*, *lyapunov.exe*, *separatrice.exe*, *p4.m*, *p4gcf.m*, *p4smallicon.png*, *portrait.png*,
    * p4\help: contents of **P4\help** folder,
    * p4\sumtables: empty folder used by P4 to store some files (**user must have read/write permissions on this folder**).
    * p4\sum_tables: shortcut to p4\sumtables.

## Contributors

- J.C. Artés (Universitat Autònoma de Barcelona)
- C. Herssens (Hasselt University)
- P. De Maesschalck (Hasselt University)
- F. Dumortier (Hasselt University)
- J. Llibre (Universitat Autònoma de Barcelona)
- O. Saleta (Universitat Autònoma de Barcelona)
