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

// -----------------------------------------------------------------------
//
//                      ROUTINES TO HANDLE POLYNOMIALS
//
// -----------------------------------------------------------------------

#include "math_polynom.hpp"

#include <QByteArray>
#include <QString>

#include <cmath>

#include "structures.hpp"

// -----------------------------------------------------------------------
//          eval_term1
// -----------------------------------------------------------------------
// Calculates p(t) for a polynomial p and a value t.
double eval_term1(const std::vector<p4polynom::term1> &p, const double t)
{
    double s = 0;
    for (auto const &q : p) {
        if (q.exp != 0)
            s += (q.coeff) * pow(t, static_cast<double>(q.exp));
        else
            s += q.coeff;
    }
    return s;
}

// -----------------------------------------------------------------------
//          eval_term2
// -----------------------------------------------------------------------
// Calculates f(x,y) for a polynomial f and values x and y.
// value refers to an array containing x and y: value[0]=x, value[1]=y
double eval_term2(const std::vector<p4polynom::term2> &f, const double *value)
{
    double s = 0;
    for (auto const &it : f) {
        if (it.exp_x != 0 && it.exp_y != 0)
            s += it.coeff * pow(value[0], static_cast<double>(it.exp_x)) *
                 pow(value[1], static_cast<double>(it.exp_y));
        else if (it.exp_x != 0)
            s += it.coeff * pow(value[0], static_cast<double>(it.exp_x));
        else if (it.exp_y != 0)
            s += it.coeff * pow(value[1], static_cast<double>(it.exp_y));
        else
            s += it.coeff;
    }
    return s;
}

// -----------------------------------------------------------------------
//          eval_term3
// -----------------------------------------------------------------------
// Calculates F( r, cos(theta), sin(theta) ) for a polynomial f and values
// of r and theta.
// value refers to an array containing r and theta: value[0]=r, value[1]=theta
double eval_term3(const std::vector<p4polynom::term3> &F, const double *value)
{
    double s = 0;
    double t;
    double Co, Si;

    Co = cos(value[1]);
    Si = sin(value[1]);

    for (auto const &it : F) {
        if (it.exp_r != 0)
            t = pow(value[0], static_cast<double>(it.exp_r));
        else
            t = 1.0;
        if (it.exp_Co != 0)
            t *= pow(Co, static_cast<double>(it.exp_Co));
        if (it.exp_Si != 0)
            t *= pow(Si, static_cast<double>(it.exp_Si));
        s += t * it.coeff;
    }
    return s;
}

// -----------------------------------------------------------------------
//          dumpPoly1
// -----------------------------------------------------------------------
// Make a string representation of a polynomial in one variable.
// The variable name to be used is given as a parameter
const char *dumpPoly1(const std::vector<p4polynom::term1> &f, const char *x)
{
    static QByteArray _s;

    double c;
    int cx;
    QString s{""};
    QString t;

    if (f.empty())
        return "0 (null)";

    for (auto const &it : f) {
        c = it.coeff;
        cx = it.exp;
        if ((c != 1 && c != -1) || cx == 0) {
            if (s.isEmpty())
                t.sprintf("%g", c);
            else {
                if (c >= 0)
                    t.sprintf(" + %g", c);
                else
                    t.sprintf(" - %g", c);
            }
            s += t;
        } else {
            if (c == -1) {
                if (s.isEmpty())
                    s = "-";
                else
                    s += " - ";
            } else if (c == 1) {
                if (!s.isEmpty())
                    s += " + ";
            }
        }
        if (cx != 0) {
            if (cx == 1)
                t = x;
            else
                t.sprintf("%s^%d", x, cx);
        }
    }
    _s = s.toLatin1();
    return _s.constData();
}

// -----------------------------------------------------------------------
//          dumpPoly2
// -----------------------------------------------------------------------
// Make a string representation of a polynomial in two variables.
// The variable names to be used are given as a parameter
const char *dumpPoly2(const std::vector<p4polynom::term2> &f, const char *x,
                      const char *y)
{
    static QByteArray _s;

    double c;
    int cx, cy;
    QString s;
    QString t;
    s = "";

    if (f.empty())
        return "0 (null)";

    for (auto const &it : f) {
        c = it.coeff;
        cx = it.exp_x;
        cy = it.exp_y;

        if ((c != 1 && c != -1) || (cx == 0 && cy == 0)) {
            if (s.isEmpty()) {
                t.sprintf("%g", c);
            } else {
                if (c >= 0)
                    t.sprintf(" + %g", c);
                else
                    t.sprintf(" - %g", -c);
            }
            s += t;
        } else {
            if (c == -1) {
                if (s.isEmpty())
                    s = "-";
                else
                    s += " - ";
            } else if (c == 1) {
                if (!s.isEmpty())
                    s += " + ";
            }
        }
        if (cx != 0) {
            if (cx == 1)
                t = x;
            else
                t.sprintf("%s^%d", x, cx);
            s += t;
        }
        if (cy != 0) {
            if (cy == 1)
                t = y;
            else
                t.sprintf("%s^%d", y, cy);
            s += t;
        }
    }
    _s = s.toLatin1();
    return _s.constData();
}

