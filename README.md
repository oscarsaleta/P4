# P4 (Polynomial Planar Phase Portraits) [src-gui]

## Description

P4 is a software that is able to draw the planar phase portrait for any polynomial differential system on the Poincaré or Poincaré-Lyapunov compactified planes.

This repository contains the C++/Qt source code for the GUI of P4 *(p4 folder)*, and C++ code for numerical computation of Lyapunov constants *(lyapunov folder)* and separatrices *(separatrice folder)*.

## Build from source

In order to build the project it is advisable to use the Qt Creator IDE and build the three separate programs using their respective *.pro* Qt configuration files.

## Binary tree

The executable files have to be put in the following tree somewhere in the file system (this repository only contains the source for the three executables *p4*, *lyapunov* and *separatrice*):

* p4
    * bin: p4, lyapunov, separatrice, p4.m, p4gcf.m, p4smallicon.png, portrait.png
    * help: contents of **help** repository
    * sum_tables: empty folder
