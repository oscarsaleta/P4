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

#include "math_separatingcurves.hpp"

static void insert_curve_point(double x0, double y0, double z0, int dashes,
                               std::vector<p4orbits::orbits_points> &lastpt)
{
    p4orbits::orbits_points newpt;

    newpt.pcoord[0] = x0;
    newpt.pcoord[1] = y0;
    newpt.pcoord[2] = z0;
    newpt.dashes = dashes;
    newpt.color = CSEPCURVE;

    lastpt.push_back(newpt);
}

bool readCurvePoints(FILE *fp, std::vector<p4orbits::orbits_points> &psep,
                     int index)
{
    int k;
    int numpoints;
    double x, y;
    double pcoord[3];
    int d, c;
    int chartindex;
    void (*chart)(double, double, double *);
    static void (*charts[12])(double, double, double *) = {
        R2_to_psphere,          U1_to_psphere,        U2_to_psphere,
        VV1_to_psphere,         VV2_to_psphere,       nullptr,
        polarcoord_to_plsphere, cylinder_to_plsphere, cylinder_to_plsphere,
        cylinder_to_plsphere,   cylinder_to_plsphere, nullptr};

    chartindex = (gVFResults.plweights_) ? 6 : 0;
    while (1) {
        chart = charts[chartindex];
        if (chart == nullptr)
            break;
        chartindex++;

        if (fscanf(fp, "%d\n", &numpoints) != 1 || numpoints < 0)
            return false;

        k = 0;
        while (k < numpoints) {
            d = 0;
            while (k < numpoints) {
                if (fscanf(fp, "%lf %lf", &x, &y) != 2)
                    break;
                k++;
                (*chart)(x, y, pcoord);
                insert_curve_point(pcoord[0], pcoord[1], pcoord[2], d, psep);
                d = 1;
                /*P5 d=GcfDashes; */
            }
            if (k < numpoints) {
                for (c = getc(fp); isspace(c);)
                    c = getc(fp);
                if (c != ',')
                    return false;
            }
        }
    }
    return true;
}
