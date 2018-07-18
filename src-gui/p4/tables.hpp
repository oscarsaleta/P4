/*  This file is part of P4
 *
 *  Copyright (C) 1996-2018  J.C. Artés, P. De Maesschalck, F. Dumortier
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

#pragma once

// -----------------------------------------------------------------------
//                      General polynomial expressions
// -----------------------------------------------------------------------

// Linked list of univariate terms a*x^i
namespace p4polynom
{
struct term1 {
    int exp;
    double coeff;

    term1(int ex = 0, double co = 0.0) : exp{ex}, coeff{co} {}
};

// Linked list of terms a*x^i*y^j
struct term2 {
    int exp_x;
    int exp_y;
    double coeff;

    term2(int xx = 0, int xy = 0, double co = 0.0)
        : exp_x{xx}, exp_y{xy}, coeff{co}
    {
    }
};

// Linked list of terms a*r^i*cos(theta)^j*sin(theta)^k
struct term3 {
    int exp_r;
    int exp_Co;
    int exp_Si;
    double coeff;

    term3(int xr = 0, int xc = 0, int xs = 0, double co = 0.0)
        : exp_r{xr}, exp_Co{xc}, exp_Si{xs}, coeff{co}
    {
    }
};
} // namespace p4polynom

// -----------------------------------------------------------------------
//                              Orbits
// -----------------------------------------------------------------------
namespace p4orbits
{
struct orbits_points {
    int color; // color of seperatrice

    double pcoord[3]; // point on the poincare sphere -> p=(X,Y,Z)
                      // or on the poincare-lyapunov sphere
                      // -> p=(0,x,y) or p=(1,r,theta)
    int dashes;
    int dir; // if we have a line of sing at infinity and have to change
             // the direction if we integrate the orbit of separatrice
             // and sometimes the type
    int type;
    orbits_points() {}
    orbits_points(int co, double pc[3], int da, int di, int ty)
        : color{co}, dashes{da}, dir{di}, type{ty}
    {
        pcoord[0] = pc[0];
        pcoord[1] = pc[1];
        pcoord[2] = pc[2];
    }
};

struct orbits {
    double pcoord[3]; // startpoint
    int color;
    std::vector<orbits_points> points; // orbit

    orbits() {}
    orbits(double pc[3], int co, std::vector<orbits_points> fo)
        : color{co}, points{fo}
    {
        pcoord[0] = pc[0];
        pcoord[1] = pc[1];
        pcoord[2] = pc[2];
    }
};
} // namespace p4orbits

// -----------------------------------------------------------------------
//                      Curves and isoclines
// -----------------------------------------------------------------------
namespace p4curves
{
struct curves { // NOTE: curveResult es identica, usarem aquesta
    std::vector<p4polynom::term2> r2, u1, u2, v1, v2;
    std::vector<p4polynom::term3> c;
    std::vector<p4orbits::orbits_points> points;

    curves() {}
    curves(std::vector<p4polynom::term2> _r2, std::vector<p4polynom::term2> _u1,
           std::vector<p4polynom::term2> _u2, std::vector<p4polynom::term2> _v1,
           std::vector<p4polynom::term2> _v2, std::vector<p4polynom::term3> _c,
           std::vector<p4orbits::orbits_points> po)
        : r2{_r2}, u1{_u1}, u2{_u2}, v1{_v1}, v2{_v2}, c{_c}, points{po}
    {
    }
};

struct isoclines : curves {
    int color;

    isoclines() {}
    isoclines(std::vector<p4polynom::term2> _r2,
              std::vector<p4polynom::term2> _u1,
              std::vector<p4polynom::term2> _u2,
              std::vector<p4polynom::term2> _v1,
              std::vector<p4polynom::term2> _v2,
              std::vector<p4polynom::term3> _c,
              std::vector<p4orbits::orbits_points> _po, int co)
        : curves(_r2, _u1, _u2, _v1, _v2, _c, _po), color{co}
    {
    }
};
} // namespace p4curves

// -----------------------------------------------------------------------
//                      Blow up structure
// -----------------------------------------------------------------------
namespace p4blowup
{
struct transformations {
    double x0, y0;              // translation point
    int c1, c2, d1, d2, d3, d4; // F(x,y)=(c1*x^d1*y^d2,c2*x^d3*y^d4)
    int d;                      // X/x^d

    transformations() {}
    transformations(double _x0, double _y0, int _c1, int _c2, int _d1, int _d2,
                    int _d3, int _d4, int _d)
        : x0{_x0}, y0{_y0}, c1{_c1}, c2{_c2}, d1{_d1}, d2{_d2}, d3{_d3},
          d4{_d4}, d{_d}
    {
    }
};

struct blow_up_points {
    // number of transformations
    int n;
    std::vector<transformations> trans;
    // last point that is not degenerate
    double x0, y0;
    // transformation matrix
    double a11, a12, a21, a22;
    // vector field
    std::vector<p4polynom::term2> vector_field_0;
    std::vector<p4polynom::term2> vector_field_1;
    // sep (t,g(t))
    std::vector<p4polynom::term1> sep;
    // type of seperatrice (STYPE_STABLE, ...)
    int type;
    // if true then use the blow up vector field if the modulus of the last
    // point of the separatrices is less than 1
    bool integrating_in_local_chart;
    // end point sep in blow up chart
    double point[2];

    std::vector<p4orbits::orbits_points> sep_points;

    blow_up_points() {}
    blow_up_points(int _n, std::vector<transformations> tr, double _x0,
                   double _y0, double _a11, double _a12, double _a21,
                   double _a22, std::vector<p4polynom::term2> ve0,
                   std::vector<p4polynom::term2> ve1,
                   std::vector<p4polynom::term1> se, int ty, bool in,
                   double po[2], std::vector<p4orbits::orbits_points> sp)
        : n{_n}, trans{tr}, x0{_x0}, y0{_y0}, a11{_a11}, a12{_a12}, a21{_a21},
          a22{_a22}, vector_field_0{ve0}, vector_field_1{ve1}, sep{se},
          type{ty}, integrating_in_local_chart{in}, sep_points{sp}
    {
        point[0] = po[0];
        point[1] = po[1];
    }
};

struct sep {
    std::vector<p4orbits::orbits_points> sep_points;
    int type; // STYPE_STABLE, UNSTABLE, CENSTABLE or CENUNSTABLE
    int direction;
    int d;
    bool notadummy; // false if separatrice is a copy of a structure (obtained
                    // through a symmetry)
    std::vector<p4polynom::term1> separatrice;
    // if d=0 -> (t,f(t)), d=1 ->(f(t),t)

    sep() {}
    sep(std::vector<p4orbits::orbits_points> sp, int ty, int di, int _d,
        bool no, std::vector<p4polynom::term1> se)
        : sep_points{sp}, type{ty}, direction{di}, d{_d}, notadummy{no},
          separatrice{se}
    {
    }
};
} // namespace p4blowup

// -----------------------------------------------------------------------
//                          Singularities
// -----------------------------------------------------------------------

// (taken from P5 source code) ----------
// in a piecewise system, singularities may coincide along the bifurcation
// lines.  In that case, we mark the singularity in a different way.
// Of course, on screen it need only be marked once and not several times.

#define POSITION_VIRTUAL 0         // virtual singularity
#define POSITION_STANDALONE 1      // stand-alone singularity
#define POSITION_COINCIDING_MAIN 2 // singularity coincides and will be drawn

#define POSITION_COINCIDING_VIRTUAL                                            \
    3 // is a virtual singularity, but coincides with a real one so do not plot
      // at all
#define POSITION_COINCIDING 4 // singularity coincides, but is already drawn
// --------------------------------------
//
namespace p4singularities
{
// part of the structure that is the same for all types
struct genericsingularity {
    double x0;
    double y0;
    // struct genericsingularity *next;
    int chart;
    int position; // POSITION_ constants

    genericsingularity() {}
    genericsingularity(double _x0, double _y0, int ch, int po)
        : x0{_x0}, y0{_y0}, chart{ch}, position{po}
    {
    }
};

struct saddle : genericsingularity {
    double epsilon;
    bool notadummy;

    std::vector<p4blowup::sep> separatrices;
    std::vector<p4polynom::term2> vector_field_0;
    std::vector<p4polynom::term2> vector_field_1;
    double a11, a12, a21, a22; // transformation matrix

    saddle() {}
    saddle(double _x0, double _y0, int ch, int po, double ep, bool no,
           std::vector<p4blowup::sep> se, std::vector<p4polynom::term2> ve0,
           std::vector<p4polynom::term2> ve1, double _a11, double _a12,
           double _a21, double _a22)
        : genericsingularity(_x0, _y0, ch, po), epsilon{ep}, notadummy{no},
          separatrices{se}, vector_field_0{ve0},
          vector_field_1{ve1}, a11{_a11}, a12{_a12}, a21{_a21}, a22{_a22}
    {
    }
};

struct semi_elementary : genericsingularity {
    double epsilon;
    bool notadummy;

    // center sep (t,f(t)), sep (g(t),t)
    std::vector<p4blowup::sep> separatrices;
    std::vector<p4polynom::term2> vector_field_0; // vector field
    std::vector<p4polynom::term2> vector_field_1; // vector field
    double a11, a12, a21, a22;                    // transformation matrix

    int type; // type of semi-elementary point

    semi_elementary() {}
    semi_elementary(double _x0, double _y0, int ch, int po, double ep, bool no,
                    std::vector<p4blowup::sep> se,
                    std::vector<p4polynom::term2> ve0,
                    std::vector<p4polynom::term2> ve1, double _a11, double _a12,
                    double _a21, double _a22, int ty)
        : genericsingularity(_x0, _y0, ch, po), epsilon{ep}, notadummy{no},
          separatrices{se}, vector_field_0{ve0}, vector_field_1{ve1}, a11{_a11},
          a12{_a12}, a21{_a21}, a22{_a22}, type{ty}
    {
    }
};

struct degenerate : genericsingularity {
    double epsilon;
    bool notadummy;

    std::vector<p4blowup::blow_up_points> blow_up;

    degenerate() {}
    degenerate(double _x0, double _y0, int ch, int po, double ep, bool no,
               std::vector<p4blowup::blow_up_points> bl)
        : genericsingularity(_x0, _y0, ch, po), epsilon{ep}, notadummy{no},
          blow_up{bl}
    {
    }
};

struct node : genericsingularity {
    int stable;

    node() {}
    node(double _x0, double _y0, int ch, int po, int st)
        : genericsingularity(_x0, _y0, ch, po), stable{st}
    {
    }
};

struct strong_focus : genericsingularity {
    int stable;

    strong_focus() {}
    strong_focus(double _x0, double _y0, int ch, int po, int st)
        : genericsingularity(_x0, _y0, ch, po), stable{st}
    {
    }
};

struct weak_focus : genericsingularity {
    int type;

    weak_focus() {}
    weak_focus(double _x0, double _y0, int ch, int po, int ty)
        : genericsingularity(_x0, _y0, ch, po), type{ty}
    {
    }
};
} // namespace p4singularities
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

#define STYPE_UNSTABLE (1) // separatrice type
#define STYPE_STABLE (-1)
#define STYPE_CENUNSTABLE 2
#define STYPE_CENSTABLE (-2)
#define STYPE_ORBIT 3

#define SINGTYPE_UNSTABLE STYPE_UNSTABLE
#define SINGTYPE_STABLE STYPE_STABLE
#define SINGTYPE_CENTER 4

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
// TODO: podria ser redundant amb p4VFStudyRegions::vfRegion
struct vfRegionResult {
    int vfIndex; // which vector field
    std::vector<int> signs;
    vfRegionResult() {}
    vfRegionResult(int vf, std::vector<int> si) : vfIndex{vf}, signs{si} {}
};
// TODO: podria ser redundant amb p4VFStudyRegions::curveRegion
struct curveRegionResult {
    int curveIndex; // which curve are we talking about
    std::vector<int> signs;
    curveRegionResult() {}
    curveRegionResult(int cu, std::vector<int> si) : curveIndex{cu}, signs{si}
    {
    }
};

// FIXME: canviar tots els CURVERESULT per p4curves::curve
/*struct curveResult {
    std::vector<p4polynom::term2> sep;
    std::vector<p4polynom::term2> sep_U1;
    std::vector<p4polynom::term2> sep_U2;
    std::vector<p4polynom::term2> sep_V1;
    std::vector<p4polynom::term2> sep_V2;
    std::vector<p4polynom::term3> sep_C;
    std::vector<p4orbits::orbits_points> sep_points;

    curveResult() {}
    curveResult(std::vector<p4polynom::term2> _sep,
                std::vector<p4polynom::term2> _sep_U1,
                std::vector<p4polynom::term2> _sep_U2,
                std::vector<p4polynom::term2> _sep_V1,
                std::vector<p4polynom::term2> _sep_V2,
                std::vector<p4polynom::term3> _sep_C,
                std::vector<p4orbits::orbits_points> _sep_points)
        : sep(_sep), sep_U1(_sep_U1), sep_U2(_sep_U2), sep_V1(_sep_V1),
          sep_V2(_sep_V2), sep_C(_sep_C), sep_points(_sep_points)
    {
    }
    ~curveResult()
    {
        sep.clear();
        sep_U1.clear();
        sep_U2.clear();
        sep_V1.clear();
        sep_V2.clear();
        sep_C.clear();
        sep_points.clear();
    }
};*/
} // namespace p4curveRegions

#define DUMP(x) m->append(s.sprintf x);
#define DUMPSTR(x) m->append(x);
#define MATHFUNC(function) (*(gVFResults.function))

#define LINESTYLE_DASHES 1
#define LINESTYLE_POINTS 0
