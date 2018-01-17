/*  This file is part of P4
 *
 *  Copyright (C) 1996-2018  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  it under the terms of the GNU Lesser General Public License as published
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

#include "file_tab.h"

#include "file_vf.h"
#include "main.h"
#include "math_changedir.h"
#include "math_charts.h"
#include "math_orbits.h"
#include "math_p4.h"
#include "math_polynom.h"
#include "math_separatrice.h"
#include "p4application.h"

#include <locale.h>
#include <iostream>

#include <QFile>
#include <QTextEdit>
#include <QTextStream>

QPVFStudy g_VFResults;

/*
    This file contains the code to read the information from reduce/maple.
    The relevant structures for saddles etc are set up here.
*/







// -----------------------------------------------------------------------
//                      QVFStudy::ReadCurve FIXME posar a P4VFStudy
// -----------------------------------------------------------------------
bool QVFStudy::readCurve(QString basename)
{
    int N, degree_curve;
    FILE *fp = nullptr;
    setlocale(LC_ALL, "C");

    fp = fopen(QFile::encodeName(basename + "_veccurve.tab"), "rt");
    if (fp == nullptr) {
        dump(basename, "Cannot open file " + basename + "_veccurve.tab");
        return false;
    }

    curves new_curve;
    if (fscanf(fp, "%d", &degree_curve) != 1)
        return false;

    if (degree_curve > 0) {
        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_curve.r2 = new term2;
        new_curve.r2->next_term2 = nullptr;

        if (!readTerm2(fp, new_curve.r2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_curve.u1 = new term2;
        new_curve.u1->next_term2 = nullptr;

        if (!readTerm2(fp, new_curve.u1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_curve.u2 = new term2;
        new_curve.u2->next_term2 = nullptr;

        if (!readTerm2(fp, new_curve.u2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_curve.v1 = new term2;
        new_curve.v1->next_term2 = nullptr;
        if (!readTerm2(fp, new_curve.v1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;
        new_curve.v2 = new term2;
        new_curve.v2->next_term2 = nullptr;
        if (!readTerm2(fp, new_curve.v2, N))
            return false;

        if (p_ != 1 || q_ != 1) {
            if (fscanf(fp, "%d", &N) != 1)
                return false;

            new_curve.c = new term3;
            new_curve.c->next_term3 = nullptr;
            if (!readTerm3(fp, new_curve.c, N))
                return false;
        }
    } else {
        return false;
    }

    curve_vector_.push_back(new_curve);
    return true;
}

// -----------------------------------------------------------------------
//                      QVFStudy::ReadIsoclines FIXME posar a P4VFStudy
// -----------------------------------------------------------------------
bool QVFStudy::readIsoclines(QString basename)
{
    int N, degree_curve;
    FILE *fp = nullptr;
    setlocale(LC_ALL, "C");

    fp = fopen(QFile::encodeName(basename + "_vecisoclines.tab"), "rt");
    if (fp == nullptr) {
        dump(basename, "Cannot open file " + basename + "_vecisoclines.tab");
        return false;
    }

    isoclines new_isocline;
    if (fscanf(fp, "%d", &degree_curve) != 1)
        return false;

    if (degree_curve > 0) {
        if (fscanf(fp, "%d", &N) != 1)
            return false;

        // prepare a new isocline and link it to the list

        new_isocline.r2 = new term2;
        new_isocline.r2->next_term2 = nullptr;

        if (!readTerm2(fp, new_isocline.r2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_isocline.u1 = new term2;
        new_isocline.u1->next_term2 = nullptr;

        if (!readTerm2(fp, new_isocline.u1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_isocline.u2 = new term2;
        new_isocline.u2->next_term2 = nullptr;

        if (!readTerm2(fp, new_isocline.u2, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;

        new_isocline.v1 = new term2;
        new_isocline.v1->next_term2 = nullptr;
        if (!readTerm2(fp, new_isocline.v1, N))
            return false;

        if (fscanf(fp, "%d", &N) != 1)
            return false;
        new_isocline.v2 = new term2;
        new_isocline.v2->next_term2 = nullptr;
        if (!readTerm2(fp, new_isocline.v2, N))
            return false;

        if (p_ != 1 || q_ != 1) {
            if (fscanf(fp, "%d", &N) != 1)
                return false;

            new_isocline.c = new term3;
            new_isocline.c->next_term3 = nullptr;
            if (!readTerm3(fp, new_isocline.c, N))
                return false;
        } else {
            new_isocline.c = nullptr;
        }
    } else {
        return false;
    }

    isocline_vector_.push_back(new_isocline);
    return true;
}




// -----------------------------------------------------------------------
//          readTerm1
// -----------------------------------------------------------------------
bool readTerm1(FILE *fp, std::vector<p4polynom::term1> &p, int N)
{
    int exp;
    double coeff;
    p.clear();
    for (int i = 0; i < N; i++) {
        if (fscanf(fp, "%d %lf", &exp, &coeff) != 2) {
            return false;
        }
        p.push_back(p4polynom::term1(exp, coeff));
    }
    return true;
}

// -----------------------------------------------------------------------
//          readTerm2
// -----------------------------------------------------------------------
bool readTerm2(FILE *fp, std::vector<p4polynom::term2> &p, int N)
{
    int xx, xy;
    double coeff;
    p.clear();
    for (int i = 0; i < N; i++) {
        if (fscanf(fp, "%d %d %lf", &xx, &xy, &coeff) != 3) {
            return false;
        }
        p.push_back(p4polynom::term2(xx, xy, coeff));
    }
    return true;
}

// -----------------------------------------------------------------------
//          readTerm3
// -----------------------------------------------------------------------
bool readTerm3(FILE *fp, std::vector<p4polynom::term3> &p, int N)
{
    int xr, xc, xs;
    double coeff;
    p.clear();
    for (int i = 0; i < N; i++) {
        if (fscanf(fp, "%d %d %d %lf", &xr, &xc, &xs, &coeff) != 4) {
            return false;
        }
        p.push_back(p4polynom::term3(xr, xc, xs, coeff));
    }
    return true;
}




