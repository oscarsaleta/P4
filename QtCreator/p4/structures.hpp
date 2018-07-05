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

// FIXME: add deleters? smart pointers? try again using vectors/lists?

// -----------------------------------------------------------------------
//                      General polynomial expressions
// -----------------------------------------------------------------------

// Linked list of univariate terms a*x^i
namespace p4polynom
{
struct term1 {
    int exp;
    double coeff;

    term1 *next_term1{nullptr};

    term1(int ex = 0, double co = 0.0, term1 *next = nullptr)
        : exp{ex}, coeff{co}, next_term1{next}
    {
    }
    ~term1()
    {
        if (next_term1 != nullptr) {
            delete next_term1;
            next_term1 = nullptr;
        }
    }
};

// Linked list of terms a*x^i*y^j
struct term2 {
    int exp_x;
    int exp_y;
    double coeff;

    term2 *next_term2{nullptr};

    term2(int xx = 0, int xy = 0, double co = 0.0, term2 *next = nullptr)
        : exp_x{xx}, exp_y{xy}, coeff{co}, next_term2{next}
    {
    }
    ~term2()
    {
        if (next_term2 != nullptr) {
            delete next_term2;
            next_term2 = nullptr;
        }
    }
};

// Linked list of terms a*r^i*cos(theta)^j*sin(theta)^k
struct term3 {
    int exp_r;
    int exp_Co;
    int exp_Si;
    double coeff;

    term3 *next_term3{nullptr};

    term3(int xr = 0, int xc = 0, int xs = 0, double co = 0.0,
          term3 *next = nullptr)
        : exp_r{xr}, exp_Co{xc}, exp_Si{xs}, coeff{co}, next_term3{next}
    {
    }
    ~term3()
    {
        if (next_term3 != nullptr) {
            delete next_term3;
            next_term3 = nullptr;
        }
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
    int dashes;       // plotted in dots or dashes
    int dir;  // if we have a line of sing at infinity and have to change
              // the direction if we integrate the orbit of separatrice
              // and sometimes the type
    int type; // type (stability) of orbit

    orbits_points *nextpt{nullptr}; // linked list to new orbit_points

    orbits_points() {}
    orbits_points(int co, double pc[3], int da, int di, int ty,
                  orbits_points *next = nullptr)
        : color{co}, dashes{da}, dir{di}, type{ty}, nextpt{next}
    {
        pcoord[0] = pc[0];
        pcoord[1] = pc[1];
        pcoord[2] = pc[2];
    }
    ~orbits_points()
    {
        if (nextpt != nullptr) {
            delete nextpt;
            nextpt = nullptr;
        }
    }
};

// delete a linked list of orbits_points
// void deleteOrbitPoints(orbits_points *p)
//{
//    orbits_points *q;
//    while (p != nullptr) {
//        q = p;
//        p = p->nextpt;
//        delete q;
//    }
//}

struct orbits {
    double pcoord[3]; // startpoint
    int color;        // color of orbit

    orbits_points *firstpt{nullptr};   // first orbit in this list
    orbits_points *currentpt{nullptr}; // current used orbit for computation
    orbits *next{nullptr};             // linked list to new orbit

    orbits() {}
    orbits(double pc[3], int co, orbits *nxt = nullptr) : color{co}, next{nxt}
    {
        pcoord[0] = pc[0];
        pcoord[1] = pc[1];
        pcoord[2] = pc[2];
    }
    ~orbits()
    {
        if (firstpt != nullptr) {
            delete firstpt;
            firstpt = nullptr;
        }
        if (next != nullptr) {
            delete next;
            next = nullptr;
        }
    }
};

// delete a linked list of orbits
// void deleteOrbits(orbits *p)
//{
//    orbits *q;
//    while (p != nullptr) {
//        q = p;
//        p = p->next;
//        deleteOrbitPoints(q->firstpt);
//        delete q;
//    }
//}
} // namespace p4orbits

// -----------------------------------------------------------------------
//                      Curves and isoclines
// -----------------------------------------------------------------------
namespace p4curves
{
struct curves { // NOTE: curveResult es identica, usarem aquesta
    p4polynom::term2 *r2{nullptr};
    p4polynom::term2 *u1{nullptr};
    p4polynom::term2 *u2{nullptr};
    p4polynom::term2 *v1{nullptr};
    p4polynom::term2 *v2{nullptr};
    p4polynom::term3 *c{nullptr};
    p4orbits::orbits_points *points{nullptr};

