# P4 (Polynomial Planar Phase Portraits) [src-gui]

## Description

P4 is a software that is able to draw the planar phase portrait for any polynomial differential system on the Poincaré or Poincaré-Lyapunov compactified planes.

## Files

* **src-gui**:
    * **p4**: C++/Qt source code for the GUI of P4,
    * **lyapunov**: C++ code for numerical computation of Lyapunov constants
    * **separatrice**: C++ code for numerical computation of separatrices

## Build from source

Generally, only source from *src-gui* needs to be compiled.

### src-gui

In order to build the project it is advisable to use the *Qt Creator* IDE and build the three separate programs (*p4*, *separatrice* and *lyapunov*) using their respective *.pro* Qt configuration files. The source in *src-gui* has been compiled with with Qt 5.6.1 (Linux binaries) and 5.7.0 (Windows binaries).

### src-mpl

The Maple scripts in this repository are compatible with Maple 2015. Older versions of Maple might still work but have not been tested. If you have Maple 2015, nothing needs to be compiled. Otherwise, use *Qt Creator* to compile the *mplscript* program and then use the *Makefile* to extract the Maple code from the *.tex* files in order to generate *p4.m* and *p4gcf.m*.

## Binary tree

The executable files have to be put in the following tree somewhere in the file system following this structure:

* **Linux**: in a folder named p4:
    * p4/bin: *p4*, *lyapunov*, *separatrice*, *p4.m*, *p4gcf.m*, *p4smallicon.png*, *portrait.png*,
    * p4/help: contents of **P4/help** folder,
    * p4/sum_tables: empty folder used by P4 to store some files.
* **Windows**: in a folder named p4:
    * p4\bin: *p4.exe*, *lyapunov.exe*, *separatrice.exe*, *p4.m*, *p4gcf.m*, *p4smallicon.png*, *portrait.png*,
    * p4\help: contents of **P4\help** folder,
    * p4\sum_tables: empty folder used by P4 to store some files.

## Contributors

- J.C. Artés (Universitat Autònoma de Barcelona)
- C. Herssens (Hasselt University)
- P. De Maesschalck (Hasselt University)
- F. Dumortier (Hasselt University)
- J. Llibre (Universitat Autònoma de Barcelona)
- O. Saleta (Universitat Autònoma de Barcelona)