// -----------------------------------------------------------------------
//          dumpPoly3
// -----------------------------------------------------------------------
// Make a string representation of a polynomial in two variables.
// The variable names to be used are given as a parameter
const char *dumpPoly3(const std::vector<p4polynom::term3> &f, const char *x,
                      const char *y, const char *z)
{
    static QByteArray _s;

    double c;
    int cx, cy, cz;
    QString s;
    QString t;
    s = "";

    if (f.empty())
        return "0 (null)";

    for (auto const &it : f) {
        c = it.coeff;
        cx = it.exp_r;
        cy = it.exp_Co;
        cz = it.exp_Si;

        if ((c != 1 && c != -1) || (cx == 0 && cy == 0 && cz == 0)) {
            if (s.length() == 0) {
                t.sprintf("%g", c);
            } else {
                if (c >= 0)
                    t.sprintf(" + %g", c);
                else
                    t.sprintf(" - %g", -c);
            }
            s += t;
        } else {
            if (c == -1) {
                if (s.length() == 0)
                    s = "-";
                else
                    s += " - ";
            } else if (c == 1) {
                if (s.length() != 0)
                    s += " + ";
            }
        }
        if (cx != 0) {
            if (cx == 1)
                t = x;
            else
                t.sprintf("%s^%d", x, cx);
            s += t;
        }
        if (cy != 0) {
            if (cy == 1)
                t = y;
            else
                t.sprintf("%s^%d", y, cy);
            s += t;
        }
        if (cz != 0) {
            if (cz == 1)
                t = z;
            else
                t.sprintf("%s^%d", z, cz);
            s += t;
        }
    }
    _s = s.toLatin1();
    return _s.constData();
}

// Following are used in preparation of GCF :

// -----------------------------------------------------------------------
//          printterm2
// -----------------------------------------------------------------------
char *printterm2(char *buf, const p4polynom::term2 &f, bool isfirst,
                 const char *x, const char *y)
{
    if (f.coeff == 0) {
        if (isfirst)
            strcpy(buf, "0");
        else
            *buf = 0;
        return buf;
    }

    if ((f.coeff == 1 || f.coeff == -1) && (f.exp_x != 0 || f.exp_y != 0)) {
        if (f.coeff < 0)
            sprintf(buf, "-");
        else if (isfirst)
            *buf = 0;
        else
            sprintf(buf, "+");

        if (f.exp_x != 0) {
            if (f.exp_x != 1)
                sprintf(buf + strlen(buf), "%s^%d", x, f.exp_x);
            else
                sprintf(buf + strlen(buf), "%s", x);

            if (f.exp_y != 0) {
                if (f.exp_y != 1)
                    sprintf(buf + strlen(buf), "*%s^%d", y, f.exp_y);
                else
                    sprintf(buf + strlen(buf), "*%s", y);
            }
        } else if (f.exp_y != 0) {
            if (f.exp_y != 1)
                sprintf(buf + strlen(buf), "%s^%d", y, f.exp_y);
            else
                sprintf(buf + strlen(buf), "%s", y);
        }

        return buf;
    }

    if (isfirst)
        sprintf(buf, "%g", f.coeff);
    else
        sprintf(buf, "%+g", f.coeff);

    if (f.exp_x != 0) {
        if (f.exp_x != 1)
            sprintf(buf + strlen(buf), "*%s^%d", x, f.exp_x);
        else
            sprintf(buf + strlen(buf), "*%s", x);
    }
    if (f.exp_y != 0) {
        if (f.exp_y != 1)
            sprintf(buf + strlen(buf), "*%s^%d", y, f.exp_y);
        else
            sprintf(buf + strlen(buf), "*%s", y);
    }

    return buf;
}

