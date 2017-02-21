[//]: # ( This file is part of P4 )
[//]: # ( )
[//]: # ( Copyright (C) 1996-2016  J.C. Artés, C. Herssens, P. De Maesschalck, )
[//]: # (                          F. Dumortier, J. Llibre, O. Saleta )
[//]: # ( )
[//]: # ( P4 is free software: you can redistribute it and/or modify )
[//]: # ( it under the terms of the GNU Lesser General Public License as published )
[//]: # ( by the Free Software Foundation, either version 3 of the License, or )
[//]: # ( (at your option) any later version. )
[//]: # ( )
[//]: # ( This program is distributed in the hope that it will be useful, )
[//]: # ( but WITHOUT ANY WARRANTY; without even the implied warranty of )
[//]: # ( MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the )
[//]: # ( GNU Lesser General Public License for more details. )
[//]: # ( )
[//]: # ( You should have received a copy of the GNU Lesser General Public License )
[//]: # ( along with this program.  If not, see <http://www.gnu.org/licenses/>. )

# SCRIPTS FOR DEPLOYING 64-bit BINARIES FOR P4

## Latest release
Check <a href="https://github.com/oscarsaleta/P4/releases">the release page</a> to download the latest version of the compiled binaries for your system.

## Description

P4 is a software that is able to draw the planar phase portrait for any polynomial differential system on the Poincaré or Poincaré-Lyapunov compactified planes.

This folder contains some scripts that help create a Windows installer and a tarball for Linux. The compiled binaries are not here, they can be found in <a href="https://github.com/oscarsaleta/P4/releases">the release page</a>.

## Files

* **Linux**: scripts for generating tarball for deploying P4.
* **Windows**: scripts for generating an installer.

## Binary tree

The executable files have to be put in the following tree somewhere in the file system following this structure (the scripts already take this into account, but this information is useful if you plan to create your own P4 installation from scratch):

* **Linux**: in a folder named p4:
    * p4/bin: *p4*, *lyapunov*, *separatrice*, *p4.m*, *p4gcf.m*, *p4smallicon.png*, *portrait.png*, *newP4icon.png*,
    * p4/help: contents of **P4/help** folder,
    * p4/sum_tables: empty folder used by P4 to store some files.
* **Windows**: in a folder named p4:
    * p4\bin: *p4.exe*, *lyapunov.exe*, *separatrice.exe*, *p4.m*, *p4gcf.m*, *p4smallicon.png*, *portrait.png*, *newP4icon.png*,
    * p4\help: contents of **P4\help** folder,
    * p4\sum_tables: empty folder used by P4 to store some files.

**Note (v3.3)**: the *sum_tables* directory is needed for P4 to work properly (it expects the directory to be there). However, git does not allow to commit an empty folder, and although this can be easily hacked, the 3.3 version of the file tree did not include this hack, so the folder is not present in the repository (**it is present** in the compressed files found in <a href="https://github.com/oscarsaleta/P4/releases">the release page</a>). Therefore, if you cloned the repository and plan to use the *binaries* directory to run P4, please create the *sum_tables* beforehand.

## Contributors

- J.C. Artés (Universitat Autònoma de Barcelona)
- C. Herssens (Hasselt University)
- P. De Maesschalck (Hasselt University)
- F. Dumortier (Hasselt University)
- J. Llibre (Universitat Autònoma de Barcelona)
- O. Saleta (Universitat Autònoma de Barcelona)