    curves() {}
    curves(p4polynom::term2 *_r2, p4polynom::term2 *_u1, p4polynom::term2 *_u2,
           p4polynom::term2 *_v1, p4polynom::term2 *_v2, p4polynom::term3 *_c,
           p4orbits::orbits_points *_points)
        : r2{_r2}, u1{_u1}, u2{_u2}, v1{_v1}, v2{_v2}, c{_c}, points{_points}
    {
    }
    ~curves()
    {
        if (r2 != nullptr) {
            delete r2;
            r2 = nullptr;
        }
        if (u1 != nullptr) {
            delete u1;
            u1 = nullptr;
        }
        if (u2 != nullptr) {
            delete u2;
            u2 = nullptr;
        }
        if (v1 != nullptr) {
            delete v1;
            v1 = nullptr;
        }
        if (v2 != nullptr) {
            delete v2;
            v2 = nullptr;
        }
        if (c != nullptr) {
            delete c;
            c = nullptr;
        }
    }
};

struct isoclines : curves {
    int color;

    isoclines() {}
    isoclines(int co, p4polynom::term2 *_r2 = nullptr,
              p4polynom::term2 *_u1 = nullptr, p4polynom::term2 *_u2 = nullptr,
              p4polynom::term2 *_v1 = nullptr, p4polynom::term2 *_v2 = nullptr,
              p4polynom::term3 *_c = nullptr,
              p4orbits::orbits_points *_points = nullptr)
        : curves(_r2, _u1, _u2, _v1, _v2, _c, _points), color{co}
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

    transformations *next_trans{nullptr};

    transformations() {}
    transformations(double _x0, double _y0, int _c1, int _c2, int _d1, int _d2,
                    int _d3, int _d4, int _d, transformations *next = nullptr)
        : x0{_x0}, y0{_y0}, c1{_c1}, c2{_c2}, d1{_d1}, d2{_d2}, d3{_d3},
          d4{_d4}, d{_d}, next_trans{next}
    {
    }
    ~transformations()
    {
        if (next_trans != nullptr) {
            delete next_trans;
            next_trans = nullptr;
        }
    }
};

struct blow_up_points {
    // number of transformations
    int n;
    // last point that is not degenerate
    double x0, y0;
    // transformation matrix
    double a11, a12, a21, a22;
    // type of seperatrice (STYPE_STABLE, ...)
    int type;
    // if true then use the blow up vector field if the modulus of the last
    // point of the separatrices is less than 1
    bool integrating_in_local_chart;
    // end point sep in blow up chart
    double point[2];

    transformations *trans{nullptr};
    // vector field
    p4polynom::term2 *vector_field[2]{nullptr, nullptr};
    // sep (t,g(t))
    p4polynom::term1 *sep{nullptr};
    p4orbits::orbits_points *first_sep_point{nullptr};
    p4orbits::orbits_points *last_sep_point{nullptr};
    blow_up_points *next_blow_up_point{nullptr};

    blow_up_points() {}
    blow_up_points(int _n, double _x0, double _y0, double _a11, double _a12,
                   double _a21, double _a22, int ty, bool in, double po[2],
                   transformations *tr = nullptr,
                   p4polynom::term2 *ve[2] = nullptr,
                   p4polynom::term1 *se = nullptr,
                   p4orbits::orbits_points *fsp = nullptr,
                   p4orbits::orbits_points *lsp = nullptr,
                   blow_up_points *next = nullptr)
        : n{_n}, x0{_x0}, y0{_y0}, a11{_a11}, a12{_a12}, a21{_a21}, a22{_a22},
          type{ty}, integrating_in_local_chart{in}, trans{tr}, sep{se},
          first_sep_point{fsp}, last_sep_point{lsp}, next_blow_up_point{next}
    {
        point[0] = po[0];
        point[1] = po[1];
        if (ve != nullptr) {
            vector_field[0] = nullptr;
            vector_field[1] = nullptr;
        }
    }
    ~blow_up_points()
    {
        if (trans != nullptr) {
            delete trans;
            trans = nullptr;
        }
        if (vector_field[0] != nullptr) {
            delete vector_field[0];
            vector_field[0] = nullptr;
        }
        if (vector_field[1] != nullptr) {
            delete vector_field[1];
            vector_field[1] = nullptr;
        }
        if (first_sep_point != nullptr) {
            delete first_sep_point;
            first_sep_point = nullptr;
        }
        if (next_blow_up_point != nullptr) {
            delete next_blow_up_point;
            next_blow_up_point = nullptr;
        }
    }
};

struct sep {
    int type; // STYPE_STABLE, UNSTABLE, CENSTABLE or CENUNSTABLE
    int direction;
    int d;
    bool notadummy; // false if separatrice is a copy of a structure (obtained
                    // through a symmetry)

