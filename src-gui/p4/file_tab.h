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

#ifndef FILE_TAB_H
#define FILE_TAB_H

#include <QObject>
#include <QString>

class QTextEdit;

// -----------------------------------------------------------------------
//                      General polynomial expressions
// -----------------------------------------------------------------------

// Linked list of univariate terms a*x^i
namespace p4polynomials
{
struct term1 {
    int exp;
    double coeff;
    struct term1 *next_term1;

    term1() : next_term1(nullptr){};
};

// typedef struct term1 *P4POLYNOM1;

// Linked list of terms a*x^i*y^j

struct term2 {
    int exp_x;
    int exp_y;
    double coeff;
    struct term2 *next_term2;

    term2() : next_term2(nullptr){};
};

// Linked list of terms a*r^i*cos(theta)^j*sin(theta)^k

// typedef struct term2 *P4POLYNOM2;

struct term3 {
    int exp_r;
    int exp_Co;
    int exp_Si;
    double coeff;
    struct term3 *next_term3;

    term3() : next_term3(nullptr){};
};

// typedef struct term3 *P4POLYNOM3;
}
// -----------------------------------------------------------------------
//                              Orbits
// -----------------------------------------------------------------------
namespace p4orbits
{
struct orbits_points {
    int color;  // color of seperatrice

    double pcoord[3];  // point on the poincare sphere -> p=(X,Y,Z)
                       // or on the poincare-lyapunov sphere
                       // -> p=(0,x,y) or p=(1,r,theta)
    int dashes;
    int dir;   // if we have a line of sing at infinity and have to change
    int type;  // the direction if we integrate the orbit of separatrice
               // and sometimes the type

    struct orbits_points *next_point;

    orbits_points() : next_point(nullptr){};
};

// typedef struct orbits_points *P4ORBIT;

struct orbits {
    double pcoord[3];  // startpoint
    int color;
    orbits_points *f_orbits;          // orbit
    orbits_points *current_f_orbits;  // orbit
    struct orbits *next_orbit;

    orbits()
        : f_orbits(nullptr), current_f_orbits(nullptr), next_orbit(nullptr){};
};
}

// -----------------------------------------------------------------------
//                      Curves and isoclines
// -----------------------------------------------------------------------
struct curves {
    p4polynomials::term2 *r2, *u1, *u2, *v1, *v2;
    p4polynomials::term3 *c;
    p4orbits::orbits_points *points;

    curves()
        : r2(nullptr),
          u1(nullptr),
          u2(nullptr),
          v1(nullptr),
          v2(nullptr),
          c(nullptr),
          points(nullptr){};
};

struct isoclines {
    p4polynomials::term2 *r2, *u1, *u2, *v1, *v2;
    p4polynomials::term3 *c;
    p4orbits::orbits_points *points;
    int color;

    isoclines()
        : r2(nullptr),
          u1(nullptr),
          u2(nullptr),
          v1(nullptr),
          v2(nullptr),
          c(nullptr),
          points(nullptr){};
};

// -----------------------------------------------------------------------
//                      Blow up structure
// -----------------------------------------------------------------------
namespace p4blowup
{
struct transformations {
    double x0, y0;               // translation point
    int c1, c2, d1, d2, d3, d4;  // F(x,y)=(c1*x^d1*y^d2,c2*x^d3*y^d4)
    int d;                       // X/x^d
    struct transformations *next_trans;

    transformations() : next_trans(nullptr){};
};

struct blow_up_points {
    int n;  // number of transformations
    struct transformations *trans;
    double x0, y0;                  // last point that is not degenerate
    double a11, a12, a21, a22;      // transformation matrix
    struct term2 *vector_field[2];  // vector field
    struct term1 *sep;              // sep (t,g(t))
    int type;                       // type of seperatrice (STYPE_STABLE, ...)
    int blow_up_vec_field;  // if true then use the blow up vector field if
    // the modulus of the last point of the separatrices is less
    // than 1
    double point[2];  // end point sep in blow up chart

    struct orbits_points *first_sep_point;
    struct orbits_points *last_sep_point;
    struct blow_up_points *next_blow_up_point;

    blow_up_points()
        : trans(nullptr),
          sep(nullptr),
          first_sep_point(nullptr),
          last_sep_point(nullptr),
          next_blow_up_point(nullptr){};
};

struct sep {
    struct orbits_points *first_sep_point;
    struct orbits_points *last_sep_point;
    int type;  // STYPE_STABLE, UNSTABLE, CENSTABLE or CENUNSTABLE
    int direction;
    int d;
    bool notadummy;  // false if separatrice is a copy of a structure (obtained
                     // through a symmetry)
    struct term1 *separatrice;  // if d=0 -> (t,f(t)), d=1 ->(f(t),t)
    struct sep *next_sep;

    sep()
        : first_sep_point(nullptr),
          last_sep_point(nullptr),
          separatrice(nullptr),
          next_sep(nullptr){};
};
}

// -----------------------------------------------------------------------
//                          Singularities
// -----------------------------------------------------------------------

// (taken from P5 source code) ----------
// in a piecewise system, singularities may coincide along the bifurcation
// lines.  In that case, we mark the singularity in a different way.
// Of course, on screen it need only be marked once and not several times.

