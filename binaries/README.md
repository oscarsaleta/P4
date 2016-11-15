# PRECOMPILED BINARIES FOR P4 (POLYNOMIAL PLANAR PHASE PORTRAITS)

## Latest release
Check <a href="https://github.com/oscarsaleta/P4-binaries/releases">the release page</a> to download the latest version of the compiled binaries for your system.

## Contributors

- O. Saleta (Universitat Autònoma de Barcelona)
- J.C. Artés (Universitat Autònoma de Barcelona)
- C. Herssens (Hasselt University)
- P. De Maesschalck (Hasselt University)
- F. Dumortier (Hasselt University)
- J. Llibre (Universitat Autònoma de Barcelona)

## Description

P4 is a software that is able to draw the planar phase portrait for any polynomial differential system on the Poincaré or Poincaré-Lyapunov compactified planes.

This repository contains the pre-compiled binaries and the correct file tree needed to run the software. There is a Linux version and a Windows version. The source code of P4 can be found in <a href="https://github.com/oscarsaleta/P4-src-gui">P4/src-gui</a>, <a href="https://github.com/oscarsaleta/P4-src-mpl">P4/src-mpl</a> and <a href="https://github.com/oscarsaleta/P4-help">P4/help</a>.

## Binary tree

The executable files have to be put in the following tree somewhere in the file system (this repository already provides the following tree):

* p4
    * bin: p4, lyapunov, separatrice, p4.m, p4gcf.m, p4smallicon.png, portrait.png
    * help: contents of **help** repository
    * sum_tables: empty folder (will be used by the program to store needed files)