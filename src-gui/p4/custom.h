/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  P4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CUSTOM_H
#define CUSTOM_H

#include "color.h"

// Defaults regarding Vector Field and Filename

#define DEFAULTFILENAME "untitled"
#define DEFAULTXDOT "0"
#define DEFAULTYDOT "0"
#define DEFAULTGCF "0"
#define MAXNUMPARAMS 99
#define MAXNUMPARAMSSHOWN 4 // when more parameters: a scroll bar is used

// Defaults, Minima and Maxima regarding Find Parameters

#define DEFAULTTYPE TYPEOFSTUDY_ALL // choose _ALL, _ONE, _FIN or _INF
#define DEFAULTNUMERIC true
#define DEFAULTTESTSEP false
#define DEFAULTSAVEALL false

#define MINPRECISION 1
#define MAXPRECISION 99
#define DEFAULTPRECISION 8

#define MINPRECISION0 0
#define MAXPRECISION0 99
#define DEFAULTPRECISION0 0

#define DEFAULTEPSILON "0.01"
#define DEFAULTX0 "0"
#define DEFAULTY0 "0"

#define MINLEVEL 0
#define MAXLEVEL 99
#define DEFAULTLEVEL 6

#define MINNUMLEVEL 0
#define MAXNUMLEVEL 99
#define DEFAULTNUMLEVEL 10

#define MINMAXLEVEL 0
#define MAXMAXLEVEL 99
#define DEFAULTMAXLEVEL 20

#define MINWEAKNESS 0
#define MAXWEAKNESS 99
#define DEFAULTWEAKNESS 4

#define MINP 1
#define MAXP 99
#define DEFAULTP 1

#define MINQ 1
#define MAXQ 99
#define DEFAULTQ 1

// by default: plot also virtual singularities
#define DEFAULTPLOTVIRTUALSINGULARITIES true

// in case of the study of one singularity: define frame

#define X_MIN -1
#define X_MAX 1
#define Y_MIN -1
#define Y_MAX 1

// parameters in the plot window (including parameters for the Runge-Kutta 7/8
// method)
// in case of a GCF: integrate using original vf
#define DEFAULT_INTCONFIG INTCONFIG_ORIGINAL
#define INTCONFIG_ORIGINAL 1 // integrate using original VF
#define INTCONFIG_REDUCED 0  // integrate using reduced VF when possible

#define DEFAULT_HMA 1.E-1 // default value for HMA (=MAXIMUM STEP SIZE)
#define MIN_HMA 1.E-32    // minimum value for HMA = 10^(-16)
#define MAX_HMA 10.0      // maximum value for HMA = 10^1

#define DEFAULT_HMI 1.E-6 // default value for HMI (=MINIMUM STEP SIZE)
#define MIN_HMI 1.E-32    // minimum value for HMI
#define MAX_HMI 10.0      // maximum value for HMI

#define DEFAULT_STEPSIZE 0.01 // default step size
                              // stepsize must be in between HMI and HMA

#define DEFAULT_PROJECTION -1.0 // default projection (Poincare sphere only)
#define MIN_PROJECTION -1.E16   // projection can be VERY negative
#define MAX_PROJECTION -1.E-32  // projection must be strictly negative

#define DEFAULT_TOLERANCE 1.E-8 // default tolerance
#define MIN_TOLERANCE 1.E-32    // minimum tolerance
#define MAX_TOLERANCE 1.0       // maximum tolerance

#define DEFAULT_INTPOINTS 200 // number of points during integration
#define MIN_INTPOINTS 1
#define MAX_INTPOINTS 32767

#define DEFAULT_LINESTYLE                                                      \
    LINESTYLE_DASHES // choose between LINESTYLE_DASHES and LINESTYLE_POINTS

#define UPDATEFREQ_STEPSIZE 100 // after each 100 points of integration: update
                                // "current step size" field in this window.

// print window

#define DEFAULT_RESOLUTION 600 // default printer resolution (DPI)
#define MIN_RESOLUTION 36      // minimum printer resolution
#define MAX_RESOLUTION 4800    // maximum printer resolution

#define DEFAULT_SYMBOLSIZE                                                     \
    1.6 // default symbol size (mm) when printing the singularities
#define MIN_SYMBOLSIZE 0.05 // minimum symbol size
#define MAX_SYMBOLSIZE 30.0 // maximum symbol size

#define DEFAULT_LINEWIDTH                                                      \
    0.3                    // default line width (mm) when printing orbits etc
#define MIN_LINEWIDTH 0.05 // minimum line width
#define MAX_LINEWIDTH 10.0 // maximum line width

// limit cycles window:
#define DEFAULT_LCORBITS                                                       \
    1 // number of orbits to integrate before progress window is updated
#define MIN_LCORBITS 1 // when dividing transverse section length by
#define MAX_LCORBITS                                                           \
    32767 //  grid: number of orbits must lie between these values
#define DEFAULT_LCPOINTS 2000 // number of points to integrate EACH orbit
#define MIN_LCPOINTS 1
#define MAX_LCPOINTS 32767
#define DEFAULT_LCGRID 0.01 // division interval for the transverse section
#define MIN_LCGRID 1.E-16
#define MAX_LCGRID 1E16