#define POSITION_VIRTUAL 0          // virtual singularity
#define POSITION_STANDALONE 1       // stand-alone singularity
#define POSITION_COINCIDING_MAIN 2  // singularity coincides and will be drawn
#define POSITION_COINCIDING_VIRTUAL \
    3  // is a virtual singularity, but coincides with a real one so do not plot
       // at all
#define POSITION_COINCIDING 4  // singularity coincides, but is already drawn
// --------------------------------------
namespace p4singularities
{
struct genericsingularity  // part of the structure that is the same for all
                           // types
{
    double x0;
    double y0;
    struct genericsingularity *next;
    int chart;
    int position;  // POSITION_ constants

    genericsingularity() : next(nullptr){};
};

struct saddle {
    double x0;
    double y0;
    struct saddle *next_saddle;
    int chart;
    int position;  // POSITION_ constants

    double epsilon;
    bool notadummy;

    struct sep *separatrices;
    struct term2 *vector_field[2];  // vector field {xdot,ydot}
    double a11, a12, a21, a22;      // transformation matrix

    saddle() : next_saddle(nullptr), separatrices(nullptr){};
};

struct semi_elementary {
    double x0;
    double y0;
    struct semi_elementary *next_se;
    int chart;
    int position;  // POSITION_ constants

    double epsilon;
    bool notadummy;

    struct sep *separatrices;       // center sep (t,f(t)), sep (g(t),t)
    struct term2 *vector_field[2];  // vector field
    double a11, a12, a21, a22;      // transformation matrix

    int type;  // type of semi-elementary point

    semi_elementary() : next_se(nullptr), separatrices(nullptr){};
};

struct degenerate {
    double x0;
    double y0;
    struct degenerate *next_de;
    int chart;
    int position;  // POSITION_ constants

    double epsilon;
    bool notadummy;

    struct blow_up_points *blow_up;

    degenerate() : next_de(nullptr), blow_up(nullptr){};
};

struct node {
    double x0;
    double y0;
    struct node *next_node;
    int chart;
    int position;  // POSITION_ constants

    int stable;

    node() : next_node(nullptr){};
};

struct strong_focus {
    double x0;
    double y0;
    struct strong_focus *next_sf;
    int chart;
    int position;  // POSITION_ constants

    int stable;

    strong_focus() : next_sf(nullptr){};
};

struct weak_focus {
    double x0;
    double y0;
    struct weak_focus *next_wf;
    int chart;
    int position;  // POSITION_ constants

    int type;

    weak_focus() : next_wf(nullptr){};
};
}
// -----------------------------------------------------------------------
//                          Some definitions
// -----------------------------------------------------------------------

#define CHART_R2 0
#define CHART_U1 1
#define CHART_U2 2
#define CHART_V1 3
#define CHART_V2 4

#define SADDLE 1
#define NODE 2
#define WEAK_FOCUS 3
#define STRONG_FOCUS 4
#define SEMI_HYPERBOLIC 5
#define NON_ELEMENTARY 6

#define STYPE_UNSTABLE (1)  // separatrice type
#define STYPE_STABLE (-1)
#define STYPE_CENUNSTABLE 2
#define STYPE_CENSTABLE (-2)
#define STYPE_ORBIT 3

#define FOCUSTYPE_UNKNOWN 0
#define FOCUSTYPE_UNSTABLE (1)
#define FOCUSTYPE_STABLE (-1)
#define FOCUSTYPE_CENTER 4

#define SETYPE_SADDLENODE_UNSTABSEP 1
#define SETYPE_SADDLENODE_UNSTABSEP2 2

#define OT_STABLE STYPE_STABLE
#define OT_UNSTABLE STYPE_UNSTABLE
#define OT_CENT_STABLE STYPE_CENSTABLE
#define OT_CENT_UNSTABLE STYPE_CENUNSTABLE
#define OT_ORBIT STYPE_ORBIT

enum TYPEOFVIEWS {
    TYPEOFVIEW_PLANE = 0,
    TYPEOFVIEW_SPHERE = 1,
    TYPEOFVIEW_U1 = 2,
    TYPEOFVIEW_U2 = 3,
    TYPEOFVIEW_V1 = 4,
    TYPEOFVIEW_V2 = 5,
    TYPEOFVIEW_U1U2 = 6,
    TYPEOFVIEW_V1V2 = 7
};

namespace p4curveRegions
{
struct vfRegionResult {
    int vfindex;  // which vector field
    int *signs;
};

struct curveRegionResult {
    int curveindex;  // which curve are we talking about
    int *signs;
};

struct curveResult {
    p4polynomials::term2 *sep;
    p4polynomials::term2 *sep_U1;
    p4polynomials::term2 *sep_U2;
    p4polynomials::term2 *sep_V1;
    p4polynomials::term2 *sep_V2;
    p4polynomials::term3 *sep_C;
    p4orbits::orbits_points *sep_points;

    curveResult() : sep_points(nullptr){};
};
}

bool readTerm1(FILE *fp, P4POLYNOM1 p, int N);
bool readTerm2(FILE *fp, P4POLYNOM2 p, int N);
bool readTerm3(FILE *fp, P4POLYNOM3 p, int N);

#define DUMP(x) m->append(s.sprintf x);
#define DUMPSTR(x) m->append(x);
#define MATHFUNC(function) (*(g_VFResults.function))

#define LINESTYLE_DASHES 1
#define LINESTYLE_POINTS 0

#endif /*FILE_TAB_H*/