    p4orbits::orbits_points *first_sep_point{nullptr};
    p4orbits::orbits_points *last_sep_point{nullptr};
    // if d=0 -> (t,f(t)), d=1 ->(f(t),t)
    p4polynom::term1 *separatrice{nullptr};
    sep *next_sep{nullptr};

    sep() {}
    sep(int ty, int di, int _d, bool no, p4orbits::orbits_points *fsp = nullptr,
        p4orbits::orbits_points *lsp = nullptr, p4polynom::term1 *se = nullptr,
        sep *next = nullptr)
        : type{ty}, direction{di}, d{_d}, notadummy{no}, first_sep_point{fsp},
          last_sep_point{lsp}, separatrice{se}, next_sep{next}
    {
    }
    ~sep()
    {
        if (first_sep_point != nullptr) {
            delete first_sep_point;
            first_sep_point = nullptr;
        }
        if (separatrice != nullptr) {
            delete separatrice;
            separatrice = nullptr;
        }
        if (next_sep != nullptr) {
            delete next_sep;
            next_sep = nullptr;
        }
    }
};
} // namespace p4blowup

namespace p4singularities
{
// -----------------------------------------------------------------------
//                          Singularities
// -----------------------------------------------------------------------

// in a piecewise system, singularities may coincide along the bifurcation
// lines.  In that case, we mark the singularity in a different way.
// Of course, on screen it need only be marked once and not several times.
enum {
    position_virtual = 0,            // virtual singularity
    position_standalone = 1,         // stand-alone singularity
    position_coinciding_main = 2,    // singularity coincides and will be drawn
    position_coinciding_virtual = 3, // is a virtual singularity, but coincides
                                     // with a real one so do not plot at all
    position_coinciding = 4 // singularity coincides, but is already drawn
};

// part of the structure that is the same for all types
struct genericsingularity {
    double x0;
    double y0;
    int chart;
    int position; // position_ constants

    genericsingularity() {}
    genericsingularity(double _x0, double _y0, int ch, int po)
        : x0{_x0}, y0{_y0}, chart{ch}, position{po}
    {
    }
};

struct saddle : genericsingularity {
    double epsilon;
    bool notadummy;
    double a11, a12, a21, a22; // transformation matrix

    p4blowup::sep *separatrices{nullptr};
    p4polynom::term2 *vector_field[2]{nullptr, nullptr};
    saddle *next_saddle{nullptr};

    saddle() {}
    saddle(double _x0, double _y0, int ch, int po, double ep, bool no,
           double _a11, double _a12, double _a21, double _a22,
           p4blowup::sep *se = nullptr, p4polynom::term2 *ve[2] = nullptr,
           saddle *next = nullptr)
        : genericsingularity(_x0, _y0, ch, po), epsilon{ep}, notadummy{no},
          a11{_a11}, a12{_a12}, a21{_a21}, a22{_a22}, separatrices{se},
          next_saddle{next}
    {
        if (ve != nullptr) {
            vector_field[0] = ve[0];
            vector_field[1] = ve[1];
        }
    }
    ~saddle()
    {
        if (separatrices != nullptr) {
            delete separatrices;
            separatrices = nullptr;
        }
        if (vector_field[0] != nullptr) {
            delete vector_field[0];
            vector_field[0] = nullptr;
        }
        if (vector_field[1] != nullptr) {
            delete vector_field[1];
            vector_field[1] = nullptr;
        }
        if (next_saddle != nullptr) {
            delete next_saddle;
            next_saddle = nullptr;
        }
    }
};

struct semi_elementary : genericsingularity {
    double epsilon;
    bool notadummy;
    double a11, a12, a21, a22; // transformation matrix
    int type;                  // type of semi-elementary point