// Greatest common factor window
#define DEFAULT_GCFPOINTS 40 // 40 horizontal and vertical points
#define MIN_GCFPOINTS 1
#define MAX_GCFPOINTS 32767
#define DEFAULT_GCFPRECIS 12 // 12 digits precision
#define MIN_GCFPRECIS 1
#define MAX_GCFPRECIS 99
#define DEFAULT_GCFMEMORY 16000 // 16 MB of memory
#define MIN_GCFMEMORY 64
#define MAX_GCFMEMORY 512000

// Curve and isoclines window
#define DEFAULT_CURVEPOINTS 400 // 40 horizontal and vertical points
#define MIN_CURVEPOINTS 1
#define MAX_CURVEPOINTS 32767
#define DEFAULT_CURVEPRECIS 12 // 12 digits precision
#define MIN_CURVEPRECIS 1
#define MAX_CURVEPRECIS 99
#define DEFAULT_CURVEMEMORY 16000 // 16 MB of memory
#define MIN_CURVEMEMORY 64
#define MAX_CURVEMEMORY 512000

// Window appearance
#define FONTSIZE +0         // 0 points larger than system font
#define TITLEFONTSIZE +2    // 2 points larger than system font
#define LEGENDFONTSIZE (-1) // 1 smaller than system font

// main P4 help file name (under the /help subdirectory of the P4 installation)

#define P4HELPFILE "p4.html"

// main reduce file (in the /reduce subdirectory of the P4 installation)

//#define MAINREDUCEFILE "find_critical_points.red"

// main maple file (in the /bin subdirectory of the P4 installation)

#define MAINMAPLEFILE "p4.m"
#define MAINMAPLEGCFFILE "p4gcf.m"

// Defining colors.
//
// Choose one of the constants defined in COLOR.H

// Color of singular points:
namespace bgColours
{
extern int CFOREGROUND; // foreground color
extern int CBACKGROUND; // background color
extern int CORBIT;      // orbits (use GREEN1 when background is white)
// IN PRINT_BITMAP.CPP: PrintColorTable shows how these colors are treated when
// printing.
// For example, when printing, BLACK can be reversed with WHITE, so that the
// BLACK background on screen is not printed black.
extern bool PRINT_WHITE_BG;
}

#define CSADDLE_NODE MAGENTA                   // saddle-node
#define CSADDLE GREEN2                         // saddle
#define CNODE_S BLUE                           // stable node
#define CNODE_U RED                            // unstable node
#define CWEAK_FOCUS bgColours::CFOREGROUND     // weak focus
#define CWEAK_FOCUS_S BLUE2                    // stable weak focus
#define CWEAK_FOCUS_U RED2                     // unstable weak focus
#define CSTRONG_FOCUS_S BLUE                   // stable strong focus
#define CSTRONG_FOCUS_U RED                    // unstable strong focus
#define CCENTER GREEN2                         // center
#define CDEGEN bgColours::CFOREGROUND          // degenerated
#define CLINEATINFINITY bgColours::CFOREGROUND // color of poincare sphere

#define CSTABLE BLUE        // stable separatrice
#define CUNSTABLE RED       // unstable separatrice
#define CCENT_STABLE BLUE2  // center-stable separatrice
#define CCENT_UNSTABLE RED1 // center-unstable separatrice

#define CW_SEP GOLD     // selected separatrice
#define CLIMIT MAGENTA2 // limit cycles
#define CSING GREEN     // curve of singularities
#define CCURV CYAN
#define CISOC PINK1

//#define   USE_SYSTEM_PRINTER          // comment when system printer fails

// defining symbol sizes:
// these are the sizes of the symbols in the plot window/legend window
//
// Note: these sizes must be even!

#define SYMBOLSIZE 6

#define SYMBOLHEIGHT SYMBOLSIZE
#define SYMBOLWIDTH SYMBOLSIZE

// plot window appearance

#define MINWIDTHPLOTWINDOW 150
#define MINHEIGHTPLOTWINDOW 150

#define NOMINALWIDTHPLOTWINDOW 500
#define NOMINALHEIGHTPLOTWINDOW 500

// MATHEMATICS

// during calculations, P4/Maple tries to simplify intermediate expressions.
// The simplify command could do well, but sometimes it is better to try to
// simplify expressions with radicals in it as well.  Unfortunately, we have
// found that radsimp contains a bug causing Maple to divide by 0 internally.
// The construction below is a workaround for this bug:

#define MAPLE_SIMPLIFY_EXPRESSIONS                                             \
    "proc(expr) try radsimp(simplify(expr),ratdenom) catch: simplify(expr) "   \
    "end try end";

// determine when the point (in the R2-chart) of the orbit goes to infinity
// if z coord on the poincare sphere is less than ZCOORD then point goes to
// infinity:

#define ZCOORD (sqrt(2) / 2)

#define RADIUS 0.25 // radius for the finite points
#define RADIUS2 (RADIUS * RADIUS)

// PRINTING

// when producing an (E)PS image, P4 will try to center the 15-cm wide image on
// a white
// page.  Therefore it needs to know the dimensions of the output paper.  By
// default,
// it will assume European A4 format.  The following dimensions are given in
// inches:

#define POSTSCRIPTPAGEWIDTH 8.2
#define POSTSCRIPTPAGEHEIGHT 11.6

// TOOLTIPS

// if you define TOOLTIPS then tool tips (black on yellow information screens)
// will appear

#define TOOLTIPS

#endif /* CUSTOM_H */