// -----------------------------------------------------------------------
//          printterm3
// -----------------------------------------------------------------------
char *printterm3(char *buf, const p4polynom::term3 &f, bool isfirst,
                 const char *r, const char *Co, const char *Si)
{
    if (f.coeff == 0) {
        if (isfirst)
            strcpy(buf, "0");
        else
            *buf = 0;
        return buf;
    }

    if ((f.coeff == 1 || f.coeff == -1) &&
        (f.exp_r != 0 || f.exp_Co != 0 || f.exp_Si != 0)) {
        if (f.coeff < 0)
            sprintf(buf, "-");
        else if (isfirst)
            *buf = 0;
        else
            sprintf(buf, "+");

        if (f.exp_r != 0) {
            if (f.exp_r != 1)
                sprintf(buf + strlen(buf), "%s^%d", r, f.exp_r);
            else
                sprintf(buf + strlen(buf), "%s", r);

            if (f.exp_Co != 0) {
                if (f.exp_Co != 1)
                    sprintf(buf + strlen(buf), "*%s^%d", Co, f.exp_Co);
                else
                    sprintf(buf + strlen(buf), "*%s", Co);
            }
            if (f.exp_Si != 0) {
                if (f.exp_Si != 1)
                    sprintf(buf + strlen(buf), "*%s^%d", Si, f.exp_Si);
                else
                    sprintf(buf + strlen(buf), "*%s", Si);
            }
        } else if (f.exp_Co != 0) {
            if (f.exp_Co != 1)
                sprintf(buf + strlen(buf), "%s^%d", Co, f.exp_Co);
            else
                sprintf(buf + strlen(buf), "%s", Co);

            if (f.exp_Si != 0) {
                if (f.exp_Si != 1)
                    sprintf(buf + strlen(buf), "*%s^%d", Si, f.exp_Si);
                else
                    sprintf(buf + strlen(buf), "*%s", Si);
            }
        } else if (f.exp_Si != 0) {
            if (f.exp_Si != 1)
                sprintf(buf + strlen(buf), "%s^%d", Si, f.exp_Si);
            else
                sprintf(buf + strlen(buf), "%s", Si);
        }

        return buf;
    }

    if (isfirst)
        sprintf(buf, "%g", f.coeff);
    else
        sprintf(buf, "%+g", f.coeff);

    if (f.exp_r != 0) {
        if (f.exp_r != 1)
            sprintf(buf + strlen(buf), "*%s^%d", r, f.exp_r);
        else
            sprintf(buf + strlen(buf), "*%s", r);
    }
    if (f.exp_Co != 0) {
        if (f.exp_Co != 1)
            sprintf(buf + strlen(buf), "*%s^%d", Co, f.exp_Co);
        else
            sprintf(buf + strlen(buf), "*%s", Co);
    }
    if (f.exp_Si != 0) {
        if (f.exp_Si != 1)
            sprintf(buf + strlen(buf), "*%s^%d", Si, f.exp_Si);
        else
            sprintf(buf + strlen(buf), "*%s", Si);
    }

    return buf;
}

// -----------------------------------------------------------------------
//          readTerm1
// -----------------------------------------------------------------------
bool readTerm1(FILE *fp, p4polynom::term1 *p, int N)
{
    auto q = p;

    if (N < 1)
        return false;

    if (fscanf(fp, "%d %lf", &(p->exp), &(p->coeff)) != 2 || p->exp < 0)
        return false;

    for (int i = 2; i <= N; i++) {
        p->next_term1 = new p4polynom::term1;
        p = p->next_term1;
        if (fscanf(fp, "%d %lf", &(p->exp), &(p->coeff)) != 2 || p->exp < 0) {
            delete q->next_term1;
            q->next_term1 = nullptr;
            return false;
        }
    }

    return true;
}

// -----------------------------------------------------------------------
//          readTerm2
// -----------------------------------------------------------------------
bool readTerm2(FILE *fp, p4polynom::term2 *p, int N)
{
    auto q = p;

    if (N < 1)
        return false;

    if (fscanf(fp, "%d %d %lf", &(p->exp_x), &(p->exp_y), &(p->coeff)) != 3 ||
        p->exp_x < 0 || p->exp_y < 0)
        return false;

    for (int i = 2; i <= N; i++) {
        p->next_term2 = new p4polynom::term2;
        p = p->next_term2;
        if (fscanf(fp, "%d %d %lf", &(p->exp_x), &(p->exp_y), &(p->coeff)) !=
                3 ||
            p->exp_x < 0 || p->exp_y < 0) {
            delete q->next_term2;
            q->next_term2 = nullptr;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//          readTerm3
// -----------------------------------------------------------------------
bool readTerm3(FILE *fp, p4polynom::term3 *p, int N)
{
    auto q = p;

    if (N < 1)
        return false;

    if (fscanf(fp, "%d %d %d %lf", &(p->exp_r), &(p->exp_Co), &(p->exp_Si),
               &(p->coeff)) != 4 ||
        p->exp_r < 0 || p->exp_Co < 0 || p->exp_Si < 0)
        return false;

    for (int i = 2; i <= N; i++) {
        p->next_term3 = new p4polynom::term3;
        p = p->next_term3;
        if (fscanf(fp, "%d %d %d %lf", &(p->exp_r), &(p->exp_Co), &(p->exp_Si),
                   &(p->coeff)) != 4 ||
            p->exp_r < 0 || p->exp_Co < 0 || p->exp_Si < 0) {
            delete q->next_term3;
            q->next_term3 = nullptr;
            return false;
        }
    }

    return true;
}
