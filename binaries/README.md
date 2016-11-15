# PRECOMPILED BINARIES FOR P4 (POLYNOMIAL PLANAR PHASE PORTRAITS)

## Latest release
Check <a href="https://github.com/oscarsaleta/P4-binaries/releases">the release page</a> to download the latest version of the compiled binaries for your system.

## Description

P4 is a software that is able to draw the planar phase portrait for any polynomial differential system on the Poincaré or Poincaré-Lyapunov compactified planes.

This folder contains the pre-compiled binaries and the correct file tree needed to run the software. There is a Linux version and a Windows version. The source code of P4 can be found in <a href="https://github.com/oscarsaleta/P4-src-gui">P4/src-gui</a>, <a href="https://github.com/oscarsaleta/P4-src-mpl">P4/src-mpl</a> and <a href="https://github.com/oscarsaleta/P4-help">P4/help</a>.

## Files

* **binaries**: latest compiled version of P4 for both Linux and Windows operative systems.

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