    // center sep (t,f(t)), sep (g(t),t)
    p4blowup::sep *separatrices{nullptr};
    p4polynom::term2 *vector_field[2]{nullptr, nullptr}; // vector field
    semi_elementary *next_se{nullptr};

    semi_elementary() {}
    semi_elementary(double _x0, double _y0, int ch, int po, double ep, bool no,
                    double _a11, double _a12, double _a21, double _a22, int ty,
                    p4blowup::sep *se = nullptr,
                    p4polynom::term2 *vf[2] = nullptr,
                    semi_elementary *next = nullptr)
        : genericsingularity(_x0, _y0, ch, po), epsilon{ep}, notadummy{no},
          a11{_a11}, a12{_a12}, a21{_a21}, a22{_a22}, type{ty},
          separatrices{se}, next_se{next}
    {
        if (vf != nullptr) {
            vector_field[0] = vf[0];
            vector_field[1] = vf[1];
        }
    }
    ~semi_elementary()
    {
        if (separatrices != nullptr) {
            delete separatrices;
            separatrices = nullptr;
        }
        if (vector_field[0] != nullptr) {
            delete vector_field[0];
            vector_field[0] = nullptr;
        }
        if (vector_field[1] != nullptr) {
            delete vector_field[1];
            vector_field[1] = nullptr;
        }
        if (next_se != nullptr) {
            delete next_se;
            next_se = nullptr;
        }
    }
};

struct degenerate : genericsingularity {
    double epsilon;
    bool notadummy;

    p4blowup::blow_up_points *blow_up{nullptr};
    degenerate *next_de{nullptr};

    degenerate() {}
    degenerate(double _x0, double _y0, int ch, int po, double ep, bool no,
               p4blowup::blow_up_points *bl = nullptr,
               degenerate *next = nullptr)
        : genericsingularity(_x0, _y0, ch, po), epsilon{ep}, notadummy{no},
          blow_up{bl}, next_de{next}
    {
    }
    ~degenerate()
    {
        if (blow_up != nullptr) {
            delete blow_up;
            blow_up = nullptr;
        }
        if (next_de != nullptr) {
            delete next_de;
            next_de = nullptr;
        }
    }
};

struct node : genericsingularity {
    int stable;
    node *next_node;

    node() {}
    node(double _x0, double _y0, int ch, int po, int st, node *next = nullptr)
        : genericsingularity(_x0, _y0, ch, po), stable{st}, next_node{next}
    {
    }
    ~node()
    {
        if (next_node != nullptr) {
            delete next_node;
            next_node = nullptr;
        }
    }
};

struct strong_focus : genericsingularity {
    int stable;
    strong_focus *next_sf;

    strong_focus() {}
    strong_focus(double _x0, double _y0, int ch, int po, int st,
                 strong_focus *next = nullptr)
        : genericsingularity(_x0, _y0, ch, po), stable{st}, next_sf{next}
    {
    }
    ~strong_focus()
    {
        if (next_sf != nullptr) {
            delete next_sf;
            next_sf = nullptr;
        }
    }
};

struct weak_focus : genericsingularity {
    int type;
    weak_focus *next_wf;

    weak_focus() {}
    weak_focus(double _x0, double _y0, int ch, int po, int ty,
               weak_focus *next = nullptr)
        : genericsingularity(_x0, _y0, ch, po), type{ty}, next_wf{next}
    {
    }
    ~weak_focus()
    {
        if (next_wf != nullptr) {
            delete next_wf;
            next_wf = nullptr;
        }
    }
};
} // namespace p4singularities

namespace p4curveRegions
{
// TODO: podria ser redundant amb p4VFStudyRegions::vfRegion
struct vfRegionResult {
    int vfIndex; // which vector field
    std::vector<int> signs;
    vfRegionResult() {}
    vfRegionResult(int vf, std::vector<int> si)
        : vfIndex{vf}, signs{std::move(si)}
    {
    }
};
// TODO: podria ser redundant amb p4VFStudyRegions::curveRegion
struct curveRegionResult {
    int curveIndex; // which curve are we talking about
    std::vector<int> signs;
    curveRegionResult() {}
    curveRegionResult(int cu, std::vector<int> si)
        : curveIndex{cu}, signs{std::move(si)}
    {
    }
};

} // namespace p4curveRegions

#define DUMP(x) m->append(s.sprintf x);
#define DUMPSTR(x) m->append(x);
#define MATHFUNC(function) (*(gVFResults.function))
