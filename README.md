# P4 (Polynomial Planar Phase Portraits) [src-gui]

## Description

P4 is a software that is able to draw the planar phase portrait for any polynomial differential system on the Poincaré or Poincaré-Lyapunov compactified planes.

This repository contains the Maple scripts that drive the computations of P4 (finding singular points, computing algebraic Lyapunov constants, blow-ups, etc.).

## Build from source

The *mplstrip* program can be executed using `make` so all the .tex files are parsed. Finally, two main files are generated: *p4.m* and *p4cgf.m*, which are the ones that must be included in the bin directory of the p4 root folder.

## Binary tree

The executable files have to be put in the following tree somewhere in the file system (this repository only contains the source to generate *p4.m* and *p4cgf.m*):

* p4
    * bin: p4, lyapunov, separatrice, p4.m, p4gcf.m, p4smallicon.png, portrait.png
    * help: contents of **P4help** repository
    * sum_tables: empty folder
