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

#include <vector>

#include "structures.hpp"

class QString;

struct positionitem {
    double pcoord[3];
    p4singularities::genericsingularity s;
    positionitem() {}
    positionitem(double *p, p4singularities::genericsingularity _s) : s{_s}
    {
        pcoord[0] = p[0];
        pcoord[1] = p[1];
        pcoord[2] = p[2];
    }
};

bool isInsideRegion_R2(const std::vector<int> &signs, const double *ucoord);
bool isInsideRegion_R2_epsilon(const std::vector<int> &signs,
                               const double *ucoord, double epsilon);
bool isInsideRegion_U1(const std::vector<int> &signs, const double *z1z2);
bool isInsideRegion_U1_epsilon(const std::vector<int> &signs,
                               const double *z1z2, double epsilon);
bool isInsideRegion_V1(const std::vector<int> &signs, const double *z1z2);
bool isInsideRegion_V1_epsilon(const std::vector<int> &signs,
                               const double *z1z2, double epsilon);
bool isInsideRegion_U2(const std::vector<int> &signs, const double *z1z2);
bool isInsideRegion_U2_epsilon(const std::vector<int> &signs,
                               const double *z1z2, double epsilon);
bool isInsideRegion_V2(const std::vector<int> &signs, const double *z1z2);
bool isInsideRegion_V2_epsilon(const std::vector<int> &signs,
                               const double *z1z2, double epsilon);
bool isInsideRegion_cyl(const std::vector<int> &signs, const double *rtheta);
bool isInsideRegion_cyl_epsilon(const std::vector<int> &signs,
                                const double *rtheta, double epsilon);
bool isInsideRegion_sphere(const std::vector<int> &signs, const double *pcoord);
bool isInsideRegion_sphere_epsilon(const std::vector<int> &signs,
                                   const double *pcoord, double epsilon);

double eval_curve(const p4curves::curves &c, const double *pcoord);

QString describeRegion(double *pcoord);
bool isInTheSameRegion(double *testpt, double *refpos);
bool isARealSingularity(double *pcoord, unsigned int vfIndex);
bool isARealSingularity(double x0, double y0, int chart, int vfIndex);

void markSingularity(p4singularities::saddle &s,
                     std::vector<positionitem> &plist, int &numpos, int vfindex,
                     bool plweights);
void markSingularity(p4singularities::semi_elementary &s,
                     std::vector<positionitem> &plist, int &numpos, int vfindex,
                     bool plweights);
void markSingularity(p4singularities::node &s, std::vector<positionitem> &plist,
                     int &numpos, int vfindex, bool plweights);
void markSingularity(p4singularities::strong_focus &s,
                     std::vector<positionitem> &plist, int &numpos, int vfindex,
                     bool plweights);
void markSingularity(p4singularities::weak_focus &s,
                     std::vector<positionitem> &plist, int &numpos, int vfindex,
                     bool plweights);
void markSingularity(p4singularities::degenerate *s,
                     std::vector<positionitem> &plist, int &numpos, int vfindex,
                     bool plweights);
